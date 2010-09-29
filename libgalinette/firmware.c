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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


// Private func
long getfilesize(int fd);


// Private globals
unsigned char *header=NULL, *data=NULL;
long filesize=0;
long datasize=0;





//public
long galinette_loadFirmware(char *path)
{
	long ret;
	int fd=-1;

	if (path)
		fd = open(path, O_RDONLY);
	
	if (fd < 0) {
		galinette_setError(ERR_FIRMWARE_FAILED_TO_OPEN,
                         "Failed to open firmware file");
		return -1;
	}

	filesize = getfilesize(fd);
	
	if (filesize < 0)
		goto loadFirmwareErr;
	
	if (filesize < FIRMWARE_HEADERSIZE+1) {
		galinette_setError(ERR_FIRMWARE_FILE_IS_TOO_SHORT,
                         "Firmware file size is too short");
		goto loadFirmwareErr;
	}
	
	if (filesize > FIRMWARE_MAXLEN) {
		galinette_setError(ERR_FIRMWARE_FILE_IS_TOO_LONG,
                         "Firmware size is too long");
		goto loadFirmwareErr;
	}
	
	if (header)
		free(header);

	header = malloc(filesize);
	if (header == NULL) {
		galinette_setError(ERR_FIRMWARE_MALLOC,
                         "Firmware malloc error");
		goto loadFirmwareErr;
	}
	

	ret = read(fd, header, filesize);
	if (ret != filesize) {
		galinette_setError(ERR_FIRMWARE_READ,
                         "Firmware read error");
		goto loadFirmwareErr;
	}
	
	datasize = filesize - FIRMWARE_HEADERSIZE;
	data     = header   + FIRMWARE_HEADERSIZE;

	close(fd);
	
	
	return datasize;

   loadFirmwareErr:

	galinette_freeFirmwareMem();
	if (fd >= 0)
		close(fd);

	return -1;
}


// public
int galinette_checkFirmwareIntegrity()
{
	int i;
	unsigned char crc[FIRMWARE_HEADERCRCLEN];

	if (header == NULL) {
		galinette_setError(ERR_FIRMWARE_NOTLOADED,
		         "Trying to check firwmare integrity without any firmware loaded");
		return ERR_FIRMWARE_NOTLOADED;
	}

	// calculate the crc for our file...
	
	memset(crc, 0, FIRMWARE_HEADERCRCLEN);
	for(i=0 ; i<datasize ; i++)
		crc[i%FIRMWARE_HEADERCRCLEN] ^= data[i];
	
	if (memcmp(header+FIRMWARE_HEADERCRCPOS, crc, FIRMWARE_HEADERCRCLEN)) {
		galinette_setError(ERR_FIRMWARE_CORRUPTED,
		         "Corrupted firmware file");
		return ERR_FIRMWARE_CORRUPTED;
	}

	return 0;
}


// public
const unsigned char *galinette_getFirmwareData(long *dataLen)
{
	if ( header && dataLen ) {
		*dataLen = datasize;
		return data;
	}
	
	return NULL;
}

// public
void galinette_freeFirmwareMem()
{
	if (header)
		free(header);
	header   = NULL;
	data     = NULL;
	filesize = 0;
	datasize = 0;
}


// private
long getfilesize(int fd)
{
	int ret;
	struct stat stbuf;
	
	ret = fstat(fd, &stbuf);
	if (ret < 0) {
		galinette_setError(ERR_FIRMWARE_UNABLE_TO_GET_FILESIZE,
		         "Failed to get file size");
		return ERR_FIRMWARE_UNABLE_TO_GET_FILESIZE;
	}

	return stbuf.st_size;
}
