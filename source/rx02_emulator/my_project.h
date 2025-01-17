//
// project definitions
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

#ifndef my_project_h
#define my_project_h

#define USE_ORIGINAL_BOARD 0
#define USE_LCD_BOARD 1
#define USE_PRO_EMBED_BOARD 0

#define USE_LCD_MENU 1


//
// configuration
//
// use:  https://github.com/PaulStoffregen/Time  TimeLib.h if defined (uncomment next line)
//#define USE_TIMELIB_H
//
// use:  https://github.com/greiman/SdFat  SdFat.h if defined (uncomment next line)
#define USE_SDFAT_H



//
// includes
//
#include <Arduino.h>
#include <HardwareSerial.h>
//
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
//
#ifdef USE_SDFAT_H
#include <SPI.h>
#include <SdFat.h>
#endif // USE_SDFAT_H
//
#ifdef USE_TIMELIB_H
#include <TimeLib.h>
#endif // USE_TIMELIB_H

#if USE_LCD_MENU
#include <Wire.h>
#include <menu.h>
#include <menuIO/liquidCrystalOut.h>
#include <menuIO/keyIn.h>
#endif


//
// local type definitions
#include "my_types.h"



//
// standard analog pins 0..5
//
// NONE



//
// standard digital pins 0.. 13,  extra mega digital pins 14..53
//
#if USE_LCD_BOARD
const byte PIN_CTLR_RUN_H      =   2; // PE4 input, interrupt
const byte PIN_CTLR_INIT_H     =   3; // PE5 input, interrupt
//
const byte PIN_CTLR_DONE_H     = 40; // PG1 output
const byte PIN_CTLR_ERROR_H    = 36; // PC1 output
const byte PIN_CTLR_12BIT_H    = 28; // PA6 input
const byte PIN_CTLR_TR_RQST_H  = 38; // PD7 output
const byte PIN_CTLR_DATAO_H    = 42; // PL7 output
const byte PIN_CTLR_DATAI_H    = 30; // PC7 input
const byte PIN_CTLR_OUT_H      = 46; // PL3 output
const byte PIN_CTLR_SHIFT_H    = 44; // PL5 output
const byte PIN_CTLR_ACLO_H     = 48; // PL1 output
const byte PIN_CTLR_DMA_MODE_L = 24; // PA2 input
//
const byte PIN_SD_CD           = 0; // set to zero if not available
#endif

#if USE_PRO_EMBED_BOARD
const byte PIN_CTLR_RUN_H      =  2; // PE4 input, interrupt
const byte PIN_CTLR_INIT_H     =  3; // PE5 input, interrupt
//
const byte PIN_CTLR_DONE_H     =  8; // PH5 output
const byte PIN_CTLR_ERROR_H    =  4; // PG5 output
const byte PIN_CTLR_12BIT_H    = 39; // PG2 input
const byte PIN_CTLR_TR_RQST_H  =  6; // PH3 output
const byte PIN_CTLR_DATAO_H    = 32; // PC5 output
const byte PIN_CTLR_DATAI_H    = 41; // PC0 input
const byte PIN_CTLR_OUT_H      = 36; // PC1 output
const byte PIN_CTLR_SHIFT_H    = 34; // PC3 output
const byte PIN_CTLR_ACLO_H     = 38; // PD7 output
const byte PIN_CTLR_DMA_MODE_L = 37; // PC0 input
//
const byte PIN_SD_CD           = 49; // PL0 card detect input, active low
#endif

#if USE_ORIGINAL_BOARD
const byte PIN_CTLR_RUN_H      =  2; // PE4 input, interrupt
const byte PIN_CTLR_INIT_H     =  3; // PE5 input, interrupt
//
const byte PIN_CTLR_DONE_H     = 28; // PA6 output
const byte PIN_CTLR_ERROR_H    = 29; // PA7 output
const byte PIN_CTLR_12BIT_H    = 30; // PC7 input
const byte PIN_CTLR_TR_RQST_H  = 31; // PC6 output
const byte PIN_CTLR_DATAO_H    = 32; // PC5 output
const byte PIN_CTLR_DATAI_H    = 33; // PC4 input
const byte PIN_CTLR_OUT_H      = 34; // PC3 output
const byte PIN_CTLR_SHIFT_H    = 35; // PC2 output
const byte PIN_CTLR_ACLO_H     = 36; // PC1 output
const byte PIN_CTLR_DMA_MODE_L = 37; // PC0 input
//
const byte PIN_SD_CD           = 49; // PL0 card detect input, active high
#endif

//SD Card pins are the same across all board variants
const byte PIN_SD_MISO         = 50; // PB3 card MISO/DO input, active high
const byte PIN_SD_MOSI         = 51; // PB2 card MOSI/DI output, active high
const byte PIN_SD_SCK          = 52; // PB1 card SCK output, active high
const byte PIN_SD_CS_L         = 53; // PB0 chip select output, active low//

#if USE_LCD_BOARD
const byte PIN_LED1            = 58; // A4, output, LED1, active high
const byte PIN_LED2            = 59; // A5, output, LED2, active high
const byte PIN_LED3            = 60; // A6, output, LED3, active high
#endif

#if USE_PRO_EMBED_BOARD
const byte PIN_LED1            = 48; // D48, output, LED1, active high
const byte PIN_LED2            = 28; // D28, output, LED2, active high
const byte PIN_LED3            = 30; // D30, output, LED3, active high
#endif

#if USE_ORIGINAL_BOARD
const byte PIN_LED1            = 10; // PB4 output, LED1, active high
const byte PIN_LED2            = 11; // PB5 output, LED2, active high
const byte PIN_LED3            = 12; // PB6 output, LED3, active high
#endif

const byte PIN_LEDY            = 13; // PB7 output, onboard yellow LED, active high//

#if USE_LCD_MENU
const byte PIN_BUTTON_MENU     = 55; // A1, Pin for Menu/Esc digital button.
const byte PIN_BUTTON_UP       = 56; // A2, Pin for Up digital button.
const byte PIN_BUTTON_DOWN     = 54; // A0, Pin for Down digital button.
const byte PIN_BUTTON_ENTER    = 57; // A3, Pin for Enter digital button.
#endif

#if USE_PRO_EMBED_BOARD
const byte PIN_BUTTON_REINIT   = 46; // D46, Pin for ReINIT digital button
#endif



#endif // my_project_h

// the end
