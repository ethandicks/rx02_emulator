#include "lcd_menu.h"
#include "rx02_driver.h"

LiquidCrystal lcd(RS, EN, 10, 9, 8, 7);

SDMenuT<CachedFSO<SdFat,32>> filePickMenu(sdcard,"Select File...","/",filePick,enterEvent);

static char filename_buffer[32];

//implementing the handler here after filePick is defined...
result filePick(eventMask event, navNode& nav, prompt &item) {
  // switch(event) {//for now events are filtered only for enter, so we dont need this checking
  //   case enterCmd:
      if (nav.root->navFocus==(navTarget*)&filePickMenu) {
        filePickMenu.selectedFile.toCharArray(filename_buffer, 32);
        
        tty->printf(F("Selected file: %s\n"), filename_buffer);
        tty->printf(F("From folder: %s\n"), filePickMenu.selectedFolder);

        rx_unit_file(0, filename_buffer);
      }
  //     break;
  // }
  return proceed;
}


result initEmulator(eventMask e) {
  tty->printf(F("Sending INIT ...\n"));
  rx_initialize(false);
  tty->printf(F("... INIT complete\n"));
  return quit;
}

result saveSETUP(eventMask e) {
  setup_write(setup_filename);
  return quit;
}

bool unit0_RW=false;
TOGGLE(unit0_RW,unit0_setRW,"Read-Only: ",doNothing,noEvent,wrapStyle//,doExit,enterEvent,noStyle
  ,VALUE("Off",false,doNothing,noEvent)
  ,VALUE("On",true,doNothing,noEvent)
);

int unit0_Mode=2;
SELECT(unit0_Mode,sel_unit0_Mode,"Mode",doNothing,noEvent,wrapStyle
  ,VALUE("RX01+RX11",0,doNothing,noEvent)
  ,VALUE("RX01+RX8E",1,doNothing,noEvent)
  ,VALUE("RX02+any",2,doNothing,noEvent)
);

int unit0_Debug=1;
SELECT(unit0_Debug,sel_unit0_Debug,"Debug",doNothing,noEvent,wrapStyle
  ,VALUE("None",0,doNothing,noEvent)
  ,VALUE("Normal",1,doNothing,noEvent)
  ,VALUE("More",2,doNothing,noEvent)
  ,VALUE("Max",3,doNothing,noEvent)
);

int unit0_Timing=0;
SELECT(unit0_Timing,sel_unit0_Timing,"Timing",doNothing,noEvent,wrapStyle
  ,VALUE("Fast",0,doNothing,noEvent)
  ,VALUE("Medium",1,doNothing,noEvent)
  ,VALUE("Normal",2,doNothing,noEvent)
);

MENU(setupUnit0,"Setup Unit0",doNothing,anyEvent,noTitle
  ,SUBMENU(filePickMenu)
  ,SUBMENU(unit0_setRW)
  ,SUBMENU(sel_unit0_Mode)
  ,SUBMENU(sel_unit0_Debug)
  ,SUBMENU(sel_unit0_Timing)
//  ,EXIT("<Back")
);

bool unit1_RW=false;
TOGGLE(unit1_RW,unit1_setRW,"Read-Only: ",doNothing,noEvent,wrapStyle//,doExit,enterEvent,noStyle
  ,VALUE("Off",false,doNothing,noEvent)
  ,VALUE("On",true,doNothing,noEvent)
);

int unit1_Mode=2;
SELECT(unit1_Mode,sel_unit1_Mode,"Mode",doNothing,noEvent,wrapStyle
  ,VALUE("RX01+RX11",0,doNothing,noEvent)
  ,VALUE("RX01+RX8E",1,doNothing,noEvent)
  ,VALUE("RX02+any",2,doNothing,noEvent)
);

int unit1_Debug=1;
SELECT(unit1_Debug,sel_unit1_Debug,"Debug",doNothing,noEvent,wrapStyle
  ,VALUE("None",0,doNothing,noEvent)
  ,VALUE("Normal",1,doNothing,noEvent)
  ,VALUE("More",2,doNothing,noEvent)
  ,VALUE("Max",3,doNothing,noEvent)
);

int unit1_Timing=0;
SELECT(unit1_Timing,sel_unit1_Timing,"Timing",doNothing,noEvent,wrapStyle
  ,VALUE("Fast",0,doNothing,noEvent)
  ,VALUE("Medium",1,doNothing,noEvent)
  ,VALUE("Normal",2,doNothing,noEvent)
);

MENU(setupUnit1,"Setup Unit1",doNothing,anyEvent,noTitle
  ,SUBMENU(unit1_setRW)
  ,SUBMENU(sel_unit1_Mode)
  ,SUBMENU(sel_unit1_Debug)
  ,SUBMENU(sel_unit1_Timing)
//  ,EXIT("<Back")
);

MENU(mainMenu,"Main menu",doNothing,noEvent,noTitle
  ,SUBMENU(setupUnit0)
  ,SUBMENU(setupUnit1)
  ,OP("INIT Emulator",initEmulator,enterEvent)
  ,OP("Save SETUP.INI",saveSETUP,enterEvent)
//  ,EXIT("<Back")
);



MENU_OUTPUTS(out, MAX_DEPTH
  ,LIQUIDCRYSTAL_OUT(lcd,{0,0,16,2})
  ,NONE
);

chainStream<0> in(NULL);//<-- this creates a NULL stream

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

/*
result alert(menuOut& o,idleEvent e) {
  if (e==idling) {
    o.setCursor(0,0);
    o.print("alert test");
    o.setCursor(0,1);
    o.print("[select] to continue...");
  }
  return proceed;
}

result doAlert(eventMask e, prompt &item) {
  nav.idleOn(alert);
  return proceed;
}
*/

void show_currently_loaded() {
  lcd.setCursor(0, 0);
  lcd.printf("0:%s", rx_unit_file(0));
  lcd.setCursor(0, 1);
  lcd.printf("1:%s", rx_unit_file(1));  
}

result menu_idle(menuOut& o,idleEvent e) {

  show_currently_loaded();
  
  switch(e) {
    case idleStart:
      //o.println("suspending menu!");
      break;
    case idling:
      //o.println("suspended...");
      break;
    case idleEnd:
      //o.println("resuming menu.");
      //nav.reset();
      break;
  }
  return proceed;
}

void lcd_menu_setup() {
  nav.idleTask=menu_idle;//point a function to be used when menu is suspended
  filePickMenu.begin();//need this after sd begin
  nav.idleOn();
  show_currently_loaded();
}
