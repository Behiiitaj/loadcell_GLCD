// ============================================================
// Example:     LCDML: graphic display with u8g
// ============================================================
// Author:      Jomelo
// Last update: 21.01.2018
// License:     MIT
// ============================================================
// Description:
// This example shows how to use the u8glib with the LCDMenuLib
// The menu can placed in a box that can be placed anywhere on
// the screen.
// ============================================================
// *********************************************************************
// special settings
// *********************************************************************
// enable this line when you are not usigng a standard arduino
// for example when your chip is an ESP or a STM or SAM or something else
//#define _LCDML_cfg_use_ram 

  // include libs
  #include <LCDMenuLib2.h>
  #include "LCDMLdisp.h"
  #include "../loadcell/loadcell.h"
  
  #include "../flash/rsem.h"
  // U8g2lib
  #include <Arduino.h>
  #include <U8g2lib.h>

  #ifdef U8X8_HAVE_HW_SPI
  #include <SPI.h>
  #endif
  #ifdef U8X8_HAVE_HW_I2C
  #include <Wire.h>
  #endif


extern SettingStruct settings;
extern InputStruct inputs;
extern LoadCellStruct loadCell;
extern HX711 scale;
extern FunctionStruct functions;

// HX711 circuit wiring
float loadCellValue = 0;  // time counter (global variable)

  
#include <stm32f1xx_hal_spi.h>
SPI_HandleTypeDef SpiHandle;
const char* units[6]= { "gm", "kg", "ton", "N","N.m","N.kg" };
const float ratioList[7]= { 1000, 100, 10, 1 , 0.1 , 0.01 , 0.001 };



//// *********************************************************************
//// U8GLIB
//// *********************************************************************
//  // U8g2 Constructor List (Frame Buffer)
//  // The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
//  // Please update the pin numbers according to your setup. Use U8X8_PIN_NONE if the reset pin is not connected
U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R2, /* CS=*/ PA4, /* reset=*/ PA8);     // (MEGA, ...
// U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R2, /* clock=*/ PA5, /* data=*/ PA7, /* CS=*/ PA4, /* reset=*/ PA8);
//  //U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R0, /* CS=*/ 12, /* reset=*/ U8X8_PIN_NONE);   // (Uno and co

  // settings for u8g lib and LCD
  #define _LCDML_DISP_w                 128            // LCD width
  #define _LCDML_DISP_h                 64             // LCD height
  // font settings
  #define _LCDML_DISP_font              u8g_font_6x13  // u8glib font (more fonts under u8g.h line 1520 ...)
  #define _LCDML_DISP_font_w            6              // font width
  #define _LCDML_DISP_font_h            13             // font height
  // cursor settings
  #define _LCDML_DISP_cursor_char       ">"            // cursor char
  #define _LCDML_DISP_cur_space_before  2              // cursor space between
  #define _LCDML_DISP_cur_space_behind  4              // cursor space between
  // menu position and size
  #define _LCDML_DISP_box_x0            0              // start point (x0, y0)
  #define _LCDML_DISP_box_y0            0              // start point (x0, y0)
  #define _LCDML_DISP_box_x1            128            // width x  (x0 + width)
  #define _LCDML_DISP_box_y1            64             // hight y  (y0 + height)
  #define _LCDML_DISP_draw_frame        1              // draw a box around the menu
   // scrollbar width
  #define _LCDML_DISP_scrollbar_w       6  // scrollbar width (if this value is < 3, the scrollbar is disabled)

  // nothing change here
  #define _LCDML_DISP_cols_max          ((_LCDML_DISP_box_x1-_LCDML_DISP_box_x0)/_LCDML_DISP_font_w)
  #define _LCDML_DISP_rows_max          ((_LCDML_DISP_box_y1-_LCDML_DISP_box_y0-((_LCDML_DISP_box_y1-_LCDML_DISP_box_y0)/_LCDML_DISP_font_h))/_LCDML_DISP_font_h)

  // rows and cols
  // when you use more rows or cols as allowed change in LCDMenuLib.h the define "_LCDML_DISP_cfg_max_rows" and "_LCDML_DISP_cfg_max_string_length"
  // the program needs more ram with this changes
  #define _LCDML_DISP_rows              _LCDML_DISP_rows_max  // max rows
  #define _LCDML_DISP_cols              20                   // max cols


// *********************************************************************
// Prototypes
// *********************************************************************
void lcdml_menu_display();
void lcdml_menu_clear();
void lcdml_menu_control();

boolean COND_hide();  // hide a menu element

void mFunc_scale(uint8_t param);
void mFunc_tare(uint8_t param);
void mFunc_timer_info(uint8_t param);
void mFunc_p2(uint8_t param);
void mFunc_screensaver(uint8_t param);
void mFunc_settingsPassword(uint8_t param);
void mFunc_welcomePage(uint8_t param);
void mFunc_back(uint8_t param);
void mFunc_goToRootMenu(uint8_t param);
void mFunc_jumpTo_timer_info(uint8_t param);
void mFunc_unit(uint8_t param);
void mFunc_ratio(uint8_t param);
void mFunc_coefficent(uint8_t param);
void mFunc_hysteresis(uint8_t param);
void mFunc_settings(uint8_t param);
void mFunc_highSetpoint(uint8_t param);
void mFunc_lowSetpoint(uint8_t param);
void mfunc_lastTime(uint8_t param);

void mFunc_dosingHighTime(uint8_t param);
void mFunc_dosingLowTime(uint8_t param);
void mFunc_dosingCycleCount(uint8_t param);
void mFunc_dosingOutputNumber(uint8_t param);
void mFunc_testFeed(uint8_t param);
void mFunc_startupTare(uint8_t param);
void mFunc_deadTare(uint8_t param);

void mFunc_password(uint8_t param);
void mFunc_resetFactory(uint8_t param);
void mfunc_avgCount(uint8_t param);
void mfunc_zeroFilter(uint8_t param);
void mFunc_Functions(uint8_t param);
void lcdml_menu_clear();
void lcdml_menu_display();
void mDyn_para(uint8_t line);
void lcdml_menu_control(void);

// *********************************************************************
// Objects
// *********************************************************************
  LCDMenuLib2_menu LCDML_0 (255, 0, 0, NULL, NULL); // root menu element (do not change)
  LCDMenuLib2 LCDML(LCDML_0, _LCDML_DISP_rows, _LCDML_DISP_cols, lcdml_menu_display, lcdml_menu_clear, lcdml_menu_control);


// *********************************************************************
// LCDML MENU/DISP
// *********************************************************************
  LCDML_add         (0  , LCDML_0         , 1  , "Settings"              , mFunc_settingsPassword);                    


  LCDML_addAdvanced (1  , LCDML_0         , 2  , COND_hide,  "Settings_SIKIM"     , NULL,          0,   _LCDML_TYPE_default); 
  LCDML_add         (2  , LCDML_0_2       , 1  , "Tare"                  , mFunc_tare);        
  LCDML_add         (3  , LCDML_0_2       , 2  , "Scale"                 , mFunc_scale);
  LCDML_addAdvanced (4  , LCDML_0_2       , 3  , NULL, "Unit"            , mFunc_unit,           1, _LCDML_TYPE_default);                 
  LCDML_addAdvanced (5  , LCDML_0_2       , 4  , NULL, "Ratio"           , mFunc_ratio,          settings.ratio, _LCDML_TYPE_default);
  LCDML_addAdvanced (6  , LCDML_0_2       , 5  , NULL, "Coefficent"      , mFunc_coefficent,     settings.unit, _LCDML_TYPE_default);
  LCDML_add         (7  , LCDML_0_2       , 6  , "Functions"             , NULL);  
  LCDML_addAdvanced (8  , LCDML_0_2       , 7  , COND_hide,  "1"         , mFunc_settings,                      0,_LCDML_TYPE_default);
  LCDML_add         (9  , LCDML_0_2_6     , 1  , "Current function"      , mFunc_Functions);
  LCDML_add         (10 , LCDML_0_2_6     , 2  , "Filling"               , NULL);                     
  LCDML_addAdvanced (11 , LCDML_0_2_6_2   , 1  , NULL, "High point"      , mFunc_highSetpoint,   settings.setPointHigh, _LCDML_TYPE_default);                     
  LCDML_addAdvanced (12 , LCDML_0_2_6_2   , 2  , NULL, "Low point"       , mFunc_lowSetpoint,    settings.setPointLow, _LCDML_TYPE_default);              
  LCDML_addAdvanced (13 , LCDML_0_2_6_2   , 3  , NULL, "Hysteresis"      , mFunc_hysteresis,     settings.Hysteresis, _LCDML_TYPE_default);
  LCDML_add         (14 , LCDML_0_2_6     , 3  , "Dosing"                , NULL);  
  LCDML_addAdvanced (15 , LCDML_0_2_6_3   , 1  , NULL, "Test feed"       , mFunc_testFeed,   settings.setPointHigh, _LCDML_TYPE_default);  
  LCDML_addAdvanced (16 , LCDML_0_2_6_3   , 2  , NULL, "High time"       , mFunc_dosingHighTime, settings.dosingFunction.highTime, _LCDML_TYPE_default);                     
  LCDML_addAdvanced (17 , LCDML_0_2_6_3   , 3  , NULL, "Low time"        , mFunc_dosingLowTime,    settings.setPointLow, _LCDML_TYPE_default); 
  LCDML_addAdvanced (18 , LCDML_0_2_6_3   , 4  , NULL, "Cycle count"     , mFunc_dosingCycleCount,     settings.Hysteresis, _LCDML_TYPE_default);
  LCDML_addAdvanced (19 , LCDML_0_2_6_3   , 5  , NULL, "Last time"       , mfunc_lastTime,     settings.dosingFunction.lastTime, _LCDML_TYPE_default);
  LCDML_addAdvanced (20 , LCDML_0_2_6_3   , 6  , NULL, "Output number pin" , mFunc_dosingOutputNumber,     settings.Hysteresis, _LCDML_TYPE_default);
  LCDML_add         (21 , LCDML_0_2       , 8  , "Filteration"           , NULL);  
  LCDML_addAdvanced (22 , LCDML_0_2_8     , 1  , NULL, "Avg count"       , mfunc_avgCount,     50, _LCDML_TYPE_default);
  LCDML_addAdvanced (23 , LCDML_0_2_8     , 2  , NULL, "Zero filter"     , mfunc_zeroFilter,     50, _LCDML_TYPE_default);
  LCDML_addAdvanced (24 , LCDML_0_2       , 9  , NULL, "Change password" , mFunc_password,       settings.password, _LCDML_TYPE_default);  
  LCDML_addAdvanced (25 , LCDML_0_2       , 10 , NULL, "Dead offset"     , mFunc_deadTare,   settings.startupTare, _LCDML_TYPE_default);
  LCDML_addAdvanced (26 , LCDML_0_2       , 11 , NULL, "Startup tare"    , mFunc_startupTare,      settings.deadTare, _LCDML_TYPE_default);
  LCDML_addAdvanced (27 , LCDML_0_2       , 12 , NULL, "Reset factory"   , mFunc_resetFactory,   settings.password, _LCDML_TYPE_default);  


  LCDML_addAdvanced (28 , LCDML_0         , 3  , COND_hide,  "screensaver"        , mFunc_screensaver,        0,   _LCDML_TYPE_default);     
  LCDML_addAdvanced (29 , LCDML_0         , 4  , COND_hide,  "welcomePage"        , mFunc_welcomePage,        0,   _LCDML_TYPE_default);


  #define _LCDML_DISP_cnt    29
  LCDML_createMenu(_LCDML_DISP_cnt);





// *********************************************************************
// SETUP
// *********************************************************************
  void lcd_setup()
  {
    u8g2.begin();

    Serial.begin(9600);                // start serial
    Serial.println(F(_LCDML_VERSION)); // only for examples
    LCDML_setup(_LCDML_DISP_cnt);

    LCDML.MENU_enRollover();
    LCDML.SCREEN_enable(mFunc_screensaver, 10000); 
    LCDML.OTHER_jumpToFunc(mFunc_welcomePage); 
  }

// *********************************************************************
// LOOP
// *********************************************************************
  void lcd_loop()
  {
    LCDML.loop();
  }

// *********************************************************************
// check some errors - do not change here anything
// *********************************************************************
# if(_LCDML_glcd_tft_box_x1 > _LCDML_glcd_tft_w)
# error _LCDML_glcd_tft_box_x1 is to big
# endif

# if(_LCDML_glcd_tft_box_y1 > _LCDML_glcd_tft_h)
# error _LCDML_glcd_tft_box_y1 is to big
# endif

// *********************************************************************
boolean COND_hide()  // hide a menu element
// ********************************************************************* 
{ 
  return false;  // hidden
} 


#define _LCDML_CONTROL_cfg      2


// *********************************************************************
// *************** (2) CONTROL OVER DIGITAL PINS ***********************
// *********************************************************************
#if(_LCDML_CONTROL_cfg == 2)
// settings
  unsigned long g_LCDML_DISP_press_time = 0;
  unsigned long g_LCDML_CONTROL_button_press_time = millis();
  bool  g_LCDML_CONTROL_button_prev       = HIGH;

  #define _LCDML_CONTROL_digital_low_active      0   
  #define _LCDML_CONTROL_digital_enable_quit     1
  #define _LCDML_CONTROL_digital_enable_lr       0
  #define _LCDML_CONTROL_digital_enter           PA2
  #define _LCDML_CONTROL_digital_up              PA1
  #define _LCDML_CONTROL_digital_down            PA0
  #define _LCDML_CONTROL_digital_quit            PA3
  //#define _LCDML_CONTROL_digital_left            12
  //#define _LCDML_CONTROL_digital_right           13
// *********************************************************************
void lcdml_menu_control(void)
{
  bool g_LCDML_button                      = digitalRead(PA3);
  // If something must init, put in in the setup condition
  if(LCDML.BT_setup()) {
    // runs only once
    // init buttons
    pinMode(_LCDML_CONTROL_digital_enter      , INPUT_PULLDOWN);
    pinMode(_LCDML_CONTROL_digital_up         , INPUT_PULLDOWN);
    pinMode(_LCDML_CONTROL_digital_down       , INPUT_PULLDOWN);
    # if(_LCDML_CONTROL_digital_enable_quit == 1)
      pinMode(_LCDML_CONTROL_digital_quit     , INPUT_PULLDOWN);
    # endif
    # if(_LCDML_CONTROL_digital_enable_lr == 1)
      pinMode(_LCDML_CONTROL_digital_left     , INPUT_PULLUP);
      pinMode(_LCDML_CONTROL_digital_right    , INPUT_PULLUP);
    # endif
  }

  #if(_LCDML_CONTROL_digital_low_active == 1)
  #  define _LCDML_CONTROL_digital_a !
  #else
  #  define _LCDML_CONTROL_digital_a
  #endif

  uint8_t but_stat = 0x00;

  bitWrite(but_stat, 0, _LCDML_CONTROL_digital_a(digitalRead(_LCDML_CONTROL_digital_enter)));
  bitWrite(but_stat, 1, _LCDML_CONTROL_digital_a(digitalRead(_LCDML_CONTROL_digital_up)));
  bitWrite(but_stat, 2, _LCDML_CONTROL_digital_a(digitalRead(_LCDML_CONTROL_digital_down)));
  #if(_LCDML_CONTROL_digital_enable_quit == 1)
  bitWrite(but_stat, 3, _LCDML_CONTROL_digital_a(digitalRead(_LCDML_CONTROL_digital_quit)));
  #endif
  #if(_LCDML_CONTROL_digital_enable_lr == 1)
  bitWrite(but_stat, 4, _LCDML_CONTROL_digital_a(digitalRead(_LCDML_CONTROL_digital_left)));
  bitWrite(but_stat, 5, _LCDML_CONTROL_digital_a(digitalRead(_LCDML_CONTROL_digital_right)));
  #endif

  if (but_stat > 0) {
    if((millis() - g_LCDML_DISP_press_time) >= 220) {
      g_LCDML_DISP_press_time = millis(); // reset press time

      if (bitRead(but_stat, 0)) { LCDML.BT_enter(); digitalWrite(PB7,HIGH); delay(75); digitalWrite(PB7,LOW);}
      if (bitRead(but_stat, 1)) { LCDML.BT_up();   digitalWrite(PB7,HIGH); delay(75); digitalWrite(PB7,LOW); }
      if (bitRead(but_stat, 2)) { LCDML.BT_down();  digitalWrite(PB7,HIGH); delay(75); digitalWrite(PB7,LOW);}
      if (bitRead(but_stat, 3)) { LCDML.BT_quit(); digitalWrite(PB7,HIGH); delay(75); digitalWrite(PB7,LOW); }
      if (bitRead(but_stat, 4)) { LCDML.BT_left();  digitalWrite(PB7,HIGH); delay(75); digitalWrite(PB7,LOW);}
      if (bitRead(but_stat, 5)) { LCDML.BT_right(); digitalWrite(PB7,HIGH); delay(75); digitalWrite(PB7,LOW); }
    }
  }
}
// *********************************************************************
// ******************************* END *********************************
// *********************************************************************



#else
  #error _LCDML_CONTROL_cfg is not defined or not in range
#endif


/* ===================================================================== *
 *                                                                       *
 * Dynamic content                                                       *
 *                                                                       *
 * ===================================================================== *
 */


uint8_t g_dynParam = 100; // when this value comes from an EEPROM, load it in setup
                          // at the moment here is no setup function (To-Do)
void mDyn_para(uint8_t line)
// *********************************************************************
{
  // check if this function is active (cursor stands on this line)
  if (line == LCDML.MENU_getCursorPos())
  {
    // make only an action when the cursor stands on this menu item
    //check Button
    if(LCDML.BT_checkAny())
    {
      if(LCDML.BT_checkEnter())
      {
        // this function checks returns the scroll disable status (0 = menu scrolling enabled, 1 = menu scrolling disabled)
        if(LCDML.MENU_getScrollDisableStatus() == 0)
        {
          // disable the menu scroll function to catch the cursor on this point
          // now it is possible to work with BT_checkUp and BT_checkDown in this function
          // this function can only be called in a menu, not in a menu function
          LCDML.MENU_disScroll();
        }
        else
        {
          // enable the normal menu scroll function
          LCDML.MENU_enScroll();
        }

        // do something
        // ...
        
        LCDML.BT_resetEnter();
      }

      // This check have only an effect when MENU_disScroll is set
      if(LCDML.BT_checkUp())
      {
        g_dynParam++;
        LCDML.BT_resetUp();
      }

      // This check have only an effect when MENU_disScroll is set
      if(LCDML.BT_checkDown())
      {
        g_dynParam--;
        LCDML.BT_resetDown();
      }


      if(LCDML.BT_checkLeft())
      {
        g_dynParam++;
        LCDML.BT_resetLeft();
      }
      
      if(LCDML.BT_checkRight())
      {
        g_dynParam--;
        LCDML.BT_resetRight();
      }
    }
  }

  char buf[20];
  sprintf (buf, "dynValue: %d", g_dynParam);

  // setup function
  u8g2.drawStr( _LCDML_DISP_box_x0+_LCDML_DISP_font_w + _LCDML_DISP_cur_space_behind,  (_LCDML_DISP_font_h * (1+line)), buf);     // the value can be changed with left or right

}
// =====================================================================
//
// Output function
//
// =====================================================================

/* ******************************************************************** */
void lcdml_menu_clear()
/* ******************************************************************** */
{
}

/* ******************************************************************** */
void lcdml_menu_display()
/* ******************************************************************** */
{
  // for first test set font here
  u8g2.setFont(_LCDML_DISP_font);

  // declaration of some variables
  // ***************
  // content variable
  char content_text[_LCDML_DISP_cols];  // save the content text of every menu element
  // menu element object
  LCDMenuLib2_menu *tmp;
  // some limit values
  uint8_t i = LCDML.MENU_getScroll();
  uint8_t maxi = _LCDML_DISP_rows + i;
  uint8_t n = 0;

   // init vars
  uint8_t n_max             = (LCDML.MENU_getChilds() >= _LCDML_DISP_rows) ? _LCDML_DISP_rows : (LCDML.MENU_getChilds());

  uint8_t scrollbar_min     = 0;
  uint8_t scrollbar_max     = LCDML.MENU_getChilds();
  uint8_t scrollbar_cur_pos = LCDML.MENU_getCursorPosAbs();
  uint8_t scroll_pos        = ((1.*n_max * _LCDML_DISP_rows) / (scrollbar_max - 1) * scrollbar_cur_pos);

  // generate content
  u8g2.firstPage();
  do {


    n = 0;
    i = LCDML.MENU_getScroll();
    // update content
    // ***************

      // clear menu
      // ***************

    // check if this element has children
    if ((tmp = LCDML.MENU_getDisplayedObj()) != NULL)
    {
      // loop to display lines
      do
      {
        // check if a menu element has a condition and if the condition be true
        if (tmp->checkCondition())
        {
          // check the type off a menu element
          if(tmp->checkType_menu() == true)
          {
            // display normal content
            LCDML_getContent(content_text, tmp->getID());
            u8g2.drawStr( _LCDML_DISP_box_x0+_LCDML_DISP_font_w + _LCDML_DISP_cur_space_behind, _LCDML_DISP_box_y0 + _LCDML_DISP_font_h * (n + 1), content_text);
          }
          else
          {
            if(tmp->checkType_dynParam()) {
              tmp->callback(n);
            }
          }
          // increment some values
          i++;
          n++;
        }
      // try to go to the next sibling and check the number of displayed rows
      } while (((tmp = tmp->getSibling(1)) != NULL) && (i < maxi));
    }

    // set cursor
    u8g2.drawStr( _LCDML_DISP_box_x0+_LCDML_DISP_cur_space_before, _LCDML_DISP_box_y0 + _LCDML_DISP_font_h * (LCDML.MENU_getCursorPos() + 1),  _LCDML_DISP_cursor_char);

    if(_LCDML_DISP_draw_frame == 1) {
       u8g2.drawFrame(_LCDML_DISP_box_x0, _LCDML_DISP_box_y0, (_LCDML_DISP_box_x1-_LCDML_DISP_box_x0), (_LCDML_DISP_box_y1-_LCDML_DISP_box_y0));
    }

    // display scrollbar when more content as rows available and with > 2
    if (scrollbar_max > n_max && _LCDML_DISP_scrollbar_w > 2)
    {
      // set frame for scrollbar
      u8g2.drawFrame(_LCDML_DISP_box_x1 - _LCDML_DISP_scrollbar_w, _LCDML_DISP_box_y0, _LCDML_DISP_scrollbar_w, _LCDML_DISP_box_y1-_LCDML_DISP_box_y0);

      // calculate scrollbar length
      uint8_t scrollbar_block_length = scrollbar_max - n_max;
      scrollbar_block_length = (_LCDML_DISP_box_y1-_LCDML_DISP_box_y0) / (scrollbar_block_length + _LCDML_DISP_rows);

      //set scrollbar
      if (scrollbar_cur_pos == 0) {                                   // top position     (min)
        u8g2.drawBox(_LCDML_DISP_box_x1 - (_LCDML_DISP_scrollbar_w-1), _LCDML_DISP_box_y0 + 1                                                     , (_LCDML_DISP_scrollbar_w-2)  , scrollbar_block_length);
      }
      else if (scrollbar_cur_pos == (scrollbar_max-1)) {            // bottom position  (max)
        u8g2.drawBox(_LCDML_DISP_box_x1 - (_LCDML_DISP_scrollbar_w-1), _LCDML_DISP_box_y1 - scrollbar_block_length                                , (_LCDML_DISP_scrollbar_w-2)  , scrollbar_block_length);
      }
      else {                                                                // between top and bottom
        u8g2.drawBox(_LCDML_DISP_box_x1 - (_LCDML_DISP_scrollbar_w-1), _LCDML_DISP_box_y0 + (scrollbar_block_length * scrollbar_cur_pos + 1),(_LCDML_DISP_scrollbar_w-2)  , scrollbar_block_length);
      }
    }
  } while ( u8g2.nextPage() );
}



const char* functionOptions[2]= { "Filling","Dosing" };
int currentNumberFunction = 0;
bool isEnterFunction;
// *********************************************************************
void mFunc_Functions(uint8_t param)
// *********************************************************************
{
  isEnterFunction = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    LCDML_UNUSED(param);
    currentNumberFunction = settings.currentFunction;
    LCDML.FUNC_setLoopInterval(1);
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkDown())
      {
        if (currentNumberFunction == 0) currentNumberFunction =1;
        else currentNumberFunction = 0;
      }
      if (LCDML.BT_checkQuit())
      {
        LCDML.FUNC_goBackToMenu(3);
      }
      if(LCDML.BT_checkEnter())
      {
        isEnterFunction = true;
      }
      u8g2.firstPage();
    do {
        // u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,7);
        u8g2.drawRFrame(27,25,73,20,7);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Select function.");
        
        u8g2.setFont(u8g_font_8x13Br);

        u8g2.drawStr(35,39,functionOptions[currentNumberFunction]);


        if (isEnterFunction)
        {
          settings.currentFunction = currentNumberFunction;
          memoryWriteSetting();
          LCDML.FUNC_goBackToMenu();
        }

    } while( u8g2.nextPage() );
  }


  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}


// *********************************************************************
void mFunc_testFeed(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          
  {
    LCDML_UNUSED(param);
    int dosingOutputNumber,fullTimeoutputNumber;
    if (settings.dosingFunction.outputNumber==0)
    {
      dosingOutputNumber = PB10;
      fullTimeoutputNumber = PB2;
    }
    else if (settings.dosingFunction.outputNumber==1)
    {
      dosingOutputNumber = PB2;
      fullTimeoutputNumber = PB10;
    }
    for (int i = 0; i < 3; i++)
    {
      functions.currentCounterNumber = i+1;
      functions.dosingHigh = true;
      digitalWrite(dosingOutputNumber,LOW);
      delay(settings.dosingFunction.highTime);
      functions.dosingHigh = false;
      digitalWrite(dosingOutputNumber,HIGH);
      delay(settings.dosingFunction.lowTime);
    }
    functions.functionActive = false;
    digitalWrite(PB2,HIGH);
    digitalWrite(PB10,HIGH);
    LCDML.FUNC_goBackToMenu();
  }
}

const char* dosingOutputOptions[2]= { "Output 1","Output 2" };
int currentNumberdosingOutput = 0;
bool isEnterdosingOutput;
// *********************************************************************
void mFunc_dosingOutputNumber(uint8_t param)
// *********************************************************************
{
  isEnterdosingOutput = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    LCDML_UNUSED(param);
    currentNumberdosingOutput = settings.dosingFunction.outputNumber;
    LCDML.FUNC_setLoopInterval(1);
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkDown())
      {
        if (currentNumberdosingOutput == 0) currentNumberdosingOutput =1;
        else currentNumberdosingOutput = 0;
      }
      if (LCDML.BT_checkQuit())
      {
        LCDML.FUNC_goBackToMenu(3);
      }
      if(LCDML.BT_checkEnter())
      {
        isEnterdosingOutput = true;
      }
      u8g2.firstPage();
    do {
        
        u8g2.drawRFrame(0,0,128,64,7);
        u8g2.drawRFrame(27,25,80,20,7);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Select output.");
        
        u8g2.setFont(u8g_font_8x13Br);

        u8g2.drawStr(35,39,dosingOutputOptions[currentNumberdosingOutput]);


        if (isEnterdosingOutput)
        {
          settings.dosingFunction.outputNumber = currentNumberdosingOutput;
          memoryWriteSetting();
          LCDML.FUNC_goBackToMenu();
        }

    } while( u8g2.nextPage() );
  }


  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}




const char* resetOptions[2]= { "No","Yes" };
int currentNumberTare = 0;
bool isEnterTare;
// *********************************************************************
void mFunc_tare(uint8_t param)
// *********************************************************************
{
  isEnterTare = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    LCDML_UNUSED(param);
    LCDML.FUNC_setLoopInterval(1);
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkDown())
      {
        if (currentNumberTare == 0) currentNumberTare =1;
        else currentNumberTare = 0;
      }
      if (LCDML.BT_checkQuit())
      {
        LCDML.FUNC_goBackToMenu(3);
      }
      if(LCDML.BT_checkEnter())
      {
        isEnterTare = true;
      }
      u8g2.firstPage();
    do {
        // u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,7);
        u8g2.drawRFrame(37,35,50,20,7);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Tare, Are you sure?");
        
        u8g2.setFont(u8g_font_9x18Br);

        u8g2.drawStr(45,49,resetOptions[currentNumberTare]);


        u8g2.setFont(u8g_font_5x8r);
        u8g2.drawStr(6,26,"Scaled val -> ");
        if (loadCell.isOver)
        {
          u8g2.drawStr(76,26,String("-OVER!-").c_str());
        }
        else
        {
          loadCellValue = loadCell.value; 
          char buf[40];
          // if ((loadCellValue >0 && loadCellValue<settings.zeroFilter) || ((loadCellValue <0 && loadCellValue>(settings.zeroFilter*-1))))
          // {
          //   loadCellValue = 0;
          // }
          dtostrf (loadCellValue,4, 1, buf);
          u8g2.drawStr( 76, 26, buf);
        }


        if (isEnterTare)
        {
          if (currentNumberTare == 1)
          {
            tareLoadCell();
            currentNumberTare = 0;
          }
          // LCDML.OTHER_jumpToFunc(mFunc_settings);
        }

    } while( u8g2.nextPage() );
  }


  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}




int currentNumberUnit = 0;
bool isEnterUnit = false;
// *********************************************************************
void mFunc_unit(uint8_t param)
// *********************************************************************
{
  isEnterUnit = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    
    currentNumberUnit = settings.unit;
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);
    
    
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkDown())
      {
        if (settings.unit == 0) settings.unit = 5;
        else settings.unit --;
      }
      if (LCDML.BT_checkQuit()) {
        if (!isEnterUnit)
        {
          settings.unit = currentNumberUnit;
          LCDML.FUNC_goBackToMenu(2);
        }
        
        }
      if(LCDML.BT_checkEnter())
      {
        isEnterUnit = true;
      }
    do {
        u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,7);
        u8g2.drawRFrame(37,21,55,20,7);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Select your unit.");
        
        u8g2.setFont(u8g_font_9x18Br);

        u8g2.drawStr(45,35,units[settings.unit]);

        if (isEnterUnit)
        {
          memoryWriteSetting();
          LCDML.OTHER_jumpToFunc(mFunc_settings);
        }

    } while( u8g2.nextPage() );
  }


  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}


int currentNumberlastTime = 1;
bool isEnterlastTime = false;
// *********************************************************************
void mfunc_lastTime(uint8_t param)
// *********************************************************************
{
  isEnterlastTime = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    currentNumberlastTime = settings.dosingFunction.lastTime;
    LCDML_UNUSED(param);
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkDown())
      {
        currentNumberlastTime--;
        if (currentNumberlastTime == 0) currentNumberlastTime = 30;
      }
      if (LCDML.BT_checkQuit())
      {
        LCDML.FUNC_goBackToMenu(3);
      }
      if(LCDML.BT_checkEnter())
      {
        isEnterlastTime = true;
      }
    do {
        u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,7);
        u8g2.drawRFrame(37,30,50,20,7);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Enter alive time of");
        u8g2.drawStr(6,20,"output as seconds.");
        
        u8g2.setFont(u8g_font_9x18Br);

        u8g2.drawStr(45,44, String(currentNumberlastTime).c_str());

        if (isEnterlastTime)
        {
          settings.dosingFunction.lastTime = currentNumberlastTime;
          memoryWriteSetting();
          LCDML.FUNC_goBackToMenu();
        }

    } while( u8g2.nextPage() );
  }


  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}


int currentNumberAvgCount = 1;
bool isEnterAvgCount = false;
// *********************************************************************
void mfunc_avgCount(uint8_t param)
// *********************************************************************
{
  isEnterAvgCount = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    currentNumberAvgCount = settings.avgCount;
    LCDML_UNUSED(param);
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkDown())
      {
        currentNumberAvgCount--;
        if (currentNumberAvgCount == 0) currentNumberAvgCount = 15;
      }
      if (LCDML.BT_checkQuit())
      {
        LCDML.FUNC_goBackToMenu(3);
      }
      if(LCDML.BT_checkEnter())
      {
        isEnterAvgCount = true;
      }
    do {
        u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,7);
        u8g2.drawRFrame(37,30,50,20,7);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Enter number of");
        u8g2.drawStr(6,20,"loadCell sampling.");
        
        u8g2.setFont(u8g_font_9x18Br);

        u8g2.drawStr(45,44, String(currentNumberAvgCount).c_str() );

        if (isEnterAvgCount)
        {
          settings.avgCount = currentNumberAvgCount;
          memoryWriteSetting();
          LCDML.OTHER_jumpToFunc(mFunc_settings);
        }

    } while( u8g2.nextPage() );
  }


  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}


int currentNumberZeroFilter = 1;
bool isEnterZeroFilter = false;
// *********************************************************************
void mfunc_zeroFilter(uint8_t param)
// *********************************************************************
{
  isEnterZeroFilter = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    currentNumberZeroFilter = settings.zeroFilter;
    LCDML_UNUSED(param);
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkDown())
      {
        currentNumberZeroFilter--;
        if (currentNumberZeroFilter == 0) currentNumberZeroFilter = 15;
      }
      if (LCDML.BT_checkQuit())
      {
        LCDML.FUNC_goBackToMenu(3);
      }
      if(LCDML.BT_checkEnter())
      {
        isEnterZeroFilter = true;
      }
    do {
        u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,7);
        u8g2.drawRFrame(37,30,50,20,7);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"How much filter");
        u8g2.drawStr(6,20,"zero value.");
        
        u8g2.setFont(u8g_font_9x18Br);

        u8g2.drawStr(45,44, String(currentNumberZeroFilter).c_str() );

        if (isEnterZeroFilter)
        {
          settings.zeroFilter = currentNumberZeroFilter;
          memoryWriteSetting();
          LCDML.OTHER_jumpToFunc(mFunc_settings);
        }

    } while( u8g2.nextPage() );
  }


  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}



int currentNumberStartupTare = 0;
bool isEnterStartupTare = false;

// *********************************************************************
void mFunc_startupTare(uint8_t param)
// *********************************************************************
{
  isEnterStartupTare = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    if (settings.startupTare) currentNumberStartupTare = 1;
    else currentNumberStartupTare = 0;
    
    LCDML_UNUSED(param);
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkDown())
      {
        if (currentNumberStartupTare == 0) currentNumberStartupTare =1;
        else currentNumberStartupTare = 0;
      }
      if (LCDML.BT_checkQuit())
      {
        LCDML.FUNC_goBackToMenu(3);
      }
      if(LCDML.BT_checkEnter())
      {
        isEnterStartupTare = true;
      }
    do {
        u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,7);
        u8g2.drawRFrame(37,21,50,20,7);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Tare on startup?");
        
        u8g2.setFont(u8g_font_9x18Br);

        u8g2.drawStr(45,35,resetOptions[currentNumberStartupTare]);

        if (isEnterStartupTare)
        {
          if (currentNumberStartupTare == 1) settings.startupTare = true;
          else settings.startupTare = false;
          
          memoryWriteSetting();
          LCDML.OTHER_jumpToFunc(mFunc_settings);
        }

    } while( u8g2.nextPage() );
  }


  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}

int currentNumberdeadTare = 0;
bool isEnterdeadTare = false;

// *********************************************************************
void mFunc_deadTare(uint8_t param)
// *********************************************************************
{
  isEnterdeadTare = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    LCDML_UNUSED(param);
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkDown())
      {
        if (currentNumberdeadTare == 0) currentNumberdeadTare =1;
        else currentNumberdeadTare = 0;
      }
      if (LCDML.BT_checkQuit())
      {
        LCDML.FUNC_goBackToMenu(3);
      }
      if(LCDML.BT_checkEnter())
      {
        isEnterdeadTare = true;
      }
    do {
        u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,7);
        u8g2.drawRFrame(37,21,50,20,7);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Set dead offset?");
        
        u8g2.setFont(u8g_font_9x18Br);

        u8g2.drawStr(45,35,resetOptions[currentNumberdeadTare]);

        if (isEnterdeadTare)
        {
          if (currentNumberdeadTare == 1)
          {
            scale.power_up();
            settings.deadTare = scale.read_average(1);
            scale.set_offset(settings.deadTare);
            scale.power_down();
            memoryWriteSetting();
          }
          
          LCDML.FUNC_goBackToMenu();
        }

    } while( u8g2.nextPage() );
  }


  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}

int currentNumberReset = 0;
bool isEnterReset = false;

// *********************************************************************
void mFunc_resetFactory(uint8_t param)
// *********************************************************************
{
  isEnterReset = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    LCDML_UNUSED(param);
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkDown())
      {
        if (currentNumberReset == 0) currentNumberReset =1;
        else currentNumberReset = 0;
      }
      if (LCDML.BT_checkQuit())
      {
        LCDML.FUNC_goBackToMenu(3);
      }
      if(LCDML.BT_checkEnter())
      {
        isEnterReset = true;
      }
    do {
        u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,7);
        u8g2.drawRFrame(37,21,50,20,7);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Reset, Are you sure?");
        
        u8g2.setFont(u8g_font_9x18Br);

        u8g2.drawStr(45,35,resetOptions[currentNumberReset]);

        if (isEnterReset)
        {
          if (currentNumberReset == 1)
          {
            loadDefaultSetting();
            memoryWriteSetting();
          }
          
          LCDML.OTHER_jumpToFunc(mFunc_settings);
        }

    } while( u8g2.nextPage() );
  }


  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}



int currentNumberRatio = 0;
bool isEnterRatio = false;
// *********************************************************************
void mFunc_ratio(uint8_t param)
// *********************************************************************
{
  isEnterRatio = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    currentNumberRatio = settings.ratio;
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);
  }



  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkDown())
      {
        if (settings.ratio == 0) settings.ratio = 6;
        else settings.ratio --;
      }
      if (LCDML.BT_checkQuit()) {
        if (!isEnterRatio)
        {
          settings.ratio = currentNumberRatio;
          LCDML.FUNC_goBackToMenu(2);
        }
        
        }
      if(LCDML.BT_checkEnter())
      {
        isEnterRatio = true;
      }
    do {
        u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,7);
        u8g2.drawRFrame(20,21,87,20,7);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Select your ratio.");
        
        u8g2.setFont(u8g_font_9x18Br);

        char buf[40];
        dtostrf (ratioList[settings.ratio],4, 3, buf);

        u8g2.drawStr(28,35,buf);

        if (isEnterRatio)
        {
          memoryWriteSetting();
          LCDML.OTHER_jumpToFunc(mFunc_settings);
        }

    } while( u8g2.nextPage() );
  }




  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}



int16_t numberpassword1 = 0;
int16_t numberpassword2 = 0;
int16_t numberpassword3 = 0;
int8_t currentNumberpassword = 1;

// *********************************************************************
void mFunc_password(uint8_t param)
// *********************************************************************
{
  char bufNumber1[1];
  char bufNumber2[1];
  char bufNumber3[1];

  int8_t xFont = 45;
  int8_t xAddation = 15;
  int8_t yFont = 37;
  int8_t rectSize = 15;
  int8_t borderRadius = 6;

  bool isEnter = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    double originalNumber = settings.password;
    int integerPart = static_cast<int>(originalNumber);
    numberpassword1 = integerPart / 100;
    numberpassword2 = (integerPart / 10) % 10;
    numberpassword3 = (integerPart / 1) % 10;
    LCDML_UNUSED(param);
  }



  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkUp())
      {
        if (currentNumberpassword==3) currentNumberpassword=1;
        else currentNumberpassword++; 
      }
      if(LCDML.BT_checkDown())
      {
        switch (currentNumberpassword)
          {
          case 1:
            if (numberpassword1 == 0) numberpassword1 = 9;
            else numberpassword1--;
            break;
          case 2:
            if (numberpassword2 == 0) numberpassword2 = 9;
            else numberpassword2--;
            break;
          case 3:
            if (numberpassword3 == 0) numberpassword3 = 9;
            else numberpassword3--;
            break;
          }
      }
      if (LCDML.BT_checkQuit()) LCDML.FUNC_goBackToMenu(1);
      if(LCDML.BT_checkEnter())
      {

        isEnter = true;
      }
    do {
        u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,borderRadius);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Enter new password.");
        
        u8g2.setFont(u8g_font_9x18Br);
        sprintf (bufNumber1, "%d", numberpassword1);
        u8g2.drawStr(xFont,yFont,bufNumber1);
        sprintf (bufNumber2, "%d", numberpassword2);
        u8g2.drawStr(xFont +xAddation,yFont,bufNumber2);
        sprintf (bufNumber3, "%d", numberpassword3);
        u8g2.drawStr(xFont +(xAddation*2),yFont,bufNumber3);

        if (isEnter)
        {
          int result = 
            (numberpassword1 * 100) + 
            (numberpassword2 * 10) + 
            (numberpassword3 * 1);

          settings.password = result;
          memoryWriteSetting();

          LCDML.OTHER_jumpToFunc(mFunc_screensaver);
        }
        switch (currentNumberpassword)
        {
        case 1:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberpassword-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 2:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberpassword-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 3:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberpassword-1)),(25),rectSize,rectSize,borderRadius);
          break;
        }
    } while( u8g2.nextPage() );
  }

  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}

int16_t numberdosingCycleCount1 = 0;
int16_t numberdosingCycleCount2 = 0;
int16_t numberdosingCycleCount3 = 0;
int8_t currentdosingCycleCount = 1;

// *********************************************************************
void mFunc_dosingCycleCount(uint8_t param)
// *********************************************************************
{
  char bufNumber1[1];
  char bufNumber2[1];
  char bufNumber3[1];

  int8_t xFont = 45;
  int8_t xAddation = 15;
  int8_t yFont = 37;
  int8_t rectSize = 15;
  int8_t borderRadius = 6;

  bool isEnter = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    double originalNumber = settings.dosingFunction.cycleCount;
    int integerPart = static_cast<int>(originalNumber);
    numberdosingCycleCount1 = integerPart / 100;
    numberdosingCycleCount2 = (integerPart / 10) % 10;
    numberdosingCycleCount3 = (integerPart / 1) % 10;
    LCDML_UNUSED(param);
  }



  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkUp())
      {
        if (currentdosingCycleCount==3) currentdosingCycleCount=1;
        else currentdosingCycleCount++; 
      }
      if(LCDML.BT_checkDown())
      {
        switch (currentdosingCycleCount)
          {
          case 1:
            if (numberdosingCycleCount1 == 0) numberdosingCycleCount1 = 9;
            else numberdosingCycleCount1--;
            break;
          case 2:
            if (numberdosingCycleCount2 == 0) numberdosingCycleCount2 = 9;
            else numberdosingCycleCount2--;
            break;
          case 3:
            if (numberdosingCycleCount3 == 0) numberdosingCycleCount3 = 9;
            else numberdosingCycleCount3--;
            break;
          }
      }
      if (LCDML.BT_checkQuit()) LCDML.FUNC_goBackToMenu(1);
      if(LCDML.BT_checkEnter())
      {

        isEnter = true;
      }
    do {
        u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,borderRadius);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Enter cycle count.");
        
        u8g2.setFont(u8g_font_9x18Br);
        sprintf (bufNumber1, "%d", numberdosingCycleCount1);
        u8g2.drawStr(xFont,yFont,bufNumber1);
        sprintf (bufNumber2, "%d", numberdosingCycleCount2);
        u8g2.drawStr(xFont +xAddation,yFont,bufNumber2);
        sprintf (bufNumber3, "%d", numberdosingCycleCount3);
        u8g2.drawStr(xFont +(xAddation*2),yFont,bufNumber3);

        if (isEnter)
        {
          int result = 
            (numberdosingCycleCount1 * 100) + 
            (numberdosingCycleCount2 * 10) + 
            (numberdosingCycleCount3 * 1);

          settings.dosingFunction.cycleCount = result;
          memoryWriteSetting();
        }
        switch (currentdosingCycleCount)
        {
        case 1:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentdosingCycleCount-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 2:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentdosingCycleCount-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 3:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentdosingCycleCount-1)),(25),rectSize,rectSize,borderRadius);
          break;
        }
    } while( u8g2.nextPage() );
  }

  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}


int16_t numberdosingHighTime1 = 0;
int16_t numberdosingHighTime2 = 0;
int16_t numberdosingHighTime3 = 0;
int16_t numberdosingHighTime4 = 0;
int16_t numberdosingHighTime5 = 0;
int8_t currentNumberDosingHighTime = 1;

// *********************************************************************
void mFunc_dosingHighTime(uint8_t param)
// *********************************************************************
{
  char bufNumber1[1];
  char bufNumber2[1];
  char bufNumber3[1];
  char bufNumber4[1];
  char bufNumber5[1];

  int8_t xFont = 25;
  int8_t xAddation = 15;
  int8_t yFont = 37;
  int8_t rectSize = 15;
  int8_t borderRadius = 6;

  bool isEnter = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    double originalNumber = settings.dosingFunction.highTime;
    int integerPart = static_cast<int>(originalNumber);
    numberdosingHighTime1 = integerPart / 10000;
    numberdosingHighTime2 = (integerPart / 1000) % 10;
    numberdosingHighTime3 = (integerPart / 100) % 10;
    numberdosingHighTime4 = (integerPart / 10) % 10;
    numberdosingHighTime5 = (integerPart / 1) % 10;
    LCDML_UNUSED(param);
  }



  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkUp())
      {
        if (currentNumberDosingHighTime==5) currentNumberDosingHighTime=1;
        else currentNumberDosingHighTime++; 
      }
      if(LCDML.BT_checkDown())
      {
        switch (currentNumberDosingHighTime)
          {
          case 1:
            if (numberdosingHighTime1 == 0) numberdosingHighTime1 = 9;
            else numberdosingHighTime1--;
            break;
          case 2:
            if (numberdosingHighTime2 == 0) numberdosingHighTime2 = 9;
            else numberdosingHighTime2--;
            break;
          case 3:
            if (numberdosingHighTime3 == 0) numberdosingHighTime3 = 9;
            else numberdosingHighTime3--;
            break;
          case 4:
            if (numberdosingHighTime4 == 0) numberdosingHighTime4 = 9;
            else numberdosingHighTime4--;
            break;
          case 5:
            if (numberdosingHighTime5 == 0) numberdosingHighTime5 = 9;
            else numberdosingHighTime5--;
            break;
          }
      }
      if (LCDML.BT_checkQuit()) LCDML.FUNC_goBackToMenu(1);
      if(LCDML.BT_checkEnter())
      {

        isEnter = true;
      }
    do {
        u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,borderRadius);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Enter high time(ms).");
        
        u8g2.setFont(u8g_font_9x18Br);
        sprintf (bufNumber1, "%d", numberdosingHighTime1);
        u8g2.drawStr(xFont,yFont,bufNumber1);
        sprintf (bufNumber2, "%d", numberdosingHighTime2);
        u8g2.drawStr(xFont +xAddation,yFont,bufNumber2);
        sprintf (bufNumber3, "%d", numberdosingHighTime3);
        u8g2.drawStr(xFont +(xAddation*2),yFont,bufNumber3);
        sprintf (bufNumber4, "%d", numberdosingHighTime4);
        u8g2.drawStr(xFont +(xAddation*3),yFont,bufNumber4);
        sprintf (bufNumber5, "%d", numberdosingHighTime5);
        u8g2.drawStr(xFont +(xAddation*4),yFont,bufNumber5);

        if (isEnter)
        {
          int result = 
            (numberdosingHighTime1 * 10000) + 
            (numberdosingHighTime2 * 1000) + 
            (numberdosingHighTime3 * 100) + 
            (numberdosingHighTime4 * 10) + 
            (numberdosingHighTime5 * 1);

          settings.dosingFunction.highTime = result;
          memoryWriteSetting();
          LCDML.FUNC_goBackToMenu();
        }
        switch (currentNumberDosingHighTime)
        {
        case 1:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberDosingHighTime-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 2:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberDosingHighTime-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 3:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberDosingHighTime-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 4:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberDosingHighTime-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 5:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberDosingHighTime-1)),(25),rectSize,rectSize,borderRadius);
          break;
        }
    } while( u8g2.nextPage() );
  }

  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}




int16_t numberdosingLowTime1 = 0;
int16_t numberdosingLowTime2 = 0;
int16_t numberdosingLowTime3 = 0;
int16_t numberdosingLowTime4 = 0;
int16_t numberdosingLowTime5 = 0;
int8_t currentNumberDosingLowTime = 1;

// *********************************************************************
void mFunc_dosingLowTime(uint8_t param)
// *********************************************************************
{
  char bufNumber1[1];
  char bufNumber2[1];
  char bufNumber3[1];
  char bufNumber4[1];
  char bufNumber5[1];

  int8_t xFont = 25;
  int8_t xAddation = 15;
  int8_t yFont = 37;
  int8_t rectSize = 15;
  int8_t borderRadius = 6;

  bool isEnter = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    double originalNumber = settings.dosingFunction.lowTime;
    int integerPart = static_cast<int>(originalNumber);
    numberdosingLowTime1 = integerPart / 10000;
    numberdosingLowTime2 = (integerPart / 1000) % 10;
    numberdosingLowTime3 = (integerPart / 100) % 10;
    numberdosingLowTime4 = (integerPart / 10) % 10;
    numberdosingLowTime5 = (integerPart / 1) % 10;
    LCDML_UNUSED(param);
  }



  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkUp())
      {
        if (currentNumberDosingLowTime==5) currentNumberDosingLowTime=1;
        else currentNumberDosingLowTime++; 
      }
      if(LCDML.BT_checkDown())
      {
        switch (currentNumberDosingLowTime)
          {
          case 1:
            if (numberdosingLowTime1 == 0) numberdosingLowTime1 = 9;
            else numberdosingLowTime1--;
            break;
          case 2:
            if (numberdosingLowTime2 == 0) numberdosingLowTime2 = 9;
            else numberdosingLowTime2--;
            break;
          case 3:
            if (numberdosingLowTime3 == 0) numberdosingLowTime3 = 9;
            else numberdosingLowTime3--;
            break;
          case 4:
            if (numberdosingLowTime4 == 0) numberdosingLowTime4 = 9;
            else numberdosingLowTime4--;
            break;
          case 5:
            if (numberdosingLowTime5 == 0) numberdosingLowTime5 = 9;
            else numberdosingLowTime5--;
            break;
          }
      }
      if (LCDML.BT_checkQuit()) LCDML.FUNC_goBackToMenu(1);
      if(LCDML.BT_checkEnter())
      {

        isEnter = true;
      }
    do {
        u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,borderRadius);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Enter low time(ms).");
        
        u8g2.setFont(u8g_font_9x18Br);
        sprintf (bufNumber1, "%d", numberdosingLowTime1);
        u8g2.drawStr(xFont,yFont,bufNumber1);
        sprintf (bufNumber2, "%d", numberdosingLowTime2);
        u8g2.drawStr(xFont +xAddation,yFont,bufNumber2);
        sprintf (bufNumber3, "%d", numberdosingLowTime3);
        u8g2.drawStr(xFont +(xAddation*2),yFont,bufNumber3);
        sprintf (bufNumber4, "%d", numberdosingLowTime4);
        u8g2.drawStr(xFont +(xAddation*3),yFont,bufNumber4);
        sprintf (bufNumber5, "%d", numberdosingLowTime5);
        u8g2.drawStr(xFont +(xAddation*4),yFont,bufNumber5);

        if (isEnter)
        {
          int result = 
            (numberdosingLowTime1 * 10000) + 
            (numberdosingLowTime2 * 1000) + 
            (numberdosingLowTime3 * 100) + 
            (numberdosingLowTime4 * 10) + 
            (numberdosingLowTime5 * 1);

          settings.dosingFunction.lowTime = result;
          memoryWriteSetting();
          LCDML.FUNC_goBackToMenu();
        }
        switch (currentNumberDosingLowTime)
        {
        case 1:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberDosingLowTime-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 2:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberDosingLowTime-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 3:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberDosingLowTime-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 4:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberDosingLowTime-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 5:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberDosingLowTime-1)),(25),rectSize,rectSize,borderRadius);
          break;
        }
    } while( u8g2.nextPage() );
  }

  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}






int16_t numberSetpointHigh1 = 0;
int16_t numberSetpointHigh2 = 0;
int16_t numberSetpointHigh3 = 0;
int16_t numberSetpointHigh4 = 0;
int8_t currentNumberSetpointHigh = 1;

// *********************************************************************
void mFunc_highSetpoint(uint8_t param)
// *********************************************************************
{
  char bufNumber1[1];
  char bufNumber2[1];
  char bufNumber3[1];
  char bufNumber4[1];

  int8_t xFont = 35;
  int8_t xAddation = 15;
  int8_t yFont = 37;
  int8_t rectSize = 15;
  int8_t borderRadius = 6;

  bool isEnter = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    double originalNumber = settings.setPointHigh;
    int integerPart = static_cast<int>(originalNumber);
    numberSetpointHigh1 = integerPart / 1000;
    numberSetpointHigh2 = (integerPart / 100) % 10;
    numberSetpointHigh3 = (integerPart / 10) % 10;
    numberSetpointHigh4 = (integerPart / 1) % 10;
    LCDML_UNUSED(param);
  }



  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkUp())
      {
        if (currentNumberSetpointHigh==4) currentNumberSetpointHigh=1;
        else currentNumberSetpointHigh++; 
      }
      if(LCDML.BT_checkDown())
      {
        switch (currentNumberSetpointHigh)
          {
          case 1:
            if (numberSetpointHigh1 == 0) numberSetpointHigh1 = 9;
            else numberSetpointHigh1--;
            break;
          case 2:
            if (numberSetpointHigh2 == 0) numberSetpointHigh2 = 9;
            else numberSetpointHigh2--;
            break;
          case 3:
            if (numberSetpointHigh3 == 0) numberSetpointHigh3 = 9;
            else numberSetpointHigh3--;
            break;
          case 4:
            if (numberSetpointHigh4 == 0) numberSetpointHigh4 = 9;
            else numberSetpointHigh4--;
            break;
          }
      }
      if (LCDML.BT_checkQuit()) LCDML.FUNC_goBackToMenu(1);
      if(LCDML.BT_checkEnter())
      {

        isEnter = true;
      }
    do {
        u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,borderRadius);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Enter high setpoint.");
        
        u8g2.setFont(u8g_font_9x18Br);
        sprintf (bufNumber1, "%d", numberSetpointHigh1);
        u8g2.drawStr(xFont,yFont,bufNumber1);
        sprintf (bufNumber2, "%d", numberSetpointHigh2);
        u8g2.drawStr(xFont +xAddation,yFont,bufNumber2);
        sprintf (bufNumber3, "%d", numberSetpointHigh3);
        u8g2.drawStr(xFont +(xAddation*2),yFont,bufNumber3);
        sprintf (bufNumber4, "%d", numberSetpointHigh4);
        u8g2.drawStr(xFont +(xAddation*3),yFont,bufNumber4);

        if (isEnter)
        {
          int result = 
            (numberSetpointHigh1 * 1000) + 
            (numberSetpointHigh2 * 100) + 
            (numberSetpointHigh3 * 10) +
            (numberSetpointHigh4);

          settings.setPointHigh = result;
          memoryWriteSetting();

          LCDML.OTHER_jumpToFunc(mFunc_settings);
        }
        switch (currentNumberSetpointHigh)
        {
        case 1:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberSetpointHigh-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 2:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberSetpointHigh-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 3:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberSetpointHigh-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 4:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberSetpointHigh-1)),(25),rectSize,rectSize,borderRadius);
          break;
        }
    } while( u8g2.nextPage() );
  }

  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}




int16_t numberSetpointLow1 = 0;
int16_t numberSetpointLow2 = 0;
int16_t numberSetpointLow3 = 0;
int16_t numberSetpointLow4 = 0;
int8_t currentNumberSetpointLow = 1;

// *********************************************************************
void mFunc_lowSetpoint(uint8_t param)
// *********************************************************************
{
  char bufNumber1[1];
  char bufNumber2[1];
  char bufNumber3[1];
  char bufNumber4[1];

  int8_t xFont = 35;
  int8_t xAddation = 15;
  int8_t yFont = 37;
  int8_t rectSize = 15;
  int8_t borderRadius = 6;

  bool isEnter = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    double originalNumber = settings.setPointLow;
    int integerPart = static_cast<int>(originalNumber);
    numberSetpointLow1 = integerPart / 1000;
    numberSetpointLow2 = (integerPart / 100) % 10;
    numberSetpointLow3 = (integerPart / 10) % 10;
    numberSetpointLow4 = (integerPart / 1) % 10;
    LCDML_UNUSED(param);
  }



  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkUp())
      {
        if (currentNumberSetpointLow==4) currentNumberSetpointLow=1;
        else currentNumberSetpointLow++; 
      }
      if(LCDML.BT_checkDown())
      {
        switch (currentNumberSetpointLow)
          {
          case 1:
            if (numberSetpointLow1 == 0) numberSetpointLow1 = 9;
            else numberSetpointLow1--;
            break;
          case 2:
            if (numberSetpointLow2 == 0) numberSetpointLow2 = 9;
            else numberSetpointLow2--;
            break;
          case 3:
            if (numberSetpointLow3 == 0) numberSetpointLow3 = 9;
            else numberSetpointLow3--;
            break;
          case 4:
            if (numberSetpointLow4 == 0) numberSetpointLow4 = 9;
            else numberSetpointLow4--;
            break;
          }
      }
      if (LCDML.BT_checkQuit()) LCDML.FUNC_goBackToMenu(1);
      if(LCDML.BT_checkEnter())
      {

        isEnter = true;
      }
    do {
        u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,borderRadius);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Enter low setpoint.");
        
        u8g2.setFont(u8g_font_9x18Br);
        sprintf (bufNumber1, "%d", numberSetpointLow1);
        u8g2.drawStr(xFont,yFont,bufNumber1);
        sprintf (bufNumber2, "%d", numberSetpointLow2);
        u8g2.drawStr(xFont +xAddation,yFont,bufNumber2);
        sprintf (bufNumber3, "%d", numberSetpointLow3);
        u8g2.drawStr(xFont +(xAddation*2),yFont,bufNumber3);
        sprintf (bufNumber4, "%d", numberSetpointLow4);
        u8g2.drawStr(xFont +(xAddation*3),yFont,bufNumber4);

        if (isEnter)
        {
          int result = 
            (numberSetpointLow1 * 1000) + 
            (numberSetpointLow2 * 100) + 
            (numberSetpointLow3 * 10)+
            (numberSetpointLow4 * 1);;

          settings.setPointLow = result;
          memoryWriteSetting();

          LCDML.OTHER_jumpToFunc(mFunc_settings);
        }
        switch (currentNumberSetpointLow)
        {
        case 1:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberSetpointLow-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 2:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberSetpointLow-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 3:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberSetpointLow-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 4:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberSetpointLow-1)),(25),rectSize,rectSize,borderRadius);
          break;
        }
    } while( u8g2.nextPage() );
  }

  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}


// void mfunc_sikim2(uint8_t param)
// {
//   LCDML.FUNC_goBackToMenu(1);
// }


int16_t numberhysteresis1 = 0;
int16_t numberhysteresis2 = 0;
int16_t numberhysteresis3 = 0;
int8_t currentNumbernumberhysteresis = 1;

// *********************************************************************
void mFunc_hysteresis(uint8_t param)
// *********************************************************************
{
  char bufNumber1[1];
  char bufNumber2[1];
  char bufNumber3[1];

  int8_t xFont = 45;
  int8_t xAddation = 15;
  int8_t yFont = 37;
  int8_t rectSize = 15;
  int8_t borderRadius = 6;

  bool isEnter = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    double originalNumber = settings.Hysteresis;
    int integerPart = static_cast<int>(originalNumber);
    numberhysteresis1 = integerPart / 100;
    numberhysteresis2 = (integerPart / 10) % 10;
    numberhysteresis3 = (integerPart / 1) % 10;
    LCDML_UNUSED(param);
  }



  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkUp())
      {
        if (currentNumbernumberhysteresis==3) currentNumbernumberhysteresis=1;
        else currentNumbernumberhysteresis++; 
      }
      if(LCDML.BT_checkDown())
      {
        switch (currentNumbernumberhysteresis)
          {
          case 1:
            if (numberhysteresis1 == 0) numberhysteresis1 = 9;
            else numberhysteresis1--;
            break;
          case 2:
            if (numberhysteresis2 == 0) numberhysteresis2 = 9;
            else numberhysteresis2--;
            break;
          case 3:
            if (numberhysteresis3 == 0) numberhysteresis3 = 9;
            else numberhysteresis3--;
            break;
          }
      }
      if (LCDML.BT_checkQuit()) LCDML.FUNC_goBackToMenu(1);
      if(LCDML.BT_checkEnter())
      {

        isEnter = true;
      }
    do {
        u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,borderRadius);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Enter hysteresis.");
        
        u8g2.setFont(u8g_font_9x18Br);
        sprintf (bufNumber1, "%d", numberhysteresis1);
        u8g2.drawStr(xFont,yFont,bufNumber1);
        sprintf (bufNumber2, "%d", numberhysteresis2);
        u8g2.drawStr(xFont +xAddation,yFont,bufNumber2);
        sprintf (bufNumber3, "%d", numberhysteresis3);
        u8g2.drawStr(xFont +(xAddation*2),yFont,bufNumber3);

        if (isEnter)
        {
          int result = 
            (numberhysteresis1 * 100) + 
            (numberhysteresis2 * 10) + 
            (numberhysteresis3 * 1);

          settings.Hysteresis = result;
          memoryWriteSetting();

          LCDML.OTHER_jumpToFunc(mFunc_settings);
        }
        switch (currentNumbernumberhysteresis)
        {
        case 1:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumbernumberhysteresis-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 2:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumbernumberhysteresis-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 3:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumbernumberhysteresis-1)),(25),rectSize,rectSize,borderRadius);
          break;
        }
    } while( u8g2.nextPage() );
  }

  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}



int16_t number1 = 1;
int16_t number2 = 0;
int16_t number3 = 0;
int16_t number4 = 0;
int8_t currentNumber = 1;

// *********************************************************************
void mFunc_scale(uint8_t param)
// *********************************************************************
{
  char bufNumber1[1];
  char bufNumber2[1];
  char bufNumber3[1];
  char bufNumber4[1];

  int8_t xFont = 30;
  int8_t xAddation = 15;
  int8_t yFont = 55;
  int8_t rectSize = 15;
  int8_t borderRadius = 6;

  bool isEnter = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);
    LCDML.FUNC_setLoopInterval(1);  
  }



  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkUp())
      {
        if (currentNumber==4) currentNumber=1;
        else currentNumber++; 
      }
      if(LCDML.BT_checkDown())
      {
        switch (currentNumber)
          {
          case 1:
            if (number1 == 0) number1 = 9;
            else number1--;
            break;
          case 2:
            if (number2 == 0) number2 = 9;
            else number2--;
            break;
          case 3:
            if (number3 == 0) number3 = 9;
            else number3--;
            break;
          case 4:
            if (number4 == 0) number4 = 9;
            else number4--;
            break;
          }
      }
      if (LCDML.BT_checkQuit()) LCDML.FUNC_goBackToMenu(1);
      if(LCDML.BT_checkEnter())
      {

        isEnter = true;
      }
      u8g2.firstPage();
    do {
        // u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,borderRadius);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Enter sample weight.");

        scale.power_up();
        float a = scale.read_average(1);
        scale.power_down();

        char buf1[40];
        dtostrf (a,4, 1, buf1);
        
        u8g2.setFont(u8g_font_5x8r);
        u8g2.drawStr(6,26,"Pure val   -> ");
        u8g2.drawStr(76,26,String(buf1).c_str());

        u8g2.drawStr(6,34,"Scaled val -> ");
        if (loadCell.isOver)
        {
          u8g2.drawStr(76,34,String("-OVER!-").c_str());
        }
        else
        {
          loadCellValue = loadCell.value; 
          char buf[40];
          if ((loadCellValue >0 && loadCellValue<settings.zeroFilter) || ((loadCellValue <0 && loadCellValue>(settings.zeroFilter*-1))))
          {
            loadCellValue = 0;
          }
          dtostrf (loadCellValue,4, 1, buf);
          u8g2.drawStr( 76, 34, buf);
        }

        u8g2.setFont(u8g_font_9x18Br);
        sprintf (bufNumber1, "%d", number1);
        u8g2.drawStr(xFont,yFont,bufNumber1);
        sprintf (bufNumber2, "%d", number2);
        u8g2.drawStr(xFont +xAddation,yFont,bufNumber2);
        sprintf (bufNumber3, "%d", number3);
        u8g2.drawStr(xFont +(xAddation*2),yFont,bufNumber3);
        sprintf (bufNumber4, "%d", number4);
        u8g2.drawStr(xFont +(xAddation*3),yFont,bufNumber4);

        u8g2.setFont(u8g_font_6x13r);
        u8g2.drawStr(90,yFont,"gm");

        if (isEnter)
        {
          float weight = (number1*1000) + (number2*100) + (number3*10) + number4;
          if (weight > 0)
          {
            calibrateLoadCell(weight);
          }
        }
        switch (currentNumber)
        {
        case 1:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumber-1)),(43),rectSize,rectSize,borderRadius);
          break;
        case 2:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumber-1)),(43),rectSize,rectSize,borderRadius);
          break;
        case 3:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumber-1)),(43),rectSize,rectSize,borderRadius);
          break;
        case 4:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumber-1)),(43),rectSize,rectSize,borderRadius);
          break;
        }
    } while( u8g2.nextPage() );
  }

  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}


// Define a fixed-point type with 3 decimal places
using FixedPoint = int;

// Constants for scaling and unscaling
const int ScaleFactor = 1000;  // 10^3 for 3 decimal places

// Convert a floating-point number to fixed-point
FixedPoint floatToFixed(double value) {
    return static_cast<FixedPoint>(value * ScaleFactor);
}

// Convert a fixed-point number to floating-point
double fixedToFloat(FixedPoint value) {
    return static_cast<double>(value) / ScaleFactor;
}


int16_t numbercoefficent1 = 0;
int16_t numbercoefficent2 = 0;
int16_t numbercoefficent3 = 0;
int16_t numbercoefficent4 = 0;
int16_t numbercoefficent5 = 0;
int16_t numbercoefficent6 = 0;
int8_t currentNumberCoefficent = 1;
// *********************************************************************
void mFunc_coefficent(uint8_t param)
// *********************************************************************
{
  char bufNumber1[1];
  char bufNumber2[1];
  char bufNumber3[1];
  char bufNumber4[1];
  char bufNumber5[1];
  char bufNumber6[1];

  int8_t xFont = 27;
  int8_t xAddation = 15;
  int8_t yFont = 35;
  int8_t rectSize = 15;
  int8_t borderRadius = 6;

  bool isEnter = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {

    FixedPoint fixedNumber = floatToFixed(settings.coefficent);
    int integerPart = fixedNumber / ScaleFactor;
    int decimalPart = fixedNumber % ScaleFactor;
    int numbercoefficent1 = (integerPart / 100) % 10;
    int numbercoefficent2 = (integerPart / 10) % 10;
    int numbercoefficent3 = integerPart % 10;
    int numbercoefficent4 = (decimalPart / 100) % 10;
    int numbercoefficent5 = (decimalPart / 10) % 10;
    int numbercoefficent6 = decimalPart % 10;



    // double originalNumber = settings.coefficent;
    // // Extract the integer part
    // int integerPart = static_cast<int>(originalNumber);
    // int integerPart2 = static_cast<int>(originalNumber*1000);
    // // Extract the decimal part
    // double decimalPart = originalNumber - integerPart;
    // // Split the integer part into individual digits
    // numbercoefficent1 = integerPart / 100;
    // numbercoefficent2 = (integerPart / 10) % 10;
    // numbercoefficent3 = (integerPart / 1) % 10;
    // numbercoefficent4 = (integerPart2 / 100)% 10;
    // numbercoefficent5 = (integerPart2 / 10) % 10;
    // numbercoefficent6 = (integerPart2 / 1) % 10;
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkUp())
      {
        if (currentNumberCoefficent==6) currentNumberCoefficent=1;
        else currentNumberCoefficent++; 
      }
      if(LCDML.BT_checkDown())
      {
        switch (currentNumberCoefficent)
          {
          case 1:
            if (numbercoefficent1 == 0) numbercoefficent1 = 9;
            else numbercoefficent1--;
            break;
          case 2:
            if (numbercoefficent2 == 0) numbercoefficent2 = 9;
            else numbercoefficent2--;
            break;
          case 3:
            if (numbercoefficent3 == 0) numbercoefficent3 = 9;
            else numbercoefficent3--;
            break;
          case 4:
            if (numbercoefficent4 == 0) numbercoefficent4 = 9;
            else numbercoefficent4--;
            break;
          case 5:
            if (numbercoefficent5 == 0) numbercoefficent5 = 9;
            else numbercoefficent5--;
            break;
          case 6:
            if (numbercoefficent6 == 0) numbercoefficent6 = 9;
            else numbercoefficent6--;
            break;
          }
      }
      if (LCDML.BT_checkQuit()) LCDML.FUNC_goBackToMenu(1);
      if(LCDML.BT_checkEnter())
      {
        isEnter = true;
      }
    do {
        u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,borderRadius);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Enter coefficent.");
        
        u8g2.setFont(u8g_font_9x18Br);
        sprintf (bufNumber1, "%d", numbercoefficent1);
        u8g2.drawStr(xFont,yFont,bufNumber1);
        sprintf (bufNumber2, "%d", numbercoefficent2);
        u8g2.drawStr(xFont +xAddation,yFont,bufNumber2);
        sprintf (bufNumber3, "%d", numbercoefficent3);
        u8g2.drawStr(xFont +(xAddation*2),yFont,bufNumber3);
        sprintf (bufNumber4, "%d", numbercoefficent4);
        u8g2.drawStr(xFont +(xAddation*3),yFont,bufNumber4);
        sprintf (bufNumber5, "%d", numbercoefficent5);
        u8g2.drawStr(xFont +(xAddation*4),yFont,bufNumber5);
        sprintf (bufNumber6, "%d", numbercoefficent6);
        u8g2.drawStr(xFont +(xAddation*5),yFont,bufNumber6);

        u8g2.drawStr(64,37,".");

        if (isEnter)
        {
          float result = 
            (numbercoefficent1 * 100) + 
            (numbercoefficent2 * 10) + 
            (numbercoefficent3 * 1) + 
            (numbercoefficent4 * 0.1) +
            (numbercoefficent5 * 0.01) + 
            (numbercoefficent6 * 0.001);

          settings.coefficent = (result);

          


          memoryWriteSetting();
          LCDML.OTHER_jumpToFunc(mFunc_settings);
        }
        switch (currentNumberCoefficent)
        {
        case 1:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberCoefficent-1)),(23),rectSize,rectSize,borderRadius);
          break;
        case 2:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberCoefficent-1)),(23),rectSize,rectSize,borderRadius);
          break;
        case 3:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberCoefficent-1)),(23),rectSize,rectSize,borderRadius);
          break;
        case 4:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberCoefficent-1)),(23),rectSize,rectSize,borderRadius);
          break;
        case 5:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberCoefficent-1)),(23),rectSize,rectSize,borderRadius);
          break;
        case 6:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberCoefficent-1)),(23),rectSize,rectSize,borderRadius);
          break;
        }
    } while( u8g2.nextPage() );
  }


  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}


void mFunc_settings(uint8_t param)
// *********************************************************************
{
  char bufNumber1[1];
  char bufNumber2[1];
  char bufNumber3[1];
  char bufNumber4[1];
  char bufNumber5[1];
  char bufNumber6[1];

  int8_t xFont = 27;
  int8_t xAddation = 15;
  int8_t yFont = 35;
  int8_t rectSize = 15;
  int8_t borderRadius = 6;

  bool isEnter = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    LCDML.OTHER_setCursorToID(2);
    LCDML.FUNC_goBackToMenu();
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkUp())
      {
        if (currentNumberCoefficent==6) currentNumberCoefficent=1;
        else currentNumberCoefficent++; 
      }
      if(LCDML.BT_checkDown())
      {
        switch (currentNumberCoefficent)
          {
          case 1:
            if (numbercoefficent1 == 0) numbercoefficent1 = 9;
            else numbercoefficent1--;
            break;
          case 2:
            if (numbercoefficent2 == 0) numbercoefficent2 = 9;
            else numbercoefficent2--;
            break;
          case 3:
            if (numbercoefficent3 == 0) numbercoefficent3 = 9;
            else numbercoefficent3--;
            break;
          case 4:
            if (numbercoefficent4 == 0) numbercoefficent4 = 9;
            else numbercoefficent4--;
            break;
          case 5:
            if (numbercoefficent5 == 0) numbercoefficent5 = 9;
            else numbercoefficent5--;
            break;
          case 6:
            if (numbercoefficent6 == 0) numbercoefficent6 = 9;
            else numbercoefficent6--;
            break;
          }
      }
      if (LCDML.BT_checkQuit()) LCDML.FUNC_goBackToMenu(1);
      if(LCDML.BT_checkEnter())
      {
        isEnter = true;
      }
    do {
        u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,borderRadius);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Enter coefficent.");
        
        u8g2.setFont(u8g_font_9x18Br);
        sprintf (bufNumber1, "%d", numbercoefficent1);
        u8g2.drawStr(xFont,yFont,bufNumber1);
        sprintf (bufNumber2, "%d", numbercoefficent2);
        u8g2.drawStr(xFont +xAddation,yFont,bufNumber2);
        sprintf (bufNumber3, "%d", numbercoefficent3);
        u8g2.drawStr(xFont +(xAddation*2),yFont,bufNumber3);
        sprintf (bufNumber4, "%d", numbercoefficent4);
        u8g2.drawStr(xFont +(xAddation*3),yFont,bufNumber4);
        sprintf (bufNumber5, "%d", numbercoefficent5);
        u8g2.drawStr(xFont +(xAddation*4),yFont,bufNumber5);
        sprintf (bufNumber6, "%d", numbercoefficent6);
        u8g2.drawStr(xFont +(xAddation*5),yFont,bufNumber6);

        u8g2.drawStr(64,37,".");

        if (isEnter)
        {
          float result = 
            (numbercoefficent1 * 100) + 
            (numbercoefficent2 * 10) + 
            (numbercoefficent3 * 1) + 
            (numbercoefficent4 * 0.1) +
            (numbercoefficent5 * 0.01) + 
            (numbercoefficent6 * 0.001);

          settings.coefficent = result;

          


          memoryWriteSetting();
          LCDML.OTHER_jumpToFunc(mFunc_screensaver);
        }
        switch (currentNumberCoefficent)
        {
        case 1:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberCoefficent-1)),(23),rectSize,rectSize,borderRadius);
          break;
        case 2:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberCoefficent-1)),(23),rectSize,rectSize,borderRadius);
          break;
        case 3:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberCoefficent-1)),(23),rectSize,rectSize,borderRadius);
          break;
        case 4:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberCoefficent-1)),(23),rectSize,rectSize,borderRadius);
          break;
        case 5:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberCoefficent-1)),(23),rectSize,rectSize,borderRadius);
          break;
        case 6:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberCoefficent-1)),(23),rectSize,rectSize,borderRadius);
          break;
        }
    } while( u8g2.nextPage() );
  }


  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}


// *********************************************************************
uint8_t g_func_timer_info = 0;  // time counter (global variable)
unsigned long g_timer_1 = 0;    // timer variable (global variable)
void mFunc_timer_info(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    g_func_timer_info = 20;       // reset and set timer

    char buf[20];
    sprintf (buf, "wait %d seconds", g_func_timer_info);

    u8g2.setFont(_LCDML_DISP_font);
    u8g2.firstPage();
    do {
      u8g2.drawStr( 0, (_LCDML_DISP_font_h * 1), buf);
      u8g2.drawStr( 0, (_LCDML_DISP_font_h * 2), "or press back button");
    } while( u8g2.nextPage() );



    LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds

    LCDML.TIMER_msReset(g_timer_1);
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
    // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
    // the quit button works in every DISP function without any checks; it starts the loop_end function

    // reset screensaver timer
    LCDML.SCREEN_resetTimer();

     // this function is called every 100 milliseconds

    // this method checks every 1000 milliseconds if it is called
    if(LCDML.TIMER_ms(g_timer_1, 1000))
    {
      g_timer_1 = millis();
      g_func_timer_info--;                // increment the value every second
      char buf[20];
      sprintf (buf, "wait %d seconds", g_func_timer_info);

      u8g2.setFont(_LCDML_DISP_font);
      u8g2.firstPage();
      do {
        u8g2.drawStr( 0, (_LCDML_DISP_font_h * 1), buf);
        u8g2.drawStr( 0, (_LCDML_DISP_font_h * 2), "or press back button");
      } while( u8g2.nextPage() );

    }

    // this function can only be ended when quit button is pressed or the time is over
    // check if the function ends normally
    if (g_func_timer_info <= 0)
    {
      // leave this function
      LCDML.FUNC_goBackToMenu();
    }
  }

  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}

// *********************************************************************
uint8_t g_button_value = 0; // button value counter (global variable)
void mFunc_p2(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    // setup function
    // print LCD content
    char buf[17];
    sprintf (buf, "count: %d of 3", 0);

    u8g2.setFont(_LCDML_DISP_font);
    u8g2.firstPage();
    do {
      u8g2.drawStr( 0, (_LCDML_DISP_font_h * 1), "press a or w button");
      u8g2.drawStr( 0, (_LCDML_DISP_font_h * 2), buf);
    } while( u8g2.nextPage() );

    // Reset Button Value
    g_button_value = 0;

    // Disable the screensaver for this function until it is closed
    LCDML.FUNC_disableScreensaver();
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
    // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
    // the quit button works in every DISP function without any checks; it starts the loop_end function

    // the quit button works in every DISP function without any checks; it starts the loop_end function
    if (LCDML.BT_checkAny()) // check if any button is pressed (enter, up, down, left, right)
    {
      if (LCDML.BT_checkLeft() || LCDML.BT_checkUp()) // check if button left is pressed
      {
        LCDML.BT_resetLeft(); // reset the left button
        LCDML.BT_resetUp(); // reset the left button
        g_button_value++;

        // update LCD content
        char buf[20];
        sprintf (buf, "count: %d of 3", g_button_value);

        u8g2.setFont(_LCDML_DISP_font);
        u8g2.firstPage();
        do {
          u8g2.drawStr( 0, (_LCDML_DISP_font_h * 1), "press a or w button");
          u8g2.drawStr( 0, (_LCDML_DISP_font_h * 2), buf);
        } while( u8g2.nextPage() );
      }
    }

   // check if button count is three
    if (g_button_value >= 3) {
      LCDML.FUNC_goBackToMenu();      // leave this function
    }
  }

  if(LCDML.FUNC_close())     // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}


#define h123_width 50
#define h123_height 50
static unsigned char h123_bits[] = {
   0x80, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0x60,
   0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x18, 0x00,
   0x00, 0x00, 0x00, 0x60, 0x00, 0x0c, 0x00, 0x00,
   0x00, 0x00, 0xc0, 0x00, 0x04, 0x00, 0x00, 0x00,
   0x00, 0x80, 0x00, 0x02, 0xfe, 0x00, 0x00, 0x00,
   0x00, 0x01, 0xc2, 0xff, 0x07, 0xfc, 0xff, 0x01,
   0x01, 0xe1, 0xff, 0x1f, 0xfc, 0xff, 0x0f, 0x02,
   0xf1, 0xff, 0x3f, 0xfc, 0xff, 0x1f, 0x02, 0xf9,
   0xff, 0x7f, 0xfc, 0xff, 0x3f, 0x02, 0xf9, 0xff,
   0x7f, 0xfc, 0xff, 0x7f, 0x02, 0xfd, 0xcf, 0x7f,
   0xfc, 0xff, 0x7f, 0x02, 0xfd, 0xc7, 0xff, 0xfc,
   0xcf, 0x7f, 0x02, 0xfd, 0xc7, 0xff, 0xfc, 0x8f,
   0xff, 0x02, 0xfd, 0xc7, 0xff, 0xfc, 0x8f, 0xff,
   0x02, 0xfd, 0xc7, 0xff, 0xfc, 0x8f, 0xff, 0x02,
   0xfd, 0xc7, 0xff, 0xfc, 0x8f, 0xff, 0x02, 0xfd,
   0xc7, 0xff, 0xfc, 0x8f, 0xff, 0x02, 0xfd, 0xc7,
   0xff, 0xfc, 0x8f, 0xff, 0x02, 0xfd, 0x07, 0x00,
   0xfc, 0xcf, 0x7f, 0x02, 0xfd, 0x07, 0x00, 0xfc,
   0xff, 0x7f, 0x02, 0xfd, 0x07, 0x00, 0xfc, 0xff,
   0x1f, 0x02, 0xfd, 0x07, 0x00, 0xfc, 0xff, 0x0f,
   0x02, 0xfd, 0xe7, 0xff, 0xfc, 0xff, 0x1f, 0x02,
   0xfd, 0xe7, 0xff, 0xfc, 0xff, 0x7f, 0x02, 0xfd,
   0xe7, 0xff, 0xfc, 0xff, 0x7f, 0x02, 0xfd, 0xe7,
   0xff, 0xfc, 0xcf, 0xff, 0x02, 0xfd, 0xe7, 0xff,
   0xfc, 0x8f, 0xff, 0x02, 0xfd, 0xe7, 0xff, 0xfc,
   0x8f, 0xff, 0x02, 0xfd, 0x87, 0xff, 0xfc, 0x8f,
   0xff, 0x02, 0xfd, 0x87, 0xff, 0xfc, 0x8f, 0xff,
   0x02, 0xfd, 0x87, 0xff, 0xfc, 0x8f, 0xff, 0x02,
   0xfd, 0x87, 0xff, 0xfc, 0x8f, 0xff, 0x02, 0xfd,
   0x87, 0xff, 0xfc, 0x8f, 0xff, 0x02, 0xfd, 0x87,
   0xff, 0xfc, 0x8f, 0xff, 0x02, 0xfd, 0x87, 0xff,
   0xfc, 0x8f, 0xff, 0x02, 0xfd, 0xc7, 0xff, 0xfc,
   0xcf, 0xff, 0x02, 0xfd, 0xcf, 0xff, 0xfc, 0xff,
   0xff, 0x02, 0xf9, 0xff, 0xff, 0xfc, 0xff, 0xff,
   0x02, 0xf9, 0xff, 0xff, 0xfc, 0xff, 0xff, 0x02,
   0xf1, 0xff, 0xff, 0xfc, 0xff, 0x7f, 0x02, 0xe1,
   0xff, 0xfc, 0xfc, 0xff, 0x3f, 0x02, 0xc1, 0x7f,
   0xfc, 0xfc, 0xff, 0x07, 0x02, 0x02, 0x1f, 0x00,
   0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00,
   0x80, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0xc0,
   0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00,
   0x60, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x80,
   0xff, 0xff, 0xff, 0xff, 0x07, 0x00};


// *********************************************************************
void mFunc_welcomePage(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    LCDML_UNUSED(param);
    u8g2.firstPage();
    do {
      u8g2.drawRFrame(0,0,128,64,7);


      u8g2.drawRFrame(4,4,50,56,7);
      u8g2.setFont(u8g_font_5x8r);
      u8g2.drawStr(14,13,"model:");
      u8g2.setFont(u8g_font_profont29);
      u8g2.drawStr(13,37,"ND");
      u8g2.setFont(u8g_font_profont17r);
      u8g2.drawStr(31,52,"12");

      //GB PICTURE
      // u8g2.drawXBMP(8,7,50,50,h123_bits);

      u8g2.setFont(u8g_font_6x12);
      u8g2.drawStr(78,19,"SMART");
      u8g2.drawStr(75,31,"WEIGHT");
      u8g2.drawStr(65,43,"INDICATOR");
      u8g2.setFont(u8g_font_5x8);
      u8g2.drawStr(82,60,"V0.8");
    } while( u8g2.nextPage() );

  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
         
  }

  if(LCDML.FUNC_close())          // ****** STABLE END *********
  {
    
  }
}


int16_t numberSettingsPassword1 = 0;
int16_t numberSettingsPassword2 = 0;
int16_t numberSettingsPassword3 = 0;
int8_t currentNumberSettingsPassword = 1;

void mFunc_settingsPassword(uint8_t param)
{
  char bufNumber1[1];
  char bufNumber2[1];
  char bufNumber3[1];
  String passwordWrong;

  int8_t xFont = 45;
  int8_t xAddation = 15;
  int8_t yFont = 37;
  int8_t rectSize = 15;
  int8_t borderRadius = 6;

  bool isEnter = false;
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    numberSettingsPassword1 = 0;
    numberSettingsPassword2 = 0;
    numberSettingsPassword3 = 0;
    passwordWrong = "";
    LCDML_UNUSED(param);
  }



  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
      if(LCDML.BT_checkUp())
      {
        passwordWrong = "";
        if (currentNumberSettingsPassword==3) currentNumberSettingsPassword=1;
        else currentNumberSettingsPassword++; 
      }
      if(LCDML.BT_checkDown())
      {
        passwordWrong = "";
        switch (currentNumberSettingsPassword)
          {
          case 1:
            if (numberSettingsPassword1 == 0) numberSettingsPassword1 = 9;
            else numberSettingsPassword1--;
            break;
          case 2:
            if (numberSettingsPassword2 == 0) numberSettingsPassword2 = 9;
            else numberSettingsPassword2--;
            break;
          case 3:
            if (numberSettingsPassword3 == 0) numberSettingsPassword3 = 9;
            else numberSettingsPassword3--;
            break;
          }
      }
      if (LCDML.BT_checkQuit()) LCDML.FUNC_goBackToMenu(1);
      if(LCDML.BT_checkEnter())
      {
        isEnter = true;
      }
    do {
        u8g2.clear();
        
        u8g2.drawRFrame(0,0,128,64,borderRadius);
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(6,10,"Enter your password.");
        
        u8g2.setFont(u8g_font_9x18Br);
        sprintf (bufNumber1, "%d", numberSettingsPassword1);
        u8g2.drawStr(xFont,yFont,bufNumber1);
        sprintf (bufNumber2, "%d", numberSettingsPassword2);
        u8g2.drawStr(xFont +xAddation,yFont,bufNumber2);
        sprintf (bufNumber3, "%d", numberSettingsPassword3);
        u8g2.drawStr(xFont +(xAddation*2),yFont,bufNumber3);


        if (isEnter)
        {
          int result = 
            (numberSettingsPassword1 * 100) + 
            (numberSettingsPassword2 * 10) + 
            (numberSettingsPassword3 * 1);
          if (result == settings.password)
          {
            LCDML.OTHER_jumpToFunc(mFunc_settings);
            passwordWrong = "True  :)";
          }
          else
          {
            passwordWrong = "Wrong!!!";
          }
        }
        u8g2.setFont(u8g_font_6x10r);
        u8g2.drawStr(42,58,passwordWrong.c_str());
        switch (currentNumberSettingsPassword)
        {
        case 1:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberSettingsPassword-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 2:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberSettingsPassword-1)),(25),rectSize,rectSize,borderRadius);
          break;
        case 3:
          u8g2.drawRFrame((xFont-3) +(xAddation*(currentNumberSettingsPassword-1)),(25),rectSize,rectSize,borderRadius);
          break;
        }
    } while( u8g2.nextPage() );
  }

  LCDML.FUNC_disableScreensaver();
  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}


unsigned long g_timer_2 = 0;
// *********************************************************************
void mFunc_screensaver(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())
  {
    LCDML_UNUSED(param);
    LCDML.FUNC_setLoopInterval(1);  
  }
  

  if(LCDML.FUNC_loop())     
  {
    if (LCDML.BT_checkQuit()) 
    {
      LCDML.FUNC_goBackToMenu(); 
    }
    if (LCDML.BT_checkEnter())
    {
      if (LCDML.BT_checkUp())
      {
        tareLoadCell();
      }  
    }
    if (LCDML.BT_checkDown()) 
    {
      if (functions.functionActive) functions.functionActive = false;
      else functions.functionActive = true; 
    }
    

      u8g2.firstPage();
      do {
        if (loadCell.isOver)
        {
          u8g2.setFont(u8g2_font_inr21_mr);
          u8g2.drawStr(13,43,String("OVER!!").c_str());
        }
        else
        {
          loadCellValue = loadCell.value; 
          char buf[40];
          if ((loadCellValue >0 && loadCellValue<settings.zeroFilter) || ((loadCellValue <0 && loadCellValue>(settings.zeroFilter*-1))))
          {
            loadCellValue = 0;
          }
          dtostrf (loadCellValue,4, 1, buf);
          u8g2.setFont(u8g2_font_inr21_mn);
          u8g2.drawStr( 10, 43, buf);
        }

        u8g2.setFont(u8g2_font_6x10_mr);
        u8g2.drawStr(86,60, String("["+String(units[settings.unit])+"]").c_str());
        
        if (settings.currentFunction==1 && functions.functionActive)
        {
          char currentCounterNumber[10];
          if (functions.keepAlive)
          {
            sprintf (currentCounterNumber, "Time : %d", functions.keepAliveCounter);
            u8g2.drawStr(4,60, currentCounterNumber);
          }
          else
          {
            sprintf (currentCounterNumber, "D -> %d", functions.currentCounterNumber);
            u8g2.drawStr(4,60, currentCounterNumber);
          }
          
        }
        else
        {
          char screenRatio[10];
          dtostrf(ratioList[settings.ratio],4,3,screenRatio);
          u8g2.drawStr(4,60, String("[X"+String(screenRatio)+"]").c_str());
        }
        




        
        u8g2.drawLine(7,16,7,48);
        u8g2.setFont(u8g2_font_6x12_mr);
        if(functions.functionActive) 
        {
          if (settings.currentFunction==0) u8g2.drawStr(1,26,String("F").c_str());
          else u8g2.drawStr(1,26,String("D").c_str());
        }
        else u8g2.drawStr(1,23,String("").c_str());


        u8g2.setFont(u8g2_font_5x8_mr);
        
        u8g2.drawStr(0,11,"I1:");
        u8g2.drawStr(30,11,"I2:");
        if (inputs.input1 == 0) { u8g2.drawDisc(20,8,4);}
        else { u8g2.drawCircle(20,8,4);}
        if (inputs.input2 == 0) { u8g2.drawDisc(50,8,4);}
        else { u8g2.drawCircle(50,8,4);}

        switch (settings.currentFunction)
        {
          case 0:
          {
            u8g2.drawStr(74,11,"O1:");
            u8g2.drawStr(104,11,"O2:");
            if (functions.highSetPoint) u8g2.drawBox(119,4,8,8);
            else u8g2.drawFrame(119,4,8,8);
            if (functions.lowSetPoint ) u8g2.drawBox(89,4,8,8);
            else u8g2.drawFrame(89,4,8,8);
            break;
          }
          case 1:
          {
            u8g2.drawStr(74,11,"O1:");
            u8g2.drawStr(104,11,"O2:");
            if (settings.dosingFunction.outputNumber==1)
            {
              if (functions.dosingHigh) u8g2.drawBox(119,4,8,8);
              else u8g2.drawFrame(119,4,8,8);
              if (functions.functionActive) u8g2.drawBox(89,4,8,8);
              else u8g2.drawFrame(89,4,8,8);
            }
            else if(settings.dosingFunction.outputNumber==0)
            {
              if (functions.dosingHigh) u8g2.drawBox(89,4,8,8);
              else u8g2.drawFrame(89,4,8,8);
              if (functions.functionActive) u8g2.drawBox(119,4,8,8);
              else u8g2.drawFrame(119,4,8,8);
            }
            break;
          }
          default:
            break;
        }
        
        
        


        

        u8g2.drawLine(0,48,128,48);
        u8g2.drawLine(0,16,128,16);
      } while( u8g2.nextPage() );
  }

  if(LCDML.FUNC_close())
  {
    LCDML.MENU_goRoot();
  }
}



// *********************************************************************
void mFunc_back(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    // end function and go an layer back
    LCDML.FUNC_goBackToMenu(1);      // leave this function and go a layer back
  }
}


// *********************************************************************
void mFunc_goToRootMenu(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    // go to root and display menu
    LCDML.MENU_goRoot();
  }
}


// *********************************************************************
void mFunc_jumpTo_timer_info(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);
    
    // Jump to main screen
    LCDML.OTHER_jumpToFunc(mFunc_timer_info);
  }
}
