// Kuba Kucharski
// 2013


// GENERAL SETTINGS

#define I2C_DELAY 0



// ------------------------------- CONFIGURATION

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
#define EXP_TAB_LENGTH	7
#define SEG_TAB_LENGTH	2




byte exp_seg_int_chip_[] = {EXP_SEG0_CHIP0_ADDR, EXP_SEG1_CHIP0_ADDR};	
byte exp_seg_int_reg_[]  = {EXP_REG_GPIOB, EXP_REG_GPIOB};	

/*
// module klawiatura_A address in i2c segments
// chwilowo zakladam, ze w jednym segmencie i2c jest tylko 1 rejestr przerwan, na 1 expanderze
// tu, inaczej niz w przypadku przyciskow, interesuje mnie aktualny stan przerwan, 
// jezeli nie ma przerwania, odczytamy 0xFF (aktywny stan przerwan to low)
// w takim wypadku nastapi skasowanie przerwania wysylanego do sterownika magistrali
// UWAGA!
// stan bitow w rejestrze odzwierciedla stan wejsc, (high = 1, low = 0)
// inaczej niz na PCA9545, 
// gdzie stanowi low na wejsciu przerwan odpowiada stan bitu 1 w rejestrze.
// Jak chcesz, mozna to zmienic w konfiguracji MCP23017
*/


byte i2cseg_[] = {I2CSEG0, I2CSEG1, I2CSEG2, I2CSEG3};	// tablica adresowania segmentow magistrali

byte exp_interrupt_src_[SEG_TAB_LENGTH][EXP_TAB_LENGTH]   = {
			     {EXP_SEG0_CHIP0_ADDR, EXP_SEG0_CHIP1_ADDR, EXP_SEG0_CHIP1_ADDR, EXP_SEG0_CHIP2_ADDR, EXP_SEG0_CHIP2_ADDR, EXP_SEG0_CHIP3_ADDR, EXP_SEG0_CHIP3_ADDR},
			     {EXP_SEG1_CHIP0_ADDR, EXP_SEG1_CHIP1_ADDR, EXP_SEG1_CHIP1_ADDR, EXP_SEG1_CHIP2_ADDR, EXP_SEG1_CHIP2_ADDR, EXP_SEG1_CHIP3_ADDR, EXP_SEG1_CHIP3_ADDR},
			    };
				
byte exp_intcap_register_[SEG_TAB_LENGTH][EXP_TAB_LENGTH] = {
			     {EXP_REG_INTCAPA, EXP_REG_INTCAPA, EXP_REG_INTCAPB, EXP_REG_INTCAPA, EXP_REG_INTCAPB, EXP_REG_INTCAPA, EXP_REG_INTCAPB},
			     {EXP_REG_INTCAPA, EXP_REG_INTCAPA, EXP_REG_INTCAPB, EXP_REG_INTCAPA, EXP_REG_INTCAPB, EXP_REG_INTCAPA, EXP_REG_INTCAPB},
			    };
				
byte exp_intf_register_[SEG_TAB_LENGTH][EXP_TAB_LENGTH]   = {
			     {EXP_REG_INTFA, EXP_REG_INTFA, EXP_REG_INTFB, EXP_REG_INTFA, EXP_REG_INTFB, EXP_REG_INTFA, EXP_REG_INTFB
			     },
			     {EXP_REG_INTFA, EXP_REG_INTFA, EXP_REG_INTFB, EXP_REG_INTFA, EXP_REG_INTFB, EXP_REG_INTFA, EXP_REG_INTFB
			     }
			    };

byte exp_seg0_chips_input_[] = {EXP_SEG0_CHIP0_ADDR, EXP_SEG0_CHIP1_ADDR, EXP_SEG0_CHIP2_ADDR, EXP_SEG0_CHIP3_ADDR};
byte exp_seg1_chips_input_[] = {EXP_SEG1_CHIP0_ADDR, EXP_SEG1_CHIP1_ADDR, EXP_SEG1_CHIP2_ADDR, EXP_SEG1_CHIP3_ADDR, EXP_SEG1_CHIP4_ADDR, EXP_SEG1_CHIP5_ADDR};
byte exp_seg1_chips_output_[] = {EXP_SEG1_CHIP4_ADDR, EXP_SEG1_CHIP5_ADDR};


//  ------------------------------- END OF CONFIGURATION


// EVALUATED

int exp_seg0_chips_input_size = sizeof(exp_seg0_chips_input_)/sizeof(byte);
int exp_seg1_chips_input_size = sizeof(exp_seg1_chips_input_)/sizeof(byte);
int exp_seg1_chips_output_size = sizeof(exp_seg1_chips_output_)/sizeof(byte);


// Initialization and library loading

#include <Wire.h>
#include <SerialCommand.h>

SerialCommand sCmd; 

byte i2cseg;							// "adres" obslugiwanego segmentu magistrali
int i2cseg_index;						// index do tablicy adresowej segmentow magistrali
int expint_index;						// index wiazacy przerwania z expanderami w segmenie magistrali
int button_index;						// index (finalnie numer) klawisza, ktory wywolal przerwanie
byte expander_addr;		// i2c adres expandera, zrodla przerwania
byte expander_regf;		// interrupt flag register
byte expander_regd;		// interrupt capture register



void setup_all() {
	/* setting up all the devices found in the tables */

	sirr(I2CSWITCH_ADDR, I2CSEG0);

	for(int i = 0; i <  exp_seg0_chips_input_size; i++) {
		setup_input(i);
		setPullUp(i);
  	 	setInterruptEnable(i);
		setDefaultValue(i);
		setSeqopDisabled(i);
		resetInterrupts(i);
	}

	sirr(I2CSWITCH_ADDR, I2CSEG1);

	for(int i = 0; i < exp_seg1_chips_input_size; i++) { 
		setup_input(i);
		setPullUp(i);
		setInterruptEnable(i);
		setDefaultValue(i);
		setSeqopDisabled(i);
		resetInterrupts(i);
	}

	for(int i = 0; i < exp_seg1_chips_output_size; i++) { 
  	  	setup_output(i);
	}

}


int i2cseg_find(){
	byte intseg = sio_raw(I2CSWITCH_ADDR);
	byte mask = B00010000;
	int success_code = 0;
	int tsize = SEG_TAB_LENGTH;
	i2cseg_index = 0; // zaczynamy od zera

	for (i2cseg_index;i2cseg_index<tsize;i2cseg_index++) {
  		if (intseg & mask){
			i2cseg = i2cseg_[i2cseg_index];
			success_code = 1;
			break;
		}
  
  	mask <<=1;
	}
	
	return success_code;
}





int chip_find() {
	int success_code = 0;
	byte exp_int_chip = exp_seg_int_chip_[i2cseg_index];
	byte exp_int_reg = exp_seg_int_reg_[i2cseg_index];
	sirr(I2CSWITCH_ADDR,i2cseg);			// otwarcie segmentu magistrali
	byte expint = sio(exp_int_chip, exp_int_reg);		// odczyt rejestr przerwan w segmencie
	int tsize = EXP_TAB_LENGTH;

	expint = ~expint;					

	byte mask = B10000000;
	expint_index = 0;

	if (expint){
		for (expint_index;expint_index<tsize;++expint_index) {
			if (expint & mask){
				success_code = 1;
				break;
			}
  			mask >>=1;
		}
	}

	return success_code;
}



int button_find() {
	expander_addr = exp_interrupt_src_[i2cseg_index][expint_index];
	expander_regf = exp_intf_register_[i2cseg_index][expint_index];
	expander_regd = exp_intcap_register_[i2cseg_index][expint_index];
	byte int_flag = sio(expander_addr, expander_regf);
	byte button   = sio(expander_addr, expander_regd);
	button        = ~button;
	button_index  = 0;
	int success_code = 0;



	if (int_flag){		
		for (byte mask = B00000001;mask;mask <<=1) {
			if (button & mask){
      		 	success_code = 1;
      		  	break;
    		}
    		++button_index;
  	  	}
	}
	return success_code;
}  



void addConsole() {
	sCmd.setDefaultHandler(unrecognized);      
	sCmd.addCommand("#", comment);

	sCmd.addCommand("h", help);  
	sCmd.addCommand("i2c", i2c_scan); 
	sCmd.addCommand("sir", sir_func);
	sCmd.addCommand("sio", sio_func);
	sCmd.addCommand("sior", sior_func);
	sCmd.addCommand("sirr", sirr_func);


	Serial.println("---------------------------------------------------------\n"); 
	Serial.println("POWERMANAGER, @83TB\n"); 
	Serial.println("Log: Engine console initialized. Ready when you are.");
	Serial.println("Hint: Type h for help");
	
}


// Main Atmega Setup

void setup() {
	Serial.begin(9600);
	Wire.begin();
	pinMode(2, INPUT);
	pinMode(13, OUTPUT);

	setup_all();
	addConsole();

}




void loop() {
	delay(100);	

	int a = digitalRead(2);
	if (a==0) {
  	//testInterrupt();
	
		if (i2cseg_find()) {
			if (chip_find()) {
				if (button_find()) {
					Serial.println();
					Serial.println("Huge success !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
					Serial.println();
					Serial.println();
					Serial.print("i2cseg_index: ");
					Serial.println(i2cseg_index);
					Serial.print("expint_index: ");
					Serial.println(expint_index);
					Serial.print("button_index: ");
					Serial.println(button_index);
					Serial.println("=====================");
				}
			}
		}
	}	

	delay(100);	
	sCmd.readSerial();     // Przetwarzanie, to wszystko co dzieje sie w petli

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

	while(Wire.available()) {
		 
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

	while(Wire.available()) { 
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

	while(Wire.available()) { 
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

	while(Wire.available()) { 
		c = Wire.read(); 
	}

	Serial.println(c, HEX);
	return c;
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


void resetInterrupts(int ad)
{
	sio(ad,0x10);
	sio(ad,0x11);


}



// ADDITIONAL FUNCTIONS

void i2c_scan() {
	byte error, address;
	int nDevices;



	nDevices = 0;
	for(address = 1; address < 127; address++ ) {
		Wire.beginTransmission(address);
		error = Wire.endTransmission();

		if (error == 0) {
			Serial.print("Info: I2C device found at address 0x");
  		  	if (address<16) Serial.print("0");
  		  	Serial.println(address,HEX);
    

  		  	nDevices++;
		}
		else if (error==4) 
		{
  			Serial.print("Info: Unknown error at address 0x");
  		if (address<16) Serial.print("0");
			Serial.println(address,HEX);
		}    
	}

	if (nDevices == 0) Serial.println("Info: No I2C devices found\n");
	else Serial.println("Log: Completed\n");

}


void comment() {}


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


// nie rozpoznano komendy
void unrecognized(const char *command) {
	Serial.println("What? I don't know this command. ");
}



