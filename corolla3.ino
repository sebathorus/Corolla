#include <Stepper.h>
#include <Adafruit_NeoPixel.h>

#define oneMotorTurn 32 //number of steps required to turn motor one complete turn
#define oneOutputTurn 32 * 64 //number of steps required to turn output shaft of the reductor one turn
#define myledPin 3

Stepper motor_stepper(oneMotorTurn, 8, 10, 9, 11);// define stepper 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(36, myledPin, NEO_RGB + NEO_KHZ800); //define RGB LEDs strip

int brightness;// light intensity
int maxbrightness = 42;
int maxRatio;
int colorTemp;//color intensity used to change light color
int red_ratio = 6;
int green_ratio = 5;
int blue_ratio = 1;
int red_led=1;// value for RED of RGB LED
int green_led=1;// value for GREEN of RGB LED
int blue_led=1;// value for BLUE of RGB LED

int pot1Pin = A2;//used to change petals position
int pot2Pin = A1;//used to change light intensity
int pot3Pin = A0;//used to change color of RGB Leds
int feedbackpotPin = A3;//used to determine petals position

int switch1Pin1 = 2;
int switch1Pin2 = 4;
int switch1Pin3 = 5;
int switch1val1 = 0;
int switch1val2 = 0;
int switch1val3 = 0;

int pot1RawValue = 0;  //variables to store the values coming from the potentiometers
int pot2RawValue = 0;
int pot3RawValue = 0;
int pot1Value = 0;//variables to store the values coming from the potentiometers mapped on different ranges
int pot2Value = 0;
int pot3Value = 0;
int feedbackpotValue = 0;
int stepper_turn = 0;


void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(myledPin, OUTPUT);
  pinMode(pot1Pin, INPUT);
  pinMode(pot2Pin, INPUT);
  pinMode(pot3Pin, INPUT);
  pinMode(feedbackpotPin, INPUT);
  pinMode(switch1Pin1, INPUT);
  pinMode(switch1Pin2, INPUT);
  pinMode(switch1Pin3, INPUT);
  strip.begin();//start RGB LEDs
  strip.show();
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

void loop() {
  // read the value from the potentiometers
  pot1RawValue = analogRead(pot1Pin);
  pot2RawValue = analogRead(pot2Pin);
  pot3RawValue = analogRead(pot3Pin);

  switch1val1 = digitalRead(switch1Pin1);
  switch1val2 = digitalRead(switch1Pin2);
  switch1val3 = digitalRead(switch1Pin3);
  
  feedbackpotValue = analogRead(feedbackpotPin);
  Serial.print("Pot1 = ");
  Serial.println(pot1Value);
  Serial.print("Pot2 = ");
  Serial.println(pot2Value);
  Serial.print("Pot3 = ");
  Serial.println(pot3Value);
  Serial.print("Feedbackpot = ");
  Serial.println(feedbackpotValue);

  if ((switch1val1 == 0)&&(switch1val2 == 0)&&(switch1val3 == 1))//start switch1 position1
    {

      pot1Value = map(pot1RawValue, 0, 1023, 0, 250);
      brightness = map(pot2RawValue, 0, 1023, 0, maxbrightness);
      colorTemp = map(pot3RawValue, 0, 1023, 0, 15);
      
      if (pot1RawValue < 300)//if the value read from the first potentiometer, pot1, is below this value, petals are closing
        {
          stepper_turn = - oneOutputTurn;
          Serial.println("Close");
        }
      if ((pot1RawValue < 300) && (feedbackpotValue < 690))//if the value read from the feedback potentiometer, is below this value (690 in this case), stepper is stopped to not damage the mechanics and/or motor
        {
          stepper_turn = 0;
          Serial.println("Close stopped");
        }
  
      if (pot1RawValue > 600)//if the value read from the first potentiometer, pot1, is above this value, petals are opening
        {
          stepper_turn = oneOutputTurn;
          Serial.println("Open");
        }
      if ((pot1RawValue > 600) && (feedbackpotValue > 780))//if the value read from the feedback potentiometer, is above this value (750 in this case), stepper is stopped to not damage the mechanics and/or motor
        {
          stepper_turn = 0;
          Serial.println("Open stopped");
        }
  
      if ((pot1RawValue > 300) && (pot1RawValue < 600))
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
    }//end switch1 position1

  if ((switch1val1 == 0)&&(switch1val2 == 1)&&(switch1val3 == 1))//start switch1 position 2
    {

      pot1Value = map(pot1RawValue, 0, 1023, 1, 252);
      pot2Value = map(pot2RawValue, 0, 1023, 1, 252);
      pot3Value = map(pot3RawValue, 0, 1023, 1, 252);

      Serial.print("Pot1 = ");
      Serial.println(pot1Value);
      Serial.print("Pot2 = ");
      Serial.println(pot2Value);
      Serial.print("Pot3 = ");
      Serial.println(pot3Value);
      
      red_led = pot1Value;
      green_led = pot2Value;
      blue_led = pot3Value;
      colorWipe(strip.Color(green_led, red_led, blue_led), 30);

      if (pot1Value > pot2Value)
          {
            if (pot2Value > pot3Value)
                      {
                        red_ratio = pot1Value / pot3Value;
                        green_ratio = pot2Value / pot3Value;
                        blue_ratio = 1;
                      }
            if (pot2Value < pot3Value)
                      {
                        red_ratio = pot1Value / pot2Value;
                        green_ratio = 1;
                        blue_ratio = pot3Value / pot2Value;
                        
                      }
            if (pot1Value < pot3Value)
                {
                  red_ratio = pot1Value / pot2Value;
                  green_ratio = 1;
                  blue_ratio = pot3Value / pot2Value;
                }
          }
          
        if (pot2Value > pot1Value)
          {
            if (pot1Value > pot3Value)
                {
                  red_ratio = pot1Value / pot3Value;
                  green_ratio = pot2Value / pot3Value;
                  blue_ratio = 1;
                }
            if (pot1Value < pot3Value)
                {
                  red_ratio = 1;
                  green_ratio = pot2Value / pot1Value;
                  blue_ratio = pot3Value / pot1Value;
                }
            if (pot2Value < pot3Value)
                {
                  red_ratio = 1;
                  green_ratio = pot2Value / pot1Value;
                  blue_ratio = pot3Value / pot1Value;
                }
          }
       maxRatio = max(red_ratio, green_ratio);
       maxRatio = max(maxRatio, blue_ratio);
      Serial.print("Red_ratio = ");
      Serial.println(red_ratio);
      Serial.print("green_ratio = ");
      Serial.println(green_ratio);
      Serial.print("Blue_ratio = ");
      Serial.println(blue_ratio);
      Serial.print("Max ratio = ");
      Serial.println(maxRatio);
    }//end switch1 position2

  if ((switch1val1 == 0)&&(switch1val2 == 1)&&(switch1val3 == 0))//start switch1 position 3
    {
      Serial.println("Mode 3 selected");
      pot1Value = map(pot1RawValue, 0, 1023, 0, 252);
      pot2Value = map(pot2RawValue, 0, 1023, 0, 252);
      pot3Value = map(pot3RawValue, 0, 1023, 0, 252);
      Serial.print("Pot1 = ");
      Serial.println(pot1Value);
      Serial.print("Pot2 = ");
      Serial.println(pot2Value);
      Serial.print("Pot3 = ");
      Serial.println(pot3Value);
      delay(500);
    }//end switch1 position 3

  if ((switch1val1 == 1)&&(switch1val2 == 1)&&(switch1val3 == 0))//start switch1 position 4
    {
      Serial.println("Mode 4 selected");
      delay(500);
    }//end switch1 position 4

  if ((switch1val1 == 1)&&(switch1val2 == 0)&&(switch1val3 == 0))//start switch1 position 5
    {
      Serial.println("Mode 5 selected");
      delay(500);
    }//end switch1 position 5
    
}//end loop
