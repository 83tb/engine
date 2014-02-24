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

SerialCommand sCmd; 


void loop() {
  sCmd.readSerial();     // Przetwarzanie, to wszystko co dzieje sie w petli
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
  
  

 
  
  setup_input(0x20);
  setup_output(0x21);
  setPullUp(0x20);
  setInputOnChange(0x20);
  setDefaultValue(0x20);
  setSeqopDisabled(0x20);
  
  
 
  Serial.begin(9600);

  // Setup callbacks for SerialCommand commands
  sCmd.setDefaultHandler(unrecognized);      // co jesli nie ma takiej komendy]
  
  /*
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
  */
  
   
  //sCmd.addCommand("ffu", flipFreqUp);
  //sCmd.addCommand("ffd", flipFreqDown);
  
   
  sCmd.addCommand("#", comment);
  sCmd.addCommand("rb", readButton);

  

  
  
  sCmd.addCommand("h", help);
  
  sCmd.addCommand("i2c", i2c_scan);

 
  sCmd.addCommand("sir", sir_func);
  sCmd.addCommand("sio", sio_func);
  
  //sCmd.addCommand("temp", temperature);
  
  
  //sensors.begin();
  
  Serial.println("-------------------\n"); 
  Serial.println("POWERMANAGER, @83TB\n"); 
  Serial.println("Log: Engine console initialized. Ready when you are.");
  Serial.println("Hint: Type h for help");
  
}






void help(){
 
 	Serial.println("");
 
 /*
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
 */
 
 	Serial.println("Hint: h: prints help");
 	Serial.println("Hint: rc: reads current");
 	Serial.println("Hint: rca: reads average current");
 	Serial.println("Hint: i2c: i2c scanner");
 	Serial.println("Hint: temp: thermal sensor on pin 2 - one wire, DS18B20");
 	Serial.println("Hint: talk: talks to i2c");
 	Serial.println("Hint: sir: talks to i2c, takes 3 arguments");
 	Serial.println("Hint: sio: fetch from i2c, takes 2 arguments");
 	Serial.println("Hint: # : comment");
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


/*




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



void temperature() {
 sensors.requestTemperatures();
 
  Serial.print("Info: {'temperature':'");
  
  
  Serial.print(sensors.getTempCByIndex(0));  
  Serial.println("'}");

  
}
*/




void sir_func() {
  
  
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
     

    // print the character
  }
  delay(500);
     
  
}




void sir(int address,int registry,int packet) {
  
  



	  /*
  address = strtol(address_, NULL, 16);
  registry = strtol(registry_, NULL, 16);
  packet = strtol(packet_, NULL, 16);
  */
 
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
     

    // print the character
  }
  delay(500);
     
  
}



void sio(int address,int registry) {


	  /*
   address = strtol(address_, NULL, 16);

   registry = strtol(registry_, NULL, 16);
 */
  
   Wire.beginTransmission(address);
   Serial.print("Adres urzadzenia: ");
   Serial.println(address);
   Serial.print("Adres rejestru: ");
   Serial.println(registry);
   
   Wire.write(registry); 
  
   
   Serial.println("Odczytano: ");
   Wire.endTransmission();  
   Wire.requestFrom(address, 1); 
   
   while(Wire.available())    // slave may send less than requested
  { 
    char c = Wire.read(); // receive a byte as character
    Serial.println(c, BIN);
     
  }
  delay(500);
  
  
}
  

void sio_func() {
  
  
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
     
  }
  delay(500);
  
  
}
  
  
  

void setup_output(char ad)
{

sir(ad,0x00,0x00);
sir(ad,0x01,0x00);


}



void setup_input(char ad)
{

sir(ad,0x00,0xff);
sir(ad,0x01,0xff);
sir(ad,0x02,0xff);
sir(ad,0x03,0xff);



}






void setPullUp(int ad)
{
	sir(ad,0x0c,0xff);
	sir(ad,0x0d,0xff);
	
}


void setInputOnChange(int ad)
{
	sir(ad,0x04,0xff);
	sir(ad,0x05,0xff);
	
}


void setDefaultValue(int ad)
{
	sir(ad,0x06,0xff);
	sir(ad,0x07,0xff);

	sir(ad,0x08,0xff); // set interupt when value against DEFVAL (INTCONn):
	
}

void setSeqopDisabled(int ad)
{
	sir(ad,0x0a,0x20);
	
}


void readButton()
{
	sio(0x20,0x10);
	sio(0x20,0x11);
	sio(0x20,0x12);
	sio(0x20,0x13);
	sio(0x20,0x14);
	sio(0x20,0x15);
	sio(0x20,0x16);
	
	
}







// odczyt klawisza
// sio 0x20 0x12 // GPIOA 0x20 A - stan aktualny
// sio 0x20 0x10 // stan z przerwania


