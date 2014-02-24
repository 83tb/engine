// Kuba Kucharski
// 2013



#include <Wire.h>
#include <SerialCommand.h>

SerialCommand sCmd; 

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  setup_input(0x20);
  setup_output(0x21);
  setPullUp(0x20);
  setInputOnChange(0x20);
  setDefaultValue(0x20);
  setSeqopDisabled(0x20);

  sCmd.setDefaultHandler(unrecognized);      // co jesli nie ma takiej komendy]
  sCmd.addCommand("#", comment);
  sCmd.addCommand("rb", readButton);
  sCmd.addCommand("h", help);  
  sCmd.addCommand("i2c", i2c_scan); 
  sCmd.addCommand("sir", sir_func);
  sCmd.addCommand("sio", sio_func);

  Serial.println("-------------------\n"); 
  Serial.println("POWERMANAGER, @83TB\n"); 
  Serial.println("Log: Engine console initialized. Ready when you are.");
  Serial.println("Hint: Type h for help");
  
}




void loop() {
  sCmd.readSerial();     // Przetwarzanie, to wszystko co dzieje sie w petli
}


void help(){
 
 	Serial.println("");

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

  
   Wire.beginTransmission(address);
   Serial.print("Adres urzadzenia: ");
   Serial.println(address);
   Serial.print("Adres rejestru: ");
   Serial.println(registry);
   
   Wire.write(registry); 
  
   
   Serial.println("Odczytano: ");
   Wire.endTransmission();  
   Wire.requestFrom(address, 1); 
   
   while(Wire.available())  
  { 
    char c = Wire.read(); 
    Serial.println(c, BIN);
     
  }
  delay(500);
  
  
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

	sir(ad,0x08,0xff); 
	
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




