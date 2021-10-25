
#include <Afz2.h>

// #define DEBUG

class EEParms: public EEUtil  // TODO
{
public:
	int				maxDouchTemp = 47;		 
	int				maxKraanTemp = 47;	

	volatile int  	vochtHoog = 90;
	volatile int  	vochtLaag = 75;
	
	volatile int  	highDelayMin = 3;	
	volatile int  	mediumDelayMin = 5;	
	volatile int  	lowDelayMin = 7;	

	volatile int  	manualTimerMin = 5;	
	volatile int    statsTimerMin = 2;

	volatile int    minDeviation = 9;

	long chk1  = 0x04010204;   

	virtual ~EEParms(){}
	EEParms(){ }

    void setup( ) //class HomeNet &netw
    {

		if( readLong(offsetof(EEParms, chk1)) == chk1  )
    	{
    		readAll();
    		vochtHoog = readInt(offsetof(EEParms, vochtHoog));
    		vochtLaag = readInt(offsetof(EEParms, vochtLaag));

    		highDelayMin = readInt(offsetof(EEParms, highDelayMin));
    		mediumDelayMin = readInt(offsetof(EEParms, mediumDelayMin));
    		lowDelayMin = readInt(offsetof(EEParms, lowDelayMin));

    		manualTimerMin = readInt(offsetof(EEParms, manualTimerMin));
			statsTimerMin = readInt(offsetof(EEParms, statsTimerMin));
			minDeviation = readInt(offsetof(EEParms, minDeviation));
 
    		// maxDouchTemp = readInt(offsetof(EEParms, maxDouchTemp));
    		// maxKraanTemp = readInt(offsetof(EEParms, maxKraanTemp));									

    		changed = false;

//    		resync();
			#ifdef DEBUG
				Serial.println(F("EEProm.readAll"));
			#endif
    	}
		else
		{
			bootCount=0;
		}
    }

	void loop()
	{
		if(changed)
		{
			#ifdef DEBUG
				Serial.println(F("Parms.loop.changed"));
			#endif
			write(offsetof(EEParms, vochtHoog), vochtHoog);
			write(offsetof(EEParms, vochtLaag), vochtLaag);

			write(offsetof(EEParms, highDelayMin), highDelayMin);
			write(offsetof(EEParms, mediumDelayMin), mediumDelayMin);
			write(offsetof(EEParms, lowDelayMin), lowDelayMin);			

			write(offsetof(EEParms, manualTimerMin), manualTimerMin);
			write(offsetof(EEParms, statsTimerMin), statsTimerMin);
			write(offsetof(EEParms, minDeviation), minDeviation);
 
			// write(offsetof(EEParms, maxDouchTemp), maxDouchTemp);
			// write(offsetof(EEParms, maxKraanTemp), maxKraanTemp);

			write(offsetof(EEParms, chk1), chk1);
			EEUtil::writeAll();
			changed = false;
		}
	}

	void setHighDelay( long newVal)
	{
		if( newVal < 0 ) newVal = 0;
		if( newVal > 30 ) newVal = 30;

		highDelayMin =  (int)newVal;
		changed = true;
	}

	void setMediumDelay( long newVal)
	{
		if( newVal < 0 ) newVal = 0;
		if( newVal > 30 ) newVal = 30;

		mediumDelayMin =  (int)newVal;
		changed = true;
	}

	void setLowDelay( long newVal)
	{
		if( newVal < 0 ) newVal = 0;
		if( newVal > 30 ) newVal = 30;

		lowDelayMin =  (int)newVal;
		changed = true;
	}	

	void setStatsTimer( long newVal)
	{
		if( newVal < 1 ) newVal = 1;
		if( newVal > 5 ) newVal = 5;

		statsTimerMin =  (int)newVal;
		changed = true;
	}
	void setManualTimer( long newVal)
	{
		if( newVal < 0 ) newVal = 0;
		if( newVal > 30 ) newVal = 30;

		manualTimerMin =  (int)newVal;
		changed = true;
	}
	void setMinDeviation( long newVal)
	{
		if( newVal < 5 ) newVal = 5;
		if( newVal > 300 ) newVal = 300;

		minDeviation =  (int)newVal;
		changed = true;
	}
	
 
	// void setDouchTemp( long newVal)
	// {
	// 	if( newVal < 20 ) newVal = 20;
	// 	if( newVal > 70 ) newVal = 70;

	// 	maxDouchTemp =  (int)newVal;
	// 	changed = true;
	// }	
	// void setKraanTemp( long newVal)
	// {
	// 	if( newVal < 20 ) newVal = 20;
	// 	if( newVal > 70 ) newVal = 70;

	// 	maxKraanTemp =  (int)newVal;
	// 	changed = true;
	// }	


	void setVochtHoog( long newVal)
	{
		if( newVal < 40 ) newVal = 40;
		if( newVal > 99 ) newVal = 99;

		vochtHoog =  (int)newVal;

		if(vochtLaag > (vochtHoog - 3))  vochtLaag = vochtHoog - 3;
		 
		changed = true;
	}
	void setVochtLaag( long newVal)
	{
		if( newVal < 40 )				newVal = 40;
		if( newVal > ( vochtHoog - 3)) 	newVal =( vochtHoog - 3);

		vochtLaag =  (int)newVal;
		changed = true;
	}
};
EEParms eeParms;

///*   bridge to Dimmer class. It is not possible to fix this in class code.   */
//void zeroCrossingInterrupt()
//{
//
//	sei();
//	myTriac.zeroCrossing();
//}
//ISR(TIMER1_COMPA_vect) // timerEvent
//{
//	sei();
//	myTriac.gateHigh();// timer1 comparator match  //ISR(TIMER2_COMPA_vect)      { myDimmer.timerEvent();    } // timer2 comparator match
//}
//ISR(TIMER1_OVF_vect) // timeOverflow
//{
//	sei();
//	myTriac.gateLow();		//ISR(TIMER2_OVF_vect)        { myDimmer.timerOverflow(); }
//}

#ifdef ISR_PARENT_TWI
	ISR(TWI_vect)  { parentNode.tw_int(); }
#endif
#ifdef ISR_CHILDS_TWI
	ISR(TWI_vect)  { childNodes.tw_int(); }
#endif

#ifdef SPI_PARENT
	ISR (SPI_STC_vect)	{ parentNode.handleInterupt();}
#endif

// #ifdef TWI_PARENT
// ISR(TWI_vect)
// {
// 	//sei();
// 	parentNode.tw_int();
// }
// #endif

void localSetVal(int id, long val)
{
	switch(id )
	{
	case 12:
		secOfTheDay = val;
		secOfTheDay = secOfTheDay % 86400;
		nextSecond = millis()+ 1000;
		myTimers.nextTimer(TIMER_SECOND_OF_THE_DAY, 3600);
		break;

	// case BME_HUM_ID:
	// 	bmeHum = val;
	// 	break;

	// case BME_TEMP_ID:
	// 	bmeTemp = val;
	// 	break;	

	// case BME_ABS_ID:
	// 	bmeAbs = val;
	// 	break;		
 
	case SET_HIGH_DELAY_ID:
		eeParms.setHighDelay(val);
		resetFan();
		break;

	case SET_MEDIUM_DELAY_ID:
		eeParms.setMediumDelay(val);
		resetFan();
		break;

	case SET_LOW_DELAY_ID:
		eeParms.setLowDelay(val);
		resetFan();
		break;		

	// case SET_FAN_MANUAL_ID:
	// 	switch(fanSpeed ){
	// 	case 3: myTimers.nextTimer( TIMER_FAN_HIGH, eeParms.manualTimerMin * 60); break;		
	// 	case 2: myTimers.nextTimer( TIMER_FAN_MEDIUM, eeParms.manualTimerMin * 60); break;		
	// 	case 1: myTimers.nextTimer( TIMER_FAN_LOW, eeParms.manualTimerMin * 60); break;	
	// 	}
	// 	myTimers.nextTimer( TIMER_FAN_HIGH, eeParms.highDelayMin * 60);
	// 	break;

	case FAN_ID:
		resetFan();
		switch(val % 4 ){
		case 3: myTimers.nextTimer( TIMER_FAN_HIGH, eeParms.manualTimerMin * 60); break;		
		case 2: myTimers.nextTimer( TIMER_FAN_MEDIUM, eeParms.manualTimerMin * 60); break;		
		case 1: myTimers.nextTimer( TIMER_FAN_LOW, eeParms.manualTimerMin * 60); break;	
		}
		fanSpeed = val % 4; 
		break;

 
	case VOCHT_HOOG_ID:
		eeParms.setVochtHoog(val);
		resetFan();
		// upload(VOCHT_LAAG_ID);
		break;
	case VOCHT_LAAG_ID:
		eeParms.setVochtLaag(val );
		resetFan();
		break;

	case MANUAL_TIMER_ID:
		eeParms.setManualTimer(val);
		resetFan();
		break;

	case DEVIATION_ID:
		eeParms.setMinDeviation(val);
		break;			

	case DEVIATION_TIMER_ID:
		eeParms.setStatsTimer(val);
		break;


	default:
		eeParms.setVal( id,  val);
		parentNode.setVal( id,  val);
		break;
	}

	if(id==2)
	{
		// tempDouch.setSyncInterval(eeParms.samplePeriode_sec);
		// #ifndef DEBUG
		// 	tempBuiten.setSyncInterval(eeParms.samplePeriode_sec);
		// 	tempKraan.setSyncInterval(eeParms.samplePeriode_sec);
		// #endif
	}
}


void nextUpload(int id){

	switch( id ){
		case FAN_ID: 				myTimers.nextTimer(TIMER_UPLOAD_FAN);			break;
		case LAMP_ID: 				myTimers.nextTimer(TIMER_LAMP_UPLOAD);			break;
		case NIGHTLY_ID: 			myTimers.nextTimer(TIMER_NIGHTLY_UPLOAD);		break;
		// case BME_HUM_ID: 			myTimers.nextTimer(TIMER_BME_UPLOAD);			break;
		case REL_HUM_ID:  myTimers.nextTimer(TIMER_UPLOAD_REL_HUM);		break;
		case HUM_TEMP_ID: myTimers.nextTimer(TIMER_UPLOAD_HUM_TEMP);	break;
		case ABS_HUM_ID:  myTimers.nextTimer(TIMER_UPLOAD_ABS_HUM ); 	break;
		case PRESS_ID:    myTimers.nextTimer(TIMER_UPLOAD_PRESS ); 		break;						
	}
}


int upload(int id)
{
	int ret = 0;
	nextUpload(id);

	switch( id )
	{
	case 2:
		upload(id, NODE_TYPE );   
		break;		
	case 8:
		upload(id, JL_VERSION );   
		break;
		
	case 12:
		upload(id,  secOfTheDay );   
		break;

	case REL_HUM_ID:
		if(relHum > 0 ){
			upload(id, relHum);
			humUploaded = relHum;
		} else {
			myTimers.nextTimer(TIMER_READ_BME, 3);	
		}
		break;

	case HUM_TEMP_ID:
		upload(id, humTemp);
		tempUploaded = humTemp;
		break;

	case ABS_HUM_ID:
		upload(id, absHum  ); 				
		absHumUploaded = absHum;
		break;

	case PRESS_ID:
		upload(id, press);
		pressUploaded = press;
		break;


	case NIGHTLY_ID:
		upload(id, nightly );   
		break;
 	

	case FAN_ID:
		upload(id, fanSpeed );  
		break;

	case LAMP_ID:
	    if(! parentNode.isTxFull() ){
			upload(id, lampOn);
			lampUploaded = lampOn;
			myTimers.nextTimer(TIMER_LAMP_UPLOAD);
		} else {
			myTimers.nextTimerMillis(TIMER_LAMP_UPLOAD, 100);
		}
		break;

	case DOUCH_TEMP_ID: tempDouch.upload(); break;
	#ifndef DEBUG
		case BUITEN_TEMP_ID: tempBuiten.upload(); break;
		case KRAAN_TEMP_ID: tempKraan.upload(); break;
	#endif
 
	case VOCHT_HOOG_ID:
		upload(id,  eeParms.vochtHoog );
		break;
	case VOCHT_LAAG_ID:
		upload(id,  eeParms.vochtLaag );
		break;
	case MANUAL_TIMER_ID:
		upload(id,  eeParms.manualTimerMin );
		break;
	case DEVIATION_TIMER_ID:
		upload(id,  eeParms.statsTimerMin );
		break;

	case DEVIATION_ID:
		upload(id,  eeParms.minDeviation );
		break;	
 	 
	case SET_HIGH_DELAY_ID:
 		upload(id,  eeParms.highDelayMin );
		break;
	case SET_MEDIUM_DELAY_ID:
 		upload(id,  eeParms.mediumDelayMin );
		break;
	case SET_LOW_DELAY_ID:
 		upload(id,  eeParms.lowDelayMin );
		break;


 
 
	// #ifdef VOLTAGE_PIN
	// 	case 11: upload(id, vin.val);    		break;
	// #endif

	default:

		if( 1==2
		 ||	eeParms.upload(id)>0
		 ||	parentNode.upload(id)>0
		){}
		break;
	}
	return ret;
}

int upload(int id, long val) { return upload(id, val, millis()); }
int upload(int id, long val, unsigned long timeStamp)
{
	#ifdef DEBUG
		Serial.print(F("@"));
		Serial.print( millis() / 1000 );
		Serial.print (F(" upl-"));
 		Serial.print(id);Serial.print(": ");Serial.println(val);
	#endif
 
	nextUpload(id);
	return parentNode.txUpload(id, val, timeStamp);
}
int uploadError(int id, long val)
{
	#ifdef DEBUG
		Serial.print("err-");Serial.print(id);Serial.print(": ");Serial.println(val);
	#endif	
	return parentNode.txError(id, val);
}

int handleParentReq( RxItem *rxItem)  // cmd, to, parm1, parm2
{
	#ifdef DEBUG
			parentNode.debug("Prnt<", rxItem);
	#endif

	if(rxItem->data.msg.node==2 || rxItem->data.msg.node == parentNode.nodeId)
	{
		return localRequest(rxItem);
	}

	if(parentNode.nodeId==0)
	{
		#ifdef DEBUG
			parentNode.debug("skip", rxItem);
		#endif
		return 0;
	}

	#ifdef DEBUG
		parentNode.debug("forward", rxItem);
	#endif

	// #ifndef SKIP_CHILD_NODES
	// 	return childNodes.putTxBuf( rxItem );
	// #endif
}


// #ifndef DHT_PIN

// int handleChildReq(RxItem *rxItem)
// {
// 	#ifdef DEBUG
// 		childNodes.debug("child<", rxItem);
// 	#endif

// 	if(rxItem->data.msg.node==2 || rxItem->data.msg.node == parentNode.nodeId)
// 	{
// 		return localRequest	(rxItem);
// 	}

// 	#ifdef DEBUG
// 		childNodes.debug("upward", rxItem);
// 		//return 0;
// 	#endif

// 	return parentNode.putTxBuf(rxItem);
// }
// #endif


int localRequest(RxItem *rxItem)
{
	#ifdef DEBUG
		parentNode.debug("locReq", rxItem);
	#endif

	int ret=0;
	int subCmd;
	int val;

	switch (  rxItem->data.msg.cmd)
	{
	// case 't':trace(); break;

	case 'L':
		switch (rxItem->data.msg.id)
		{
		// case 1:
		// 	break;
		}
		break;

	case 'l':
	    subCmd = rxItem->data.msg.val & 0x000000ff ;
    	val = rxItem->data.msg.val >> 8;
		switch (rxItem->data.msg.id)
		{
		case 1:
			parentNode.localCmd(subCmd, val);
			break;
 		case 2:
			parentNode.localCmd(subCmd+100, val);
			break;
		}
		break; 

	case 's':
	case 'S':
		localSetVal(rxItem->data.msg.id, rxItem->data.msg.val);
		upload(rxItem->data.msg.id);

		break;
	case 'r':
	case 'R':
		upload(rxItem->data.msg.id);
		break;
	case 'b':
		eeParms.resetBootCount();
 		upload(3);
		break;
	case 'B':
		wdt_enable(WDTO_15MS);
		while(true){
			delay(500);
			asm volatile ("  jmp 0");
		}

	default:
		eeParms.handleRequest(rxItem);
//		util.handleRequest(rxItem);
		break;
	}

	return ret;
}

void isr230v(){
	curr230vcount++;
}

void setup()  // TODO
{
	// delay(300);

	Serial.begin(115200);  //9600
	while(!Serial) {} // Wait

	pinMode(FAN_1_PIN, OUTPUT);
	pinMode(FAN_2_PIN, OUTPUT);
	pinMode(FAN_3_PIN, OUTPUT);

	// pinMode(50, OUTPUT);
	// pinMode(51, OUTPUT);
	// pinMode(52, OUTPUT);

	#ifdef BME_PIN
		digitalWrite(BME_PIN, HIGH);  // prevent start of a SPI transaction before initialization. 
		pinMode(BME_PIN, OUTPUT);
		SPI.begin();
	#endif


	#ifdef DETECT_230V_PIN
		pinMode(DETECT_230V_PIN, INPUT_PULLUP);
	#endif
 
	eeParms.onUpload(upload);
	eeParms.setup( );

	#ifdef VOLTAGE_PIN
		vin.onUpload(upload);
	#endif

	#ifdef DEBUG
		Serial.println(F("DEBUG Afz...")); Serial.flush();
	#else
		Serial.println(F("Afz..."));Serial.flush();
	#endif

	int nodeId = NODE_ID;
	//if(nodeId==0) nodeId=eeParms.nodeId;

	parentNode.onReceive( handleParentReq);
	parentNode.onError(uploadError);
	parentNode.onUpload(upload);
	parentNode.nodeId = nodeId;
	parentNode.isParent = true;

	// #ifndef DHT_PIN
	// 	childNodes.onReceive( handleChildReq);
	// 	childNodes.nodeId = parentNode.nodeId;
	// #endif

	parentNode.begin();
 
	tempDouch.tempMin = 500;
	tempDouch.onUpload(upload, DOUCH_TEMP_ID);
	tempDouch.onError(uploadError);
	tempDouch.setSyncInterval(eeParms.samplePeriode_sec);
 

	#ifndef DEBUG
		tempBuiten.onUpload(upload, 21);
		tempBuiten.onError(uploadError);
		tempBuiten.setSyncInterval(eeParms.samplePeriode_sec);

		tempKraan.tempMin = 500;
		tempKraan.onUpload(upload, 23);
		tempKraan.onError(uploadError);
		tempKraan.setSyncInterval(eeParms.samplePeriode_sec);
	#endif

	#ifdef TEMP_PIN
		// tempAfzuiging.tempMin = 500;
		tempAfzuiging.onUpload(upload, FAN_TEMP);
		tempAfzuiging.onError(uploadError);
	#endif

	// parentNode.txUpload(0, eeParms.bootCount);

	myTimers.nextTimer(TIMER_TRACE, 1);
	// myTimers.nextTimer(TIMER_UPLOAD_HUMIDITY_ABS, 3);
	// myTimers.nextTimer(TIMER_UPLOAD_HUMIDITY_MEDIAN, 10);
	myTimers.nextTimer(TIMER_UPLOAD_ON_BOOT, 0);
	myTimers.nextTimer(TIMER_230, 0);
	// myTimers.nextTimer(TIMER_STATS_ON_HUMID, eeParms.statsTimerMin * 60);
	myTimers.nextTimer(TIMER_SECOND_OF_THE_DAY, 3);

	myTimers.nextTimerMillis(TIMER_READ_BME, 300);

	// myTimers.nextTimer(TIMER_STATS_ON_HUMID, 21); 
	// myTimers.nextTimer(TIMER_TEST, 4);

	#ifdef DETECT_230V_PIN
		attachInterrupt(digitalPinToInterrupt(DETECT_230V_PIN), isr230v, CHANGE ); //LOW CHANGE RISING FALLING 
	#endif

	wdt_reset();
	wdt_enable(WDTO_8S);
	// wdt_disable();
}

 

void resetFan(){

	myTimers.timerOff( TIMER_FAN_HIGH );
	myTimers.timerOff( TIMER_FAN_MEDIUM );
	myTimers.timerOff( TIMER_FAN_LOW );
	fanSpeed = 0;
}

void fanLoop(){

	int currSpeed = fanSpeed;
	int currHumidity = 50;
	 
	bool currNightly = nightly;

	if( secOfTheDay > 25200L   //  7:00 am 25200L  9:00  32400L
	   && secOfTheDay < 82800L // 23:00 pm 82800    23:30 84600
	){

		nightly = false;
	} else {

		nightly = true;
	}

	if(nightly != currNightly) myTimers.nextTimer(TIMER_NIGHTLY_UPLOAD,0);
 
	#ifdef DHT_PIN
		currHumidity = dhtDouch.humidity;

		if( medianHumidity.enabled() ){
			currHumidity = medianHumidity.median();
		}
	#else
		if(bmeHum > 0){
			currHumidity = bmeHum;
		}

	#endif

	/*
	 *  set timers on max tempratures, humidity and standard deviation
	 */
	if( currHumidity >= eeParms.vochtHoog 
 
	){
		myTimers.nextTimer(TIMER_FAN_HIGH,   eeParms.highDelayMin * 60);
	} 


	if( myTimers.isTime(TIMER_LAMP_ON)
	 || currHumidity >= eeParms.vochtLaag
	//  || ( statsOnHumidity._cnt < 7 && ( humDeviation * 10 > eeParms.minDeviation ) )
	){
		myTimers.nextTimer(TIMER_FAN_MEDIUM,  eeParms.mediumDelayMin * 60);
	}	


	#ifdef DHT_PIN
		if( ! dhtDouch.isValid()
		){
			myTimers.nextTimer(TIMER_FAN_MEDIUM,  60);
		}   
	#endif


	if( myTimers.isTimerActive( TIMER_FAN_HIGH ) ){

		myTimers.nextTimer( TIMER_FAN_MEDIUM, eeParms.mediumDelayMin * 60);
		myTimers.nextTimer( TIMER_FAN_LOW, eeParms.lowDelayMin * 60);

		if(nightly) fanSpeed = 2; else fanSpeed = 3;

		if( myTimers.isTime( TIMER_FAN_HIGH) ){
			myTimers.timerOff( TIMER_FAN_HIGH );
			fanSpeed = 2;			
		}

	} else if( myTimers.isTimerActive( TIMER_FAN_MEDIUM ) ){

		myTimers.nextTimer( TIMER_FAN_LOW,  eeParms.lowDelayMin * 60);
		fanSpeed = 2;

		if( myTimers.isTime( TIMER_FAN_MEDIUM ) ){
			myTimers.timerOff( TIMER_FAN_MEDIUM );
			fanSpeed = 1;			
		}

	} else if( myTimers.isTimerActive( TIMER_FAN_LOW ) ){

		fanSpeed = 1;

		if( myTimers.isTime( TIMER_FAN_LOW ) ){
			myTimers.timerOff( TIMER_FAN_LOW );
			fanSpeed = 0;
		}
	} else {

		fanSpeed = 0;
	}	


	/*
	 *  translate FAN_timers to actual speed HIGH, MEDIUM, LOW, OFF
	 */
	// if( myTimers.isTimerActive( TIMER_FAN_HIGH ) ){

	// 	myTimers.nextTimer( TIMER_FAN_MEDIUM, eeParms.mediumDelayMin * 60);
	// 	myTimers.nextTimer( TIMER_FAN_LOW, eeParms.lowDelayMin * 60);
	// 	if( myTimers.isTime( TIMER_FAN_HIGH ) || nightly ){

	// 		myTimers.timerOff( TIMER_FAN_HIGH );
	// 		fanSpeed = 2;
	// 	} else {
 
	// 		fanSpeed = 3;
	// 	}

	// } else if( myTimers.isTimerActive( TIMER_FAN_MEDIUM ) ){

	// 	myTimers.nextTimer( TIMER_FAN_LOW,  eeParms.lowDelayMin * 60);
	// 	if( myTimers.isTime( TIMER_FAN_MEDIUM ) ){

	// 		myTimers.timerOff( TIMER_FAN_MEDIUM );
	// 		fanSpeed = 1;			
	// 	} else {
			
	// 		fanSpeed = 2;
	// 	}

	// } else if( myTimers.isTimerActive( TIMER_FAN_LOW ) ){

	// 	if( myTimers.isTime( TIMER_FAN_LOW ) ){
	// 		myTimers.timerOff( TIMER_FAN_LOW );
	// 		fanSpeed = 0;
	// 	} else {
	// 		fanSpeed = 1;
	// 	}

	// } else{

	// 	if(fanSpeed>0 && nightly) fanSpeed--;
	// }

	if( fanSpeed != currSpeed){
		myTimers.nextTimer(TIMER_UPLOAD_FAN, 0);
	}

	{
		if( digitalRead(FAN_1_PIN) && fanSpeed!=1  ) digitalWrite(FAN_1_PIN, 0); 
		if( digitalRead(FAN_2_PIN) && fanSpeed!=2  ) digitalWrite(FAN_2_PIN, 0); 
		if( digitalRead(FAN_3_PIN) && fanSpeed!=3  ) digitalWrite(FAN_3_PIN, 0); 

		if( fanSpeed==1 && !digitalRead(FAN_1_PIN) ) digitalWrite(FAN_1_PIN, 1); 
		if( fanSpeed==2 && !digitalRead(FAN_2_PIN) ) digitalWrite(FAN_2_PIN, 1); 
		if( fanSpeed==3 && !digitalRead(FAN_3_PIN) ) digitalWrite(FAN_3_PIN, 1); 
	}
}

void v230loop(){

	if(myTimers.isTime(TIMER_230)){
		myTimers.nextTimerMillis(TIMER_230, 70);
		lampOn =  curr230vcount > prev230vcount + 5;

		prev230vcount = curr230vcount;
		if( lampOn != lampUploaded ){
			myTimers.nextTimer(TIMER_LAMP_UPLOAD, 0);
			if(lampOn) myTimers.nextTimer(TIMER_LAMP_ON, 3);			
		}

		if(!lampOn) myTimers.timerOff(TIMER_LAMP_ON);
	}
}

void statsOnHumidityLoop(){

	// if(myTimers.isTime(TIMER_STATS_ON_HUMID)){
	// 	myTimers.nextTimer(TIMER_STATS_ON_HUMID, eeParms.statsTimerMin * 60);  // every x minutes
	// 	// myTimers.nextTimer(TIMER_STATS_ON_HUMID, 21); 
	// 	if(medianHumidity.enabled() ){
	// 		statsOnHumidity.add(medianHumidity._median);
	// 		humDeviation = statsOnHumidity.std();
	// 		upload(DOUCH_HUMIDITY_STATS_ID, humDeviation*100);
	// 	}
	// }
}

void dayTimeLoop(){

	if(millis()>=nextSecond)
	{
		secOfTheDay++;
		secOfTheDay = secOfTheDay % 86400;
		nextSecond = millis()+ 1000;
	}
}


#ifdef BME_PIN

bool BMEInit(){

	if( !bmeStarted 
	 && bme.begin() 
	){
		bmeStarted = true;
		switch(bme.chipModel())
		{
			case BME280::ChipModel_BME280:
			#ifdef DEBUG
				Serial.println("Found BME280 sensor! Success.");
			#endif
			break;
			case BME280::ChipModel_BMP280:
			Serial.println("Found BMP280 sensor! No Humidity available.");
			break;
			default:
			Serial.println("Found UNKNOWN sensor! Error!");
		}		
	}

	return bmeStarted;
}


bool BMELoop(){

	if( myTimers.isTime(TIMER_READ_BME)
	){
		if( ! bmeStarted ){

			if( ! BMEInit() ){
				myTimers.nextTimer(TIMER_READ_BME, 3);
				// #ifdef DEBUG		
					Serial.println("bmeErr");
				// #endif	
			} 		
		}

		if( bmeStarted ){

			myTimers.nextTimer(TIMER_READ_BME, 21);		
			bme.read(bmePress, bmeTemp, bmeHum, tempUnit, presUnit);


			if( bmePress == NAN || bmeTemp == NAN || bmeHum == NAN ) {
				Serial.println("bmeReadErr");
				return false;
			}

			relHum = bmeHum;
			humTemp = bmeTemp;
			// absHum = bmeHum * 0.42 * exp(bmeTemp*0.06235398) * 10;  /* in   0.01 g/m3 */
			absHum = bmeHum * 4.2 * exp(bmeTemp*0.06235398);  /* in   0.01 g/m3 */
			press = bmePress / 100;	

			if( relHum != humUploaded ){
				myTimers.nextTimerMillis(TIMER_UPLOAD_REL_HUM, 700);
			} 
			if( humTemp != tempUploaded ) {
				myTimers.nextTimerMillis(TIMER_UPLOAD_HUM_TEMP, 800);
			}
			if( absHum != absHumUploaded ) {
				myTimers.nextTimerMillis(TIMER_UPLOAD_ABS_HUM, 900);
			}
			if( press != pressUploaded ) myTimers.nextTimerMillis(TIMER_UPLOAD_PRESS, 1000);
 
			return true;
		}
	}
	return false;
}
#endif
 
void loop()  // TODO
{


	// if(myTimers.isTime(TIMER_TEST)){
	// 	Serial.print(F("TIMER_TEST: "));

	// 	// fan2.setManual(1);
	// 	// myTimers.timerOff(TIMER_TEST);

	// 	// fanSpeed = ( fanSpeed + 1 ) % 4 ;
	// 	// Serial.println( fanSpeed);

	// 	myTimers.nextTimer(TIMER_TEST, 7);
	// }

	dayTimeLoop();
	v230loop();
	fanLoop();

	#ifdef BME_PIN
		BMELoop(); 
	#endif

 
	tempDouch.loop();

	#ifndef DEBUG
		tempBuiten.loop();
		tempKraan.loop();
	#endif

	// #ifdef TEMP_PIN
	// 	tempAfzuiging.loop();
	// #endif

	eeParms.loop();	
	parentNode.loop();

	// #ifndef DHT_PIN
	// 	childNodes.loop();
	// #endif

	if( parentNode.isReady() 
	 && parentNode.isTxEmpty()
	){
		if( myTimers.isTime(TIMER_UPLOAD_ON_BOOT)
		){
			myTimers.nextTimerMillis(TIMER_UPLOAD_ON_BOOT, TWI_SEND_ERROR_INTERVAL);
			switch( uploadOnBootCount )
			{
			case 1:
			    if(millis()<60000) upload(1);
				break;    // boottimerr
			case 4: upload(3); break; //bootcount
			case 5: upload(50); break; //buildin led
			// settings
			case 7: upload(2); break;
			case 8: upload(8); break;	// version 

			case 9: upload(VOCHT_HOOG_ID); break;
			case 10: upload(VOCHT_LAAG_ID); break;
			case 11: upload(MANUAL_TIMER_ID); break;
			case 12: upload(DEVIATION_TIMER_ID); break;
			case 13: upload(DEVIATION_ID); break;
			case 14: upload(MAX_DOUCH_TEMP_ID); break;
			case 15: upload(MAX_KRAAN_TEMP_ID); break;

			case 16: upload(FAN_ID); break;

			case 18: upload(LAMP_ID); break;

			case 20: upload(SET_HIGH_DELAY_ID); 	break;
			case 21: upload(SET_MEDIUM_DELAY_ID); 	break;
			case 22: upload(SET_LOW_DELAY_ID); 		break;

			case 23: upload(NIGHTLY_ID); 			break;
			// case 24: upload(BME_HUM_ID); 			break; 

			case 30: myTimers.timerOff(TIMER_UPLOAD_ON_BOOT); break;			
			}

			uploadOnBootCount++;

		}

		if( myTimers.isTime(TIMER_UPLOAD_REL_HUM)) {
			upload(REL_HUM_ID);
			myTimers.nextTimer(TIMER_UPLOAD_REL_HUM );
			return;
		}

		if( myTimers.isTime(TIMER_UPLOAD_ABS_HUM)) {
			upload(ABS_HUM_ID);
			myTimers.nextTimer(TIMER_UPLOAD_ABS_HUM );
			return;
		}

		if( myTimers.isTime(TIMER_UPLOAD_HUM_TEMP)) {
			upload(HUM_TEMP_ID);
			myTimers.nextTimer(TIMER_UPLOAD_HUM_TEMP );
			return;
		}

		if( myTimers.isTime(TIMER_UPLOAD_PRESS)) {
			upload(PRESS_ID);
			myTimers.nextTimer(TIMER_UPLOAD_PRESS );
			return;
		}

		if( myTimers.isTime(TIMER_UPLOAD_FAN)) {
			upload(FAN_ID);
			myTimers.nextTimer(TIMER_UPLOAD_FAN);
			return;
		}
		if( myTimers.isTime(TIMER_LAMP_UPLOAD)) {
			upload(LAMP_ID);
			myTimers.nextTimer(TIMER_LAMP_UPLOAD);
			return;
		}
 		if( myTimers.isTime(TIMER_SECOND_OF_THE_DAY)){
			parentNode.txCmd('D', parentNode.nodeId, 12);
			myTimers.nextTimer(TIMER_SECOND_OF_THE_DAY, 7);
			return;
		}
		if( myTimers.isTime(TIMER_NIGHTLY_UPLOAD)) {
			upload(NIGHTLY_ID);
			myTimers.nextTimer(TIMER_NIGHTLY_UPLOAD);
			return;
		}
		// if( myTimers.isTime(TIMER_BME_UPLOAD)) {
		// 	upload(BME_HUM_ID);
		// 	myTimers.nextTimer(TIMER_BME_UPLOAD);
		// 	return;
		// }		
	}

	// parentNode.loopSerial();  // enable test input from console. Not working in VSCode :(

	#ifdef DEBUG		
		if( myTimers.isTime(TIMER_TRACE)){ trace();}
	#endif

	wdt_reset();
	delay(0);
}
 
#ifdef DEBUG
void trace()
	{
		myTimers.nextTimer(TIMER_TRACE, TRACE_SEC);

		Serial.print(F("@"));
		Serial.print( millis() / 1000 );
		Serial.print (F(": "));

	//	Serial.print(F(", free="));	Serial.print(System::ramFree());
	//	Serial.println();

		Serial.print(F("fan:"));	Serial.print(fanSpeed);
		Serial.print(F(", n@"));	Serial.print(myTimers.timers[TIMER_FAN_HIGH ]/1000);
		Serial.print(F("-"));	Serial.print(myTimers.timers[TIMER_FAN_MEDIUM ]/1000);
		Serial.print(F("-"));	Serial.print(myTimers.timers[TIMER_FAN_LOW ]/1000);

		Serial.print(F(", hoog:")); Serial.print( eeParms.vochtHoog );
		Serial.print(F("%, laag:")); Serial.print( eeParms.vochtLaag );
		// Serial.print(F("%, ")); Serial.print( eeParms.maxDouchTemp ); Serial.print(" C");
		// Serial.print(F(", std:"));	Serial.print(statsOnHumidity.std());
		// Serial.print(F(", minDevi:")); Serial.print( eeParms.minDeviation );
		Serial.print(F(", onBt:"));	Serial.print(uploadOnBootCount);
	
 		Serial.print(F(", night:"));	Serial.print(nightly);	
		#ifdef DETECT_230V_PIN
			Serial.print(", 230v:"); Serial.print( lampOn );
			Serial.print("-"); Serial.print( curr230vcount );
		#endif

		// Serial.print(F(", time "));	Serial.print(secOfTheDay/3600);	
		// Serial.print(F(":"));	Serial.print( ( secOfTheDay - ( secOfTheDay/3600) * 3600) / 60);	


		Serial.print(", bme: ");
		Serial.print(humTemp);
		Serial.print("°"+ String(tempUnit == BME280::TempUnit_Celsius ? 'C' :'F'));
		Serial.print(", ");
		Serial.print(relHum);
		Serial.print("% - ");
		Serial.print( (float) absHum / 100.0);
		Serial.print(" g/m3, ");
		Serial.print(press);
		Serial.print(" mbar");

		Serial.println();	

		parentNode.trace("nw");

		// digitalWrite(50, !digitalRead(50) );
		// digitalWrite(51, !digitalRead(51) );
		// digitalWrite(52, !digitalRead(52) );
		// digitalWrite(53, !digitalRead(53) );

		tempDouch.trace("temp");	
		// tempAfzuiging.trace("afz");
 
	}
#endif