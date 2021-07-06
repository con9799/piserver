#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FastLED.h>
Adafruit_SSD1306 display(128, 32, &Wire, -1);
CRGB leds[36];

int currentMenu = 0;
String bstate, fanstate;
int ledstate = 1;
int oldledstate;
int shutdown = 0;
float fanTemp = 0;
unsigned long upinter[7] = {0, 0, 0, 0, 0, 0};
int currentPixels[3] = {0, 0, 0};
int currentStates[2] = {0, 0};
uint8_t hue = 0;
int SpeedDelay = 100;
CHSV colorStart = CHSV(96,255,255); 
CHSV colorTarget = CHSV(192,255,255); 
CHSV colorCurrent = colorStart;
int ledAuto = 0;
int rnadomm = 1;
int warning = 0;
String dh = "-";
String dip = "-";
String dtun = "-";
String dcu = "-";
String dmu = "-";
String dda = "-";
String ddu = "-";
String ddp = "-";
String dswa = "-";
String dswu = "-";
String dct = "-";
String dgt = "-";
String dv = "-";
String dup = "-";
String dfun = "-";

int freeRAM() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}


String readConfig(int readAddress, int maxAddress) {
  String rdCRes = "";
  int iraL = EEPROM.read(readAddress) + 1;
  for (int ira = (readAddress + 1); ira < maxAddress; ira++)
  {
    if (ira > iraL)
    {
      break;
    }
    else {
      char iras = EEPROM.read(ira);
      rdCRes = rdCRes + (String(iras));

    }
  }
  return rdCRes;
};

void writeConfig(int writeAddress, const char str[], int maxAddress) {
  int strLen = strlen(str);
  int cStr = 0;
  EEPROM.write(writeAddress, writeAddress + strLen - 1);
  for (int wi = (writeAddress + 1); wi < (maxAddress + strLen); wi++ ) {
    if (wi > (writeAddress + strLen))
    {
      break;
    }
    else {
      EEPROM.write(wi, str[cStr]);
      cStr++;
    }
  }
};

void removeConfig(int readAddress, int maxAddress) {
  for (int i = readAddress ; i < maxAddress ; i++) {
    EEPROM.write(i, 0);
  }
}

void eraseConfig() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}

void setup() {
  pinMode(5,OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.setTimeout(50);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  FastLED.addLeds<WS2812, 7, GRB>(leds, 36);
  FastLED.clear();

  showDisplay(currentMenu);
  analogWrite(3,0);

  bstate = readConfig(5, 10);	  if(bstate == ""){bstate = F("on");}
  fanstate = readConfig(15, 30);  if(fanstate == ""){fanstate = F("auto");}
  String ledstatee = readConfig(50, 100); if(ledstatee == ""){ledstate = 1;}else{ledstate = ledstatee.toInt();}
  delay(3000);
  currentMenu = 1;
  showDisplay(currentMenu);
}

void loop() {
	if (Serial.available() > 0) 
  	{
    	toKenize( Serial.readString() );
  	}
  	if (shutdown == 0)
  	{
  		controlMenu();
  		enterMenu();
  		fan();
  		setFanTemp();
  		led();
  		randomMenu();
  	}
}
void enterMenu(){
  int button2Val = digitalRead(13);
  if (button2Val == LOW) {
  	buzzer();
  	upinter[4] = millis();
  	//Buzzer settings
    if(currentMenu == 4){
      if(bstate == F("on")){
        currentMenu = 9;
        showDisplay(currentMenu);  
      }else if(bstate == F("off")){
        currentMenu = 10;
        showDisplay(currentMenu);  
      }
      delay(300);
      return;
    }
    if(currentMenu == 9){
      currentMenu = 10;
      showDisplay(currentMenu);
      writeConfig(5, "off", 10);
      bstate = F("off");
    }else if(currentMenu == 10){
      currentMenu = 9;
      showDisplay(currentMenu);
      writeConfig(5, "on", 10);
      bstate = F("on");
    }

    //Fan settings
    if(currentMenu == 5){
      	currentMenu = 11;
      	String toDisplay;
      	if (fanstate != F("auto")){
      		toDisplay = fanstate + F("%");
      	}else{
      		toDisplay = fanstate;
		}
      	display.clearDisplay();
    	display.setTextSize(1);  
    	display.setTextColor(SSD1306_WHITE); 
    	display.setCursor(60,1); 
    	display.println(F("Fan")); 
    	display.drawLine(0, 10, 128, 10, SSD1306_WHITE);    

    	display.setCursor(1,20); 
    	display.print(F("Speed:"));
    	display.setCursor(65,20); 
    	display.print(toDisplay);
    
    	display.display();  
    	delay(300);
      	return;
    }
    if(currentMenu == 11){
      	if(fanstate == F("auto")){
      		fanstate = F("5");

      		writeConfig(15, fanstate.c_str(), 30);

      		display.clearDisplay();
    		display.setTextSize(1);  
    		display.setTextColor(SSD1306_WHITE); 
    		display.setCursor(60,1); 
    		display.println(F("Fan")); 
    		display.drawLine(0, 10, 128, 10, SSD1306_WHITE);    

    		display.setCursor(1,20); 
    		display.print(F("Speed:"));
    		display.setCursor(65,20); 
    		display.print(F("5%"));
    
    		display.display();
      	}else{
      		int fanIstate = fanstate.toInt();
      		fanIstate = fanIstate + 5;
      		String toDisplay;
      		if (fanIstate > 100)
      		{
      			fanstate = F("auto");
      		}else{
      			fanstate = String(fanIstate);
      		}
      		if (fanstate != F("auto"))
      		{
      			toDisplay = fanstate + F("%");
      		}else{

      			toDisplay = fanstate;
      		}
      		writeConfig(15, fanstate.c_str(), 30);

      		display.clearDisplay();
    		display.setTextSize(1);  
    		display.setTextColor(SSD1306_WHITE); 
    		display.setCursor(60,1); 
    		display.println(F("Fan")); 
    		display.drawLine(0, 10, 128, 10, SSD1306_WHITE);    

    		display.setCursor(1,20); 
    		display.print(F("Speed:"));
    		display.setCursor(65,20); 
    		display.print(toDisplay);
    
    		display.display();
      	}
      	delay(300);
      	return;
    }

    if(currentMenu == 11){
      	if(fanstate == F("auto")){
      		fanstate = F("5");

      		writeConfig(15, fanstate.c_str(), 30);

      		display.clearDisplay();
    		display.setTextSize(1);  
    		display.setTextColor(SSD1306_WHITE); 
    		display.setCursor(60,1); 
    		display.println(F("Fan")); 
    		display.drawLine(0, 10, 128, 10, SSD1306_WHITE);    

    		display.setCursor(1,20); 
    		display.print(F("Speed:"));
    		display.setCursor(65,20); 
    		display.print(F("5%"));
    
    		display.display();
      	}else{
      		int fanIstate = fanstate.toInt();
      		fanIstate = fanIstate + 5;
      		String toDisplay;
      		if (fanIstate > 100)
      		{
      			fanstate = F("auto");
      		}else{
      			fanstate = String(fanIstate);
      		}
      		if (fanstate != F("auto"))
      		{
      			toDisplay = fanstate + F("%");
      		}else{

      			toDisplay = fanstate;
      		}
      		writeConfig(15, fanstate.c_str(), 30);

      		display.clearDisplay();
    		display.setTextSize(1);  
    		display.setTextColor(SSD1306_WHITE); 
    		display.setCursor(60,1); 
    		display.println(F("Fan")); 
    		display.drawLine(0, 10, 128, 10, SSD1306_WHITE);    

    		display.setCursor(1,20); 
    		display.print(F("Speed:"));
    		display.setCursor(65,20); 
    		display.print(toDisplay);
    
    		display.display();
      	}
      	
    }

    //Led settings
    if(currentMenu == 6){
      	currentMenu = 12;

      	controlSubPage(F("Led"), F("Animation:"), String(ledstate));
    	delay(300);
      	return;
    }
    if(currentMenu == 12){
    	ledstate++;
    	if (ledstate > 8)
    	{
    		ledstate = 0;
    	}
    	writeConfig(50, String(ledstate).c_str(), 100);
		controlSubPage(F("Led"), F("Animation:"), String(ledstate));
      	
    }
    //Restart menu
    if(currentMenu == 7){
      	currentMenu = 13;

      	display.clearDisplay();
    	display.setTextSize(1);  
    	display.setTextColor(SSD1306_WHITE); 
    	display.setCursor(2,10); 
    	display.print(F("Do you really restart")); 
    	display.setCursor(50,22); 
    	display.println(F("server?")); 
    	display.display();  
    	delay(300);
      	return;
    }
    if(currentMenu == 13){
    	Serial.println(F("restart"));
    	int x = 0;
      	for(;;) {
      		display.clearDisplay();
    		display.setTextSize(1);  
    		display.setTextColor(SSD1306_WHITE); 
    		display.setCursor(24,14); 
    		display.print(F("Restarting")); 
    		switch(x) {
        		case 0: display.print(F(".")); x=1;break;
        		case 1: display.print(F("..")); x=2;break;
        		case 2: display.print(F("...")); x=3; break;
        		case 3: display.print(F("....")); x=4; break;
        		case 4: display.print(F(".....")); x=0; break;
      		}
    		display.display();
    		delay(200);
      	}
    }
    //Shutdown menu
    if(currentMenu == 8){
      	currentMenu = 14;

      	display.clearDisplay();
    	display.setTextSize(1);  
    	display.setTextColor(SSD1306_WHITE); 
    	display.setCursor(20,10); 
    	display.print(F("Do you really ")); 
    	display.setCursor(20,22); 
    	display.println(F("shutdown server?")); 
    	display.display();  
    	delay(300);
      	return;
    }
    if(currentMenu == 14){
    	Serial.println(F("shutdown"));
    	int x = 0;
    	int y = 0;
      	for(;;) {
      		display.clearDisplay();
    		display.setTextSize(1);  
    		display.setTextColor(SSD1306_WHITE); 
    		display.setCursor(24,14); 
    		display.print(F("Shutdown")); 
    		switch(x) {
        		case 0: display.print(F(".")); x=1;break;
        		case 1: display.print(F("..")); x=2;break;
        		case 2: display.print(F("...")); x=3; break;
        		case 3: display.print(F("....")); x=4; break;
        		case 4: display.print(F(".....")); x=0; break;
      		}

    		display.display();
    		switch(x) {
        		case 0:
        			FastLED.clear();
      				for (int i = 0; i <= 36; i++) {
      					leds[i] = CRGB ( 255, 255, 255);
    					FastLED.show();
    					delay(10);
  					}
  					y= 1;
        		break;
        		case 1:
        			FastLED.clear();
      				for (int i = 36; i >= 0; i--) {
      					leds[i] = CRGB ( 255, 255, 255);
    					FastLED.show();
    					delay(10);
  					}
  					y = 0;
        		break;
      		}
    		delay(200);
    		unsigned long currentSHTime = millis();
  			if (currentSHTime - upinter[3] >= 15000) {
  				display.clearDisplay();
  				display.display();
  				ledstate = 0;
  				fanstate = F("0");
  				analogWrite(5,0);
  				setAll(0, 0, 0);
  				currentMenu = 0;
  				shutdown = 1;
  				upinter[3] = currentSHTime;
  				break;
  			}
    		
      	}
    }
    if (currentMenu == 15)
    {
    	currentMenu = 1;
    	showDisplay(currentMenu);
    	warning = 0;
    	upinter[6] = 0;
    	ledstate = 7;
    }
  delay(300);
  }
  
}

void controlMenu(){
  int buttonVal = digitalRead(12);
  if (buttonVal == LOW) {
  	buzzer();
  	upinter[4] = millis();
    currentMenu++;
    if(currentMenu > 8){
      currentMenu = 1;
    }
    showDisplay(currentMenu);
  	delay(300);
  }
}

void showDisplay(int page){
  if(page == 0){
    display.clearDisplay();
    display.setTextSize(2);            
    display.setTextColor(SSD1306_WHITE); 
    display.setCursor(30,3); 
    display.println(F("DEVCON"));
    
    display.setTextSize(1);            
    display.setCursor(2,24); 
    display.println(F("(c) Manuchehr Usmonov"));
    
    display.display();
  }
  if(page == 1){
    display.clearDisplay();
    display.setTextSize(1);            
    display.setTextColor(SSD1306_WHITE); 
    int textPosition = (128 - textWidth(dh.c_str()))/2;
    display.setCursor(textPosition,0); 
    display.println(dh);
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
    display.setCursor(1,15); 
    display.print(F("IP:  ")); display.print(dip);
    display.setCursor(1,25); 
    display.print(F("Tun: ")); display.print(dtun);
    display.display();
  }
  if(page == 2){
    display.clearDisplay();
    display.setTextSize(1);  
    display.setTextColor(SSD1306_WHITE);     
    display.setCursor(1,1); 
    display.print(F("CPU: ")); display.print(dcu);
    display.setCursor(60,1); 
    display.print(F("|"));
    display.setCursor(74,1); 
    display.print(F("MEM: ")); display.print(dmu);
    display.setCursor(1,12); 
    display.print(F("Disk: ")); display.print(ddp); display.print("/");display.print(ddu);display.print("/");display.print(dda);
    display.setCursor(1,24); 
    display.print(F("Swap: ")); display.print(dswu); display.print(" | "); display.print(dswa);
    
    display.display();
  }
  if(page == 3){
    display.clearDisplay();
    display.setTextSize(1);  
    display.setTextColor(SSD1306_WHITE); 
    display.setCursor(18,1); 
    display.print(F("Uptime: ")); display.print(dup); 
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);    
    display.setCursor(1,15); 
    display.print(F("CPU: ")); display.print(dct); 
    display.setCursor(60,15); 
    display.print(F("|"));
    display.setCursor(72,15); 
    display.print(F("PRO: ")); display.print(dgt);
    display.setCursor(1,25); 
    display.print(F("FAN: ")); display.print(dfun);
    display.setCursor(60,25); 
    display.print(F("|"));
    display.setCursor(72,25); 
    display.print(F("Vol: ")); display.print(dv);
    display.display();
  }
  if(page == 4){
    controlPage(0);
  }
  if(page == 5){
    controlPage(1);
  }
  if(page == 6){
    controlPage(2);
  }
  if(page == 7){
    controlPage(3);
  }
  if(page == 8){
    controlPage(4);
  }
  if(page == 9){
    controlSubPage(F("Buzzer"), F("State:"), F("On"));
  }
  if(page == 10){
  	controlSubPage(F("Buzzer"), F("State:"), F("Off"));
  }
  if(page == 15){
  	display.clearDisplay();
    display.setTextSize(2);  
    display.setTextColor(SSD1306_WHITE); 
    display.setCursor(20,10); 
    display.print(F("WARNING")); 
    display.display();  
    delay(300);
    return;
  }
}

void controlPage(int n){
	display.clearDisplay();
    display.setTextSize(1);  
    display.setTextColor(SSD1306_WHITE); 
    display.setCursor(45,1); 
    display.println(F("Control")); 
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);    

    display.setCursor(5,15); 
    if (n == 0){display.print(F("* Buz"));}else{display.print(F("Buz"));}
    display.setCursor(55,15); 
    if (n == 1){display.print(F("* Fan"));}else{display.print(F("Fan"));}

    display.setCursor(95,15); 
    if (n == 2){display.print(F("* Led"));}else{display.print(F("Led"));}

    display.setCursor(5,25); 
    if (n == 3){display.print(F("* Restart"));}else{display.print(F("Restart"));}
    display.setCursor(65,25); 
    if (n == 4){display.print(F("* Shutdown"));}else{display.print(F("Shutdown"));}
    display.display();
}

void controlSubPage(String ti, String it, String va){
	display.clearDisplay();
    display.setTextSize(1);  
    display.setTextColor(SSD1306_WHITE); 
    display.setCursor(45,1); 
    display.println(ti); 
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);    

    display.setCursor(1,20); 
    display.print(it);
    display.setCursor(65,20); 
    display.print(va);
    
    display.display();
}
uint16_t charWidth(const char ch)
{
  char onecharstring[2] = {ch, 0};
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(onecharstring, 0, 0, &x1, &y1, &w, &h);
  return w;
}

uint16_t textWidth(const char* text)
{
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  return w;
}

void fan(){
	float fanSpeed;
	if (fanstate == F("auto"))
	{
		fanSpeed = map(fanTemp, 45, 75, 30, 255);
	}else{
		fanSpeed = map(fanstate.toInt(),0,100,30,255);
	}
	cFanSpeed(fanSpeed);
  if(fanSpeed < 29){fanSpeed = 0;}
	analogWrite(5, fanSpeed);
}

void setFanTemp(){
	float ct = dct.toInt();
	fanTemp = ct;

	if (fanTemp > 70)
	{
		warning = 1;
    if (ledstate != 10)
    {
      oldledstate = ledstate;
    }
		ledstate = 10;
		digitalWrite(10, HIGH);
    delay(50);
    digitalWrite(10, LOW);
    delay(100);
		if (currentMenu != 15)
		{
			currentMenu = 15;
			showDisplay(currentMenu);
		}
	}else{
		warning = 0;
    if (currentMenu == 15)
    {
      currentMenu = 1;
      ledstate = oldledstate;
      showDisplay(currentMenu);
    }
	}
}

int cFanSpeed(float fanSpeed){
	int pspeed = map(fanSpeed,30,255,0,100);
	if (pspeed < 0){pspeed = 0;}
	dfun = String(pspeed) + F("%");
}

void led(){
	switch(ledstate)
	{
		case 0: setAll(0,0,0); break;
		case 1: rb(0); break;
		case 2: rb(1); break;
		case 3: rb(2); break;
		case 4: rb(3); break;
		case 5: rb(4); break;
		case 6: rainbow(); break;
		case 7: RGBLoop(); break;
		case 8: SnowSparkle(); break;
		case 10: warningLed(); break;
	}
	
}

void warningLed(){
	unsigned long currentTime = millis();
  	if (currentTime - upinter[6] >= 100) {
  		if (warning == 1)
  		{
  			Serial.println(F("warning"));
  		}
		if(currentStates[1] == 0){
			setAll(255, 0, 0);
			currentStates[1] = 1;
		}else{
			setAll(0, 0, 0);
			currentStates[1] = 0;
		}
		upinter[6] = currentTime;
	}
}

void rb(int color){
  unsigned long currentTime = millis();
  if (currentTime - upinter[0] >= 40) {
    if(currentStates[0] == 0){
      if(color == 0){
        leds[currentPixels[0]] = CRGB ( 0, 0, 255);
      }else if(color == 1){
        leds[currentPixels[0]] = CRGB ( 0, 255, 0);
      }else if(color == 2){
        leds[currentPixels[0]] = CRGB ( 0, 255, 0);
      }else if(color == 3){
        leds[currentPixels[0]] = CRGB ( 0, 255, 255);
      }else if(color == 4){
        leds[currentPixels[0]] = CRGB ( random(255), random(255), random(255));
      }
      FastLED.show();  
      currentPixels[0]++;
      if(currentPixels[0] >= 36-1){
        currentStates[0] = 1;
      }
    }
    if(currentStates[0] == 1){
      if(color == 0){
        leds[currentPixels[0]] = CRGB ( 255, 0, 0);
      }else if(color == 1){
        leds[currentPixels[0]] = CRGB ( 255, 0, 0);
      }else if(color == 2){
        leds[currentPixels[0]] = CRGB ( 0, 0, 255);
      }else if(color == 3){
        leds[currentPixels[0]] = CRGB ( 255, 255, 0);
      }else if(color == 4){
        leds[currentPixels[0]] = CRGB ( random(255), random(255), random(255));
      }
      FastLED.show();  
      currentPixels[0]--;
      if(currentPixels[0] == 0){
        currentStates[0] = 0;
      }
    }
    upinter[0] = currentTime;
  } 
}

void rainbow(){
  for (int i = 0; i < 36; ++i) {
    leds[i] = CHSV(hue + (i * 10), 255, 255);
  }

  EVERY_N_MILLISECONDS(15){
    hue++;
  }
  
  FastLED.show();
}

void RGBLoop(){
  EVERY_N_MILLISECONDS(50){
    static uint8_t k; 
    if ( colorCurrent.h == colorTarget.h ) { 
      colorStart = colorCurrent;
      colorTarget = CHSV(random8(),255,255);
      k = 0;
    }

    colorCurrent = blend(colorStart, colorTarget, k, SHORTEST_HUES);
    fill_solid( leds, 36, colorCurrent );
    leds[0] = colorTarget;
    k++;
  }

  FastLED.show();
}

void SnowSparkle() {
  setAll(0x10,0x10,0x10);
  unsigned long currentTime = millis();
  if (currentTime - upinter[1] >= SpeedDelay) {
    int Pixel = random(36);
    leds[Pixel].r = 0xff;
    leds[Pixel].g = 0xff;
    leds[Pixel].b = 0xff;
    FastLED.show();
    delay(20);
    leds[Pixel].r = 0x10;
    leds[Pixel].g = 0x10;
    leds[Pixel].b = 0x10;
    FastLED.show();
    SpeedDelay = random(100,1000);
    upinter[1] = currentTime;
  }
  
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < 36; i++ ) {
    leds[i] = CRGB ( red, green, blue);
  }
  FastLED.show();
}

void buzzer(){
	if (bstate == F("on") && warning == 0)
	{
		digitalWrite(10, HIGH);
		delay(50);
		digitalWrite(10, LOW);
    delay(30);
	}
}

void randomMenu(){
	unsigned long cT = millis();
  	if (cT - upinter[4] >= 30000) {
  		if (currentMenu != 15)
  		{
        currentMenu = rnadomm;
  			showDisplay(rnadomm);
  			buzzer();
  			upinter[4] = cT;
  			rnadomm++;
  			if (rnadomm == 4)
  			{
  				rnadomm = 1;
  			}
  		}
  	}
}

void toKenize(String data)
{
	String xx = data.substring(0,2);
	int x = xx.toInt();
	String result = data.substring(3);
	switch(x){
		case 1: dh = result; break;
		case 2: dip = result; break;
		case 3: dtun = result; break;
		case 4: dcu = result; break;
		case 5: dmu = result; break;
		case 6: dda = result; break;
		case 7: ddu = result; break;
		case 8: ddp = result; break;
		case 9: dswa = result; break;
		case 10: dswu = result; break;
		case 11: dct = result; break;
		case 12: dgt = result; break;
		case 13: dv = result; break;
		case 14: dup = result; break;
	}
  showDisplay(currentMenu);
    	
}
