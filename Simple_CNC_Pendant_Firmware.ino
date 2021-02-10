#include <Arduino.h>
#include <U8g2lib.h>
#include <U8x8lib.h>
#include <Encoder.h>
#include <Keyboard.h>

/*
    Simple HID CNC Interface - CNCjs
*/

//global variables
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
Encoder knobMain(0, 1); // main jog wheel

//state holders
long positionMain  = 0;
int animationFrame =1;
bool enabled=false;
int lastAxisPos = 99;

//graphics
const byte MotherShipGfx[][8] PROGMEM = {
{ 0xfc, 0x3f, 0xb6, 0x6d, 0xff, 0xff, 0x9c, 0x39},
{ 0xfc, 0x00, 0x4a, 0x01, 0xff, 0x03, 0xb5, 0x02}
};

const byte InvaderTopGfx[][8] PROGMEM = {
{ 0x18, 0x3c, 0x7e, 0xdb, 0xff, 0x24, 0x5a, 0xa5},
{ 0x18, 0x3c, 0x7e, 0xdb, 0xff, 0x5a, 0x81, 0x42}
};

const byte InvaderMiddleGfx[][16] PROGMEM = {
{ 0x04, 0x01, 0x88, 0x00, 0xfc, 0x01, 0x76, 0x03, 0xff, 0x07, 0xfd, 0x05, 0x05, 0x05, 0xd8, 0x00},
{ 0x04, 0x01, 0x88, 0x00, 0xfd, 0x05, 0x75, 0x05, 0xff, 0x07, 0xfc, 0x01, 0x04, 0x01, 0x02, 0x02}
};
 
const byte InvaderBottomGfx[][16] PROGMEM = {
{ 0xf0, 0x00, 0xfe, 0x07, 0xff, 0x0f, 0x67, 0x0e, 0xff, 0x0f, 0x9c, 0x03, 0x06, 0x06, 0x0c, 0x03},
{ 0xf0, 0x00, 0xfe, 0x07, 0xff, 0x0f, 0x67, 0x0e, 0xff, 0x0f, 0x9c, 0x03, 0x62, 0x04, 0x01, 0x08}
};


void setup()
{
  //start the display driver
  u8g2.begin();
   
  //setup gpio pins
  // switches
  pinMode(A3, INPUT);  // enable
  digitalWrite(A3, HIGH);  // Pull the button high
  pinMode(A2, OUTPUT);  // enable LED
  digitalWrite(A2, LOW);  // Pull the LED low
  pinMode(4, INPUT);  // E-Stop
  digitalWrite(4, HIGH);  // Pull the button high

  // buttons
  pinMode(5, INPUT);  // Zero
  digitalWrite(5, HIGH);  // Pull the button high
  pinMode(6, INPUT);  // 1
  digitalWrite(6, HIGH);  // Pull the button high
  pinMode(7, INPUT);  // 2
  digitalWrite(7, HIGH);  // Pull the button high
  pinMode(8, INPUT);  // 3
  digitalWrite(8, HIGH);  // Pull the button high
  pinMode(9, INPUT);  // 4
  digitalWrite(9, HIGH);  // Pull the button high

  // rotarys
  pinMode(A0, INPUT);  // Axis switch
  pinMode(A1, INPUT);  // Step switch

}

void loop()
{
  u8g2.clearBuffer();          // clear the internal memory
  processButtons();  
  u8g2.sendBuffer();          // transfer internal memory to the display
}

void processButtons()
{
  //process e-stop
  if (digitalRead(4) == 0)  // if the button goes low
  {
    //cncjs : pause
    Keyboard.press('!');
    delay(250);  // delay to limit key presses.
    Keyboard.releaseAll();
  }
  
  //process enable button
  if (digitalRead(A3) == 0)  // if the button goes low
  {
    enabled=true;
    digitalWrite(A2,HIGH);//turn on ring light
  }
  else
  {
    enabled=false;
    digitalWrite(A2,LOW);//turn off ring light
    return;
  }

  //screen text
  u8g2.setFont(u8g2_font_trixel_square_tr); // choose a suitable font
  u8g2.drawStr(70,8,"DIY.Engineering");  // write something to the internal memory

  u8g2.setFont(u8g2_font_tenfatguys_t_all);
  u8g2.drawStr(10,30,"AXIS");  // write something to the internal memory
  u8g2.drawStr(75,30,"STEP");  // write something to the internal memory

  // zero
  if (digitalRead(5) == 0)  // if the button goes low
  {
    u8g2.drawCircle(50,55,3);

    //cncjs : home
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('h');
    delay(100);  // delay to limit key presses.
    Keyboard.releaseAll();
  }else{
    u8g2.drawDisc(50,55,3);
  }
  
  // one
  if (digitalRead(6) == 0)  // if the button goes low
  {
    u8g2.drawCircle(10,5,3);
    //cncjs : unlock
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('u');
    delay(100);  // delay to limit key presses.
    Keyboard.releaseAll();
  }else{
    u8g2.drawDisc(10,5,3);
  }
  
  // two
  if (digitalRead(7) == 0)  // if the button goes low
  {
    u8g2.drawCircle(20,5,3);
    //cncjs : reset
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('r');
    delay(100);  // delay to limit key presses.
    Keyboard.releaseAll();
  }else{
    u8g2.drawDisc(20,5,3);
  }
  
  // three
  if (digitalRead(8) == 0)  // if the button goes low
  {
    u8g2.drawCircle(30,5,3);
    /* insert hot keys here
        //cncjs : reset
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('r');
        delay(100);  // delay to limit key presses.
        Keyboard.releaseAll();
    */
  }else{
    u8g2.drawDisc(30,5,3);
  }

  // four
  if (digitalRead(9) == 0)  // if the button goes low
  {
    u8g2.drawCircle(40,5,3);
    /* insert hot keys here
        //cncjs : reset
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('r');
        delay(100);  // delay to limit key presses.
        Keyboard.releaseAll();
    */
  }else{
    u8g2.drawDisc(40,5,3);
  }

  //draw lines
  u8g2.drawBox(0, 12, 128, 3);
  u8g2.drawBox(63, 12, 3, 50);
  
  //draw wheel invader
  u8g2.drawXBMP(52, 0, 10, 10, InvaderMiddleGfx[animationFrame]);

  //axis rotary switch
  //divide and round the input as float
  float axisValueFloat = round(analogRead(A0) / 150);
  
  //cast to integer for e.g. indexing arrays
  int axisValue = axisValueFloat;
  u8g2.setFont(u8g2_font_bubble_tr);
  switch(axisValue)
  {
    case 0:
      //cncjs : home
      u8g2.drawStr(22,55,"X");  // draw axis name
      if(lastAxisPos!=axisValue)
      {
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('x');
        delay(100);  
        Keyboard.releaseAll();
      }
    break;
    case 1:
      u8g2.drawStr(22,55,"Y");  // draw axis name
      if(lastAxisPos!=axisValue)
      {
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('y');
        delay(100);  
        Keyboard.releaseAll();
      }
    break;
    case 2:
      u8g2.drawStr(22,55,"Z");  // draw axis name
      if(lastAxisPos!=axisValue)
      {
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('z');
        delay(100);  
        Keyboard.releaseAll();
      }    
      break;
    case 3:
      u8g2.drawStr(22,55,"A");  // draw axis name
      if(lastAxisPos!=axisValue)
      {
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('a');
        delay(100);  
        Keyboard.releaseAll();
      }
      break;
    case 4:
      u8g2.drawStr(22,55,"B");  // draw axis name
      if(lastAxisPos!=axisValue)
      {
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('b');
        delay(100);  
        Keyboard.releaseAll();
      }
      break;
    case 5:
      u8g2.drawStr(22,55,"C");  // draw axis name
      if(lastAxisPos!=axisValue)
      {
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('c');
        delay(100);  
        Keyboard.releaseAll();
      }    
      break;
  }

  //update state
  lastAxisPos = axisValue;
  
  //divide and round the input as float
  float stepValueFloat = round(analogRead(A1) / 150);
  
  //cast to integer for e.g. indexing arrays
  int stepValue = stepValueFloat;
  switch(stepValue)
  {
    case 0:
      u8g2.drawStr(85,55,"1x");  // write step name
    break;
    case 1:
      u8g2.drawStr(75,55,"2x");  // write step name
    break;
    case 2:
      u8g2.drawStr(75,55,"3x");  // write step name
    break;
    case 3:
      u8g2.drawStr(75,55,"4x");  // write step name
    break;
    case 4:
      u8g2.drawStr(75,55,"5x");  // write step name
    break;
    case 5:
      u8g2.drawStr(75,55,"6x");  // write step name
    break;
  }

  //process the main rotary movement
  long newMain;
  newMain = knobMain.read();
  
  //jogging flag 
  if (newMain > positionMain)
  {
    float delta=((float)newMain - (float)positionMain)/(float)50.0;
    int spd=delta<0.15?0:delta<0.3?1:delta<0.45?2:delta<0.6?3:delta<0.75?4:delta<0.90?5:6;
    switch(spd)
    {
      case 0:
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_SHIFT);
        Keyboard.press('1');
        Keyboard.releaseAll();
        break;
      case 1:
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_SHIFT);
        Keyboard.press('2');
        Keyboard.releaseAll();
        break;
      case 2:
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_SHIFT);
        Keyboard.press('3');
        Keyboard.releaseAll();
        break;
      case 3:
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_SHIFT);
        Keyboard.press('4');
        Keyboard.releaseAll();
        break;
      case 4:
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_SHIFT);
        Keyboard.press('5');
        Keyboard.releaseAll();
        break;
      case 5:
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_SHIFT);
        Keyboard.press('6');
        Keyboard.releaseAll();
        break;
      case 6:
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_SHIFT);
        Keyboard.press('7');
        Keyboard.releaseAll();
        break;
    }
   animationFrame=1-animationFrame;
   delay(15-(spd*2.5));
  }
  else if (newMain < positionMain)
  {
    float delta=((float)positionMain-(float)newMain)/(float)50;
    int spd=delta<0.15?0:delta<0.3?1:delta<0.45?2:delta<0.6?3:delta<0.75?4:delta<0.90?5:6;

    switch(spd)
    {
      case 0:
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('1');
        Keyboard.releaseAll();
        break;
      case 1:
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('2');
        Keyboard.releaseAll();
        break;
      case 2:
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('3');
        Keyboard.releaseAll();
        break;
      case 3:
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('4');
        Keyboard.releaseAll();
        break;
      case 4:
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('5');
        Keyboard.releaseAll();
        break;
      case 5:
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('6');
        Keyboard.releaseAll();
        break;
      case 6:
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('7');
        Keyboard.releaseAll();
        break;
    }
    animationFrame=1-animationFrame;
    delay(15-(spd*2.5));
  }
  positionMain = newMain;
}
