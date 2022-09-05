#include <SPI.h>
#include <SdFat.h>
#include <menu.h>
#include <menuIO/liquidCrystalOut.h>
#include <menuIO/chainStream.h>
#include <plugin/SdFatMenu.h>

extern SdFat sdcard; // SDcard access state

extern HardwareSerial *tty;
extern char setup_filename[];
extern void setup_write (char *name);

using namespace Menu;

#define MAX_DEPTH 3

// LCD /////////////////////////////////////////
#define RS 12 
#define EN 11

//function to handle file select
// declared here and implemented bellow because we need
// to give it as event handler for `filePickMenu`
// and we also need to refer to `filePickMenu` inside the function
result filePick(eventMask event, navNode& nav, prompt &item);
