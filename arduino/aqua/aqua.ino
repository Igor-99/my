#include <Wire.h> //Подключаем библиотеку для использования I2C интерфейса с модулем RTC 


#include <OneWire.h>
#include <DallasTemperature.h>

#include "RTClib.h" //Подключаем библиотеку для использования модуля часов реального времени RTC
 
#include <EEPROM.h>


OneWire oneWire(A3);
DallasTemperature ds(&oneWire); 

#include <LiquidCrystal_I2C.h> // библиотека экрана
LiquidCrystal_I2C lcd(0x27,20,4);



int PWM_LW_MIN = 0; //Если необходим ток покоя на LED - изменить эту константу 
int PWM_LW_MAX = 250; //Если необходимо ограничить максимальную яркость - уменьшить значение 
#define PWM_LW_PIN 3 //Пин порта, где будет ШИМ LW 

#define mn 60UL //Дополнительные константы для удобства 
#define hr 3600UL //Отражают соответствующие количества секунд 
#define d 86400UL 
#define dMinute 60UL //Константы для удобства перевода времени в функциях, соответсвующее количество секунд в 1 минуте
#define dHour 3600UL //Константы для удобства перевода времени в функциях, соответсвующее количество секунд в 1 часе

RTC_DS1307 RTC; 

int  RH = 0;
int  RM = 0;
int  RD = 0;
int  ZH = 0;
int  ZM = 0;
int  ZD = 0;
int  PS = 0;
int  ScD = 250;
int  ScN = 50; 


const int buttonPin = 2;
int buttonState = 0;
const int buttonPin2 = 4;
int buttonState2 = 0; 
const int buttonPin3 = 5;
int buttonState3 = 0;    

// устанавливаем дату и время по умолчанию
int  YEARSET = 2018;
int  MONTHSET = 4;
int  DAYSET = 1;
int  HOURSET = 10;
int  MINUTESET = 20;

int pos = 1; // уровень меню 1
int pos1 = 1; // уровень меню 2


//******************************************************************************************** 



void setup(){ 
 
Serial.begin(9600);
Wire.begin(); //Инициируем I2C интерфейс 

RH = EEPROM.read(0);
RM = EEPROM.read(1);
RD = EEPROM.read(2);
ZH = EEPROM.read(3);
ZM = EEPROM.read(4);
ZD = EEPROM.read(5);

ds.begin();  

RTC.begin(); //Инициирум RTC модуль 


pinMode(buttonPin, INPUT);
pinMode(buttonPin2, INPUT);
pinMode(buttonPin3, INPUT);
      
  

lcd.init(); // запускаем библиотеку экрана 
lcd.backlight();

RTC.adjust(DateTime(YEARSET, MONTHSET, DAYSET, HOURSET , MINUTESET , 01));  
if (! RTC.begin()) { 
RTC.adjust(DateTime(YEARSET, MONTHSET, DAYSET, HOURSET , MINUTESET , 01));   
delay (500);
}
// if (! RTC.isrunning()) { 
// Serial.println("RTC is NOT running!"); 
// RTC.adjust(DateTime(2014, 7, 12, 22, 48, 1)); 
// } 
//RTC.adjust(DateTime(2012, 7, 31, 21, 53, 1)); 
//RTC.adjust(DateTime(__DATE__, __TIME__)); 
} // КОНЕЦ ИНИЦИАЛИЗАЦИИ 

//******************************************************************************************** 


void loop() // ПРОГРАММЫй безусловный ЦИКЛ 
{
  

   
// long sunrise_start = RH*hr+RM*mn; //Начало восхода в 9 - 45 
// long sunrise_duration = RD*mn; //Длительность восхода 30 минут 
// long sunset_start = ZH*hr+ZM*mn; //начало заката в 21-15 
// long sunset_duration = ZD*mn; //Длительность заката 30 минут 

long sunrise_start = RH*dHour+RM*dMinute; //Начало восхода 
long sunrise_duration = RD*dMinute; //Длительность восхода 
long sunset_start = ZH*dHour+ZM*dMinute; //начало заката 
long sunset_duration = ZD*dMinute; //Длительность заката 

long pwm_LW; 
DateTime myTime = RTC.now(); //Читаем данные времени из RTC при каждом выполнении цикла 
long Day_time = myTime.unixtime() % 86400; //сохраняем в переменную - время в формате UNIX 
if ((Day_time<sunrise_start) || //Если с начала суток меньше чем начало восхода 
(Day_time>=sunset_start+sunset_duration)) { //Или больше чем начало заката + длительность 
pwm_LW = PWM_LW_MIN; //Величина для записи в порт равна минимуму 


//********************************************************************************************* 
// обработка интервала восхода 
//********************************************************************************************* 
}else if ((Day_time>=sunrise_start) && //Если с начала суток больше чем начало восхода 
(Day_time<sunrise_start+sunrise_duration)){ //И меньше чем начало восхода + длительность 

pwm_LW = ((Day_time - sunrise_start)*(PWM_LW_MAX-PWM_LW_MIN)) / sunrise_duration; //Вычисляем для рассвета величину для записи в порт ШИМ 


//********************************************************************************************* 
// обработка интервала заката 
//********************************************************************************************* 
}else if ((Day_time>=sunset_start) && //Если начала суток больше чем начало заката и меньше чем 
(Day_time<sunset_start+sunset_duration)){ //начало заката плюс длительность 

pwm_LW = ((sunset_start+sunset_duration - Day_time)*(PWM_LW_MAX-PWM_LW_MIN)) / sunrise_duration; //Вычисляем для заката величину для записи в порт ШИМ 


//******************************************************************************************** 
// обработка интервала от конца рассвета и до начала заката, 
// когда свет должен быть включен на максимальную яркость 
//******************************************************************************************** 
}else { 
pwm_LW = PWM_LW_MAX; //Устанавливаем максимальную величину для записи в порт ШИМ 

} 

analogWrite(PWM_LW_PIN, pwm_LW); //Пишем в порт вычисленное значение 



if (Day_time>=sunrise_start && Day_time<=sunset_start ){
  analogWrite(11, ScD);
}
  else {
 analogWrite(11, ScN);
}
  

  
buttonState = digitalRead(buttonPin);
if (buttonState == HIGH) { 
  pos++;
  lcd.clear();
  delay (250); 
  }


if (pos >= 16){
  lcd.clear();
  pos=1;
    }

switch (pos){
case 1:
      ds.requestTemperatures();
      //lcd.clear();
      lcd.setCursor(8,1); 
      lcd.print("t "); 
      lcd.print(ds.getTempCByIndex(0)); 
      lcd.print("C");
      lcd.setCursor(0,0); 
      lcd.print("AQUA"); 
      lcd.setCursor(6,0); 
      if (myTime.day() < 10) lcd.print("0"); 
      lcd.print(myTime.day(),DEC); 
      lcd.print("-"); 
      if (myTime.month() < 10) lcd.print("0"); 
      lcd.print(myTime.month(),DEC); 
      lcd.print("-"); 
      lcd.print(myTime.year(),DEC); 
      lcd.setCursor(0,1); 
      if (myTime.hour() < 10) lcd.print("0"); 
      lcd.print(myTime.hour(),DEC); 
      lcd.print(":"); 
      if (myTime.minute() < 10) lcd.print("0"); 
      lcd.print(myTime.minute(),DEC);
      break;
      
case 2:
       
      lcd.setCursor(1,0); 
      lcd.print("RASSVET START");
      buttonState2 = digitalRead(buttonPin2);
      if (buttonState2 == HIGH) { 
        RH++;
   
        delay(100); 
        }
      if (RH >= 24)  {
        RH = 0; 
        }
        buttonState3 = digitalRead(buttonPin3);
      if (buttonState3 == HIGH) { 
        RM++;
  
        delay(100);  
        }
      if (RM >= 60)  {
        RM = 0; 
        }
      lcd.setCursor(5,1);
      if ( RH < 10) lcd.print("0");
      
      lcd.print(RH);
      lcd.print(":");
      if ( RM < 10) lcd.print("0");
        
      lcd.print(RM);
      break;
      
case 3:
     
      lcd.setCursor(2,0); 
      lcd.print("RASSVET LONG");
      buttonState2 = digitalRead(buttonPin2);
      if (buttonState2 == HIGH) { 
        RD--;
        delay(100); 
        }
      if (RD > 60)  {
        RD = 0; 
        }
      buttonState3 = digitalRead(buttonPin3);
      if (buttonState3 == HIGH) { 
        RD++;
        delay(100); 
        }
      if (RD < 0)  {
        RD = 60;  
        }
      lcd.setCursor(7,1);
      if ( RD < 10) lcd.print("0");  
      lcd.print(RD); 
      break;
case 4:
      
      lcd.setCursor(3,0); 
      lcd.print("ZAKAT START");
      buttonState2 = digitalRead(buttonPin2);
      if (buttonState2 == HIGH) {
        ZH++;
        delay(100);
        }
      if (ZH >= 24)  {
        ZH = 0; 
        }
        buttonState3 = digitalRead(buttonPin3);
      if (buttonState3 == HIGH) { 
        ZM++;
        delay(100);
        }
      if (ZM >= 60)  {
        ZM = 0; 
        }
      lcd.setCursor(6,1);
      if ( ZH < 10) lcd.print("0");  
      lcd.print(ZH);
      lcd.print(":");
      if ( ZM < 10) lcd.print("0");  
      lcd.print(ZM);
      break;
     
case 5:
       
      lcd.setCursor(3,0); 
      lcd.print("ZAKAT LONG");
      buttonState2 = digitalRead(buttonPin2);
      if (buttonState2 == HIGH) { 
        ZD--;
        delay(100); 
        }
      if (ZD > 60)  {
        ZD = 0; 
        }
      buttonState3 = digitalRead(buttonPin3);
      if (buttonState3 == HIGH) { 
        ZD++;
        delay(100); 
        }
      if (ZD < 0)  {
        ZD = 60;  
        }
      lcd.setCursor(7,1);
      if ( ZD < 10) lcd.print("0");  
      lcd.print(ZD); 
      break;
     
case 6:
     
      lcd.setCursor(0,0); 
      lcd.print("POWER LAMP");
      lcd.print(" ");
      PS = PWM_LW_MAX / 2.5;
      lcd.print(PS);
      lcd.print(" ");
      lcd.print("%");     
      buttonState3 = digitalRead(buttonPin3);
      if (buttonState3 == HIGH) { 
        PWM_LW_MAX = PWM_LW_MAX + 25;
         
         delay(150); 
         lcd.clear();
        }
      if (PWM_LW_MAX > 250)  {
        PWM_LW_MAX = 250;
         
        }
       buttonState2 = digitalRead(buttonPin2);
      if (buttonState2 == HIGH) { 
       PWM_LW_MAX = PWM_LW_MAX - 25;
         
        delay(150);
        lcd.clear();
        }
      if (PWM_LW_MAX < 0)  {
        PWM_LW_MAX = 0;  
        }
        lcd.setCursor(3,1);
        if (PS >= 10 && PS <20 ) { 
        lcd.print("*");
        }
        if (PS >=20 && PS <30 ) { 
        lcd.print("**");
        }
        if (PS >=30 && PS <40 ) { 
        lcd.print("***");
        } 
        if (PS >=40 && PS <50 ) { 
        lcd.print("****");
        } 
        if (PS >=50 && PS <60 ) { 
        lcd.print("*****");
        } 
        if (PS >=60 && PS <70 ) { 
        lcd.print("******");
        }
        if (PS >=70 && PS <80 ) { 
        lcd.print("*******");
        }
        if (PS >=80 && PS <90 ) { 
        lcd.print("********");
        }
        if (PS >=90 && PS <100 ) { 
        lcd.print("*********");
        }
        if (PS >=100) { 
        lcd.print("**********");
        }
      break;
      
case 7:
     
      lcd.setCursor(0,0); 
      lcd.print("NIGHT POWER");
      lcd.print(" ");
      PS = PWM_LW_MIN / 2.5;
      lcd.print(PS);
      lcd.print("%");     
      buttonState3 = digitalRead(buttonPin3);
      if (buttonState3 == HIGH) { 
        PWM_LW_MIN = PWM_LW_MIN + 25;
         
         delay(150);
         lcd.clear(); 
        }
      if (PWM_LW_MIN > 250)  {
        PWM_LW_MIN = 250; 
        }
       buttonState2 = digitalRead(buttonPin2);
      if (buttonState2 == HIGH) { 
       PWM_LW_MIN = PWM_LW_MIN - 25;
        
        delay(150);
        lcd.clear(); 
        }
      if (PWM_LW_MIN < 0)  {
        PWM_LW_MIN = 0;  
        }
        lcd.setCursor(3,1);
        if (PS >= 10 && PS <20 ) { 
        lcd.print("*");
        }
        if (PS >=20 && PS <30 ) { 
        lcd.print("**");
        }
        if (PS >=30 && PS <40 ) { 
        lcd.print("***");
        } 
        if (PS >=40 && PS <50 ) { 
        lcd.print("****");
        } 
        if (PS >=50 && PS <60 ) { 
        lcd.print("*****");
        } 
        if (PS >=60 && PS <70 ) { 
        lcd.print("******");
        }
        if (PS >=70 && PS <80 ) { 
        lcd.print("*******");
        }
        if (PS >=80 && PS <90 ) { 
        lcd.print("********");
        }
        if (PS >=90 && PS <100 ) { 
        lcd.print("*********");
        }
        if (PS >=100) { 
        lcd.print("**********");
        }
      break;

case 8:
     
      lcd.setCursor(1,0); 
      lcd.print("DISP DAY");
      lcd.print(" ");
      PS = ScD / 2.5;
      lcd.print(PS);
      lcd.print(" ");
      lcd.print("%");     
      buttonState3 = digitalRead(buttonPin3);
      if (buttonState3 == HIGH) { 
        ScD = ScD + 25;
         
         delay(150); 
         lcd.clear();
        }
      if (ScD > 250)  {
        ScD = 250;
         
        }
       buttonState2 = digitalRead(buttonPin2);
      if (buttonState2 == HIGH) { 
       ScD = ScD - 25;
         
        delay(150);
        lcd.clear();
        }
      if (ScD < 0)  {
        ScD = 0;  
        }
        lcd.setCursor(3,1);
        if (PS >= 10 && PS <20 ) { 
        lcd.print("*");
        }
        if (PS >=20 && PS <30 ) { 
        lcd.print("**");
        }
        if (PS >=30 && PS <40 ) { 
        lcd.print("***");
        } 
        if (PS >=40 && PS <50 ) { 
        lcd.print("****");
        } 
        if (PS >=50 && PS <60 ) { 
        lcd.print("*****");
        } 
        if (PS >=60 && PS <70 ) { 
        lcd.print("******");
        }
        if (PS >=70 && PS <80 ) { 
        lcd.print("*******");
        }
        if (PS >=80 && PS <90 ) { 
        lcd.print("********");
        }
        if (PS >=90 && PS <100 ) { 
        lcd.print("*********");
        }
        if (PS >=100) { 
        lcd.print("**********");
        }
      break;

case 9:
     
      lcd.setCursor(0,0); 
      lcd.print("DISP NIGHT");
      lcd.print(" ");
      PS = ScN / 2.5;
      lcd.print(PS);
      lcd.print(" ");
      lcd.print("%");     
      buttonState3 = digitalRead(buttonPin3);
      if (buttonState3 == HIGH) { 
       ScN = ScN + 25;
         
         delay(150); 
         lcd.clear();
        }
      if (ScN > 250)  {
        ScN = 250;
         
        }
       buttonState2 = digitalRead(buttonPin2);
      if (buttonState2 == HIGH) { 
       ScN = ScN - 25;
         
        delay(150);
        lcd.clear();
        }
      if (ScN < 0)  {
        ScN = 0;  
        }
        lcd.setCursor(3,1);
        if (PS >= 10 && PS <20 ) { 
        lcd.print("*");
        }
        if (PS >=20 && PS <30 ) { 
        lcd.print("**");
        }
        if (PS >=30 && PS <40 ) { 
        lcd.print("***");
        } 
        if (PS >=40 && PS <50 ) { 
        lcd.print("****");
        } 
        if (PS >=50 && PS <60 ) { 
        lcd.print("*****");
        } 
        if (PS >=60 && PS <70 ) { 
        lcd.print("******");
        }
        if (PS >=70 && PS <80 ) { 
        lcd.print("*******");
        }
        if (PS >=80 && PS <90 ) { 
        lcd.print("********");
        }
        if (PS >=90 && PS <100 ) { 
        lcd.print("*********");
        }
        if (PS >=100) { 
        lcd.print("**********");
        }
      break;
case 10:
      lcd.setCursor(0,0); 
      lcd.print("Year Set"); 
      lcd.setCursor(0,1); 
      lcd.print(YEARSET);
      buttonState2 = digitalRead(buttonPin2);
      if (buttonState2 == HIGH) { 
      YEARSET = YEARSET - 1;
      delay(100);
      // lcd.clear();
      }
      buttonState3 = digitalRead(buttonPin3);
      if (buttonState3 == HIGH) { 
      YEARSET = YEARSET + 1;
      delay(100);
      // lcd.clear();
      } 
      break;       
case 11:
      lcd.setCursor(0,0); 
      lcd.print("Mount Set"); 
      lcd.setCursor(0,1); 
      lcd.print(MONTHSET);
      buttonState2 = digitalRead(buttonPin2);
      if (buttonState2 == HIGH) { 
      MONTHSET = MONTHSET - 1;
      delay(100);
      // lcd.clear();
      }
      buttonState3 = digitalRead(buttonPin3);
      if (buttonState3 == HIGH) { 
      MONTHSET = MONTHSET + 1;
      delay(100);
      // lcd.clear();
      } 
      break;       
case 12:
      lcd.setCursor(0,0); 
      lcd.print("DAYSET Set"); 
      lcd.setCursor(0,1); 
      lcd.print(DAYSET);
      buttonState2 = digitalRead(buttonPin2);
      if (buttonState2 == HIGH) { 
      DAYSET = DAYSET - 1;
      delay(100);
      // lcd.clear();
      }
      buttonState3 = digitalRead(buttonPin3);
      if (buttonState3 == HIGH) { 
      DAYSET = DAYSET + 1;
      delay(100);
      // lcd.clear();
      } 
      break;             
case 13:
      lcd.setCursor(0,0); 
      lcd.print("HOURSET Set"); 
      lcd.setCursor(0,1); 
      lcd.print(HOURSET);
      buttonState2 = digitalRead(buttonPin2);
      if (buttonState2 == HIGH) { 
      HOURSET = HOURSET - 1;
      delay(100);
      // lcd.clear();
      }
      buttonState3 = digitalRead(buttonPin3);
      if (buttonState3 == HIGH) { 
      HOURSET = HOURSET + 1;
      delay(100);
      // lcd.clear();
      } 
      break;   
case 14:
      lcd.setCursor(0,0); 
      lcd.print("MINUTESET Set"); 
      lcd.setCursor(0,1); 
      lcd.print(MINUTESET);
      buttonState2 = digitalRead(buttonPin2);
      if (buttonState2 == HIGH) { 
      MINUTESET = MINUTESET - 1;
      delay(100);
      // lcd.clear();
      }
      buttonState3 = digitalRead(buttonPin3);
      if (buttonState3 == HIGH) { 
      MINUTESET = MINUTESET + 1;
      delay(100);
      // lcd.clear();
      } 
      break;               
case 15:
      lcd.setCursor(0,0); 
      lcd.print(DAYSET); 
      lcd.print("."); 
      lcd.print(MONTHSET);
      lcd.print(".");               
      lcd.print(YEARSET); 
                        
      lcd.setCursor(10,0); 
      lcd.print(HOURSET); 
      lcd.print(":"); 
      lcd.print(MINUTESET); 
              
      if (buttonState == HIGH) { 
      lcd.clear();
      delay (1000) ;        
      RTC.adjust(DateTime(YEARSET, MONTHSET, DAYSET, HOURSET , MINUTESET , 01)); 
      lcd.print("OK Date is SET"); 
      delay (2000);
      pos = 16;
      }
 case 16:
      lcd.setCursor(1,0); 
      lcd.print("SAVE TO MEMORY");
      buttonState3 = digitalRead(buttonPin3);
      if (buttonState3 == HIGH) {
      lcd.clear();  
      lcd.setCursor(4,0);
      lcd.print("SAVE GO");
      delay(500);
      for (int thisChar = 0; thisChar < 16; thisChar++) {
      lcd.setCursor(thisChar, 1);
      lcd.print("*");
      delay(50);
      }
      EEPROM.write(0, RH);
      EEPROM.write(1, RM);
      EEPROM.write(2, RD);
      EEPROM.write(3, ZH);
      EEPROM.write(4, ZM);
      EEPROM.write(5, ZD);
      RTC.adjust(DateTime(YEARSET, MONTHSET, DAYSET, HOURSET , MINUTESET , 01)); 
      lcd.print("OK Date is SET"); 
      lcd.clear(); 
      pos = 1; 
      }
      break;     
 //delay (100);

      }

     
}




