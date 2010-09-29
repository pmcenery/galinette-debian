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
#include <stdlib.h>

// Private func
void defaultErrFunc(int errnum, const char *errmsg);



// Private globals
char  *errMsg                     = "Everything is ok";
int    errNum                     = 0;
void (*errFunc)(int, const char*) = &defaultErrFunc;





int galinette_getErrnum()
{
	return errNum;
}


const char *galinette_getErrMsg()
{
	return errMsg;
}


void galinette_setError(int errnum, char *errmsg)
{
	errMsg = errmsg;
	errNum = errnum;
}


void galinette_error()
{
	errFunc(errNum, errMsg);
}


void galinette_setErrorCallback( void (*yourErrFunc)(int, const char*) )
{
	if ( yourErrFunc )
		errFunc = yourErrFunc;
}


void defaultErrFunc(int errnum, const char *errmsg)
{
	fprintf( stderr, "Error #%d : '%s'\n", errnum, errmsg);
	exit(errnum);
}
