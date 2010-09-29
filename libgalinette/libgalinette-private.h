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

/***************************************
 *            private const            *
 ***************************************/
#define FIRMWARE_HEADERSIZE               64
#define FIRMWARE_HEADERCRCLEN             4
#define FIRMWARE_HEADERCRCPOS             60
#define FIRMWARE_MAXLEN                   128*1024

/***************************************
 *         private prototypes          *
 ***************************************/


// Error

/***************************************
 *          private globals            *
 ***************************************/
extern struct ftdi_context ftdic;
