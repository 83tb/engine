// Kuba Kucharski
// 2013


#define I2C_DELAY 0

#include <Wire.h>
#include <SerialCommand.h>

SerialCommand sCmd; 



  #define I2CSEG0 B00000001
  #define I2CSEG1 B00000010
  #define I2CSEG2 B00000100		// nc
  #define I2CSEG3 B00001000		// nc

  #define I2CSWITCH_ADDR 0x68
  
  #define EXP_SEG0_CHIP0_ADDR 0x20	// remote module klawiatura_A      -     4 x 2 przyciski
  #define EXP_SEG0_CHIP1_ADDR 0x21	// remote module klawiatura_B (+C) - 2 x 4 x 2 przyciski
  #define EXP_SEG0_CHIP2_ADDR 0x22	// remote module klawiatura_B (+C) - 2 x 4 x 2 przyciski
  #define EXP_SEG0_CHIP3_ADDR 0x23	// remote module klawiatura_B (+C) - 2 x 4 x 2 przyciski
//  #define EXP_SEG0_CHIP4_ADDR 0x24	// n/a
//  #define EXP_SEG0_CHIP5_ADDR 0x25	// n/a
//  #define EXP_SEG0_CHIP6_ADDR 0x26	// n/a
//  #define EXP_SEG0_CHIP7_ADDR 0x27	// n/a

  #define EXP_SEG1_CHIP0_ADDR 0x20	// local module klawiatura_A      -     4 x 2 przyciski
  #define EXP_SEG1_CHIP1_ADDR 0x21	// local module klawiatura_B (+C) - 2 x 4 x 2 przyciski
  #define EXP_SEG1_CHIP2_ADDR 0x22	// local module klawiatura_B (+C) - 2 x 4 x 2 przyciski
  #define EXP_SEG1_CHIP3_ADDR 0x23	// local module klawiatura_B (+C) - 2 x 4 x 2 przyciski
  #define EXP_SEG1_CHIP4_ADDR 0x24	// local module klawiatura_O expander 0
  #define EXP_SEG1_CHIP5_ADDR 0x25	// local module klawiatura_O expander 1
//  #define EXP_SEG1_CHIP6_ADDR 0x26	// n/a
//  #define EXP_SEG1_CHIP7_ADDR 0x27	// n/a

  #define EXP_REG_IODIRA    0x00
  #define EXP_REG_IODIRB    0x01
  #define EXP_REG_IPOLA     0x02
  #define EXP_REG_IPOLB     0x03
  #define EXP_REG_GPINTENA  0x04
  #define EXP_REG_GPINTENB  0x05
  #define EXP_REG_DEFVALA   0x06
  #define EXP_REG_DEFVALB   0x07
  #define EXP_REG_INTCONA   0x08
  #define EXP_REG_INTCONB   0x09
  #define EXP_REG_IOCON     0x0A
  #define EXP_REG_IOCON     0x0B
  #define EXP_REG_GPPUA     0x0C
  #define EXP_REG_GPPUB     0x0D
  #define EXP_REG_INTFA     0x0E
  #define EXP_REG_INTFB     0x0F
  #define EXP_REG_INTCAPA   0x10
  #define EXP_REG_INTCAPB   0x11
  #define EXP_REG_GPIOA     0x12
  #define EXP_REG_GPIOB     0x13
  #define EXP_REG_OLATA     0x14
  #define EXP_REG_OLATB     0x15
  #define EXP_LENGTH		7


  
  

  byte exp_seg_int_chip_[] = {EXP_SEG0_CHIP0_ADDR, EXP_SEG1_CHIP0_ADDR};	// module klawiatura_A address in i2c segments
  byte exp_seg_int_reg_[]  = {EXP_REG_GPIOB, EXP_REG_GPIOB};	// chwilowo zakladam, ze w jednym segmencie i2c jest tylko 1 rejestr przerwan, na 1 expanderze
						// tu, inaczej niz w przypadku przyciskow, interesuje mnie aktualny stan przerwan, 
						// jezeli nie ma przerwania, odczytamy 0xFF (aktywny stan przerwan to low)
						// w takim wypadku nastapi skasowanie przerwania wysylanego do sterownika magistrali
						// UWAGA!
						// stan bitow w rejestrze odzwierciedla stan wejsc, (high = 1, low = 0)
						// inaczej niz na PCA9545, 
						// gdzie stanowi low na wejsciu przerwan odpowiada stan bitu 1 w rejestrze.
						// Jak chcesz, mozna to zmienic w konfiguracji MCP23017

  byte i2cseg_[] = {I2CSEG0, I2CSEG1, I2CSEG2, I2CSEG3};	// tablica adresowania segmentow magistrali
  byte i2cseg;							// "adres" obslugiwanego segmentu magistrali
  int i2cseg_index;						// index do tablicy adresowej segmentow magistrali
  int expint_index;						// index wiazacy przerwania z expanderami w segmenie magistrali
  int button_index;						// index (finalnie numer) klawisza, ktory wywolal przerwanie
  
  
  byte exp_interrupt_src_[2][7]   = {
                      {EXP_SEG0_CHIP0_ADDR, EXP_SEG0_CHIP1_ADDR, EXP_SEG0_CHIP1_ADDR, EXP_SEG0_CHIP2_ADDR, EXP_SEG0_CHIP2_ADDR, EXP_SEG0_CHIP3_ADDR, EXP_SEG0_CHIP3_ADDR},
				   {EXP_SEG1_CHIP0_ADDR, EXP_SEG1_CHIP2_ADDR, EXP_SEG1_CHIP1_ADDR, EXP_SEG1_CHIP2_ADDR, EXP_SEG1_CHIP2_ADDR, EXP_SEG1_CHIP3_ADDR, EXP_SEG1_CHIP3_ADDR},
				  };
  byte exp_intcap_register_[2][7] = {{EXP_REG_INTCAPA, EXP_REG_INTCAPA, EXP_REG_INTCAPB, EXP_REG_INTCAPA, EXP_REG_INTCAPB, EXP_REG_INTCAPA, EXP_REG_INTCAPB},
				   {EXP_REG_INTCAPA, EXP_REG_INTCAPA, EXP_REG_INTCAPB, EXP_REG_INTCAPA, EXP_REG_INTCAPB, EXP_REG_INTCAPA, EXP_REG_INTCAPB},
				  };
  byte exp_intf_register_[2][7]   = {{EXP_REG_INTFA, EXP_REG_INTFA, EXP_REG_INTFB, EXP_REG_INTFA, EXP_REG_INTFB, EXP_REG_INTFA, EXP_REG_INTFB
				   },
				   {EXP_REG_INTFA, EXP_REG_INTFA, EXP_REG_INTFB, EXP_REG_INTFA, EXP_REG_INTFB, EXP_REG_INTFA, EXP_REG_INTFB
				   }
				  };

  byte expander_addr;		// i2c adres expandera, zrodla przerwania
  byte expander_regf;		// interrupt flag register
  byte expander_regd;		// interrupt capture register






  
	
	




int i2cseg_find(){
  byte intseg = sio_raw(I2CSWITCH_ADDR);
  byte mask = B00010000;
  int success_code = 0;
  int tsize = sizeof(i2cseg_)/sizeof(*i2cseg_);
  
  
  i2cseg_index = 0; // zaczynamy od zera
  
  for (i2cseg_index;i2cseg_index<tsize;i2cseg_index++) 
  {
	  if (intseg & mask){
		  success_code = 1;
	  
    	  Serial.print("segment 0: ");
    	  Serial.println(intseg, BIN);
    	  i2cseg = i2cseg_[i2cseg_index];	//  i2cseg = I2CSEG0;
    	  success_code = 1;
		  
		  break;
	  
	  }
	  
  	  mask <<=1;
	  
	  
  }
  
  return success_code;
  
 
}


int button_find() {
  byte int_flag = sio(expander_addr, expander_regf);
  int success_code = 0;
  expander_addr = exp_interrupt_src_[i2cseg_index][expint_index];
  expander_regf = exp_intf_register_[i2cseg_index][expint_index];
  expander_regd = exp_intcap_register_[i2cseg_index][expint_index];
  byte button = sio(expander_addr, expander_regd);
  
  button_index = 0;
  
  
  if (int_flag){		
	  for (byte mask = BB00000001;mask;mask <<=1) 
	  {
	    
	    ++button_index;
	    
	  }
	  success_code = 1;
  }
  
  return success_code;
}  


int chip_find() {
  int success_code = 0;
  byte exp_int_chip = exp_seg_int_chip_[i2cseg_index];
  byte exp_int_reg = exp_seg_int_reg_[i2cseg_index];
  sirr(I2CSWITCH_ADDR,i2cseg);			// otwarcie segmentu magistrali
  byte expint = sio(exp_int_chip, exp_int_reg);		// odczyt rejestr przerwan w segmencie
  int tsize = EXP_LENGTH;
  expint = ~expint;					
  
  byte mask = B10000000;
  expint_index = 0;
  
  if (expint){

  	for (expint_index;expint_index<tsize;expint_index++) {
  
  		if (expint & mask){
    		Serial.print("interrupt from expander 0: ");
    		Serial.println(expint, BIN);
			
    		success_code = 1;		
	
		}
		
  
  	  mask >>=1;			

  
      }
  
 

}

 return success_code;
 
}



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
	  //testInterrupt();
  		if (i2cseg_find()) {
  		if (chip_find()) {
  			if (button_find()) {
  				Serial.println("Huge success");
  			}
  		}
  	}
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





