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
 *            public const             *
 ***************************************/

// Errors from firwmare
#define ERR_FIRMWARE_FAILED_TO_OPEN           -0x10
#define ERR_FIRMWARE_UNABLE_TO_GET_FILESIZE   -0x11
#define ERR_FIRMWARE_FILE_IS_TOO_SHORT        -0x12
#define ERR_FIRMWARE_FILE_IS_TOO_LONG         -0x13
#define ERR_FIRMWARE_MALLOC                   -0x14
#define ERR_FIRMWARE_READ                     -0x15
#define ERR_FIRMWARE_NOTLOADED                -0x16
#define ERR_FIRMWARE_CORRUPTED                -0x17

// Errors from device
#define ERR_NO_FTDI_DEVICE_FOUND              -0x20
#define ERR_USB_FIND_BUS_FAILED               -0x21
#define ERR_USB_FIND_DEVICE_FAILED            -0x22
#define ERR_OUT_OF_MEMORY                     -0x23
#define ERR_UNKNOWN                           -0x24
#define ERR_UNABLE_TO_CONFIGURE_DEVICE        -0x25
#define ERR_UNABLE_TO_OPEN_DEVICE             -0x26
#define ERR_UNABLE_TO_CLAIM_DEVICE            -0x27
#define ERR_RESET_FAILED                      -0x28
#define ERR_SET_BAUDRATE_FAILED               -0x29
#define ERR_INVALID_BAUDRATE                  -0x2A
#define ERR_SET_LINE_PROPERTY_FAILED          -0x2B
#define ERR_SET_FLOW_CONTROL_FAILED           -0x2C
#define ERR_DEVICE_NOT_CONNECTED              -0x2D
#define ERR_DEVICE_NOT_INITIALIZED            -0x2E
#define ERR_DEVICE_NO_DETECTION_BEFORE_OPEN   -0x2F
#define ERR_DEVICE_SERIAL_NOT_FOUND           -0x30

// Errors from flash
#define ERR_FLASH_FTDIREAD                    -0x40
#define ERR_FLASH_FTDIWRITE                   -0x41
#define ERR_FLASH_FTDIPURGERX                 -0x42

// ????????????????
#define PACKETLEN                              128



/***************************************
 *         public prototypes           *
 ***************************************/


// Error
int galinette_getErrnum();
const char *galinette_getErrMsg();

/** Set error num and error message without calling error func
  * @param errnum error number
  * @param errmsg error message
  */
void galinette_setError(int errnum, char *errmsg);

/** Call error function (galinette_setError should have been called before) */
void galinette_error();

/** Allow you to override default error func
  * @param yourErrFunc : your own error function
  */
void galinette_setErrorCallback( void (*yourErrFunc)(int, const char*) );





// Firmware file access

/** Load the firmware file specified in memory
  * @param path the path to the firmware file to load
  * @return the firmware data size or -1 in case of error
  */
long galinette_loadFirmware(char *path);

/** Check integrity of the current firmware in memory
  * @return 0 if valid, 1 if invalid, -1 if an error occured
  */
int  galinette_checkFirmwareIntegrity();

/** Return a read-only pointer on the firmware data
  * @param dataLen a long where the data Len will be stored by the func
  * @return the data, or NULL if there's no firwmare loaded
  */
const unsigned char *galinette_getFirmwareData(long *dataLen);

/** Free memory allocated for firmware.
  * Call this function once you're done with firmware
  */
void galinette_freeFirmwareMem();








int galinette_flash(const unsigned char *firmwarebuf, long filelen);
void galinette_setProgressCallback(void (*yourCallback)(unsigned long, unsigned long, unsigned long, const char *));





// Device access

/** Detect all ftdi devices connected on the system.
  * You have to call it before trying to open any device.
  * @return >0 : the number of devices connected
  *          0 : if no device detected (error is set)
  *         <0 : if an error occurred
  */
short galinette_detect();

/** Connect to the first device found on the system.
  * @return  0 if everything ok
  *         <0 if an error occurred
  */
int galinette_connect();

/** Connect to one device on the system, by a device index
  * @param devnum device index, between 0 and the return value of detect() - 1
  * @return  0 if everything ok
  *         <0 if an error occurred
  */
int galinette_connectByNumber(short devnum);

/** Connect to one device on the system, by a the device serial
  * @param serial device serial
  * @param seriallen number of char in serial
  * @return  0 if everything ok
  *         <0 if an error occurred
  */
int galinette_connectBySerial(const char *serial, int seriallen);

/** Disconnect from the device and release all memory used for detection and connection
  * @return  0 if everything ok
  *         <0 if an error occurred
  */
int galinette_disconnect();
