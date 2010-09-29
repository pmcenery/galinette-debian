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
#include <ftdi.h>
#include <stdio.h>
#include <string.h>

short hasBeenInit=0, isConnected=0, deviceNumber=0;
struct ftdi_context      ftdic;
struct ftdi_device_list *devlist=NULL;


void freedevicelist();
int connectByFtdiDev(struct usb_device *dev);
int confdevice();


short galinette_detect() {
	int ret;
	
	if ( ! hasBeenInit ) {
		ftdi_init(&ftdic);
		hasBeenInit=1;
	}

	freedevicelist();

	ret = ftdi_usb_find_all(&ftdic, &devlist, 0x0403, 0x6001);
	
	if ( ret <= 0 ) {
		switch(ret) {
			case 0:
				galinette_setError(ERR_NO_FTDI_DEVICE_FOUND,
				         "Device detection : No ftdi device found");
				return 0;
				break;
			case -1:
				galinette_setError(ERR_USB_FIND_BUS_FAILED,
				         "Device detection : usb_find_busses() failed");
				break;
			case -2:
				galinette_setError(ERR_USB_FIND_DEVICE_FAILED,
				         "Device detection : usb_find_devices() failed");
				break;
			case -3:
				galinette_setError(ERR_OUT_OF_MEMORY,
				         "Device detection : Out of memory");
				break;
			default:
				galinette_setError(ERR_UNKNOWN,
				         "Device detection : Unknown error");
				break;
		}
		return galinette_getErrnum();
	}

	deviceNumber = ret;
	return ret;
}



int galinette_connect() {
	return galinette_connectByNumber(0);
}

int galinette_connectByNumber(short devnum) {
	int i;
	struct ftdi_device_list *tmpdevlist=devlist;
	
	if (devnum<0 || devnum>=deviceNumber) {
		galinette_setError(ERR_UNABLE_TO_CONFIGURE_DEVICE,
		         "Device connect : unable to find desired device - buggy !");
		return ERR_UNABLE_TO_CONFIGURE_DEVICE;
	}
	
	for(i=0 ; i<devnum && devlist ; i++)
		tmpdevlist = tmpdevlist->next;

	if (i != devnum || devlist == NULL) {
		galinette_setError(ERR_UNABLE_TO_CONFIGURE_DEVICE,
		         "Device connect : unable to find desired device - buggy !");
		return ERR_UNABLE_TO_CONFIGURE_DEVICE;
	}

	return connectByFtdiDev(tmpdevlist->dev);
}


int galinette_connectBySerial(const char *serial, int seriallen) {
	char buf[seriallen+1];
	struct ftdi_device_list *tmpdevlist=devlist;
	int isFound = 0;

	
	while(tmpdevlist && !isFound) {
		ftdi_usb_get_strings(&ftdic, tmpdevlist->dev, NULL, 0, NULL, 0, buf, seriallen+1);

		if (strncmp(serial, buf, seriallen) == 0)
			isFound = 1;
		else
			tmpdevlist = tmpdevlist->next;
	}

	if (!isFound) {
		galinette_setError(ERR_DEVICE_SERIAL_NOT_FOUND,
		         "Device connect : serial not found");
		return ERR_DEVICE_SERIAL_NOT_FOUND;
	}

	return connectByFtdiDev(tmpdevlist->dev);
}


// private
int connectByFtdiDev(struct usb_device *dev) {
	int ret;

	if ( ! hasBeenInit ) {
		ftdi_init(&ftdic);
		hasBeenInit=1;
	}

	if (devlist == NULL) {
		galinette_setError(ERR_DEVICE_NO_DETECTION_BEFORE_OPEN,
		         "Device connect : you must detect device before trying to open it");
		return ERR_DEVICE_NO_DETECTION_BEFORE_OPEN;
	}

	
	// Device connection
	ret = ftdi_usb_open_dev(&ftdic, dev);
	if ( ret != 0 ) {
		switch(ret) {
			case -3:
				galinette_setError(ERR_UNABLE_TO_CONFIGURE_DEVICE,
				         "Device connect : unable to config device");
				break;
			case -4:
				galinette_setError(ERR_UNABLE_TO_OPEN_DEVICE,
				         "Device connect : unable to open device");
				break;
			case -5:
				galinette_setError(ERR_UNABLE_TO_CLAIM_DEVICE,
				         "Device connect : unable to claim device");
				break;
			case -6:
				galinette_setError(ERR_RESET_FAILED,
				         "Device connect : reset failed");
				break;
			case -7:
				galinette_setError(ERR_SET_BAUDRATE_FAILED,
				         "Device connect : set baudrate failed");
				break;
			default:
				galinette_setError(ERR_UNKNOWN,
				         "Device connect : Unknown error");
				break;
			
		}
		return galinette_getErrnum();
	}

	freedevicelist();
	isConnected = 1;
	
	if ( confdevice() < 0 )
		galinette_error();
	
	return 0;
}



// private
int confdevice() {
	int ret;

	if ( ! isConnected ) {
		galinette_setError(ERR_DEVICE_NOT_CONNECTED,
		         "Device conf : Device not connected");
		return ERR_DEVICE_NOT_CONNECTED;
	}
	
	ret = ftdi_set_baudrate(&ftdic, 115200);
	if ( ret != 0 ) {
		switch(ret) {
			case -1:
				galinette_setError(ERR_INVALID_BAUDRATE,
				         "Device conf : invalid baudrate");
				break;
			case -2:
				galinette_setError(ERR_SET_BAUDRATE_FAILED,
				         "Device conf : setting baudrate failed");
				break;
			default:
				galinette_setError(ERR_UNKNOWN,
				         "Device conf : set baudrate unknown error");
				break;
		}
		return galinette_getErrnum();
	}
	
	ret = ftdi_set_line_property(&ftdic, BITS_8, STOP_BIT_1, NONE);
	if ( ret != 0 ) {
		galinette_setError(ERR_SET_LINE_PROPERTY_FAILED,
		         "Device conf : set line properties failed");
		return ERR_SET_LINE_PROPERTY_FAILED;
	}

	ret = ftdi_setflowctrl(&ftdic, SIO_DISABLE_FLOW_CTRL);
	if ( ret != 0 ) {
		galinette_setError(ERR_SET_FLOW_CONTROL_FAILED,
		         "Device conf : set flow control failed");
		return ERR_SET_FLOW_CONTROL_FAILED;
	}
	
	return 0;
}


int galinette_disconnect() {
	int ret;

	freedevicelist();
	
	if ( ! isConnected ) {
		galinette_setError(ERR_DEVICE_NOT_CONNECTED,
		         "Device disconnect : Device not connected");
		return ERR_DEVICE_NOT_CONNECTED;
	}
	
	ret = ftdi_usb_close(&ftdic);
	if ( ret != 0 ) {
		switch(ret) {
			case -1:
				galinette_setError(ERR_INVALID_BAUDRATE,
				         "Device disconnect : usb_release failed");
				break;
			case -2:
				galinette_setError(ERR_SET_BAUDRATE_FAILED,
				         "Device disconnect : usb_close failed");
				break;
			default:
				galinette_setError(ERR_UNKNOWN,
				         "Device disconnect : unknown error");
				break;
		}
		return galinette_getErrnum();
	}

	isConnected = 0;

	
	if ( ! hasBeenInit ) {
		galinette_setError(ERR_DEVICE_NOT_INITIALIZED,
		         "Device disconnect : Device not connected");
		return ERR_DEVICE_NOT_INITIALIZED;
	}

	ftdi_deinit(&ftdic);
	hasBeenInit = 0;
	
	return 0;
}


// private
void freedevicelist()
{
	if (devlist) {
		ftdi_list_free(&devlist);
		devlist = NULL;
	}

	deviceNumber = 0;
}
