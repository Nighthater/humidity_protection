// Libaries
#include <dhtnew.h>
#include "math.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pin Definitions
#define FAN 4       // Relay Pin

// Display Definition
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


DHTNEW SensorIn(2);
DHTNEW SensorOut(3);

uint32_t count = 0;
uint32_t start, stop;

uint32_t errors[10] = { 0,0, 0,0, 0,0, 0,0, 0,0 };

// Initializing Sensors


// Measuring Variables
float h = 0.0;
float t = 0.0;

float h_out = 0.0;
float t_out = 0.0;

// Calculating Variables
float absolute_humidity_in = 0.0;
float absolute_humidity_out = 0.0;

// Var before loop
float abs_hum_before_activation = 0.0;

//cycles counters
int cycles_inside = 0;
int cycles_outside = 0;


void setup() {
  Serial.begin(9600);
  Serial.println("1");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  
  // Defines the Output of the Fan (Relay Pin) and the Leds
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FAN, OUTPUT);
  Serial.println("2");

  // Relay is ACTIVE LOW, leds Active HIGH
  digitalWrite(FAN, HIGH);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 25);
  display.println(F("Initializing"));
  display.display(); 
  delay(1000);

  display.clearDisplay();
  display.drawLine(63, 0, 63, 63, WHITE);
  display.drawLine(0, 10, 127, 10, WHITE);
  display.setCursor(13, 1);
  display.println(F("Inside"));
  display.setCursor(74, 1);
  display.println(F("Outside"));
  display.display();
  Serial.println("3");
  
}

void loop() {
  display.clearDisplay();
  display.drawLine(63, 0, 63, 63, WHITE);
  display.drawLine(0, 10, 127, 10, WHITE);
  display.display();
  delay(500);
  display.setCursor(13, 1);
  display.println(F("Inside"));
  display.setCursor(74, 1);
  display.println(F("Outside"));
  display.display();
  delay(500);
  display.display();
  Serial.println("4");
  
  
  // FAN OFF
  digitalWrite(FAN, HIGH);

  delay(1500);
  start = micros();
  int chk = SensorIn.read();
  stop = micros();
  delay(1500);
  Serial.println("5");
  switch (chk)
  {
    case DHTLIB_OK:
      errors[0]++;
      cycles_inside = 0;
      break;
    case DHTLIB_ERROR_CHECKSUM:
      errors[1]++;
      cycles_inside++;
      break;
    case DHTLIB_ERROR_TIMEOUT_A:
      errors[2]++;
      cycles_inside++;
      break;
    case DHTLIB_ERROR_TIMEOUT_B:
      errors[3]++;
      cycles_inside++;
      break;
    case DHTLIB_ERROR_TIMEOUT_C:
      errors[4]++;
      cycles_inside++;
      break;
    case DHTLIB_ERROR_TIMEOUT_D:
      errors[5]++;
      cycles_inside++;
      break;
    case DHTLIB_ERROR_SENSOR_NOT_READY:
      cycles_inside++;
      errors[6]++;
       break;
    case DHTLIB_ERROR_BIT_SHIFT:
      errors[7]++;
      cycles_inside++;
       break;
    default:
      errors[8]++;
      cycles_inside++;
      break;
  }
  Serial.println("6");
  if(cycles_inside==0){
    display.fillRect(0, 11, 62, 50, BLACK);
    
    h = SensorIn.getHumidity();
    delay(50);
    t = SensorIn.getTemperature();
    
    display.setCursor(10,16);
    display.println(t);
    display.setCursor(10,26);
    display.println(h);
    delay(1000);
    absolute_humidity_in = (6.112*pow(2.71828182,((17.67*float(t))/(float(t)+243.5)))*float(h)*18.02 )/((273.15+float(t))*100*0.08314);
    display.setCursor(10,36);
    display.println(absolute_humidity_in);
    display.display();
    delay(2000);
  }
  else{
    display.fillRect(0, 11, 62, 50, BLACK);
    display.setCursor(10,16);
    display.setTextColor(WHITE);
    display.print(F("ERR"));
    display.setCursor(10,26);
    display.print(F("ERR"));
    display.setCursor(10,36);
    display.print(F("ERR"));
    display.setCursor(10,46);
    display.print(cycles_inside);
    display.display();
    delay(2000);
  }



  
  start = micros();
  chk = SensorOut.read();
  stop = micros();

  delay(1500);
  
  switch (chk)
  {
    case DHTLIB_OK:
      errors[0]++;
      cycles_outside = 0;
      break;
    case DHTLIB_ERROR_CHECKSUM:
      errors[1]++;
      cycles_outside++;
      break;
    case DHTLIB_ERROR_TIMEOUT_A:
      errors[2]++;
      cycles_outside++;
      break;
    case DHTLIB_ERROR_TIMEOUT_B:
      errors[3]++;
      cycles_outside++;
      break;
    case DHTLIB_ERROR_TIMEOUT_C:
      errors[4]++;
      cycles_outside++;
      break;
    case DHTLIB_ERROR_TIMEOUT_D:
      errors[5]++;
      cycles_outside++;
      break;
    case DHTLIB_ERROR_SENSOR_NOT_READY:
      errors[6]++;
      cycles_outside++;
      break;
    case DHTLIB_ERROR_BIT_SHIFT:
      errors[7]++;
      cycles_outside++;
      break;
    default:
      errors[8]++;
      cycles_outside++;
      break;
  }

   if(cycles_outside==0){
    display.fillRect(65, 11, 60, 50, BLACK);
    
    h_out = SensorOut.getHumidity();
    delay(50);
    t_out = SensorOut.getTemperature();
    
    display.setCursor(74,16);
    display.println(t_out);
    display.setCursor(74,26);
    display.println(h_out);
    delay(1000);
    absolute_humidity_out = (6.112*pow(2.71828182,((17.67*float(t_out))/(float(t_out)+243.5)))*float(h_out)*18.02 )/((273.15+float(t_out))*100*0.08314);
    display.setCursor(74,36);
    display.println(absolute_humidity_out);
    display.display();
    delay(2000);
  }
  else{
    display.fillRect(65, 11, 60, 50, BLACK);
    display.setCursor(74,16);
    display.print(F("ERR"));
    display.setCursor(74,26);
    display.print(F("ERR"));
    display.setCursor(74,36);
    display.print(F("ERR"));
    display.setCursor(74,46);
    display.print(cycles_outside);
    display.display();
    delay(2000);
    
  }
delay(1000);
  



  // ELECTRICAL STUFFS


  
   if ( h >= 70)
  {
    if (absolute_humidity_in >= absolute_humidity_out) // IF hum_in >=70% and inside greater than out, than alarm
    {
      display.clearDisplay();
      display.setCursor(20, 25);
      display.println(F("CASE YELLOW\n   CAN'T CIRCULATE"));
      display.display();
      delay(3000);
      return;
    }
  }
  else
  {
    return;
  }

  
  
    
  if (absolute_humidity_in - absolute_humidity_out >= 1.0)
  {
    display.clearDisplay();
    display.setCursor(20, 25);
    display.println(F("FAN ON"));
    display.display();
    delay(1000);
  }
  else
  {
    display.clearDisplay();
    display.setCursor(20, 25);
    display.println(F("CASE YELLOW\n   CAN'T CIRCULATE"));
    display.display();
    delay(3000);
    return;
  }
  

  // Relay is ACTIVE LOW
  digitalWrite(FAN, LOW);

  // For Loop lasts 10 Minutes, enough to exchange all air (including Lounge) at least 2-3 times
  
  for (int n = 0; n<=600; n++)
  {     
    display.clearDisplay();
    display.setCursor(20, 25);
    display.println(F("FAN ON..."));
    display.display();
    delay(1000);
  } 







  //draw stuff anew

  // loop

}
