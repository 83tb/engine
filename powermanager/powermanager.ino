// Kuba Kucharski
// 2013


#define I2C_DELAY 0

#include <Wire.h>
#include <SerialCommand.h>

SerialCommand sCmd; 

volatile int state = LOW;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);
  
  setup_input(0x20);
  setup_output(0x21);
  setPullUp(0x20);
  setInterruptEnable(0x20);
  setDefaultValue(0x20);
  setSeqopDisabled(0x20);
  resetInterrupts(0x20);

  sCmd.setDefaultHandler(unrecognized);      
  sCmd.addCommand("#", comment);
  sCmd.addCommand("rb", readButton);
  sCmd.addCommand("h", help);  
  sCmd.addCommand("i2c", i2c_scan); 
  sCmd.addCommand("sir", sir_func);
  sCmd.addCommand("sio", sio_func);
  sCmd.addCommand("sior", sior_func);
  sCmd.addCommand("sirr", sirr_func);
  


  
  sCmd.addCommand("read", i2c_read);
  

  Serial.println("---------------------------------------------------------\n"); 
  Serial.println("POWERMANAGER, @83TB\n"); 
  Serial.println("Log: Engine console initialized. Ready when you are.");
  Serial.println("Hint: Type h for help");
  
  
  // attachInterrupt(0, testInterrupt, FALLING); //pin 2
  
  
  
}




void loop() {
  delay(100);	
  
  int a = digitalRead(2);
  if (a==0) {
	  testInterrupt();
  }	
  delay(100);	
  
  sCmd.readSerial();     // Przetwarzanie, to wszystko co dzieje sie w petli
}


void help(){
 
 	Serial.println("");

 	Serial.println("Hint: h: prints help");
 	Serial.println("Hint: i2c: i2c scanner");
 	Serial.println("Hint: sir: talks to i2c, takes 3 arguments");
 	Serial.println("Hint: sio: fetch from i2c, takes 2 arguments");
	Serial.println("Hint: rb: read button");
	Serial.println("Hint: read: read i2c");
 	Serial.println("Hint: # : comment");
	Serial.println("\n");

}



void i2c_scan() {
  byte error, address;
  int nDevices;

  

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
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
    Serial.println("Log: Completed\n");

 
  
}
  




void comment() {
 
  
}


// nie rozpoznano komendy
void unrecognized(const char *command) {
  Serial.println("What? I don't know this command. ");
}


void sir_func() {
  
  
  int address;
  int registry;
  int packet;
  char *arg;

  
  arg = sCmd.next();
  if (arg != NULL) {
	  address = strtol(arg, NULL, 16);

  }
  else {
	  Serial.println("No arguments");
  }

  arg = sCmd.next();
  if (arg != NULL) {
	  registry = strtol(arg, NULL, 16);

    
  }
  else {
	  Serial.println("No second argument");
  }
  
  
  arg = sCmd.next();
  if (arg != NULL) {
	  packet = strtol(arg, NULL, 16);
    
  }
  else {
    Serial.println("No third argument");
  }
  
  sir(address,registry,packet);
  
}




void sir(int address,int registry,int packet) {
	
   Serial.println("---------------------------------------------------------");
   
   Wire.beginTransmission(address);
   Serial.print("Adres urzadzenia: 0x");
   Serial.print(address, HEX);
   Serial.print("    Adres rejestru: 0x");
   Serial.println(registry, HEX);
   
   Wire.write(registry); 
   
   Serial.print("Wysylamy pakiet: 0x");
   Serial.println(packet, HEX);
   Wire.write(packet);    
   
   Serial.print("Odczytano:   ");
   Wire.endTransmission(); 
   Wire.requestFrom(address, 1); 
   
   while(Wire.available())    // slave may send less than requested
  { 
    byte c = Wire.read(); // receive a byte as character
    Serial.print(c, BIN);
	Serial.print(" | 0x");
    Serial.println(c, HEX);

  }
  delay(I2C_DELAY);
     
  
}






void sirr(int address,int packet) {
	
   Serial.println("---------------------------------------------------------");
   
   Wire.beginTransmission(address);
   Serial.print("Adres urzadzenia: 0x");
   Serial.print(address, HEX);

   
   Serial.print("Wysylamy pakiet: 0x");
   Serial.println(packet, HEX);
   Wire.write(packet);    
   
   Serial.print("Odczytano:   ");
   Wire.endTransmission(); 
   Wire.requestFrom(address, 1); 
   
   while(Wire.available())    // slave may send less than requested
  { 
    byte c = Wire.read(); // receive a byte as character
    Serial.print(c, BIN);
	Serial.print(" | 0x");
    Serial.println(c, HEX);

  }
  delay(I2C_DELAY);
     
  
}


int sio(int address,int registry) {
  byte c;
   Serial.println("---------------------------------------------------------");
   Wire.beginTransmission(address);
   Serial.print("Adres urzadzenia: 0x");
   Serial.print(address, HEX);
   Serial.print("   Adres rejestru: 0x");
   Serial.println(registry, HEX);
   
   Wire.write(registry); 
  
   
   Serial.print("Odczytano:     ");
   Wire.endTransmission();  
   Wire.requestFrom(address, 1); 
   
   while(Wire.available())  
  { 
    c = Wire.read(); 
    Serial.print(c, BIN);
	Serial.print(" | 0x");
    Serial.println(c, HEX);
	
     
  }
  delay(I2C_DELAY);
  return c;
  
}




int sio_raw(int address) {
byte c;
   

   Wire.requestFrom(address, 1); 
   
   while(Wire.available())  
  { 
    c = Wire.read(); 
    
 
	
     
  }

  Serial.println(c, HEX);
  return c;
}



void ledon(int value) {
  
  // state = ~state;
  
  sir(0x21,0x12,value);
  sir(0x21,0x13,0x00);
  
}

void i2c_read() {

  	int address;
	char *arg;

	arg = sCmd.next();
	if (arg != NULL) {
   
	    address = strtol(arg, NULL, 16);
	    Wire.requestFrom(address, 1); 
   
	    while(Wire.available())  
	   { 
	     char c = Wire.read(); 
	     Serial.print(c, BIN);
	 	Serial.print(" | 0x");
	     Serial.println(c, HEX);
	
     
	   }
	   delay(I2C_DELAY);
		
		
	    
	  }
	  
	
	
	else {
	    Serial.println("Add address of the device, like 0x20");
	}
  
   
  
  
}
  
  

void sio_func() {
  
  
  int address;
  int registry;
  int packet;
  char *arg;


  arg = sCmd.next();
  if (arg != NULL) {
	  address = strtol(arg, NULL, 16);   
  }
  
  else {
	  Serial.println("No arguments");
  }

  arg = sCmd.next();
  if (arg != NULL) {
	  registry = strtol(arg, NULL, 16);
  }
  
  else {
	  Serial.println("No second argument");
  }
  
   sio(address,registry);
  
}
  



void sirr_func() {
  
  
  int address;
  int registry;
  int packet;
  char *arg;


  arg = sCmd.next();
  if (arg != NULL) {
	  address = strtol(arg, NULL, 16);   
  }
  
  else {
	  Serial.println("No arguments");
  }

  arg = sCmd.next();
  if (arg != NULL) {
	  registry = strtol(arg, NULL, 16);
  }
  
  else {
	  Serial.println("No second argument");
  }
  
   sirr(address,registry);
  
}  

void sior_func() {
  
  
  int address;
  int registry;
  int packet;
  char *arg;


  arg = sCmd.next();
  if (arg != NULL) {
	  address = strtol(arg, NULL, 16);   
  }
  



  
  else {
	  Serial.println("No arguments");
  }
  
  sio_raw(address);
  
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
sir(ad,0x02,0x00);
sir(ad,0x03,0x00);



}






void setPullUp(int ad)
{
	sir(ad,0x0c,0xff);
	sir(ad,0x0d,0xff);
	
}


// set interrupt enable
void setInterruptEnable(int ad)
{
	sir(ad,0x04,0xff);
	sir(ad,0x05,0xff);
	
}


void setDefaultValue(int ad)
{
	sir(ad,0x06,0xff);
	sir(ad,0x07,0xff);


	// set interrupt on change
	sir(ad,0x08,0xff); 
	sir(ad,0x09,0xff);
	
	
}

void setSeqopDisabled(int ad)
{
	sir(ad,0x0a,0x20);
	
}


// funkcje - czy jest przerwanie
// czy jest mag


void readButton()
{        

          
        	byte value = sio_raw(0x68);
        	Serial.println(value, HEX);
                
                parse(value);
                
                // ledon(value);
                
                
               
                // podziel na pol
                // byte
                
                // jak policzyc wartosc z value2
                // idziemy bit po bicie od najstarszeg0, pierwsze 4
                // jezeli 0 to nastepny bit
                // jezeli 1 to wlaczasz ten kawalek magistrali
                // najmlodszymi bitami ktore mozna ustawiac
                // odczytuje rejestry
                // przerwanie pochodzi z pierwszego miejsca w hirearchii
                
                
                // odczytac urzadzenia 
                // zamknij magistrale
                
                // sprawdzamy - jak nie ma przerwania - wychodzimy z petli
                
                
                
                // sirr [output
                
                
                
                
        	
      //  }
  	
        
	
}







void resetInterrupts(int ad)
{
	sio(ad,0x10);
	sio(ad,0x11);
	
	
}




void testInterrupt()
{
        
  
        Serial.println("doing");
        // interrupts();
	readButton();
        Serial.println("done");
         

	
	
}



byte reverse(byte b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}


void parse(byte data)
{
  
  byte mask;
  
  
  
  for (mask = 10000000; mask>0; mask >>= 1) { //iterate through bit mask
    if (data & mask){ // if bitwise AND resolves to true
      
      sirr(0x68, reverse(mask));
      unsigned int value = sio(0x20,0x11);;
      Serial.println(value, HEX);

      ledon(value);
      
      
    }
    
    delay(1); //delay
  }
}
