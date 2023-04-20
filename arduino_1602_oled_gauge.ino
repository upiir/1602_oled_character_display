// simple project using Arduino UNO and 16x2 OLED character display to display smooth filling gauges and animated digits
// created by upir, 2023
// youtube channel: https://www.youtube.com/upir_upir

// FULL TUTORIAL: https://youtu.be/MEhJtpkjwnc
// Source files: https://github.com/upiir/1602_oled_character_display

// Links related to this project:
// 16x2 OLED Character Display IIC - https://s.click.aliexpress.com/e/_DlwXF69
// Arduino UNO - https://s.click.aliexpress.com/e/_AXDw1h
// Arduino breadboard prototyping shield - https://s.click.aliexpress.com/e/_ApbCwx
// Arduino Library - https://github.com/gadjet/1602-OLED-Arduino-Library
// I2C Scanner sketch - https://playground.arduino.cc/Main/I2cScanner/

// My other videos with 16x2 character display:
// Smooth Arduino 16x2 Gauge - https://youtu.be/cx9CoGqpsfg
// Arduino Gauge in 11 Minutes - https://youtu.be/upE17NHrdPc
// DIY Battery Indicator - https://youtu.be/Mq0WBPKGRew
// 1 DISPLAY 3 SENSORS - https://youtu.be/lj_7UmM0EPY
// small display - BIG DIGITS - https://youtu.be/SXSujfeN_QI



#include "Wire.h"
#include "OLedI2C.h"
OLedI2C LCD;

// this variable holds all the characters (both lines) that will be displays on the character display
char line_final[56] = {144, 214, 214, 216, 32, 146, 214, 214, 215, 32, 32, 18, 49, 221, 130, 134,  // first line 16 characters
                       32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,  // required space between two lines
                       145, 214, 214, 217, 32, 147, 214, 214, 218, 32, 32, 19, 55, 221, 133, 128}; // second line 16 characters
 
int num_value_max; // number to be displayed on the right side of the first line
int num_value_min; // number to be displayed on the right side of the second line
char num_value_max_buffer[5]; // helper buffer for converting integer to a c-style string
char num_value_min_buffer[5]; // helper buffer for converting integer to a c-style string

int gauge_value_note = 0;        // gauge value for the gauge next to the note icon
int gauge_value_doublenote = 25; // gauge value for the gauge next to the double note icon
int gauge_value_bell = 50;       // gauge value for the gauge next to the bell icon
int gauge_value_heart = 75;      // gauge value for the gauge next to the heart icon

void setup()
{
  Wire.begin(); // begin the I2C communication
  LCD.init(); // initialization for the LCD display
}

// custom function to draw a gauge 
void generate_gauge (int value, int value_max, int gauge_chars_length, int gauge_chars_start) {

  for (int i = 0; i < gauge_chars_length; i++) { // go over every single character in the gauge

    float min_char_value = (value_max / gauge_chars_length * 1.0) * i;       // min. value for the currently drawn character
    float max_char_value = (value_max / gauge_chars_length * 1.0) * (i + 1); // max. value for the currently drawn character   
    float one_step = (value_max / gauge_chars_length * 1.0) / 5.0;           // one step = 1/5 of the character width (since the character is 5px wide)
 
    if (value >= max_char_value) {line_final[gauge_chars_start + i] = 214;}                        // fully filed character
    else if (value <= min_char_value) {line_final[gauge_chars_start + i] = 32;}                    // empty character
    else {                                                                                         // partially filled character 
      if (value >= (min_char_value + one_step * 4)) {line_final[gauge_chars_start + i] = 215;}     // 4 vertical lines filled
      else if (value >= (min_char_value + one_step * 3)) {line_final[gauge_chars_start + i] = 216;}// 3 vertical lines filled
      else if (value >= (min_char_value + one_step * 2)) {line_final[gauge_chars_start + i] = 217;}// 2 vertical lines filled
      else if (value >= (min_char_value + one_step * 1)) {line_final[gauge_chars_start + i] = 218;}// 1 vertical line filled
    }  
  }
}

void loop()
{

  // set the new values for the gauges
  gauge_value_note++;
  gauge_value_doublenote++;
  gauge_value_bell++;
  gauge_value_heart++;

  // restrict the gauge values between 0-100
  if (gauge_value_note > 100) {gauge_value_note = 0;}
  if (gauge_value_doublenote > 100) {gauge_value_doublenote = 0;}
  if (gauge_value_bell > 100) {gauge_value_bell = 0;}
  if (gauge_value_heart > 100) {gauge_value_heart = 0;}      

  // generate four gauges based on the values above
  generate_gauge(gauge_value_note, 100, 4, 1);  // generate_gauge( value, maximum value, number of characters, where to place the gauge in the main string)
  generate_gauge(gauge_value_doublenote, 100, 4, 41);
  generate_gauge(gauge_value_bell, 100, 4, 6);
  generate_gauge(gauge_value_heart, 100, 4, 46);      

  // code below is for the digits/numbers displays on the right side of the display
  
  num_value_max++; // increment value by one
  if (num_value_max > 999) {num_value_max = 0;} // if the value is over 3 digits, set it back to zero
  num_value_min--; // decrement value by one
  if (num_value_min < 0) {num_value_min = 999;} // if the value is below zero, set it back to max. value (999)

  sprintf(num_value_max_buffer, "%03d", num_value_max); // convert the value into string with 3 leading zeros
  sprintf(num_value_min_buffer, "%03d", num_value_min); // convert the value into string with 3 leading zeros

  num_value_max_buffer[4] = 0; // set end of the string
  num_value_max_buffer[3] = num_value_max_buffer[2] + 80; // convert the digit to use small digit font
  num_value_max_buffer[2] = num_value_max_buffer[1] + 80; // convert the digit to use small digit font
  num_value_max_buffer[1] = 221; // special character for the decimal point

  num_value_min_buffer[4] = 0; // set end of the string
  num_value_min_buffer[3] = num_value_min_buffer[2] + 80; // convert the digit to use small digit font
  num_value_min_buffer[2] = num_value_min_buffer[1] + 80; // convert the digit to use small digit font
  num_value_min_buffer[1] = 221; // special character for the decimal point

  // manually copy the "max" value to the main string
  line_final[12] = num_value_max_buffer[0];
  line_final[13] = num_value_max_buffer[1];
  line_final[14] = num_value_max_buffer[2];
  line_final[15] = num_value_max_buffer[3]; 

  // manually copy the "min" value to the main string
  line_final[52] = num_value_min_buffer[0];   
  line_final[53] = num_value_min_buffer[1];   
  line_final[54] = num_value_min_buffer[2];   
  line_final[55] = num_value_min_buffer[3];         

  
  // draw the line_final to the LCD, in includes both lines
  LCD.sendString(line_final,0,0);

}
