#include <Stepper.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "rgb_lcd.h"

#define oneMotorTurn 32 //number of steps required to turn motor one complete turn
#define oneOutputTurn 32 * 64 //number of steps required to turn output shaft of the reductor one turn
#define halfOutputTurn 32 * 32 //number of steps required to turn output shaft of the reductor a half turn
#define myledPin 3

rgb_lcd lcd;
String lcdString;
Stepper motor_stepper(oneMotorTurn, 8, 10, 9, 11);// define stepper control pins
Adafruit_NeoPixel strip = Adafruit_NeoPixel(36, myledPin, NEO_RGB + NEO_KHZ800); //define RGB LEDs strip

//int petalPos;
int brightness;// light intensity
int maxbrightness = 42;//maximum brightness allowed to met condition maxbrightness * maxRatio <=255. I put 42 as start value to match the start values for red, green and ratios.
int maxRatio;//biggest color ratio - used to calculate maxbrightness
int colorTemp;//color intensity used to change light color
int red_ratio = 6;
int green_ratio = 5;
int blue_ratio = 1;
int red_led=1;// value for RED of RGB LED
int green_led=1;// value for GREEN of RGB LED
int blue_led=1;// value for BLUE of RGB LED
int myPixels[36] = {};
int red_lcd=110;// value for RED of RGB LED - I choose these default value for a pale orange backlight color
int green_lcd=90;// value for GREEN of RGB LED
int blue_lcd=10;// value for BLUE of RGB LED

int pot1Pin = A2;//first control potentiometer
int pot2Pin = A1;//second control potentiometer
int pot3Pin = A0;//third control potentiometer
int feedbackpotPin = A3;//feedback potentiometer, used to determine petals position

int switch1Pin1 = 2;//pins from linear switch
int switch1Pin2 = 4;
int switch1Pin3 = 5;
int switch1val1 = 0;
int switch1val2 = 0;
int switch1val3 = 0;
int selectedMode = 1;

int pot1RawValue1, pot1RawValue2, pot1RawValue3, pot1RawValue5;  //variables to store the values coming from the potentiometers
int pot2RawValue1, pot2RawValue2, pot2RawValue3, pot2RawValue5;
int pot3RawValue1, pot3RawValue2, pot3RawValue3, pot3RawValue5;
int patternSelector, patternVar1, patternVar2;
int ledPot1Value = 0;//variables to store the values coming from the potentiometers mapped on different ranges
int ledPot2Value = 0;
int ledPot3Value = 0;
int lcdPot1Value = 0;//variables to store the values coming from the potentiometers mapped on different ranges
int lcdPot2Value = 0;
int lcdPot3Value = 0;
int feedbackpotValue = 0;
int stepper_turn = 0;

void setup() {
  pinMode(myledPin, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(pot1Pin, INPUT);
  pinMode(pot2Pin, INPUT);
  pinMode(pot3Pin, INPUT);
  pinMode(feedbackpotPin, INPUT);
  pinMode(switch1Pin1, INPUT);
  pinMode(switch1Pin2, INPUT);
  pinMode(switch1Pin3, INPUT);
  strip.begin();//start RGB LEDs
  strip.show();
  lcd.begin(16, 2);
  lcd.setRGB(red_lcd, green_lcd, blue_lcd);
  Serial.begin(9600);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void progressive(uint32_t c, uint8_t ledrow) 
     {
      Serial.print("Led row = ");
      Serial.println(ledrow);
      for (int x = 0; x<=ledrow; x++)
      {
      strip.setPixelColor(x, c);
      strip.setPixelColor(x+6, c);
      strip.setPixelColor(x+12, c);
      strip.setPixelColor(x+18, c);
      strip.setPixelColor(x+24, c);
      strip.setPixelColor(x+30, c);
      strip.show();
      }
      if (ledrow <= 4)
          {
            strip.setPixelColor(ledrow+1, strip.Color(0, 0, 0));
            strip.setPixelColor(ledrow+7, strip.Color(0, 0, 0));
            strip.setPixelColor(ledrow+13, strip.Color(0, 0, 0));
            strip.setPixelColor(ledrow+19, strip.Color(0, 0, 0));
            strip.setPixelColor(ledrow+25, strip.Color(0, 0, 0));
            strip.setPixelColor(ledrow+31, strip.Color(0, 0, 0));
            strip.show();
          }
  }

void loop() {
  
  switch1val1 = digitalRead(switch1Pin1);// read the value from the switch
  switch1val2 = digitalRead(switch1Pin2);
  switch1val3 = digitalRead(switch1Pin3);
  
  Serial.print("Selected mode = ");
  Serial.println(selectedMode);

   //each switch position corespond to a combination of one, two or three pins, so is required to check all three pins at once
   if ((switch1val1 == 0)&&(switch1val2 == 0)&&(switch1val3 == 1))//start switch1 position1 - change petals position, brightness and light temperature
    {
      pot1RawValue1 = analogRead(pot1Pin);// read the value from the potentiometers
      pot2RawValue1 = analogRead(pot2Pin);
      pot3RawValue1 = analogRead(pot3Pin);
      feedbackpotValue = analogRead(feedbackpotPin);
      Serial.print("pot1RawValue1 = ");
      Serial.println(pot1RawValue1);
      Serial.print("pot2RawValue1 = ");
      Serial.println(pot2RawValue1);
      Serial.print("pot3RawValue1 = ");
      Serial.println(pot3RawValue1);
      Serial.print("Feedbackpot = ");
      Serial.println(feedbackpotValue);
      lcd.clear();
      lcd.setCursor(5,0);//coordinates are (column, row) - 0,0 is upper left
      lcd.write("Mode 1");
      if (selectedMode == 2)
          {
            if ((pot1RawValue1 < pot1RawValue2-20) || (pot1RawValue1 > pot1RawValue2+20) || (pot2RawValue1 < pot2RawValue2-20) || (pot2RawValue1 > pot2RawValue2+20) || (pot3RawValue1 < pot3RawValue2-20) || (pot3RawValue1 > pot3RawValue2+20))//I put this condition to not change the Mode parameters when switch from other Mode
              {
                selectedMode = 1;
              }
          }
       if (selectedMode == 3)
          {
            if ((pot1RawValue1 < pot1RawValue3-20) || (pot1RawValue1 > pot1RawValue3+20) || (pot2RawValue1 < pot2RawValue3-20) || (pot2RawValue1 > pot2RawValue3+20) || (pot3RawValue1 < pot3RawValue3-20) || (pot3RawValue1 > pot3RawValue3+20))//I put this condition to not change the Mode parameters when switch from other Mode
              {
                selectedMode = 1;
              }
          }
        if (selectedMode == 1)
          {
            brightness = map(pot2RawValue1, 0, 1023, 0, maxbrightness);
            colorTemp = map(pot3RawValue1, 0, 1023, 0, 15);
            
            Serial.print("brightness = ");
            Serial.println(brightness);
            Serial.print("colorTemp = ");
            Serial.println(colorTemp);
          
            if (pot1RawValue1 < 300)//if the value read from the first potentiometer, pot1, is below this value, petals are closing
              {
                stepper_turn = - halfOutputTurn;
                Serial.println("Close");
              }
            if ((pot1RawValue1 < 300) && (feedbackpotValue < 690))//if the value read from the feedback potentiometer, is below this value (690 in this case), stepper is stopped to not damage the mechanics and/or motor
              {
                stepper_turn = 0;
                Serial.println("Close stopped");
              }
        
            if (pot1RawValue1 > 600)//if the value read from the first potentiometer, pot1, is above this value, petals are opening
              {
                stepper_turn = halfOutputTurn;
                Serial.println("Open");
              }
            if ((pot1RawValue1 > 600) && (feedbackpotValue > 780))//if the value read from the feedback potentiometer, is above this value (750 in this case), stepper is stopped to not damage the mechanics and/or motor
              {
                stepper_turn = 0;
                Serial.println("Open stopped");
              }
        
            if ((pot1RawValue1 > 300) && (pot1RawValue1 < 600))
             {
                stepper_turn = 0;
                Serial.println("Stepper stop");
             }
        
            motor_stepper.setSpeed(500);   
            motor_stepper.step(stepper_turn);
      
            red_led = (brightness * red_ratio) - colorTemp;
            green_led = (brightness * green_ratio) - colorTemp;
            blue_led = (brightness * blue_ratio) + colorTemp;
            colorWipe(strip.Color(green_led, red_led, blue_led), 30);
        }
     delay(300);//this delay is needed for LCD display. Without delay the display is very dim
    }//end switch1 position1

  if ((switch1val1 == 0)&&(switch1val2 == 1)&&(switch1val3 == 1))//start switch1 position 2 - change LEDs colors
    {
      pot1RawValue2 = analogRead(pot1Pin);// read the value from the potentiometers
      pot2RawValue2 = analogRead(pot2Pin);
      pot3RawValue2 = analogRead(pot3Pin);
      lcd.clear();
      lcd.setCursor(5,0);//coordinates are (column, row) - 0,0 is upper left
      lcd.write("Mode 2");
      
      if (selectedMode == 1)
          {
            if ((pot1RawValue2 < pot1RawValue1-20) || (pot1RawValue2 > pot1RawValue1+20) || (pot2RawValue2 < pot2RawValue1-20) || (pot2RawValue2 > pot2RawValue1+20) || (pot3RawValue2 < pot3RawValue1-20) || (pot3RawValue2 > pot3RawValue1+20))
              {
                selectedMode = 2;
              }
          }
       if (selectedMode == 3)
          {
            if ((pot1RawValue2 < pot1RawValue3-20) || (pot1RawValue2 > pot1RawValue3+20) || (pot2RawValue2 < pot2RawValue3-20) || (pot2RawValue2 > pot2RawValue3+20) || (pot3RawValue2 < pot3RawValue3-20) || (pot3RawValue2 > pot3RawValue3+20))
              {
                selectedMode = 2;
              }
          }
        if (selectedMode == 2)
          {
            ledPot1Value = map(pot1RawValue2, 0, 1023, 1, 252);
            ledPot2Value = map(pot2RawValue2, 0, 1023, 1, 252);
            ledPot3Value = map(pot3RawValue2, 0, 1023, 1, 252);
          /*Serial.print("Pot1 = ");
          Serial.println(pot1Value);
          Serial.print("Pot2 = ");
          Serial.println(pot2Value);
          Serial.print("Pot3 = ");
          Serial.println(pot3Value);*/
            red_led = ledPot1Value;
            green_led = ledPot2Value;
            blue_led = ledPot3Value;
    
            lcdString = "R" + String(red_led) + " G " + String(green_led) + " B " + String(blue_led);
            lcd.setCursor(0,1);
            lcd.print(lcdString);
          
            colorWipe(strip.Color(green_led, red_led, blue_led), 30);
            
          // start calculate the colors ratios
             if (ledPot1Value > ledPot2Value)
                  {
                    if (ledPot2Value > ledPot3Value)
                              {
                                red_ratio = ledPot1Value / ledPot3Value;
                                green_ratio = ledPot2Value / ledPot3Value;
                                blue_ratio = 1;
                              }
                    if (ledPot2Value < ledPot3Value)
                              {
                                red_ratio = ledPot1Value / ledPot2Value;
                                green_ratio = 1;
                                blue_ratio = ledPot3Value / ledPot2Value;
                                
                              }
                    if (ledPot1Value < ledPot3Value)
                        {
                          red_ratio = ledPot1Value / ledPot2Value;
                          green_ratio = 1;
                          blue_ratio = ledPot3Value / ledPot2Value;
                        }
                  }
              if (ledPot2Value > ledPot1Value)
                  {
                    if (ledPot1Value > ledPot3Value)
                        {
                          red_ratio = ledPot1Value / ledPot3Value;
                          green_ratio = ledPot2Value / ledPot3Value;
                          blue_ratio = 1;
                        }
                    if (ledPot1Value < ledPot3Value)
                        {
                          red_ratio = 1;
                          green_ratio = ledPot2Value / ledPot1Value;
                          blue_ratio = ledPot3Value / ledPot1Value;
                        }
                    if (ledPot2Value < ledPot3Value)
                        {
                          red_ratio = 1;
                          green_ratio = ledPot2Value / ledPot1Value;
                          blue_ratio = ledPot3Value / ledPot1Value;
                        }
                  }
             // end calculate the colors ratios
          maxRatio = max(red_ratio, green_ratio);
          maxRatio = max(maxRatio, blue_ratio);//calculate the biggest ratio
          maxbrightness = 255/maxRatio;//calculate the maximum brightness
          /*Serial.print("Red_ratio = ");
          Serial.println(red_ratio);
          Serial.print("green_ratio = ");
          Serial.println(green_ratio);
          Serial.print("Blue_ratio = ");
          Serial.println(blue_ratio);
          Serial.print("Max ratio = ");
          Serial.println(maxRatio);
          Serial.print("Max brightness = ");
          Serial.println(maxbrightness);*/
         }
      delay(300);
    }//end switch1 position2

  if ((switch1val1 == 0)&&(switch1val2 == 1)&&(switch1val3 == 0))//start switch1 position 3 - change LCD display background color
    {
      pot1RawValue3 = analogRead(pot1Pin);// read the value from the potentiometers
      pot2RawValue3 = analogRead(pot2Pin);
      pot3RawValue3 = analogRead(pot3Pin);
      lcd.clear();
      lcd.setCursor(5,0);//column, row - 0,0 is upper left
      lcd.write("Mode 3");
      lcdString = "R" + String(red_lcd) + " G " + String(green_lcd) + " B " + String(blue_lcd);
      lcd.setCursor(0,1);
      lcd.print(lcdString);

      if (selectedMode == 1)
          {
            if ((pot1RawValue3 < pot1RawValue1-20) || (pot1RawValue3 > pot1RawValue1+20) || (pot2RawValue3 < pot2RawValue1-20) || (pot2RawValue3 > pot2RawValue1+20) || (pot3RawValue3 < pot3RawValue1-20) || (pot3RawValue3 > pot3RawValue1+20))
              {
                selectedMode = 3;
              }
          }
      if (selectedMode == 2)
          {
            if ((pot1RawValue3 < pot1RawValue2-20) || (pot1RawValue3 > pot1RawValue2+20) || (pot2RawValue3 < pot2RawValue2-20) || (pot2RawValue3 > pot2RawValue2+20) || (pot3RawValue3 < pot3RawValue2-20) || (pot3RawValue3 > pot3RawValue2+20))
              {
                selectedMode = 3;
              }
          }
      if (selectedMode == 3)
          {       
                Serial.println("Mode 3 selected");
                red_lcd = map(pot1RawValue3, 0, 1023, 1, 252);
                green_lcd = map(pot2RawValue3, 0, 1023, 1, 252);
                blue_lcd = map(pot3RawValue3, 0, 1023, 1, 252);
                Serial.print("Red LCD = ");
                Serial.println(red_lcd);
                Serial.print("Green LCD = ");
                Serial.println(green_lcd);
                Serial.print("Blue LCD = ");
                Serial.println(blue_lcd);
                lcd.setRGB(red_lcd, green_lcd, blue_lcd);
                delay(500);
                selectedMode = 3;
             }
      delay(300);
    }//end switch1 position 3

  if ((switch1val1 == 1)&&(switch1val2 == 1)&&(switch1val3 == 0))//start switch1 position 4
    {
      Serial.println("Mode 4 selected");
      delay(500);
    }//end switch1 position 4

  if ((switch1val1 == 1)&&(switch1val2 == 0)&&(switch1val3 == 0))//start switch1 position 5
    {
      Serial.println("Mode 5 selected");
      lcd.clear();
      lcd.setCursor(5,0);//column, row - 0,0 is upper left
      lcd.write("Mode 5");
      pot1RawValue5 = analogRead(pot1Pin);// read the value from the potentiometers
      pot2RawValue5 = analogRead(pot2Pin);
      pot3RawValue5 = analogRead(pot3Pin);

      patternSelector = map(pot1RawValue5, 0, 1023, 1, 5);
      Serial.print("pattern = ");
      Serial.println(patternSelector);
      if (patternSelector == 1)
          {
            patternVar1 = map(pot2RawValue5, 0, 1023, 0, maxbrightness);
            patternVar2 =  map(pot3RawValue5, 0, 1023, 0, 5);
            
            red_led = (patternVar1 * red_ratio) - colorTemp;
            green_led = (patternVar1 * green_ratio) - colorTemp;
            blue_led = (patternVar1 * blue_ratio) + colorTemp;
            
            progressive(strip.Color(red_led, green_led, blue_led), patternVar2);
          }
      if (patternSelector == 2)//random pixels
          {
            patternVar1 = map(pot2RawValue5, 0, 1023, 0, maxbrightness);
            patternVar2 =  pot3RawValue5;
            
            red_led = (patternVar1 * red_ratio) - colorTemp;
            green_led = (patternVar1 * green_ratio) - colorTemp;
            blue_led = (patternVar1 * blue_ratio) + colorTemp;

            int randomPixel = random(36);
            if (myPixels[randomPixel] == 0)
                {
                  strip.setPixelColor(randomPixel, strip.Color(red_led, green_led, blue_led));
                  myPixels[randomPixel] = 1;
                }
            randomPixel = random(36);
            if (myPixels[randomPixel] == 1)
                {
                  strip.setPixelColor(randomPixel, strip.Color(0, 0, 0));
                  myPixels[randomPixel] = 0;
                }
            strip.show();
            delay(patternVar2);
          }
      if (patternSelector == 3)//random pixels, random colors
          {
            patternVar1 = map(pot2RawValue5, 0, 1023, 0, maxbrightness);
            patternVar2 =  pot3RawValue5;
            
            int rand_red_ratio = random(6);
            int rand_green_ratio = random(6);
            int rand_blue_ratio = random(6);
            
            red_led = (patternVar1 * rand_red_ratio);
            green_led = (patternVar1 * rand_green_ratio);
            blue_led = (patternVar1 * rand_blue_ratio);

            for (int x = 0; x < 6; x++)
                {
                  progressive(strip.Color(red_led, green_led, blue_led), x);
                  delay(patternVar2);
                }
            delay(patternVar2);
          }
      if (patternSelector == 4)
          {
            patternVar1 = map(pot2RawValue5, 0, 1023, 0, 252);
            patternVar2 =  map(pot2RawValue5, 0, 1023, 0, 5);
          }
      if (patternSelector == 5)
          {
            patternVar1 = map(pot2RawValue5, 0, 1023, 0, 252);
            patternVar2 =  map(pot2RawValue5, 0, 1023, 0, 5);
          }
      delay(300);
    }//end switch1 position 5
    
}//end loop
