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
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


void progressbar(unsigned long min, unsigned long max, unsigned long val, const char *comment);
void clearLine();
void myErrFunc(int errnum, const char *errmsg);

int main(int argc, char **argv) {
	int ret;
	const unsigned char *buf;
	long filelen;

	int opt, sflag=0;
	char *svalue=NULL, *firmware;

	opterr = 0;
     
	while ((opt = getopt (argc, argv, "s:")) != -1) {
		switch (opt) {
			case 's':
				sflag = 1;
				svalue = optarg;
				break;
			case '?':
				if (optopt == 's')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				return -1;
			default:
				return -2;
		}
	}


	galinette_setErrorCallback(myErrFunc);
	
	if ( (!sflag && argc != 2 ) || (sflag && argc != 4) ) {
		galinette_setError(0, "Usage : galinette-flash [-s usbDeviceSerial] firmware.b1b");
		galinette_error();
	}
	
	ret = galinette_detect();
	
	if ( ret <= 0 )
		galinette_error();

	if ( ret > 1) {
		printf("%d devices found, you must specify the USB serial of the device you want to flash (option -s)", ret);
		return -1;
	}

	if (sflag) {
		if ( galinette_connectBySerial(svalue, strlen(svalue)) < 0 )
			galinette_error();
		firmware = argv[3];
	}
	else {
		if ( galinette_connect() < 0 )
			galinette_error();
		firmware = argv[1];
	}

	if ( galinette_loadFirmware(firmware) < 0 )
		galinette_error();
	
	if ( galinette_checkFirmwareIntegrity() != 0 )
		galinette_error();
	
	buf = galinette_getFirmwareData(&filelen);

	galinette_setProgressCallback(progressbar);

	if ( galinette_flash(buf, filelen) < 0 )
		galinette_error();

	clearLine();

	if ( galinette_disconnect() < 0 )
		galinette_error();

	return 0;
}


void clearLine()
{
	printf("\r%.80s", "");
}



void progressbar(unsigned long min, unsigned long max, unsigned long val, const char *comment)
{
	static unsigned short counter=0;	
	short percent = 100*(((float)val+0.5-(float)min)/(float)max-(float)min);
	int i;
	char pgbar[12];
	char *anim = "-\\|/";
	
	for(i=0 ; i<percent/10 && i<11 ; i++)
		pgbar[i]='=';
	pgbar[i]='>';
	pgbar[i+1]='\0';

	printf("\r%c %3d%% [%-10.10s] %-60.60s", anim[counter%4], percent, pgbar, comment);
	
	fflush(stdout);

	counter++;	
}


void myErrFunc(int errnum, const char *errmsg)
{
	fprintf( stderr, "Error : %s\n", errmsg);
	exit(errnum);
}
