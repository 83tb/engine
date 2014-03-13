// Kuba Kucharski
// 2013



// Initialization and library loading

#include <Wire.h>
#include <SerialCommand.h>
#include <avr/pgmspace.h>

SerialCommand sCmd;

// GENERAL SETTINGS

#define I2C_DELAY 0
#define DEBUG false


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
// #define EXP_REG_IOCON     0x0B
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
#define BUT_TAB_LENGTH	8




PROGMEM byte exp_seg_int_chip_[] = {EXP_SEG0_CHIP0_ADDR, EXP_SEG1_CHIP0_ADDR};
PROGMEM byte exp_seg_int_reg_[]  = {EXP_REG_GPIOB, EXP_REG_GPIOB};

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


PROGMEM byte i2cseg_[] = {I2CSEG0, I2CSEG1, I2CSEG2, I2CSEG3};	// tablica adresowania segmentow magistrali

PROGMEM byte exp_interrupt_src_[SEG_TAB_LENGTH][EXP_TAB_LENGTH]   = {
    {EXP_SEG0_CHIP0_ADDR, EXP_SEG0_CHIP1_ADDR, EXP_SEG0_CHIP1_ADDR, EXP_SEG0_CHIP2_ADDR, EXP_SEG0_CHIP2_ADDR, EXP_SEG0_CHIP3_ADDR, EXP_SEG0_CHIP3_ADDR},
    {EXP_SEG1_CHIP0_ADDR, EXP_SEG1_CHIP1_ADDR, EXP_SEG1_CHIP1_ADDR, EXP_SEG1_CHIP2_ADDR, EXP_SEG1_CHIP2_ADDR, EXP_SEG1_CHIP3_ADDR, EXP_SEG1_CHIP3_ADDR},
};

PROGMEM byte exp_intcap_register_[SEG_TAB_LENGTH][EXP_TAB_LENGTH] = {
    {EXP_REG_INTCAPA, EXP_REG_INTCAPA, EXP_REG_INTCAPB, EXP_REG_INTCAPA, EXP_REG_INTCAPB, EXP_REG_INTCAPA, EXP_REG_INTCAPB},
    {EXP_REG_INTCAPA, EXP_REG_INTCAPA, EXP_REG_INTCAPB, EXP_REG_INTCAPA, EXP_REG_INTCAPB, EXP_REG_INTCAPA, EXP_REG_INTCAPB},
};

PROGMEM byte exp_intf_register_[SEG_TAB_LENGTH][EXP_TAB_LENGTH]   = {
    {   EXP_REG_INTFA, EXP_REG_INTFA, EXP_REG_INTFB, EXP_REG_INTFA, EXP_REG_INTFB, EXP_REG_INTFA, EXP_REG_INTFB
    },
    {   EXP_REG_INTFA, EXP_REG_INTFA, EXP_REG_INTFB, EXP_REG_INTFA, EXP_REG_INTFB, EXP_REG_INTFA, EXP_REG_INTFB
    }
};

#define MCP23017_CONFIGS	 2
#define MCP23017_CFG_REGS	15
#define MCP23017_CFG01		0x00
#define MCP23017_CFG02		0x01



PROGMEM byte setup_MCP23017_cfg_tab_[MCP23017_CONFIGS][MCP23017_CFG_REGS] = {
    {
        B00100000,	// EXP_REG_IOCON
        B11111111,	// EXP_REG_OLATA
        B11111111,	// EXP_REG_OLATB
        B11111111,	// EXP_REG_IODIRA
        B11111111,	// EXP_REG_IODIRB
        B00000000,	// EXP_REG_IPOLA
        B00000000,	// EXP_REG_IPOLB
        B11111111,	// EXP_REG_DEFVALA
        B11111111,	// EXP_REG_DEFVALB
        B11111111,	// EXP_REG_GPPUA
        B11111111,	// EXP_REG_GPPUB
        B11111111,	// EXP_REG_INTCONA
        B11111111,	// EXP_REG_INTCONB
        B11111111,	// EXP_REG_GPINTENA
        B11111111	// EXP_REG_GPINTENB
    },
    {
        B00100000,	// EXP_REG_IOCON
        B11111111,	// EXP_REG_OLATA
        B11111111,	// EXP_REG_OLATB
        B00000000,	// EXP_REG_IODIRA
        B00000000,	// EXP_REG_IODIRB
        B00000000,	// EXP_REG_IPOLA
        B00000000,	// EXP_REG_IPOLB
        B11111111,	// EXP_REG_DEFVALA
        B11111111,	// EXP_REG_DEFVALB
        B11111111,	// EXP_REG_GPPUA
        B11111111,	// EXP_REG_GPPUB
        B11111111,	// EXP_REG_INTCONA
        B11111111,	// EXP_REG_INTCONB
        B00000000,	// EXP_REG_GPINTENA
        B00000000	// EXP_REG_GPINTENB
    }
};

PROGMEM byte setup_MCP23017_reg_tab_[] = {
    EXP_REG_IOCON,
    EXP_REG_OLATA,
    EXP_REG_OLATB,
    EXP_REG_IODIRA,
    EXP_REG_IODIRB,
    EXP_REG_IPOLA,
    EXP_REG_IPOLB,
    EXP_REG_DEFVALA,
    EXP_REG_DEFVALB,
    EXP_REG_GPPUA,
    EXP_REG_GPPUB,
    EXP_REG_INTCONA,
    EXP_REG_INTCONB,
    EXP_REG_GPINTENA,
    EXP_REG_GPINTENB
};

PROGMEM int exp_seg_iend_[] = {4, 10};

PROGMEM byte exp_MCP23017_chips_[][2] = {
    {EXP_SEG0_CHIP0_ADDR, MCP23017_CFG01},
    {EXP_SEG0_CHIP1_ADDR, MCP23017_CFG01},
    {EXP_SEG0_CHIP2_ADDR, MCP23017_CFG01},
    {EXP_SEG0_CHIP3_ADDR, MCP23017_CFG01},
    {EXP_SEG1_CHIP0_ADDR, MCP23017_CFG01},
    {EXP_SEG1_CHIP1_ADDR, MCP23017_CFG01},
    {EXP_SEG1_CHIP2_ADDR, MCP23017_CFG01},
    {EXP_SEG1_CHIP3_ADDR, MCP23017_CFG01},
    {EXP_SEG1_CHIP4_ADDR, MCP23017_CFG02},
    {EXP_SEG1_CHIP5_ADDR, MCP23017_CFG02}
};

byte exp_seg0_chips_input_[] = {EXP_SEG0_CHIP0_ADDR, EXP_SEG0_CHIP1_ADDR, EXP_SEG0_CHIP2_ADDR, EXP_SEG0_CHIP3_ADDR};
byte exp_seg1_chips_input_[] = {EXP_SEG1_CHIP0_ADDR, EXP_SEG1_CHIP1_ADDR, EXP_SEG1_CHIP2_ADDR, EXP_SEG1_CHIP3_ADDR};
byte exp_seg1_chips_output_[] = {EXP_SEG1_CHIP4_ADDR, EXP_SEG1_CHIP5_ADDR};


//  ------------------------------- END OF CONFIGURATION

//  ------------------------------- EVENT HANDLER TABLES


/*
 * To sa indeksy do indeksowania tablicy akcji.
 * wypelnia tablice przepisujaca wcisnienty klawisz na indeks.
 */

#define I001	0x00
#define I002	0x01
#define I003	0x02
#define I004	0x03
#define I005	0x04
#define I006	0x05
#define I007	0x06
#define I008	0x07
#define I009	0x08
#define I010	0x09
#define I011	0x0A
#define I012	0x0B
#define I013	0x0C
#define I014	0x0D
#define I015	0x0E
#define I016	0x0F
#define I017	0x10
#define I018	0x11
#define I019	0x12
#define I020	0x13
#define I021	0x14
#define I022	0x15
#define I023	0x16
#define I024	0x17
#define I025	0x18
#define I026	0x19
#define I027	0x1A
#define I028	0x1B
#define I029	0x1C
#define I030	0x1D
#define I031	0x1E
#define I032	0x1F
#define I033	0x20
#define I034	0x21
#define I035	0x22
#define I036	0x23
#define I037	0x24
#define I038	0x25
#define I039	0x26
#define I040	0x27
#define I041	0x28
#define I042	0x29
#define I043	0x2A
#define I044	0x2B
#define I045	0x2C
#define I046	0x2D
#define I047	0x2E
#define I048	0x2F
#define I049	0x30
#define I050	0x31
#define I051	0x32
#define I052	0x33
#define I053	0x34
#define I054	0x35
#define I055	0x36
#define I056	0x37
#define ACTION_TAB_LENGTH	56

/*
 * ta tablica przepisuje adres wcisnietego klawisza na indeks do tablicy akcji
 */

PROGMEM byte button_action_[SEG_TAB_LENGTH][EXP_TAB_LENGTH][BUT_TAB_LENGTH] = {
    {
        {I001, I002, I003, I004, I005, I006, I007, I008},
        {I009, I010, I011, I012, I013, I014, I015, I016},
        {I017, I018, I019, I020, I021, I022, I023, I024},
        {I025, I026, I027, I028, I029, I030, I031, I032},
        {I033, I034, I035, I036, I037, I038, I039, I040},
        {I041, I042, I044, I044, I045, I046, I046, I048},
        {I049, I050, I051, I052, I053, I054, I055, I056}
    },
    {
        {I001, I002, I003, I004, I005, I006, I007, I008},
        {I009, I010, I011, I012, I013, I014, I015, I016},
        {I017, I018, I019, I020, I021, I022, I023, I024},
        {I025, I026, I027, I028, I029, I030, I031, I032},
        {I033, I034, I035, I036, I037, I038, I039, I040},
        {I041, I042, I044, I044, I045, I046, I046, I048},
        {I049, I050, I051, I052, I053, I054, I055, I056}
    }
};

/*
 * ta tablica definiuje akcje i podaje indeks do tablicy parametrow tej akcji,
 * kazda akcja ma swoja tablice, ktora opisuje wartosci parametrow potrzebne
 * do wykonania tej acji.
 * rozne akcje moga miec rozna ilosc parametrow.
 */
#define NOP			0x00	// akcja pusta, dla przyciskow jeszcz nie przypisanych
#define MCP23017LAT_ON_A	0x01	// ustaw na 1 zadane bity jednego rejestru OLATA MC23017 dostepnego za zadanym - pierwszym PCA9545
#define MCP23017LAT_OFF_A	0x02	// ustaw na 0 zadane bity jednego rejestru OLATA MC23017 dostepnego za zadanym - pierwszym PCA9545
#define MCP23017LAT_ON_B	0x03	// ustaw na 1 zadane bity jednego rejestru OLATB MC23017 dostepnego za zadanym - pierwszym PCA9545
#define MCP23017LAT_OFF_B	0x04	// ustaw na 0 zadane bity jednego rejestru OLATB MC23017 dostepnego za zadanym - pierwszym PCA9545
#define MULTIACTION		0x0F	// pobierz wskaznik do akcji z tablicy multiactions, wykonaj i pobierz wskaznik do nastepnej akcji
//  #define MCP23017LAT_ON_A_C	0x05	// ustaw na 1 zadane bity jednego rejestru OLATA MC23017 dostepnego za zadanym - pierwszym PCA9545 i wykonaj nastepna akcje
//  #define MCP23017LAT_OFF_A_C	0x06	// ustaw na 0 zadane bity jednego rejestru OLATA MC23017 dostepnego za zadanym - pierwszym PCA9545 i wykonaj nastepna akcje
//  #define MCP23017LAT_ON_B_C	0x07	// ustaw na 1 zadane bity jednego rejestru OLATB MC23017 dostepnego za zadanym - pierwszym PCA9545 i wykonaj nastepna akcje
//  #define MCP23017LAT_OFF_A_C	0x08	// ustaw na 0 zadane bity jednego rejestru OLATB MC23017 dostepnego za zadanym - pierwszym PCA9545 i wykonaj nastepna akcje

PROGMEM byte actions_[ACTION_TAB_LENGTH][2] = {
    {MCP23017LAT_ON_A, 0x00},	//I001 - kazda sekcja osobno
    {MCP23017LAT_OFF_A, 0x00},	//I002
    {MCP23017LAT_ON_A, 0x01},	//I003
    {MCP23017LAT_OFF_A, 0x01},	//I004
    {MCP23017LAT_ON_A, 0x02},	//I005
    {MCP23017LAT_OFF_A, 0x02},	//I006
    {MCP23017LAT_ON_A, 0x03},	//I007
    {MCP23017LAT_OFF_A, 0x03},	//I008
    {MCP23017LAT_ON_A, 0x04},	//I009
    {MCP23017LAT_OFF_A, 0x04},	//I010
    {MCP23017LAT_ON_A, 0x05},	//I011
    {MCP23017LAT_OFF_A, 0x05},	//I012
    {MCP23017LAT_ON_A, 0x06},	//I013
    {MCP23017LAT_OFF_A, 0x06},	//I014
    {MCP23017LAT_ON_A, 0x07},	//I015
    {MCP23017LAT_OFF_A, 0x07},	//I016
    {MCP23017LAT_ON_B, 0x00},	//I017
    {MCP23017LAT_OFF_B, 0x00},	//I018
    {MCP23017LAT_ON_B, 0x01},	//I019
    {MCP23017LAT_OFF_B, 0x01},	//I020
    {MCP23017LAT_ON_B, 0x02},	//I021
    {MCP23017LAT_OFF_B, 0x02},	//I022
    {MCP23017LAT_ON_B, 0x03},	//I023
    {MCP23017LAT_OFF_B, 0x03},	//I024
    {MCP23017LAT_ON_B, 0x04},	//I025
    {MCP23017LAT_OFF_B, 0x04},	//I026
    {MCP23017LAT_ON_B, 0x05},	//I027
    {MCP23017LAT_OFF_B, 0x05},	//I028
    {MCP23017LAT_ON_B, 0x06},	//I029
    {MCP23017LAT_OFF_B, 0x06},	//I030
    {MCP23017LAT_ON_B, 0x07},	//I031
    {MCP23017LAT_OFF_B, 0x07},	//I032
    {MCP23017LAT_ON_A, 0x08},	//I033
    {MCP23017LAT_OFF_A, 0x08},	//I034
    {MCP23017LAT_ON_A, 0x09},	//I035
    {MCP23017LAT_OFF_A, 0x09},	//I036
    {MCP23017LAT_ON_A, 0x0A},	//I037
    {MCP23017LAT_OFF_A, 0x0A},	//I038
    {MCP23017LAT_ON_A, 0x0B},	//I039
    {MCP23017LAT_OFF_A, 0x0B},	//I040
    {MCP23017LAT_ON_A, 0x0C},	//I041
    {MCP23017LAT_OFF_A, 0x0C},	//I042
    {MCP23017LAT_ON_A, 0x0D},	//I043
    {MCP23017LAT_OFF_A, 0x0D},	//I044
    {MCP23017LAT_ON_A, 0x0E},	//I045
    {MCP23017LAT_OFF_A, 0x0E},	//I046
    {MCP23017LAT_ON_A, 0x0F},	//I047
    {MCP23017LAT_OFF_A, 0x0F},	//I048
    {MULTIACTION, 0x01},	//I049 - wszystko ON
    {MULTIACTION, 0x04},	//I050 - wszystko OFF_
    {MULTIACTION, 0x03},	//I051 - wszystkie regaly ON
    {MULTIACTION, 0x06},	//I052 - wszystkie regaly OFF
    {MCP23017LAT_ON_A, 0x10},	//I053 - obie sekcje openspace
    {MCP23017LAT_OFF_A, 0x10},	//I024
    {NOP, 0x00},	//I055
    {NOP, 0x00},	//I056
};

/*
 * tablica wspolna dla ON_A, OFF_A, ON_B i OFF_B
 * kolumny to:
 * - adres segmentu (wartosc do wpisania do rejestru) PCA9545,
 * - adres MC23017 w segmencie
 * - maska bitow, ktore maja byc zmienione.
 */
#define ON_OFF_TAB_LENGTH	19
#define ON_OFF_TAB_COLUMNS	3
PROGMEM byte latMCP23017_on_off_tab_[ON_OFF_TAB_LENGTH][ON_OFF_TAB_COLUMNS] = {
    {I2CSEG0, EXP_SEG1_CHIP4_ADDR, B00000001},	// 00
    {I2CSEG0, EXP_SEG1_CHIP4_ADDR, B00000010},	// 01
    {I2CSEG0, EXP_SEG1_CHIP4_ADDR, B00000100},	// 02
    {I2CSEG0, EXP_SEG1_CHIP4_ADDR, B00001000},	// 03
    {I2CSEG0, EXP_SEG1_CHIP4_ADDR, B00010000},	// 04
    {I2CSEG0, EXP_SEG1_CHIP4_ADDR, B00100000},	// 05
    {I2CSEG0, EXP_SEG1_CHIP4_ADDR, B01000000},	// 06
    {I2CSEG0, EXP_SEG1_CHIP4_ADDR, B10000000},	// 07
    {I2CSEG0, EXP_SEG1_CHIP5_ADDR, B00000001},	// 08
    {I2CSEG0, EXP_SEG1_CHIP5_ADDR, B00000010},	// 09
    {I2CSEG0, EXP_SEG1_CHIP5_ADDR, B00000100},	// 0A
    {I2CSEG0, EXP_SEG1_CHIP5_ADDR, B00001000},	// 0B
    {I2CSEG0, EXP_SEG1_CHIP5_ADDR, B00010000},	// 0C
    {I2CSEG0, EXP_SEG1_CHIP5_ADDR, B00100000},	// 0D
    {I2CSEG0, EXP_SEG1_CHIP5_ADDR, B01000000},	// 0E
    {I2CSEG0, EXP_SEG1_CHIP5_ADDR, B10000000},	// 0F
    {I2CSEG0, EXP_SEG1_CHIP5_ADDR, B00000011},	// 10 - open space, obie sekcje
    {I2CSEG0, EXP_SEG1_CHIP5_ADDR, B11111111},	// 11 - wszystko
    {I2CSEG0, EXP_SEG1_CHIP4_ADDR, B11111111}	// 12 - wszystkie regaly
};

/*
 * tablica dla ON_ OFF_ z kontynuacją
 * kolumny:
 * - adres segmentu (wartosc do wpisania do rejestru) PCA9545,
 * - adres MC23017 w segmencie,
 * - maska bitow, ktore maja byc zmienione,
 * - nastepna akcja,
 * - indeks w tablicy akcji
 */

#define MULTIACTION_TAB_LENGTH	11
#define MULTIACTION_TAB_COLUMNS	4


PROGMEM byte multiaction_tab_[MULTIACTION_TAB_LENGTH][MULTIACTION_TAB_COLUMNS] = {
    {NOP, 0x00, 0x00},				// 00
    {MCP23017LAT_ON_A, 0x12, MULTIACTION, 0x02},		// 01
    {MCP23017LAT_ON_B, 0x12, MCP23017LAT_ON_A, 0x11},		// 02
    {MCP23017LAT_ON_A, 0x12, MCP23017LAT_ON_B, 0x12},		// 03
    {MCP23017LAT_OFF_A, 0x12, MULTIACTION, 0x05},		// 04
    {MCP23017LAT_OFF_B, 0x12, MCP23017LAT_OFF_A, 0x11},		// 05
    {MCP23017LAT_OFF_A, 0x12, MCP23017LAT_OFF_B, 0x12}		// 06
};



// EVALUATED

int exp_seg0_chips_input_size = sizeof(exp_seg0_chips_input_)/sizeof(byte);
int exp_seg1_chips_input_size = sizeof(exp_seg1_chips_input_)/sizeof(byte);
int exp_seg1_chips_output_size = sizeof(exp_seg1_chips_output_)/sizeof(byte);


byte i2cseg;							// "adres" obslugiwanego segmentu magistrali
int i2cseg_index;						// index do tablicy adresowej segmentow magistrali
int expint_index;						// index wiazacy przerwania z expanderami w segmenie magistrali
int button_index;						// index (finalnie numer) klawisza, ktory wywolal przerwanie
byte expander_addr;		// i2c adres expandera, zrodla przerwania
byte expander_regf;		// interrupt flag register
byte expander_regd;		// interrupt capture register

// Event handling ----------------------

void event_handle(byte i2cseg_index, byte expint_index, byte button_index) {
    byte button_action;
    byte actionid;
    byte actindex;
    byte next_actionid = 0x00;
    byte next_actindex = 0x00;
    button_action =  pgm_read_byte(&button_action_[i2cseg_index][expint_index][button_index]);
    actionid = pgm_read_byte(&actions_[button_action][0]);
    actindex = pgm_read_byte(&actions_[button_action][1]);

	#if DEBUG
    Serial.println(F("==============================================================event_handle: "));
    Serial.print(F("---------button_action: "));
    Serial.print(button_action, HEX);
    Serial.print(F(" = "));
    Serial.println(button_action, DEC);
    Serial.print(F("actionid: "));
    Serial.println(actionid, HEX);
    Serial.print(F("actindex: "));
    Serial.println(actindex, HEX);
    Serial.print(F("--------------next_actionid: "));
    Serial.println(next_actionid, HEX);
    Serial.print(F("--------------: "));
    Serial.println();
	#endif
	
    int a = 0;
    while(actionid) {
        if (actionid == MULTIACTION) {
            next_actionid =  pgm_read_byte(&multiaction_tab_[actindex][2]);
            next_actindex =  pgm_read_byte(&multiaction_tab_[actindex][3]);
            actionid =  pgm_read_byte(&multiaction_tab_[actindex][0]);
            actindex = pgm_read_byte(&multiaction_tab_[actindex][1]);
#if DEBUG
            Serial.print(F("******************                          -=-=-=-=  MULTIACTION =-=-=-=-"));
#endif
        }
#if DEBUG
        Serial.println(F("==============================================================inside loop while: "));
        Serial.print(F("actionid: "));
        Serial.println(actionid, HEX);
        Serial.print(("actindex: "));
        Serial.println(actindex, HEX);
        Serial.print(F("next_actionid: "));
        Serial.println(next_actionid, HEX);
        Serial.print(F("next_actindex: "));
        Serial.println(next_actindex, HEX);
#endif
//  Serial.println(F("NOW CALLING: latMCP23017: "));
		
        if (actionid == MCP23017LAT_ON_A || actionid == MCP23017LAT_ON_B || actionid == MCP23017LAT_OFF_A || actionid == MCP23017LAT_OFF_B ) {
#if DEBUG
            Serial.println(F("NOW CALLING: latMCP23017: "));
#endif
            latMCP23017(actionid, actindex);
        }
        actionid = next_actionid;
        actindex = next_actindex;
        next_actionid = 0x00;
        next_actindex = 0x00;
        a++;
        Serial.print(F("aaaaaaaaaaaaaaaaaaaaa: "));
        Serial.println(a);

        if (a >= 5) {
            break;
        }
    }
}


void latMCP23017(byte actionid, byte actindex) {
#if DEBUG
    Serial.println(F("==============================================================latMCP23017: "));
    Serial.print(F("actionid: "));
    Serial.println(actionid, HEX);
    Serial.print(F("actindex: "));
    Serial.println(actindex, HEX);
#endif
    byte olatreg;
    byte i2cseg = pgm_read_byte(&latMCP23017_on_off_tab_[actindex][0]);
    byte expander_addr = pgm_read_byte(&latMCP23017_on_off_tab_[actindex][1]);
    byte reg_mask = pgm_read_byte(&latMCP23017_on_off_tab_[actindex][2]);
#if DEBUG
    Serial.print(F("i2cseg: "));
    Serial.println(i2cseg, HEX);
    Serial.print(F("expander_addr: "));
    Serial.println(expander_addr, HEX);
    Serial.print(F("reg_mask: "));
    Serial.println(reg_mask, BIN);
#endif



    if (actionid == MCP23017LAT_ON_A || actionid == MCP23017LAT_OFF_A) {
        olatreg = EXP_REG_OLATA;
    }
    else if (actionid == MCP23017LAT_ON_B || actionid == MCP23017LAT_OFF_B) {
        olatreg = EXP_REG_OLATB;
    }
    sirr(I2CSWITCH_ADDR,i2cseg);
    byte reg_value = sio(expander_addr, olatreg);
#if DEBUG
    Serial.print(F("--------------reg_value: "));
    Serial.println(reg_value);
#endif
    if (actionid == MCP23017LAT_OFF_A || actionid == MCP23017LAT_OFF_B) {
#if DEBUG
        Serial.print(F("OFF OFF OFF: "));
#endif
        reg_mask = ~reg_mask;
        reg_value = reg_value & reg_mask;
    } else {
#if DEBUG
        Serial.print(F("ON ON ON:"));
#endif
        reg_value = reg_value | reg_mask;
    }
#if DEBUG
    Serial.print(F("--------------new reg_value: "));
    Serial.println(reg_value);
#endif

    sir(expander_addr, olatreg, reg_value);
}

// Event handling END --------------------

// MCP23017 setup

void setupMCP23017() {
    int jbeg = 0;
    for(int i = 0; i < SEG_TAB_LENGTH; i++) {
        i2cseg = pgm_read_byte(&i2cseg_[i]);
        sirr(I2CSWITCH_ADDR ,i2cseg);			// otwarcie segmentu magistrali
        int jend = pgm_read_byte(&exp_seg_iend_[i]);
        for(int j = jbeg; j < jend; j++) {		// od pierwszego do ostatniego elementu w segmencie
            byte chip_addr = pgm_read_byte(&exp_MCP23017_chips_[j][0]);
            int cfg_index = pgm_read_byte(&exp_MCP23017_chips_[j][1]);
#if DEBUG
	    Serial.print(F("chip No:"));
	    Serial.println(j, DEC);
#endif
            for(int k = 0; k < MCP23017_CFG_REGS; k++) {
                byte chip_reg = pgm_read_byte(&setup_MCP23017_reg_tab_[k]);
                byte reg_value = pgm_read_byte(&setup_MCP23017_cfg_tab_[cfg_index][k]);
#if DEBUG
		Serial.print(F("=<> CFG: "));
		Serial.print(F("chip: "));
		Serial.print(chip_addr, HEX);
		Serial.print(F(" register: "));
		Serial.print(chip_reg, HEX);
		Serial.print(F(" value: "));
		Serial.println(reg_value, BIN);
#endif
		sir(chip_addr, chip_reg, reg_value);
            }
        }
        jbeg = jend;
    }
}
/*
void setupAll() {
    /* setting up all the devices found in the tables */
/*
    sirr(I2CSWITCH_ADDR, I2CSEG0);

    for(int i = 0; i <  exp_seg0_chips_input_size; i++) {
        setupInput(i);
        setPullUp(i);
        setInterruptEnable(i);
        setDefaultValue(i);
        setSeqopDisabled(i);
        resetInterrupts(i);
    }

    sirr(I2CSWITCH_ADDR, I2CSEG1);

    for(int i = 0; i < exp_seg1_chips_input_size; i++) {
        setupInput(i);
        setPullUp(i);
        setInterruptEnable(i);
        setDefaultValue(i);
        setSeqopDisabled(i);
        resetInterrupts(i);
    }

    for(int i = 0; i < exp_seg1_chips_output_size; i++) {
        setupOutput(i);
    }

}
*/

int i2csegFind() {
    byte intseg = sio_raw(I2CSWITCH_ADDR);
    byte mask = B00010000;		// specyfic for PCA9545 -last bit of resetInterrupts
    int success_code = 0;
    int tsize = SEG_TAB_LENGTH;
    i2cseg_index = 0; // zaczynamy od zera

    for (i2cseg_index; i2cseg_index<tsize; i2cseg_index++) {
        if (intseg & mask) {
            i2cseg =  pgm_read_byte(&i2cseg_[i2cseg_index]);
            success_code = 1;
            break;
        }

        mask <<=1;
    }

    return success_code;
}





int chipFind() {
    int success_code = 0;
    byte exp_int_chip = pgm_read_byte(&exp_seg_int_chip_[i2cseg_index]);
    byte exp_int_reg = pgm_read_byte(&exp_seg_int_reg_[i2cseg_index]);
    sirr(I2CSWITCH_ADDR,i2cseg);			// otwarcie segmentu magistrali
    byte expint = sio(exp_int_chip, exp_int_reg);		// odczyt rejestr przerwan w segmencie
    int tsize = EXP_TAB_LENGTH;

    expint = ~expint;

    byte mask = B10000000;
    expint_index = 0;

    if (expint) {
        for (expint_index; expint_index<tsize; ++expint_index) {
            if (expint & mask) {
                success_code = 1;
                break;
            }
            mask >>=1;
        }
    }

    return success_code;
}



int buttonFind() {
    expander_addr = pgm_read_byte(&exp_interrupt_src_[i2cseg_index][expint_index]);
    expander_regf = pgm_read_byte(&exp_intf_register_[i2cseg_index][expint_index]);
    expander_regd = pgm_read_byte(&exp_intcap_register_[i2cseg_index][expint_index]);
    byte int_flag = sio(expander_addr, expander_regf);
    byte button   = sio(expander_addr, expander_regd);
    button        = ~button;
    button_index  = 0;
    int success_code = 0;



    if (int_flag) {
        for (byte mask = B00000001; mask; mask <<=1) {
            if (button & mask) {
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
    sCmd.addCommand("i2c", i2cScan);
    sCmd.addCommand("sir", sir_func);
    sCmd.addCommand("sio", sio_func);
    sCmd.addCommand("sior", sior_func);
    sCmd.addCommand("sirr", sirr_func);


    Serial.println(F("---------------------------------------------------------\n"));
    Serial.println(F("POWERMANAGER, @83TB\n"));
    Serial.println(F("Log: Engine console initialized. Ready when you are."));
    Serial.println(F("Hint: Type h for help"));

}


// Main Atmega Setup

void setup() {
    Serial.begin(9600);
    Wire.begin();
    pinMode(2, INPUT);
    pinMode(13, OUTPUT);

//    setupAll();
    setupMCP23017();
    addConsole();

}




void loop() {
    delay(100);

    int a = digitalRead(2);
    if (a==0) {
        //testInterrupt();
        int success_read = 0;
        if (i2csegFind()) {
            if (chipFind()) {
                if (buttonFind()) {
#if DEBUG
                    Serial.println();
                    Serial.println(F("Huge success !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
                    Serial.println();
                    Serial.println();
                    Serial.print(F("i2cseg_index: "));
                    Serial.println(i2cseg_index);
                    Serial.print(F("expint_index: "));
                    Serial.println(expint_index);
                    Serial.print(F("button_index: "));
                    Serial.println(button_index);
                    Serial.println(F("====================="));
                    success_read = 1;
#endif
                }
            }
        }
        if (success_read) {
#if DEBUG
            Serial.println(F("teraz wykonanie: !!! :)  !!!"));
#endif
            event_handle(i2cseg_index, expint_index, button_index);
        }
    }

    delay(100);
    sCmd.readSerial();     // Przetwarzanie, to wszystko co dzieje sie w petli

}







// ADDITIONAL FUNCTIONS

void i2cScan() {
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
            Serial.print(F("Info: Unknown error at address 0x"));
            if (address<16) Serial.print("0");
            Serial.println(address,HEX);
        }
    }

    if (nDevices == 0) Serial.println(F("Info: No I2C devices found\n"));
    else Serial.println(F("Log: Completed\n"));

}


void comment() {}


void help() {

    Serial.println("");

    Serial.println(F("Hint: h: prints help"));
    Serial.println(F("Hint: i2c: i2c scanner"));
    Serial.println(F("Hint: sir: talks to i2c, takes 3 arguments"));
    Serial.println(F("Hint: sio: fetch from i2c, takes 2 arguments"));
    Serial.println(F("Hint: # : comment"));
    Serial.println(F("\n"));

}


// nie rozpoznano komendy
void unrecognized(const char *command) {
    Serial.println(F("What? I don't know this command. "));
}



// CONSOLE I2C FUNCTIONS

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
        Serial.println(F("No arguments"));
    }

    arg = sCmd.next();
    if (arg != NULL) {
        registry = strtol(arg, NULL, 16);
    }
    else {
        Serial.println(F("No second argument"));
    }


    arg = sCmd.next();
    if (arg != NULL) {
        packet = strtol(arg, NULL, 16);
    }
    else {
        Serial.println(F("No third argument"));
    }

    sir(address,registry,packet);

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
        Serial.println(F("No second argument"));
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
        Serial.println(F("No arguments"));
    }

    arg = sCmd.next();
    if (arg != NULL) {
        registry = strtol(arg, NULL, 16);
    }

    else {
        Serial.println(F("No second argument"));
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
        Serial.println(F("No arguments"));
    }

    sio_raw(address);

}


// I2C GENERAL LIBRARY, READING, WRITING ETC


void sir(int address,int registry,int packet) {

#if DEBUG
    Serial.println(F("---------------------------------------------------------"));
#endif
    Wire.beginTransmission(address);
#if DEBUG
    Serial.print(F("Adres urzadzenia: 0x"));
    Serial.print(address, HEX);
    Serial.print(F("    Adres rejestru: 0x"));
    Serial.println(registry, HEX);
#endif

    Wire.write(registry);
#if DEBUG
    Serial.print(F("Wysylamy pakiet: 0x"));
    Serial.println(packet, HEX);
#endif
    Wire.write(packet);
#if DEBUG
    Serial.print(F("Odczytano:   "));
#endif
    Wire.endTransmission();
    Wire.requestFrom(address, 1);

    while(Wire.available()) {

        byte c = Wire.read(); // receive a byte as character
		#if DEBUG
        Serial.print(c, BIN);
        Serial.print(" | 0x");
        Serial.println(c, HEX);
		#endif
    }
    delay(I2C_DELAY);


}




void sirr(int address,int packet) {

#if DEBUG
    Serial.println(("---------------------------------------------------------"));
#endif
    Wire.beginTransmission(address);
#if DEBUG
    Serial.print(F("Adres urzadzenia: 0x"));
    Serial.print(address, HEX);


    Serial.print(F("Wysylamy pakiet: 0x"));
    Serial.println(packet, HEX);
#endif
    Wire.write(packet);
#if DEBUG
    Serial.print(F("Odczytano:   "));
#endif
    Wire.endTransmission();
    Wire.requestFrom(address, 1);

    while(Wire.available()) {
        byte c = Wire.read(); // receive a byte as character
#if DEBUG
        Serial.print(c, BIN);
        Serial.print(" | 0x");
        Serial.println(c, HEX);
#endif

    }
    delay(I2C_DELAY);

}


int sio(int address,int registry) {
    byte c;
#if DEBUG
    Serial.println(F("---------------------------------------------------------"));
#endif
    Wire.beginTransmission(address);
#if DEBUG
    Serial.print(F("Adres urzadzenia: 0x"));
    Serial.print(address, HEX);
    Serial.print(F("   Adres rejestru: 0x"));
    Serial.println(registry, HEX);
#endif

    Wire.write(registry);

#if DEBUG
    Serial.print(F("Odczytano:     "));
#endif
    Wire.endTransmission();
    Wire.requestFrom(address, 1);

    while(Wire.available()) {
        c = Wire.read();
#if DEBUG
        Serial.print(c, BIN);
        Serial.print(" | 0x");
        Serial.println(c, HEX);
#endif
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
#if DEBUG
    Serial.println(c, HEX);
#endif
    return c;
}




// MCP231017 SPECIFIC SETUP
/*
void setupOutput(char ad)
{

    sir(ad,0x00,0x00);
    sir(ad,0x01,0x00);

}



void setupInput(char ad)
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
*/

