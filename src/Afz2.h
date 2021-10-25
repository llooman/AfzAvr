#define JL_VERSION 20100

// #define DEBUG

#define NODE_ID 24
#define NODE_TYPE 24
#define TRACE_SEC 3
 
#define DETECT_230V_PIN 2

// #define TEMP_PIN 3
 
#define FAN_1_PIN 4 
#define FAN_2_PIN 5 
#define FAN_3_PIN 6

#define TEMPS_PIN 7


#define BME_PIN 53  	// 10 53 CSB					Chip Select
#define BME_MOSI 51		// 11 51 SDI SDA 	Rx	MOSI	Data in			geel
#define BME_MISO 50		// 12 50 SDO 	 	Tx	MISO	Data out		groen
#define BME_SCK 52		// 13 52 SCK SCL 				Clock

// #define SPI_PIN 13
// #define HN_SLAVE 0

#define SERIAL_PORT 0
#define ISR_PARENT_TWI

// #define VOLTAGE_PIN A3
// #define VOLTAGE_FACTOR 10.800

/*
2021-07-17 3.00.00 re-add BME 2560 

2021-07-02 2.01.00 remove BME
				   secOfTheDay
				   serialChilds
2020-11-25 2.00.00 Add BME
				   Customize BME280 strip I2C, temp, pressure to save Flash memory

2020-10-30 1.01.00 UniversalDHT FatDHT22 
2020-10-21 1.00.01 switch to ITHO speed 0-3

2020-07-26 1.04.00 Upload next / if( ! espNode.isTxBufFull()
2020-05-31 1.03.00 refactor 2 relais2

1.02.00 refactor 2 myTimers

2018-10-27 	Don't persist eeprom.dimmer!
			default 0 on boot, set to 2 on setup. always calculate needed fan speed.
			Skip timer when fan is already on.
2018-06-09 	disable triac upload
2020-01-25 3 bootcount, cleanup code* 

1.00.00 2018-06-01
*/

#define FAN_ID 50  			// vertilator aan/uit  131 sleep 231 manual on
// #define SET_FAN_MANUAL_ID 51
#define FAN_TEMP 55
#define NIGHTLY_ID 56

#define LAMP_ID 45

#define VOCHT_HOOG_ID 32
#define VOCHT_LAAG_ID 33
#define MANUAL_TIMER_ID 36
#define DEVIATION_TIMER_ID 37
#define DEVIATION_ID 43

#define MAX_DOUCH_TEMP_ID 38
#define MAX_KRAAN_TEMP_ID 39

#define SET_HIGH_DELAY_ID 40
#define SET_MEDIUM_DELAY_ID 41
#define SET_LOW_DELAY_ID 42

 

#define REL_HUM_ID 24
#define HUM_TEMP_ID 25
#define ABS_HUM_ID 26
#define HUM_MEDIAN_ID 27
#define HUM_DEVIATION_ID 28
#define PRESS_ID 29

// DS18B20's
#define BUITEN_TEMP_ID 21		// Buiten
#define DOUCH_TEMP_ID 22		// Douch
#define KRAAN_TEMP_ID 23		// Kraan Bad en wastafel (lange draad terug naar de ketel !!!!)

// DHT In douch: 
// #define DOUCH_HUMIDITY_ID 25		// rel vochtigheid, %
// #define DOUCH_LUCHT_TEMP_ID 26		// temp
// #define DOUCH_HUMIDITY_ABS_ID 27	// absolute humidity (g/m3)
// #define DOUCH_HUMIDITY_MEDIAN_ID 28	// median vochtigheid %
// #define DOUCH_HUMIDITY_STATS_ID 29	

// #define COLD_HUMIDITY_ID 34
// #define DELTA_HUMIDITY_ID 35    //

// when compare between 2 humidities
// #define ABS_DELTA_ID 45			// verschil Abs vochtigheid binnen - buiten    g/m3
 
/*
	Een voor ons behaaglijke relatieve vochtigheid ligt tussen 40% ("droge lippen") en 70% ("klam").
	Bij bijvoorbeeld 20 graden C kan er maximaal 17,28 gram/m3 water damp in lucht
	en bij een RV van 50% zit er dus in werkelijkheid in 0,50 * 17,28 = 8,64 gram/m3.
	Als de maximale hoeveelheid waterdamp aanwezig is (bij een bepaalde temperatuur), wordt dit verzadigd genoemd.
	Door de wisselende temperatuur en vochtigheid verschilt de relatieve vochtigheid buitenshuis sterker dan binnenshuis.

	temperatuur
	(graden C)	verzadigd
	(gram/m3)
	-20	0,90
	-15	1,41
	-10	2,15
	-5	3,26
	0	4,84
	5	6,83
	10	9,40
	15	12,85
	20	17,28
	25	23,05
	30	30,34
	35	39,56

absHumidity = (RH1*0.42*Math.exp(T1*10*0.006235398)/10 ;   g/m3

 */

#ifndef _Afzuiging_H_
#define _Afzuiging_H_

#include "Arduino.h"   // optiboot !! COM10 linker USB
#include <inttypes.h>
#include <avr/wdt.h>
#include "EEUtil.h"
#include <MyTimers.h>
#include <NetwTWI.h>
#include <NetwSerial.h>
#include <Stats.h>
#include <DS18B20.h>

#include <SPI.h>                         // Needed for legacy versions of Arduino.
#include <BME280Spi.h>
// #include <BME280SpiSw.h>

// #include <Voltage.h>

//#include "LowPower.h"
//System sys;

//#define SPI_PARENT
//#include <NetwSPI.h>
//NetwSPI parentNode(NODE_ID);

NetwTWI parentNode;

// settings(CHIP_SELECT_PIN, MOSI_PIN, MISO_PIN, SCK_PIN);
// #define BME_PIN 53  	// 10 CSB					Chip Select
// #define BME_MOSI 51		// 11 SDI SDA Rx	MOSI	Data in			geel
// #define BME_MISO 50		// 12 SDO 	  Tx	MISO	Data out		groen
// #define BME_SCK 52		// 13 SCK SCL 				Clock

#ifdef BME_PIN

	// //BME280Spi::Settings settings(BME_PIN, BME_MOSI, BME_MISO, BME_SCK);     // Default : forced mode, standby time = 1000 ms
	BME280Spi::Settings settings(BME_PIN  );     // Default : forced mode, standby time = 1000 ms
											// Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,
	BME280Spi bme(settings);

	// BME280SpiSw::Settings settings2(BME_PIN, BME_MOSI, BME_MISO, BME_SCK);
	// BME280SpiSw bme2(settings2);

	
#endif 


bool bmeStarted = false;
 
Stats medianHumidity;
Stats statsOnHumidity;

float humDeviation;
float medianUploaded = 0;

float bmeTemp(NAN);
float bmeHum(NAN);
float bmePress(NAN);

BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
BME280::PresUnit presUnit(BME280::PresUnit_Pa);

long relHum = 0;
long humTemp = 0;
long absHum = 0;
long press = 0;

long humUploaded = 0;
long tempUploaded = 0;
long absHumUploaded = 0;
long pressUploaded = 0;

const byte dsKraan[] PROGMEM = {0x28, 0xff, 0xab, 0x19, 0xa0, 0x16, 0x03, 0xf2};  //28 FF AB 19 A0 16 3 F2
const byte ds104[] PROGMEM = {0x28, 0xde, 0xc3, 0x57, 0x05, 0x00, 0x00, 0xce};  // waterdicht
const byte ds107[] PROGMEM = {0x28, 0x37, 0xDB, 0x8C, 0x06, 0x00, 0x00, 0x42};  // waterdicht zelf   28 37 DB 8C 6 0 0 42

#ifdef TEMPS_PIN
#ifdef DEBUG  // invalid shading by IDE !!!!
DS18B20 tempDouch( TEMPS_PIN);  // ds04, ds103 TEMP_SENSOR_CV
#else
DS18B20 tempBuiten(TEMPS_PIN, ds107);  //, ds107  wit , 21
DS18B20 tempDouch( TEMPS_PIN, ds104);  // ds04, ds103 TEMP_SENSOR_CV , 22
DS18B20 tempKraan( TEMPS_PIN, dsKraan);  //  ds103 TEMP_SENSOR_CV , 23
#endif
#endif

#ifdef TEMP_PIN
DS18B20 tempAfzuiging( TEMP_PIN);  // ds04, ds103 TEMP_SENSOR_CV
#endif

#define TIMERS_COUNT 20
MyTimers myTimers(TIMERS_COUNT);
#define TIMER_TRACE 0
#define TIMER_TEST 1
#define TIMER_UPLOAD_ON_BOOT 2
#define TIMER_UPLOAD_FAN 3
#define TIMER_FAN_HIGH 4
#define TIMER_FAN_MEDIUM 5
#define TIMER_FAN_LOW 6
#define TIMER_230 7
#define TIMER_LAMP_UPLOAD 8
#define TIMER_LAMP_ON 9
#define TIMER_SECOND_OF_THE_DAY 10
#define TIMER_NIGHTLY_UPLOAD 11
#define TIMER_READ_BME 12
#define TIMER_UPLOAD_REL_HUM 13
#define TIMER_UPLOAD_HUM_TEMP 14
#define TIMER_UPLOAD_ABS_HUM 15
#define TIMER_UPLOAD_HUM_MEDIAN 16
#define TIMER_UPLOAD_DEVIATION 17
#define TIMER_UPLOAD_PRESS 18
#define TIMER_STATS_ON_HUMID 19

 



int 	uploadOnBootCount=0;

unsigned long  	secOfTheDay = 0;
unsigned long 	nextSecond = 0;

unsigned long 	testMinute = 0;

 
// float humDeviation;
// float medianUploaded = 0;

// long bmeHum = 0;
// long bmeTemp = 0;
long bmeAbs = 0;

bool nightly = false;
int fanSpeed = 0;
int prevVocht = 0;

bool lampOn = false;
bool lampUploaded = false;

volatile unsigned long curr230vcount = 0;
		 unsigned long prev230vcount = 0;
		 unsigned long curr230vcountStamp = 0;


int preheatSec = 120;
int prevVal = LOW;
long th, tl, h, l, ppm = 0;


// void  freeze( unsigned long millisec );
void localSetVal(int id, long val);
int  upload(int id, long val, unsigned long timeStamp);
int  upload(int id, long val) ;
int  upload(int id);
int  uploadError(int id, long val);
int  handleParentReq( RxItem *rxItem) ;
int localRequest(RxItem *rxItem);
void trace();


void refreshSettings(void);
int evaluateFanOnOff(bool on, bool isRunning);
 
void resetFan(void);
void fanLoop(void);
void isr230v(void);
void isrMHZ19(void);

#endif