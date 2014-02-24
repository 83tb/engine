// Kuba Kucharski
// 2013

/*

Enable Pin - 13
Freq - 5
Boost - 6
Reset - 7

Log: 



*/


#include <OneWire.h>
#include <DallasTemperature.h>


#include <Wire.h>
#include <SerialCommand.h>



#include "Adafruit_MCP23017.h"


Adafruit_MCP23017 mcp;


#define MAIN_PWM 10
#define ANALOG_PWM 9
#define RCPIN 0
#define RCAPIN 1

#define ENABLE 13
#define FREQ 5
#define BOOST 6
#define RESET 7

#define ONE_WIRE_BUS 2

SerialCommand sCmd;     // obiekt komunikacji Serial
//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);



void pulse(int high) {
 
  
  for (int steps = 20; steps < high; steps++) { 
    // turn the pin on:
  analogWrite(MAIN_PWM, steps);
  analogWrite(ANALOG_PWM, steps);
  delay(10);
 
  }
  
  for (int steps = high; steps > 20; steps--) { 
    // turn the pin on:
  analogWrite(MAIN_PWM, steps);
  analogWrite(ANALOG_PWM, steps);
  delay(10);
 
  }
  
  
 
  
  
}




void setup() {
       // Ustawiam PIN jako wyjscie analogowe
     // Dioda wylaczona
  pinMode(MAIN_PWM, OUTPUT); 
 
 
  // zmien to gdy przeniesiesz sie za szmita
   
  pinMode(ENABLE, OUTPUT);  
  pinMode(FREQ, OUTPUT);  
  pinMode(BOOST, OUTPUT);  
  pinMode(RESET, OUTPUT);  
  digitalWrite(ENABLE, HIGH); 
  digitalWrite(BOOST, HIGH); 
  
  
 
  digitalWrite(RESET, HIGH); 
  
  pulse(40);
  pulse(40);
  pulse(40);
 
  
  Wire.begin();
  
 setup_input(0x20);
 setup_output(0x21);
 
  
 //setup_output(0x68);
  
  
  

  Serial.begin(9600);

  // Setup callbacks for SerialCommand commands
  sCmd.setDefaultHandler(unrecognized);      // co jesli nie ma takiej komendy]
  
  sCmd.addCommand("spf", setPwmAndFreq);
  sCmd.addCommand("sdl", setDigitalLevel);

  sCmd.addCommand("sa", setAnalogPwm);
  sCmd.addCommand("rc", readCurrent);
  sCmd.addCommand("rca", readCurrentAvg);
  
  sCmd.addCommand("enbl", enable);
  sCmd.addCommand("dsbl", disable);
  
  sCmd.addCommand("bst", boost);
  sCmd.addCommand("unbst", unboost);
   sCmd.addCommand("rst", resetBoard);
   
  //sCmd.addCommand("ffu", flipFreqUp);
  //sCmd.addCommand("ffd", flipFreqDown);
  
   
  sCmd.addCommand("#", comment);
  sCmd.addCommand("ww", writeWire);
  
  sCmd.addCommand("mt", match);
  
  
  sCmd.addCommand("h", help);
  
  sCmd.addCommand("i2c", i2c_scan);
  //sCmd.addCommand("talk", talk);
 
  sCmd.addCommand("sir", sir);
  sCmd.addCommand("sio", sio);
  
  //sCmd.addCommand("temp", temperature);
  
  
  //sensors.begin();
  
  Serial.println("-------------------\n"); 
  Serial.println("POWERMANAGER, @83TB\n"); 
  Serial.println("Log: Engine console initialized. Ready when you are.");
  Serial.println("Hint: Type h for help");
  
}


/*
void temperature() {
 sensors.requestTemperatures();
 
  Serial.print("Info: {'temperature':'");
  
  
  Serial.print(sensors.getTempCByIndex(0));  
  Serial.println("'}");

  
}
*/

void help(){
 
 Serial.println("");
 
 Serial.println("Hint: sdl: sets digital level (automatic freq)");
 Serial.println("Hint: spf: sets pwm and frequency");
 Serial.println("Hint: sa: sets analog PWM");
 Serial.println("Hint: bst: boost ON");
 Serial.println("Hint: ubst: boost OFF");
 Serial.println("Hint: enbl: turn ON");
 Serial.println("Hint: dsbl: turn OFF");
 Serial.println("Hint: rst: resets LED control board");
 //Serial.println("Hint: ffu: FREQ ON");
 //Serial.println("Hint: ffd: FREQ OFF");
  
 Serial.println("Hint: h: prints help");
 Serial.println("Hint: rc: reads current");
 Serial.println("Hint: rca: reads average current");
 Serial.println("Hint: i2c: i2c scanner");
 Serial.println("Hint: temp: thermal sensor on pin 2 - one wire, DS18B20");
 Serial.println("Hint: talk: talks to i2c");
 Serial.println("Hint: sir: talks to i2c, takes 3 arguments");
 
 
 
 Serial.println("\nHint: Undocumented");
 Serial.println("Hint: rw: readWire");
 Serial.println("Hint: ww: writeWire");
 
 
 
 
 
 
 Serial.println("\n");
 

 
 
 
 
 
  
}



void i2c_scan() {
  byte error, address;
  int nDevices;

  Serial.println("Log: Scanning");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("Info: I2C device found at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
        

      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Info: Unknown error at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("Info: No I2C devices found\n");
  else
    Serial.println("Log: I2C scan completed\n");

 
  
}
  
void loop() {
  sCmd.readSerial();     // Przetwarzanie, to wszystko co dzieje sie w petli
}



void writeWire() {
 
  Wire.beginTransmission(39); // transmit to device #44 (0x2f)
                              // device address is specified in datasheet
  Wire.write(byte(0x00));            // sends instruction byte  
  Wire.write(40);             // sends potentiometer value byte  
  Wire.endTransmission();     // stop transmitting

}


void readWire() {
 
   Wire.requestFrom(39, 1);    // request 6 bytes from slave device #2

   while(Wire.available())    // slave may send less than requested
  { 
    char c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }
  delay(500);
}




void setPwmAndFreq() {
  int aNumber;
  char *arg;

  // Serial.println("We're in setFrequency");
  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atoi(arg);    // Konwertuje char na int
  //  Serial.print("First argument was: ");
    
    setPwmFrequency(MAIN_PWM, aNumber);
    
  }
  else {
    Serial.println("No arguments");
  }

  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atol(arg);
   // Serial.print("Second argument was: ");
   
    analogWrite(MAIN_PWM, aNumber);
    
    
  }
  else {
    Serial.println("No second argument");
  }
}



void setDigitalLevel() {
  char *arg;
  arg = sCmd.next();    // Przyklad z buforowaniem, w ten sposob mozemy przesylac argumenty
  if (arg != NULL) {    
    
   
    int aNumber = atoi(arg);
    int DIVISOR;
    // 0- 7 Divisor 256
    // 8-63 Divisor 64
    // 64 - 192, Divisor 8
    // 193 - 248 Divisor 64
    // 249 - 255 Divisor 256
    
    switch (aNumber) {
      case 0 ... 7:    // your hand is on the sensor
      DIVISOR = 256;
      break;
      case 8 ... 63:    // your hand is close to the sensor
      DIVISOR = 64;
      break;
    case 64 ... 192:    // your hand is a few inches from the sensor
      DIVISOR = 8;
      break;
    case 193 ... 248:    // your hand is nowhere near the sensor
      DIVISOR = 64;
      break;
    case 249 ... 255:    // your hand is nowhere near the sensor
      DIVISOR = 256;
      break;
      
      
     } 
  
    
    setPwmFrequency(MAIN_PWM, DIVISOR);

    analogWrite(MAIN_PWM, aNumber);

    
    
    
  
  
  }
  else {
    Serial.println("Please, add an argument 0-255");
  }
}






void setAnalogPwm() {
  char *arg;
  arg = sCmd.next();    // Przyklad z buforowaniem, w ten sposob mozemy przesylac argumenty
  if (arg != NULL) {    

 
    int aNumber = atoi(arg);
   
  
    
    analogWrite(ANALOG_PWM, aNumber);
    Serial.println("Log: Analog PWM has been set");
    
    
    
  
  
  }
  else {
    Serial.println("Please, add an argument 0-255");
  }
}







void readCurrent() {
  float val;
  val = analogRead(RCPIN);    // read the input pin
  
  float A = val*2;
  
  Serial.print("Info: {'current':'");
  
  
  Serial.print(val);  
  Serial.println("'}");
  
  
}




void comment() {
 
  
}


// nie rozpoznano komendy
void unrecognized(const char *command) {
  Serial.println("What? I don't know this command. ");
}




void enable() {
  Serial.println("Log: Lamp On");
  digitalWrite(ENABLE, HIGH);
}

void disable() {
  Serial.println("Log: Lamp Off");
   digitalWrite(ENABLE, LOW);
}



void boost() {
  Serial.println("Log: Boost On");
  digitalWrite(BOOST, LOW);
  
}

void unboost() {
  Serial.println("Log: Boost Off");
   digitalWrite(BOOST, HIGH);
   resetBoard();
}


void flipFreqUp() {
  Serial.println("Log: FREQ UP");
  digitalWrite(FREQ, HIGH);
}

void flipFreqDown() {
  Serial.println("Log: FREQ DOWN");
  digitalWrite(FREQ, LOW);
}



void resetBoard() {
  Serial.println("Log: Board resetted");
  

 
   
   digitalWrite(RESET, LOW);
   delay(100);
   digitalWrite(RESET, HIGH);
 
}


void readCurrentAvg() {
  float val;
  val = analogRead(RCAPIN);    // read the input pin
  
   Serial.print("Info: {'avg_current':'");
  
  
  Serial.print(val);  
  Serial.println("'}");
  
  
}

/**
 * Divides a given PWM pin frequency by a divisor.
 * 
 * The resulting frequency is equal to the base frequency divided by
 * the given divisor:
 *   - Base frequencies:
 *      o The base frequency for pins 3, 9, 10, and 11 is 31250 Hz.
 *      o The base frequency for pins 5 and 6 is 62500 Hz.
 *   - Divisors:
 *      o The divisors available on pins 5, 6, 9 and 10 are: 1, 8, 64,
 *        256, and 1024.
 *      o The divisors available on pins 3 and 11 are: 1, 8, 32, 64,
 *        128, 256, and 1024.
 * 
 * PWM frequencies are tied together in pairs of pins. If one in a
 * pair is changed, the other is also changed to match:
 *   - Pins 5 and 6 are paired on timer0
 *   - Pins 9 and 10 are paired on timer1
 *   - Pins 3 and 11 are paired on timer2
 * 
 * Note that this function will have side effects on anything else
 * that uses timers:
 *   - Changes on pins 3, 5, 6, or 11 may cause the delay() and
 *     millis() functions to stop working. Other timing-related
 *     functions may also be affected.
 *   - Changes on pins 9 or 10 will cause the Servo library to function
 *     incorrectly.
 * 
 * Thanks to macegr of the Arduino forums for his documentation of the
 * PWM frequency divisors. His post can be viewed at:
 *   http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1235060559/0#4
 */
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}



#define MCP23017_ADDRESS 0x20

// registers
#define MCP23017_IODIRA 0x00
#define MCP23017_IPOLA 0x02
#define MCP23017_GPINTENA 0x04 // 1 to interrupts
#define MCP23017_DEFVALA 0x06  // 1 to interrupts
#define MCP23017_INTCONA 0x08 // 1 to interrupts, if it's set to 0 it's compared to value, if 0 it's for default value
#define MCP23017_IOCONA 0x0A
#define MCP23017_GPPUA 0x0C // tez na 1
#define MCP23017_INTFA 0x0E
#define MCP23017_INTCAPA 0x10
#define MCP23017_GPIOA 0x12
#define MCP23017_OLATA 0x14 


#define MCP23017_IODIRB 0x01
#define MCP23017_IPOLB 0x03
#define MCP23017_GPINTENB 0x05
#define MCP23017_DEFVALB 0x07
#define MCP23017_INTCONB 0x09
#define MCP23017_IOCONB 0x0B
#define MCP23017_GPPUB 0x0D
#define MCP23017_INTFB 0x0F
#define MCP23017_INTCAPB 0x11
#define MCP23017_GPIOB 0x13
#define MCP23017_OLATB 0x15


void talk() {
  // The LED will 'echo' the button
  
  mcp.begin(0x27);      // use default address 0
  mcp.pinMode(3, INPUT);
  mcp.pullUp(3, HIGH);  // turn on a 100K pullup internally
  Serial.println(digitalRead(3));
  
}




void sir() {
  
  
  int address;
  int registry;
  int packet;
  char *arg;

  // Serial.println("We're in setFrequency");
  arg = sCmd.next();
  if (arg != NULL) {
   
  //  Serial.print("First argument was: ");
    address = strtol(arg, NULL, 16);
     // transmit to device #44 (0x2f)
    
  }
  else {
    Serial.println("No arguments");
  }

  arg = sCmd.next();
  if (arg != NULL) {
     registry = strtol(arg, NULL, 16);
   // Serial.print("Second argument was: ");
   
   
    
    
  }
  else {
    Serial.println("No second argument");
  }
  
  
  arg = sCmd.next();
  if (arg != NULL) {
    packet = strtol(arg, NULL, 16);
   // Serial.print("Second argument was: ");
   
   
    
    
  }
  else {
    Serial.println("No third argument");
  }
  
  
  
  
   Wire.beginTransmission(address);
   Serial.print("Adres urzadzenia: ");
   Serial.println(address);
   Serial.print("Adres rejestru: ");
   Serial.println(registry);
   
   Wire.write(registry); 
   
   Serial.print("Wysylamy pakiet: ");
   Serial.println(packet);
   Wire.write(packet);    
   
   Serial.println("Odczytano: ");
   Wire.endTransmission(); 
   Wire.requestFrom(address, 1); 
   
   while(Wire.available())    // slave may send less than requested
  { 
    char c = Wire.read(); // receive a byte as character
    Serial.println(c, BIN);
      Serial.println(c);

    // print the character
  }
  delay(500);
     
  
}





void sio() {
  
  
  int address;
  int registry;
  int packet;
  char *arg;

  // Serial.println("We're in setFrequency");
  arg = sCmd.next();
  if (arg != NULL) {
   
  //  Serial.print("First argument was: ");
    address = strtol(arg, NULL, 16);
     // transmit to device #44 (0x2f)
    
  }
  else {
    Serial.println("No arguments");
  }

  arg = sCmd.next();
  if (arg != NULL) {
     registry = strtol(arg, NULL, 16);
   // Serial.print("Second argument was: ");
   
   
    
    
  }
  else {
    Serial.println("No second argument");
  }
  
  /*
  arg = sCmd.next();
  if (arg != NULL) {
    long packet = strtol(arg, NULL, 16);
   // Serial.print("Second argument was: ");
   
   
    
    
  }
  else {
    Serial.println("No third argument");
  }
  
  */
  
  
   Wire.beginTransmission(address);
   Serial.print("Adres urzadzenia: ");
   Serial.println(address);
   Serial.print("Adres rejestru: ");
   Serial.println(registry);
   
   Wire.write(registry); 
   //Wire.write(packet);    
   
   Serial.println("Odczytano: ");
   Wire.endTransmission();  
   Wire.requestFrom(address, 1); 
   
   while(Wire.available())    // slave may send less than requested
  { 
    char c = Wire.read(); // receive a byte as character
    Serial.println(c, BIN);
      Serial.println(c);
  }
  delay(500);
  
  
}
  
  
  

void setup_output(char ad)
{

 
 // setup device
 
 Wire.begin(); // wake up I2C bus
// set I/O pins to outputs
 Wire.beginTransmission(ad);
 Wire.write(0x00); // IODIRA register
 Wire.write(0x00); // set all of port A to outputs
 Wire.endTransmission();
 Wire.beginTransmission(0xad);
 Wire.write(0x01); // IODIRB register
 Wire.write(0x00); // set all of port B to outputs
 Wire.endTransmission();

}



void setup_input(char ad)
{

 
 // setup device
 
 Wire.begin(); // wake up I2C bus
// set I/O pins to outputs
 Wire.beginTransmission(ad);
 Wire.write(0x04); 
 Wire.write(0xFF); 
 Wire.endTransmission();
 
 Wire.beginTransmission(ad);
 Wire.write(0x06); 
 Wire.write(0xFF); 
 Wire.endTransmission();
 
 Wire.beginTransmission(ad);
 Wire.write(0x08);
 Wire.write(0xFF); 
 Wire.endTransmission();
 
 Wire.beginTransmission(ad);
 Wire.write(0x0C); 
 Wire.write(0xFF); 
 Wire.endTransmission();
 



}



void match()
{
 
 Wire.requestFrom(0x20, 12); 
 char c;  
 while(Wire.available())    // slave may send less than requested
    { 
    c = Wire.read(); // receive a byte as character
    Serial.println(c, BIN);
      Serial.println(c);
  }
 Wire.beginTransmission(0x21);
 Wire.write(0x12); // GPIOA
 
 Wire.write(c); // port A
 Wire.endTransmission();
 Wire.beginTransmission(0x21);
 Wire.write(0x13); // GPIOB
 Wire.write(c); // port B
 Wire.endTransmission();
 delay(100);
 
}



