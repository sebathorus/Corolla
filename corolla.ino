#include <Stepper.h>
#include <Adafruit_NeoPixel.h>

#define oneMotorTurn 32 //number of steps required to turn motor one complete turn
#define oneOutputTurn 32 * 64 //number of steps required to turn output shaft of the reductor one turn
#define myledPin 3

Stepper motor_stepper(oneMotorTurn, 8, 10, 9, 11);// define stepper 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(36, myledPin, NEO_RGB + NEO_KHZ800); //define RGB LEDs strip

int intensity;// light intensity
int colorintensity;//color intensity used to change light color
int red_led=1;// value for RED of RGB LED
int green_led=1;// value for GREEN of RGB LED
int blue_led=1;// value for BLUE of RGB LED

int pot1Pin = A2;//used to change petals position
int pot2Pin = A1;//used to change light intensity
int pot3Pin = A0;//used to change color of RGB Leds
int feedbackpotPin = A3;//used to determine petals position
//int ledPin = 13;      //select the pin for the LED
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
  pinMode(ledPin, OUTPUT);
  pinMode(pot1Pin, INPUT);
  pinMode(pot2Pin, INPUT);
  pinMode(pot3Pin, INPUT);
  pinMode(feedbackpotPin, INPUT);
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

  pot1Value = pot1RawValue;
  pot2Value = map(pot2RawValue, 0, 1023, 0, 250);
  pot3Value = map(pot3RawValue, 0, 1023, 0, 20);
  
  feedbackpotValue = analogRead(feedbackpotPin);
  Serial.print("Pot1 = ");
  Serial.println(pot1Value);
  Serial.print("Pot2 = ");
  Serial.println(pot2Value);
  Serial.print("Pot3 = ");
  Serial.println(pot3Value);
  Serial.print("Feedbackpot = ");
  Serial.println(feedbackpotValue);

  if (pot1Value < 300)//if the value read from the first potentiometer, pot1, is below this value, petals are closing
    {
      stepper_turn = - oneOutputTurn;
      Serial.println("Close");
    }
  if ((pot1Value < 300) && (feedbackpotValue < 690))//if the value read from the feedback potentiometer, is below this value (690 in this case), stepper is stopped to not damage the mechanics and/or motor
    {
      stepper_turn = 0;
      Serial.println("Close stopped");
    }
  
  if (pot1Value > 600)//if the value read from the first potentiometer, pot1, is above this value, petals are opening
    {
      stepper_turn = oneOutputTurn;
      Serial.println("Open");
    }
  if ((pot1Value > 600) && (feedbackpotValue > 750))//if the value read from the feedback potentiometer, is above this value (750 in this case), stepper is stopped to not damage the mechanics and/or motor
    {
      stepper_turn = 0;
      Serial.println("Open stopped");
    }
  
  if ((pot1Value > 300) && (pot1Value < 600))
    {
      stepper_turn = 0;
      Serial.println("Stepper stop");
    }
  motor_stepper.setSpeed(500);   
  motor_stepper.step(stepper_turn);

  red_led = pot2Value;
  green_led = pot2Value;
  blue_led = pot2Value/pot3Value;
  colorWipe(strip.Color(red_led, green_led, blue_led), 50);
  delay(50);
}
