/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */


#include <Wire.h>
#include <SerialCommand.h>

  #define I2CSEG0 00000001
  #define I2CSEG1 00000010
  #define I2CSEG2 00000100		// nc
  #define I2CSEG3 00001000		// nc

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





  void setup() {   
	            
  }

  void loop() {
	
	
  	// glowna petla
	
	if (i2cseg_find()) {
		if (chip_find()) {
			if (button_find()) {
				Serial.println("Huge success");
			}
		}
	}
	
	
	
  }



int i2cseg_find(){
  byte intseg = sio_raw(I2CSWITCH_ADDR)
  byte mask = 00010000;
  int success_code = 0;
  int size = sizeof(i2cseg_index)/sizeof(*i2cseg_index)
  
  
  i2cseg_index = 0; // zaczynamy od zera
  
  for (i2cseg_index,i2cseg_index<size,i2cseg_index++) {
	  if (intseg & mask){
		  success_code = 1;
	  
    	  Serial.print("segment 0: ");
    	  Serial.println(intseg, BIN);
    	  i2cseg = i2cseg_[i2cseg_ind]	//  i2cseg = I2CSEG0;
    	  success_code = 1;
		  
		  break;
	  
	  }
	  
  	  mask <<=1;
	  
	  
  }
  
  return success_code;
  
 
}

int chip_find(){
  int success_code = 0;
  byte exp_int_chip = exp_seg_int_chip_[i2cseg_index];
  byte exp_int_reg = exp_seg_int_reg_[i2cseg_index];
  sirr(i2cswitch, i2cseg);			// otwarcie segmentu magistrali
  byte expint = sio(exp_int_chip, exp_int_reg);		// odczyt rejestr przerwan w segmencie 
  expint = ~expint;					
  
  byte mask = 10000000;
  expint_index = 0;
  
  if (expint){

  	for (expint_index,expint_index<size,expint_index++) {
  
  		if (expint & mask){
    		Serial.print("interrupt from expander 0: ");
    		Serial.println(expint, BIN);
			
    		success_code = 1;		
	
		}
		
  
  	  mask >>=1;			

  
  }
  
  return success_code;

}


int button_find()
{
  byte int_flag = sio(expander_addr, expander_regf);
  int success_code = 0;
  expander_addr = exp_interrupt_src_[i2cseg_index][expint_index];
  expander_regf = exp_intf_register_[i2cseg_index][expint_index];
  expander_regd = exp_intcap_register_[i2cseg_index][expint_index];
  byte button = sio(expander_addr, expander_regd);
  
  button_index = 0;
  
  
  if (int_flag){		
	  for (byte mask = 00000001,mask,expint_index++) {
	  {
	    mask <<=1;
	    ++button_index;
	  }
	  
  }
  
  return succes;
}  
