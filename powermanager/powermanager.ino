// Kuba Kucharski
// 2013


#define I2C_DELAY 0

#include <Wire.h>
#include <SerialCommand.h>

SerialCommand sCmd; 

// volatile int state = LOW;

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
  sCmd.addCommand("read", i2c_read);
  

  Serial.println("---------------------------------------------------------\n"); 
  Serial.println("POWERMANAGER, @83TB\n"); 
  Serial.println("Log: Engine console initialized. Ready when you are.");
  Serial.println("Hint: Type h for help");
  
  
  attachInterrupt(0, testInterrupt, FALLING); //pin 2
  
  
  
}




void loop() {
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
    char c = Wire.read(); // receive a byte as character
    Serial.print(c, BIN);
	Serial.print(" | 0x");
    Serial.println(c, HEX);

  }
  delay(I2C_DELAY);
     
  
}



void sio(int address,int registry) {

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
    char c = Wire.read(); 
    Serial.print(c, BIN);
	Serial.print(" | 0x");
    Serial.println(c, HEX);
	
     
  }
  delay(I2C_DELAY);
  
  
}




void sio_raw(int address,int registry) {

   
   Wire.beginTransmission(address);
   
   Wire.write(registry); 
  
   
 
   Wire.endTransmission();  
   Wire.requestFrom(address, 1); 
   
   while(Wire.available())  
  { 
    char c = Wire.read(); 
    
 
	
     
  }

  
  
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


void readButton()
{
	sio(0x20,0x10);
	
	sio(0x20,0x11);
	
	
}


void resetInterrupts(int ad)
{
	sio(ad,0x10);
	sio(ad,0x11);
	
	
}




void testInterrupt()
{
        
        // 
	
        digitalWrite(13, HIGH); 
        Serial.println("doing");
        interrupts();
        detachInterrupt(0);
        readButton();
        Serial.println("done");
        digitalWrite(13, LOW); 
        attachInterrupt(0, testInterrupt, FALLING);
	
	
}



