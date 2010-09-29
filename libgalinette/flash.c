/*
 *  Copyright (C) Boris Lechner 2009 2010 <boris.lechner.dev@gmail.com>
 *   
 *  This file is part of Galinette.
 *
 *  Galinette is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License.
 *
 *  Galinette is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License along
 *  with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "libgalinette.h"
#include "libgalinette-private.h"
#include <stdio.h>
#include <ftdi.h>


void (*progressCallback)(unsigned long, unsigned long, unsigned long, const char *) = NULL;

// private
// Returns 0 if ok, <0 if an error occurred
int waitForStartFlashMsg() {
	int i, ret;
	unsigned char c=0;

	while(c != '*') {
	
//DEBUG		printf("DEBUG: Wait for start packet\n");
		for(ret=0 ; ret==0 ;) {
			if (progressCallback)
				progressCallback(0, 100, 0, "Waiting for the device...");
			ret = ftdi_read_data(&ftdic, &c, 1);
		}

		if (ret < 0) {
			galinette_setError(ERR_FLASH_FTDIREAD,
			         ftdi_get_error_string(&ftdic));
			return ERR_FLASH_FTDIREAD;
		}

//DEBUG		printf("DEBUG: Received a char : '%c'\n", c);
		// Init packet (may be facultative)
		if ( c == '+' ) {
			c = '#';				
			if (ftdi_usb_purge_rx_buffer(&ftdic) < 0 ) {
				galinette_setError(ERR_FLASH_FTDIPURGERX,
				         ftdi_get_error_string(&ftdic));
				return ERR_FLASH_FTDIPURGERX;
			}
			
			for(i=0 ; i<4 && ret>0 ; i++) {
				ret = ftdi_write_data(&ftdic, &c, 1);
//DEBUG				printf("DEBUG: sent '#'\n");
				usleep(2);

				if (ret < 0) {
					galinette_setError(ERR_FLASH_FTDIWRITE,
					         ftdi_get_error_string(&ftdic));
					return ERR_FLASH_FTDIWRITE;
				}
			}
		}
		
		// Start packet
		if (c == '*') {
			usleep(10);
			ret = ftdi_write_data(&ftdic, &c, 1);
//DEBUG			printf("DEBUG: sent '*'\n");
			usleep(10);

			if (ret < 0) {
				galinette_setError(ERR_FLASH_FTDIWRITE,
				         ftdi_get_error_string(&ftdic));
				return ERR_FLASH_FTDIWRITE;
			}
		}
	}

	return 0;
}


// private
// Returns 0 if ok, <0 if an error occurred
int waitForReadyToReceiveMsg() {
	int ret;
	unsigned char c;

	for( ret=0, c=0 ; ret>=0 && c!=0xaa ; )	
		ret = ftdi_read_data(&ftdic, &c, 1);
//DEBUG	printf("DEBUG: received 0xaa\n");
	
	if ( ret < 0 ) {
		galinette_setError(ERR_FLASH_FTDIREAD,
		         ftdi_get_error_string(&ftdic));
		return ERR_FLASH_FTDIREAD;
	}

	return 0;
}


// private
// Returns 0 if ok, <0 if an error occurred
int sendPacketLen(unsigned char bytesToSend) {
	int ret;
	ret = ftdi_write_data(&ftdic, &bytesToSend, 1);
	if (ret < 0) {
		galinette_setError(ERR_FLASH_FTDIWRITE,
		         ftdi_get_error_string(&ftdic));
		return ERR_FLASH_FTDIWRITE;
	}

	usleep(1);

	return 0;
}

// private
// Returns 0 if ok, <0 if an error occurred
int sendPacket(unsigned char *packetCRC, unsigned long *totalCRC, const unsigned char *buf, unsigned char bytesToSend) {
	int i, ret=0;
	*packetCRC = 0;

	for( i=0 ; i<bytesToSend ; i++ ) {
		ret = ftdi_write_data(&ftdic, (unsigned char *) buf+i, 1);
		if ( ret < 0 ) {
			galinette_setError(ERR_FLASH_FTDIWRITE,
			         ftdi_get_error_string(&ftdic));
			return ERR_FLASH_FTDIWRITE;
		}
		else {
			*packetCRC ^= buf[i];
			*totalCRC  += buf[i];
		}
	}

	return 0;
}

// private
// Returns 0 if ok, <0 if an error occurred
int sendCRC(unsigned char *packetCRC) {
	int ret;
	ret = ftdi_write_data(&ftdic, packetCRC, 1);
//DEBUG	printf("DEBUG: packet and crc sent. Crc=%d\n", *packetCRC);
	usleep(1);
	if ( ret < 0 ) {
		galinette_setError(ERR_FLASH_FTDIWRITE,
		         ftdi_get_error_string(&ftdic));
		return ERR_FLASH_FTDIWRITE;
	}
	return 0;
}


// private
// Returns 0 if ok, <0 if an error occurred
int waitForMsg(unsigned char *c) {
	int ret;
	for(ret=0 ; ret==0 ;)	
		ret = ftdi_read_data(&ftdic, c, 1);

	if ( ret < 0 ) {
		galinette_setError(ERR_FLASH_FTDIREAD,
		         ftdi_get_error_string(&ftdic));
		return ERR_FLASH_FTDIREAD;
	}

//DEBUG	printf("DEBUG: received '%x'\n", *c);
	return 0;
}


// public
// Returns 0 if ok, <0 if an error occurred
int galinette_flash(const unsigned char *firmwarebuf, long filelen) {
	int ret, bytesSent=0;
	unsigned char bytesToSend=PACKETLEN, ack;

	unsigned char packetCRC;
	unsigned long totalCRC=0;
	
	waitForStartFlashMsg();
	
	// Let's start flashing
//DEBUG	printf("DEBUG: start flashing\n");
	// Sending packets until whole firmware file is sent
	while(bytesSent < filelen) {
//DEBUG		printf("DEBUG: %d/%ld (sent/toSend)\n", bytesSent, filelen);
		if (filelen - bytesSent < bytesToSend)
			bytesToSend = filelen - bytesSent;
		
		ret = waitForReadyToReceiveMsg();
		if (ret < 0) goto flasherr;

		ret = sendPacketLen(bytesToSend);
		if (ret < 0) goto flasherr;

		ret = sendPacket(&packetCRC, &totalCRC, firmwarebuf+bytesSent, bytesToSend);
		if (ret < 0) goto flasherr;
		
		ret = sendCRC(&packetCRC);
		if (ret < 0) goto flasherr;

		// Waiting for ack or nack
		ret = waitForMsg(&ack);
		if (ret < 0) goto flasherr;
		
		if( ack == 0xcc ) 
			bytesSent += bytesToSend;
		// Official NAK is 0xcc, but we resend with
		// every non-ack value

		if (progressCallback)
			progressCallback(0, filelen, bytesSent, "Flashing, don't disconnect the device !");
	}
	
//DEBUG	printf("DEBUG: sending EOT\n");
	// Everything is done, send the termination char
	ack = 0;
	ret = ftdi_write_data(&ftdic, &ack, 1);
	if ( ret < 0 ) {
		galinette_setError(ERR_FLASH_FTDIWRITE,
		         ftdi_get_error_string(&ftdic));
		goto flasherr;
	}

	if (progressCallback)
		progressCallback(0, filelen, bytesSent, "Done !");
	
//DEBUG	printf("DEBUG: Complete CRC : %ld\n", totalCRC);
	return 0;
flasherr:
	galinette_error();
	return galinette_getErrnum();
}


void galinette_setProgressCallback(void (*yourCallback)(unsigned long, unsigned long, unsigned long, const char *)) {
	progressCallback = yourCallback;
}
