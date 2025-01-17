//
// sdcard_driver - SD card Fat filesystem access routines
//
// Copyright (c) 2015-2022, Donald N North
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// DONALD N NORTH BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Donald N North shall not be
// used in advertising or otherwise to promote the sale, use or other dealings
// in this Software without prior written authorization from Donald N North.
//
// 24 Aug 2016 - donorth - Initial code
//



//
// includes
//
#include "my_project.h"
#include "sdcard_driver.h"



// PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE
// PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE
// PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE



//
// private data
//
static HardwareSerial *debugPort = NULL; // debug serial output

static uint8_t debugLevel = 0; // debug level, 0=NONE, higher for more

static File sdfile; // SD card file data structure

static uint8_t initOk = FALSE; // set to TRUE on initialize OK



#ifdef USE_TIMELIB_H
//
// date/time callback function
//
void dateTime (uint16_t *dateptr, uint16_t *timeptr)
{
    // get current time
    time_t t = now();

    // return date using FAT_DATE(yyyy,mm,dd) macro to format fields
    *dateptr = FAT_DATE(year(t), month(t), day(t));

    // return time using FAT_TIME(hh,mm,ss) macro to format fields
    *timeptr = FAT_TIME(hour(t), minute(t), second(t));

    // and done
    return;
}
#endif // USE_TIMELIB_H



// PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC
// PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC
// PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC



//
// public data
//

SdFat sdcard; // SD card filesystem data structure



//
// initialize the SDcard subsystem
//
uint8_t sd_initialize (void)
{
    // configure pins
    pinMode(PIN_SD_CD, INPUT);
    pinMode(PIN_SD_MISO, INPUT);
    pinMode(PIN_SD_MOSI, OUTPUT);
    pinMode(PIN_SD_SCK, OUTPUT);
    pinMode(PIN_SD_CS_L, OUTPUT);

    // set default values
    digitalWrite(PIN_SD_MOSI, HIGH);
    digitalWrite(PIN_SD_SCK, HIGH);
    digitalWrite(PIN_SD_CS_L, HIGH);

    // start out not ok
    initOk = FALSE;

    // check for card detect
    if (PIN_SD_CD > 0)
// Sparkfun board is active high, the socket on the Pro Embed board is active low    
#if USE_PRO_EMBED_BOARD    
      if (digitalRead(PIN_SD_CD)) return initOk;
#else
      if (!digitalRead(PIN_SD_CD)) return initOk;
#endif
      

    // init card, check for success
    if (!sdcard.begin(PIN_SD_CS_L, SPI_FULL_SPEED)) { sdcard.initErrorHalt(); return initOk; }

    // some card parameters
    uint8_t ctype = sdcard.card()->type();
#if SD_FAT_VERSION <= 20007
    uint32_t csize = ((sdcard.card()->cardSize()/1024L)*512L)/1024L;
#else
    uint32_t csize = ((sdcard.card()->sectorCount()/1024L)*512L)/1024L;
#endif
    if (debugLevel) {
        debugPort->printf(F("SD: libVersion=%d.%d.%d\n"), SD_FAT_VERSION/10000, (SD_FAT_VERSION/100)%100, SD_FAT_VERSION%100);
        debugPort->printf(F("SD: cardType=SD%d\n"), ctype);
        debugPort->printf(F("SD: cardSize=%luMB\n"), csize);
    }

#if SD_FAT_VERSION < 20000
    // check card is open - it should be FAT16 or FAT32
    if (!sdcard.vwd()->isOpen()) return initOk;
#endif

    // some volume parameters
    uint8_t vtype = sdcard.vol()->fatType();
#if SD_FAT_VERSION <= 20007
    uint32_t vbpc = sdcard.vol()->blocksPerCluster()*512L;
#else
    uint32_t vbpc = sdcard.vol()->bytesPerCluster();
#endif
    uint32_t vucc = sdcard.vol()->clusterCount();
    uint32_t vsize = (vbpc/1024L)*(vucc/1024L);
    if (debugLevel) {
        debugPort->printf(F("SD: volType=FAT%d\n"), vtype);
        debugPort->printf(F("SD: volBytesPerCluster=%lu\n"), vbpc);
        debugPort->printf(F("SD: volClusterCount=%lu\n"), vucc);
        debugPort->printf(F("SD: volSizeBytes=%luMB\n"), vsize);
    }

#ifdef USE_TIMELIB_H
    // set date time callback function
    SdFile::dateTimeCallback(dateTime);
#endif // USE_TIMELIB_H

    // success!
    initOk = TRUE;

    // list files
    sd_list_files(debugPort);

    // and done
    return initOk;
}



//
// setup the SDcard subsystem debug port
//
void sd_debug (HardwareSerial *serialPort, uint8_t level)
{
    // setup which serial port to use
    debugPort = serialPort;

    // and the debug level
    debugLevel = level;

    // and done
    return;
}



//
// list files on the SDcard
//
void sd_list_files (HardwareSerial *port)
{
    if (initOk) sdcard.ls(port, LS_DATE | LS_SIZE | LS_R);
    return;
}



//
// remove a file on the SDcard
//
uint8_t sd_remove_file (char *name)
{
    if (!initOk) return FALSE;

    if (sdcard.exists(name)) return sdcard.remove(name);

    return FALSE;
}



//
// check that file extension matches supplied string, return true/false
//
uint8_t sd_file_ext_matches (char *name, const char *ext)
{
    // get ptr to the last filename dot, or NULL
    char *ptr = strrchr(name, '.');

    // if no extension, return false; else return TRUE if extension matches string
    return ptr == NULL ? FALSE : (strcasecmp(ptr, ext) == 0 ? TRUE : FALSE);
}



//
// read bytes from an sdcard file
//
uint16_t sd_read_bytes (char *name, uint32_t pos, uint8_t *buf, uint16_t len)
{
    uint16_t num;

    // error if not setup
    if (!initOk) return 0;

    // close existing file, if open
    if (sdfile.isOpen()) sdfile.close();

    // open current file
    sdfile = sdcard.open(name, FILE_READ);

    // seek to required position
    if (sdfile.seek(pos) == false) { sdfile.close(); return 0; }

    // read requested number of bytes
    num = sdfile.read(buf, len);

    // close the file, done for now
    sdfile.close();

    // and return number of bytes read
    return num;
}



//
// write bytes to an sdcard file
//
uint16_t sd_write_bytes (char *name, uint32_t pos, uint8_t *buf, uint16_t len)
{
    uint16_t num;

    // error if not setup
    if (!initOk) return 0;

    // close existing file, if open
    if (sdfile.isOpen()) sdfile.close();

    // open current file
    sdfile = sdcard.open(name, FILE_WRITE);

    // seek to required position
    if (sdfile.seek(pos) == false) { sdfile.close(); return 0; }

    // write requested number of bytes
    num = sdfile.write(buf, len);

    // close the file, done for now
    sdfile.close();

    // and return number of bytes written
    return num;
}



//
// get size of an sdcard file
//
uint32_t sd_get_file_size (char *name)
{
    uint32_t size;

    // error if not setup
    if (!initOk) return 0;

    // missing files have zero size
    if (!sdcard.exists(name)) return 0;

    // close existing file, if open
    if (sdfile.isOpen()) sdfile.close();

    // open current file
    sdfile = sdcard.open(name, FILE_WRITE);

    // get file size
    size = sdfile.size();

    // close the file, done for now
    sdfile.close();

    // return the size
    return size;
}



//
// set size of an sdcard file AND fill with zeroes; start at begin (mode=0) or end (mode=1)
//
uint32_t sd_set_file_size (char *name, uint32_t size, uint8_t mode)
{
    void    *buf;
    uint16_t len;
    uint32_t pos;

    // error if not setup
    if (!initOk) return 0;

    // close existing file, if open
    if (sdfile.isOpen()) sdfile.close();

    // get a buffer
    for (len = 1024; len > 0; len >>= 1) { if ((buf = malloc(len)) != NULL) break; }

    // fail if no memory
    if (len == 0 || buf == NULL) return 0;

    // zero the buffer
    memset(buf, 0x00, len);

    // open current file
    sdfile = sdcard.open(name, FILE_WRITE);

    // fill file with data, start at begin (mode=0) or at end (mod=1)
    for (pos = (mode == SD_POS_AT_END ? sdfile.size() : 0); pos < size; pos += len) { sdfile.write(buf, len); }

    // free buffer
    free(buf);

    // truncate file if too large
    sdfile.truncate(size);

    // close the file, done for now
    sdfile.close();

    // return the size
    return sd_get_file_size(name);
}



// the end
