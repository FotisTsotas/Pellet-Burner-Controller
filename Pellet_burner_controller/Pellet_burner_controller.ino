#include <EEPROM.h>
#include <max6675.h>
#include <DallasTemperature.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_SI2C.h>
#include <Servo.h>
#include <Wire.h>
byte thermometro[] = {
 B00100,
  B01010,
  B01010,
  B01010,
  B01110,
  B11111,
  B11111,
  B01110
};
byte fwtia[] = {
    B00100,
  B00100,
  B01010,
  B01010,
  B10001,
  B10001,
  B01110,
  B00000
};
byte watemp[] = {
  B00100,
  B00111,
  B00100,
  B00111,
  B00100,
  B01110,
  B11111,
  B01110
};
#define ONE_WIRE_BUS 5
#define sel digitalRead(3) == 0
#define up digitalRead(4) == 0
#define down digitalRead(11)==0 
const int pResistor = A0; // Photoresistor at Arduino analog pin A0
const int resist=2;//antistasi pin 2
const int aeras=1; // aeras pin 1
const int koxlias=7;//koxlias pin7
const int enausi=6;
const int voltage=A2;
const int buzz=12;
bool MAX=true;
bool manual;
bool eksod=true;
bool tick = false;
bool cut=true;
bool pell=false;
bool clen  = false;
bool yes=false;//katastasi thermostati
bool diat=false;
bool noignit=true;
char state=0;//menu chars
char state3=100;
char state2=100;
int counter;
int count=0;
int anamonh1;
int arips;
int anamonh2;
int anamonh3;
float ripsi3;
float ripsi2;
float ripsi1;
float value;//times fotias
float dip2=180000;//xronoi ripseis  
float dip1=120000;//xronoi katharismou
float vOUT = 0.0;
float vIN = 0.0;
float R1 = 30000.0;
float R2 = 7500.0;
int ktcSO = 8; //kauaseria 8-9 -10
int ktcCS = 9;
int ktcCLK = 10;
int time=0;//xronos gia enausi
int summ=0;//xronos gia katharismo
int pos = 0;
int volt = 0;
int Delay= 1000;
unsigned long previousMillis = 0; 
unsigned long currentMillis = 0;
MAX6675 ktc(ktcCLK, ktcCS, ktcSO);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Servo servo;

void setup(){//begin
  pinMode(resist, OUTPUT);//antistasi 
  pinMode(aeras, OUTPUT);  //aeras    
  pinMode(enausi, OUTPUT);//enausi 
  pinMode(koxlias, OUTPUT);  //aeras 
  pinMode(pResistor, INPUT);//foto antistasi
  pinMode(up,INPUT);
  pinMode(sel,INPUT);
  pinMode(down,INPUT);
  pinMode(voltage, INPUT_PULLUP);
  pinMode(buzz, OUTPUT);//buzzer 
  currentMillis = millis();
  sensors.begin();
  digitalWrite(aeras, HIGH);  
  digitalWrite(resist, HIGH);  
  digitalWrite(koxlias, HIGH);
  digitalWrite(enausi, HIGH);  
  lcd.begin(20, 4);
  lcd.setCursor(0,0);
  lcd.print("PELLET BURNER ");
  lcd.setCursor(0,1);
  lcd.print("******************");
  delay(1000);
  thermo();
  lcd.createChar(0, thermometro);
  lcd.createChar(1,fwtia);   
  lcd.createChar(2,watemp);   
    while(yes == false) {//arxiki katastasi kaustira
      standby();
      tick =false;
    }      
  value = analogRead(pResistor);
    if (value<800 || ktc.readCelsius()>=35){
      start();
    }else{
      pellet();
      tick=false;  
      pell=true;
    }
}

void loop(){
  sensors.requestTemperatures();
  thermo();
   while((yes)== true) {//katastasi kaustira
    start(); 
   }
  tick = false;
    while((yes)== false) {
      if (noignit==true ){
        standby();
      }else{
       standby2();
      } 
      if ((yes)== true){
        tick = true;
      }
      if (tick == true){
        if (diat==true){
          start();
        }
        if (diat==false){
          do{
            kaysa();
          }while(ktc.readCelsius()>30);
          summ=0;
            if (noignit==true || tick==false){
            standby();
            }
            if (pell==false){
            pellet();
            tick=false; 
            pell=true;
            }  
        } 
      }
    }
}

void clean(){  //sunartisi katharismou
  thermo(); 
  clen=true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("KATHARISMOS ");
  lcd.setCursor(0, 1);
  digitalWrite(aeras,LOW);
  lcd.print("KAYSAERIA ");
  lcd.print(ktc.readCelsius());      
  delay(dip1) ;   
}

void pellet(){//sunartisi  gia pellet
  thermo();   
  mainmenu();
    if (!eksod){
      menu();//MENU-----------
      count=0;
      delay(100);
    }else{
    int kappa=EEPROM.read(6);
      while (kappa>0) {
        noautoantistasi();     
        kappa--;
        lcd.clear();
        digitalWrite(aeras, HIGH);  // aeras kleistos
        digitalWrite(resist, LOW);  // antistasi anoixti
        digitalWrite(koxlias, HIGH);// koxlias ANOIXTOS
        digitalWrite(enausi, LOW);  // enausi  
        lcd.setCursor(0, 0);
        lcd.print("EISAGWGI PELLET");
        lcd.setCursor(0, 1);
        lcd.print(kappa);
        lcd.print(" sec");   
        delay(1000);
      }
    }
}
 
void start(){
  sensors.requestTemperatures();
  sensors.getTempCByIndex(0);
  mainmenu();
    if (!eksod){
      menu();//MENU-----------
      count=0;
      delay(100);
    }else{
      automanual();    
      mainmenu();  
      value = analogRead(pResistor);//TIMI FOTOANTISTASHS             
      thermo(); 
        if ((value >=500) && ktc.readCelsius()<=25  ){// SUNTHIKI ANAFLEKSIS
          noautoantistasi();       
          time=time+5;
          noignit=true;
          digitalWrite(aeras,HIGH );  // aeras kleistos
          digitalWrite(resist, LOW);  // antistasi anoixti
          digitalWrite(koxlias, HIGH);// koxlias kleistos
          digitalWrite(enausi, HIGH);  // enausi
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("ANAFLEKSI ");
          lcd.print(time);
          lcd.print(" sec");
          lcd.setCursor(0,1);
          lcd.print("FOTIA ");
          lcd.print(value);
          summ=0;
            if (time >=140 && time<=145){
              noautoantistasi();          
              digitalWrite(aeras, LOW);  
              lcd.clear();
              lcd.setCursor(0,1);
              lcd.print("AERAS ANOIXTOS 1");//ANOIGMA AERA
            }
            else if (time >=180 && time<=185){
              noautoantistasi();
              lcd.clear();
              digitalWrite(aeras, LOW);  
              lcd.setCursor(0,1);
              lcd.print("AERAS ANOIXTOS 2");//ANOIGMA AERA
            }
            else if (time >=210 && time<=460 ){
              noautoantistasi();          
              digitalWrite(aeras, LOW);  
              lcd.clear();
              lcd.setCursor(0,1);
              lcd.print("AERAS ANOIXTOS");//ANOIGMA AERA
            }
            else if (time>461) {//ERROR TIME
              summ=0;
              digitalWrite(aeras, LOW);  
              digitalWrite(resist, HIGH);  // antistasi anoixti
              lcd.clear();
              lcd.setCursor(4,1);
              lcd.print("PROVLIMA");//ERROR
              digitalWrite(buzz,HIGH);
              delay(500);
              digitalWrite(buzz,LOW);
              delay(500);
            }
          delay(5000);
        }
        else{ 
          if ( ktc.readCelsius()>=0 && ktc.readCelsius()<=30  ){ //anixneusi spithas fotias eisodos se xamili enausi
            noautoantistasi();    
              while (value > 950){
                digitalWrite(aeras, HIGH);
                digitalWrite(resist, HIGH);  // antistasi KLEISTI
                digitalWrite(koxlias, HIGH);// koxlias ANOIXTOS 
                digitalWrite(enausi, HIGH);  // enausi
                lcd.clear();
                lcd.setCursor(4,1);
                lcd.print("PROVLIMA");//ERROR
                digitalWrite(buzz,HIGH);
                delay(500);
                digitalWrite(buzz,LOW);
                delay(500);
              }                  
            lcd.clear();
            lcd.setCursor(1, 0);
            lcd.print("XAMILI ENAUSI");
            digitalWrite(aeras, LOW);  // aeras anoixtos
            digitalWrite(resist, LOW);  // antistasi anoixti
            digitalWrite(koxlias, HIGH);// koxlias kleistos
            digitalWrite(enausi, HIGH);  // enausi       
            summ=0;
            lcd.setCursor(0,1);
            lcd.print("KAYSAERIA ");
            lcd.print(ktc.readCelsius());
            lcd.print(" C");
            pell=false;
            noignit=true;
            delay(2000);
          }
          else if(ktc.readCelsius()>30 && ktc.readCelsius()<65 ) {//zestama me tin leitourgeia timer
            noautoantistasi();  
              while (value > 900){
                digitalWrite(aeras, HIGH);
                digitalWrite(resist, HIGH);  // antistasi KLEISTI
                digitalWrite(koxlias, HIGH);// koxlias ANOIXTOS 
                digitalWrite(enausi, HIGH);  // enausi
                lcd.clear();
                lcd.setCursor(4,1);
                lcd.print("PROVLIMA");//ERROR
                digitalWrite(buzz,HIGH);
                delay(500);
                digitalWrite(buzz,LOW);
                delay(500);
              }            
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("ZESTAMA ");
            time=0;
            digitalWrite(resist, HIGH); 
            pos=130;
            summ=0;
            servo.write(pos);
            digitalWrite(aeras, LOW);
            digitalWrite(enausi, HIGH);  // enausi
            lcd.setCursor(0,1);
            lcd.setCursor(0,1);
            lcd.print("KAYSAERIA ");
            lcd.print(ktc.readCelsius());
            lcd.print(" C");
            pell=false;
            noignit=false;
            timer();//sunartisi timer
            delay(500);
          }
          else if (ktc.readCelsius()>=65){
            thermo(); 
            sensors.requestTemperatures();
            sensors.getTempCByIndex(0);
              while (value > 900){
                digitalWrite(aeras, HIGH);
                digitalWrite(resist, HIGH);  // antistasi KLEISTI
                digitalWrite(koxlias, HIGH);// koxlias ANOIXTOS 
                digitalWrite(enausi, HIGH);  // enausi
                lcd.clear();
                lcd.setCursor(4,1);
                lcd.print("PROVLIMA");//ERROR
                digitalWrite(buzz,HIGH);
                delay(500);
                digitalWrite(buzz,LOW);
                delay(500);            
              }      
              if (!manual){
                if( sensors.getTempCByIndex(0)<= 50 && MAX==true){
                  timer1();
                }
                else if(sensors.getTempCByIndex(0) >= 45 && sensors.getTempCByIndex(0) <50){
                  timer2(); 
                  if(sensors.getTempCByIndex(0) < 45){
                    MAX=true;
                  }
                }
                else {
                  timer3();
                  MAX=false; 
                    if(sensors.getTempCByIndex(0) < 45){
                     MAX=true;
                    }
                }
              }
              else if(manual){
                xeriokinito();
              }
         }
       }
    }
}
 void lcdPrint(){
  sensors.requestTemperatures();
  sensors.getTempCByIndex(0);
  value = analogRead(pResistor);
    if(currentMillis - previousMillis >= Delay){  
      previousMillis += Delay;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("KANONIKH LEIT.");
      lcd.setCursor(14,0);
      lcd.print (anamonh1);
      lcd.setCursor(0,1 );
      lcd.write(byte(1));
      lcd.setCursor(2,1 );
      lcd.print(value);
      delay(500); 
      time=0;
      summ=0;
      lcd.setCursor(0,1 );
      lcd.write(byte(0));
      lcd.setCursor(2,1 );
      lcd.print(ktc.readCelsius());
      delay(1000);
      lcd.print(" C");
      lcd.setCursor(0,1 );
      lcd.write(byte(2));
      lcd.setCursor(2,1 );
      lcd.print(sensors.getTempCByIndex(0));
      delay(500);
      pell=false;
      noignit=false;
      clen=false;      
    }
}
    
void timer1(){ // timer1 !!!!!!!!!!!!!!!!!!!!!!!!
  anamonh1=EEPROM.read(0);  
  ripsi1=EEPROM.read(1);
  value = analogRead(pResistor);
  sensors.requestTemperatures();
  sensors.getTempCByIndex(0);
  thermo(); 
  automanual();
  digitalWrite(enausi,LOW);
    while (ripsi1>0) {
      digitalWrite(enausi,LOW);
      lcd.setCursor(8,1);
      lcd.print("            ");
      lcd.setCursor(0,1);
      lcd.print("HIGH ");
      lcd.setCursor(9,0);
      lcd.print (ripsi1);
      lcd.print (" sec ");
      ripsi1 = ripsi1-0.5;
      noautoantistasi();      
      delay(500);
    }
  digitalWrite(enausi,HIGH);
    while (anamonh1 >0){   
      currentMillis = millis();  
      automanual();    
      mainmenu();  
      lcdPrint();
      digitalWrite(enausi,HIGH);
      anamonh1=anamonh1-2;
      digitalWrite(aeras, LOW);  
      digitalWrite(resist, HIGH); 
      digitalWrite(koxlias, HIGH);
      noautoantistasi();      
    }
}


void timer2(){ // timer 2 !!!!!!!!!!!!!!!!!!!!!!!!!
  anamonh1=EEPROM.read(2);  
  ripsi1=EEPROM.read(3);
  value = analogRead(pResistor);
  sensors.requestTemperatures();
  sensors.getTempCByIndex(0);
  thermo(); 
  automanual();
  digitalWrite(enausi,LOW);
    while (ripsi1>0) {
      digitalWrite(enausi,LOW);
      lcd.setCursor(8,1);
      lcd.print("            ");
      lcd.setCursor(0,1);
      lcd.print("MEDIUM ");
      lcd.setCursor(9,0);
      lcd.print (ripsi1);
      lcd.print (" sec ");
      ripsi1 = ripsi1-0.5;
      noautoantistasi();      
      delay(500);
    }
  digitalWrite(enausi,HIGH);
    while (anamonh1 >0){   
      currentMillis = millis();  
      automanual();    
      mainmenu();  
      lcdPrint();
      digitalWrite(enausi,HIGH);
      anamonh1=anamonh1-2;
      digitalWrite(aeras, LOW);  
      digitalWrite(resist, HIGH); 
      digitalWrite(koxlias, HIGH);
      noautoantistasi();      
    }
} 


void timer3(){ // timer 3 !!!!!!!!!!!!!!!!!!!!!!!!! 
  anamonh1=EEPROM.read(4);  
  ripsi1=EEPROM.read(5);         
  sensors.requestTemperatures();
  sensors.getTempCByIndex(0);
  value = analogRead(pResistor);
  thermo(); 
  automanual();
  digitalWrite(enausi,LOW);
    while (ripsi1>0) {
      digitalWrite(enausi,LOW);
      lcd.setCursor(8,1);
      lcd.print("            ");
      lcd.setCursor(0,1);
      lcd.print("LOW ");
      lcd.setCursor(9,0);
      lcd.print (ripsi1);
      lcd.print (" sec ");
      ripsi1 = ripsi1-0.5;
      noautoantistasi();      
      delay(500);
    }
  digitalWrite(enausi,HIGH);
    while (anamonh1 >0){   
      currentMillis = millis();  
      automanual();    
      mainmenu();  
      lcdPrint();
      digitalWrite(enausi,HIGH);
      anamonh1=anamonh1-2;
      digitalWrite(aeras, LOW);  
      digitalWrite(resist, HIGH); 
      digitalWrite(koxlias, HIGH);
      noautoantistasi();      
    }
}
  
void standby(){//sunartisi anamonis prin ginei anafleksi,mono aeras anoixtos
  thermo(); 
  sensors.requestTemperatures();
  sensors.getTempCByIndex(0);
  noautoantistasi();      
  summ++;
  digitalWrite(aeras, LOW);
  digitalWrite(resist, HIGH);
  digitalWrite(koxlias, HIGH);
  digitalWrite(enausi, HIGH); 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print ("KLEISIMO ");
  lcd.setCursor(8,0);
  lcd.print (summ);
  lcd.print (" sec");
  time=0;
  lcd.setCursor(0,1);
  lcd.print("KAYSAERIA ");
  lcd.print(ktc.readCelsius());
  lcd.print(" C");
    while (summ>=240 && yes==false || yes==false && ktc.readCelsius()<=0 ){//tha allaksei argotera
      thermo();
      noautoantistasi();      
      digitalWrite(aeras, HIGH);  
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("KLEISTO");
      lcd.setCursor(0,1);
      lcd.print("KAYSAERIA ");
      lcd.print(ktc.readCelsius());
      lcd.print(" C");
      delay(1000);
    }
  delay(1000); 
}
   
void standby2(){
  value = analogRead(pResistor);
  thermo();
  digitalWrite(resist, HIGH);
  digitalWrite(koxlias, HIGH);
  clean();
    if (clen==true){
      digitalWrite(buzz,HIGH);
      delay(500);
      digitalWrite(buzz,LOW);
      delay(500);
    } 
    while(clen==true && yes==false){
      thermo();
      digitalWrite(aeras, HIGH);  
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("KLEISTO");
      lcd.setCursor(0,1);
      lcd.print(ktc.readCelsius());
      diat=false;
      delay(1000);
    }
}

void timer(){//zestama
  int takk=30;//xronos anamonis
  int tokk=5;//xronos ripsis
    while (tokk>0){
    digitalWrite(koxlias,LOW);
    delay(1500);
    digitalWrite(koxlias,HIGH);
    delay(500);
    digitalWrite(koxlias,LOW);
    delay(1500);
    lcd.setCursor(8,0);
    lcd.print (tokk);
    lcd.print (" sec");
    tokk=0;
    noautoantistasi();      
    }
  tokk=5;
    while (takk>0) {
    automanual();
    digitalWrite(koxlias,HIGH);
    lcd.setCursor(8,0);
    lcd.print (takk);
    lcd.print (" sec");
    takk--;
    noautoantistasi();      
    delay(1000);
    }
  takk=28;
}

void kaysa(){//elegxos epanakinisis
  noautoantistasi();      
  thermo();     
  digitalWrite(aeras, HIGH);  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ANAMONI KAYSA");
  lcd.setCursor(0,1);
  lcd.print(ktc.readCelsius());
  delay(1000);
}

void thermo(){//sunartisi gia thn anixneusi tasis apo ton thermostati xwrou
  volt = analogRead(voltage);
  vOUT = (volt * 5.0) / 1024.0;
  vIN = vOUT / (R2/(R1+R2));
    if (vIN>15){//tasi sunthikis
      yes=true;
    }
    else {
      yes=false;
    }
}


void noautoantistasi(){
  while (!sel){
    digitalWrite(resist,LOW);
  }
  while (!down){
    digitalWrite(koxlias,LOW);
  }
  while (!up){
    digitalWrite(aeras,LOW);
  }
}
  
void automanual(){
  while(!up){ // automato h xeirokinito 
    if (manual){
      manual= !manual;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("AYTOMATO ");//mhden
      delay(1000);
    }
    else if (!manual){
      manual= !manual;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("XERIROKINITO ");//ena
      delay(1000);
    }
  }
}

void mainmenu(){
  while(!sel && eksod){
    lcd.clear();
    lcd.print(count);
    count++;
      if(count>1){
        eksod=!eksod;
        lcd.setCursor(0, 0);
        lcd.print("EISODOS STO MENU");
      }
    delay(1000);
  }
}

void xeriokinito(){ //xeriokinito
  thermo();  
  sensors.requestTemperatures();
  sensors.getTempCByIndex(0); 
  noautoantistasi();      
    while (value > 900){
      digitalWrite(aeras, HIGH);
      digitalWrite(resist, HIGH);  // antistasi KLEISTI
      digitalWrite(koxlias, HIGH);// koxlias ANOIXTOS 
      digitalWrite(enausi, HIGH);  // enausi
      lcd.clear();
      lcd.setCursor(4,1);
      lcd.print("PROVLIMA");//ERROR
      digitalWrite(buzz,HIGH);
      delay(500);
      digitalWrite(buzz,LOW);
      delay(500);
    }     
  digitalWrite(aeras, LOW);  // aeras anoixtos
  digitalWrite(resist, HIGH);  // antistasi KLEISTI
  digitalWrite(koxlias, LOW);// koxlias ANOIXTOS
  digitalWrite(enausi, HIGH);  // enausi     
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("KANONIKH LEIT.");
  lcd.setCursor(0,1 );
  lcd.write(byte(1));
  lcd.setCursor(2,1 );
  lcd.print(value);
  delay(2000);  
  time=0;
  summ=0;
  lcd.setCursor(0,1);
  lcd.write(byte(0));
  lcd.setCursor(2,1 );
  lcd.print(ktc.readCelsius());
  lcd.print(" C");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("KANONIKH LEIT.");
  lcd.setCursor(0,1 );
  lcd.write(byte(2));
  lcd.setCursor(2,1 );
  lcd.print(sensors.getTempCByIndex(0));
  delay(1000);  
  pell=false;
  noignit=false;
  clen=false;
  delay(2000);
}
    
void updown(){
  if(!up){
    lcd.clear();
    state--;
    state2--;
    state= constrain(state,0,5);
  }
  if(!down){
    lcd.clear();
    state++;
    state2++;
    state= constrain(state,0,5); 
  }
}

void menu()//main menu
{
  switch(state){
    case 0://timer1
      digitalWrite(enausi,HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("TIMER 1");
      lcd.setCursor(13,0);
      lcd.print("<-");
      lcd.setCursor(0, 1);
      lcd.print("TIMER 2");
      updown();
      counter=0;
       while(!sel){
          count++;
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print(count);
           if(count<2){
            state2=0;
            state=100;
            delay(100);
           }  
           else if(count>2){//eksodos apo to menu
            eksod=!eksod;
            state=0;
            state2=100;
            state3=100;
            lcd.setCursor(0, 0);
            lcd.print("EKSODOS APO MENU");
           }
          delay(1000);
        }
    break;
    case 1://timer2
      lcd.setCursor(0, 0);
      lcd.print("TIMER 1");
      lcd.setCursor(13,1);
      lcd.print("<-");
      lcd.setCursor(0, 1);
      lcd.print("TIMER 2");
      updown();
      counter=0;
        while(!sel){
          count++;
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print(count);
            if(count<2){
              state2=2;
              state=100;
              state3=100;
              delay(100);
            }
            else if(count>2){//eksodos apo to menu
              eksod=!eksod;
              state=0;
              state2=100;
              state3=100;
              lcd.setCursor(0, 0);
              lcd.print("EKSODOS APO MENU");
            }
          delay(1000);
        }
    break;
    case 2://timer3
      lcd.setCursor(0, 0);
      lcd.print("TIMER 3");
      lcd.setCursor(13,0);
      lcd.print("<-");
      lcd.setCursor(0, 1);
      lcd.print("TIMER RIPSIS");
      updown();
      counter=0;
        while(!sel){
        count++;
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(count);
          if(count<2){
            state2=4;
            state=100;
            state3=100;
            delay(100);
          }
          else if(count>2){//eksodos apo to menu
            eksod=!eksod;
            state=0;
            state2=100;
            state3=100;
            lcd.setCursor(0, 0);
            lcd.print("EKSODOS APO MENU");
          }
        delay(1000);
      }
    break;
    case 3://timmer ripis
      lcd.setCursor(0, 0);
      lcd.print("TIMER 3");
      lcd.setCursor(13,1);
      lcd.print("<-");
      lcd.setCursor(0, 1);
      lcd.print("TIMER RIPSIS");
      updown();
      counter=0;
      arips=EEPROM.read(6);  
        while(!sel){
          count++;
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print(count);
           if(count<2){
            state2=100;
            state=100;
            state3=6;
            delay(100);
          }
          else if(count>2){//eksodos apo to menu
            eksod=!eksod;
            state=0;
            state2=100;
            state3=100;
            lcd.setCursor(0, 0);
            lcd.print("EKSODOS APO MENU");
          }
         delay(1000);
        }
    break;
    case 4:
      state=0;
    break;
}

///////////////////////////////////////////////////////////////////////
switch(state2){
  case 0://timer2
    state=100;
    state3=100;
    lcd.setCursor(0, 0);
    lcd.print("XRON.AN.1");
    lcd.setCursor(13,0);
    lcd.print("<-");
    lcd.setCursor(0, 1);
    lcd.print("XRON.RIP.1");
    updown();
    counter=0;
    anamonh1=EEPROM.read(0);  
      while(!sel){
        count++;
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(count);
          if(count<=1){
            state3=0;
            state=100;
            state2=100;
            delay(100);
            }
            else if(count>1){
              state=0;
              state2=100;
              state3=100;
            }
        delay(1000);
      }
  break;
  case 1://timer1
    state=100;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("XRON.AN.1");
    lcd.setCursor(13,1);
    lcd.print("<-");
    lcd.setCursor(0, 1);
    lcd.print("XRON.RIP.1");
    updown();
    counter=0;
    ripsi1=EEPROM.read(1);
    while(!sel){
      count++;
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print(count);
        if(count<=1){
          state3=1;
          state=100;
          state2=100;
          delay(100);
        }
        else if(count>1){
          state=0;
          state2=100;
          state3=100;
          }
       delay(1000);
  }
  break;
  case 2://timer2
    state=100;
    state3=100;
    lcd.setCursor(0, 0);
    lcd.print("XRON.AN.2");
    lcd.setCursor(13,0);
    lcd.print("<-");
    lcd.setCursor(0, 1);
    lcd.print("XRON.RIP.2");
    updown();
    counter=0;
    anamonh2=EEPROM.read(2); 
      while(!sel){
      count++;
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print(count);
        if(count<=1){
          state3=2;
          state=100;
          state2=100;
          delay(100);
          }
          else if(count>1){
            state=0;
            state2=100;
            state3=100;
          }
        delay(1000);
      }
  break;
    case 3://timer2
    state=100;
    state3=100;
    lcd.setCursor(0, 0);
    lcd.print("XRON.AN.2");
    lcd.setCursor(13,1);
    lcd.print("<-");
    lcd.setCursor(0, 1);
    lcd.print("XRON.RIP.2");
    updown();
    counter=0;
    ripsi2=EEPROM.read(3);
      while(!sel){
        count++;
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(count);
          if(count<=1){
            state3=3;
            state=100;
            state2=100;
            delay(100);
          } 
          else if(count>1){
            state=0;
            state2=100;
            state3=100;
            }
          delay(1000);
        }
  break;
  case 4://timer3
    state=100;
    state3=100;
    lcd.setCursor(0, 0);
    lcd.print("XRON.AN.3");
    lcd.setCursor(13,0);
    lcd.print("<-");
    lcd.setCursor(0, 1);
    lcd.print("XRON.RIP.3");
    updown();
    counter=0;
    anamonh3=EEPROM.read(4); 
      while(!sel){
        count++;
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(count);
          if(count<=1){
            state3=4;
            state=100;
            state2=100;
            delay(100);
          }
          else if(count>1){
          state=0;
          state2=100;
          state3=100;
          }
        delay(1000);
      }
  break;
  case 5://timer3
    state=100;
    state3=100;
    lcd.setCursor(0, 0);
    lcd.print("XRON.AN.3");
    lcd.setCursor(13,1);
    lcd.print("<-");
    lcd.setCursor(0, 1);
    lcd.print("XRON.RIP.3");
    updown();
    counter=0;
    ripsi3=EEPROM.read(5);
      while(!sel){
        count++;
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(count);
          if(count<=1){
            state3=5;
            state=100;
            state2=100;
            delay(100);
          }
          else if(count>1){
            state=0;
            state2=100;
            state3=100;
          }
        delay(1000);
      }
  break;
}
/////////////////////////////////////////////////////////////////////////////
switch(state3){//stete3
  case 0://timer1
    lcd.clear();
    lcd.print("XRONOS ANAM1 = ");
    lcd.setCursor(0,1);
    lcd.print( anamonh1); 
      if(!up){
        anamonh1++;
        anamonh1 =constrain(anamonh1, 0, 25);
        delay(100);
      }
      else if(!down){
        anamonh1--;
        anamonh1 =constrain(anamonh1, 0,25);
        delay(100);
      }
      else if(!sel){
        lcd.clear();
        state=100;
        state2=0;
        state3=100;
        EEPROM.write(0,anamonh1);  
      }
    delay(100);
  break;
  case 1://timer1
    lcd.clear();
    lcd.print("XRONOS RIPSIS1 = ");
    lcd.setCursor(0,1);
    lcd.print( ripsi1); 
      if(!up){
        ripsi1 = ripsi1+0.5;
        ripsi1 =constrain(ripsi1, 0, 4);;
        delay(100);
        }
      else if(!down){  
        ripsi1 = ripsi1-0.5;
        ripsi1 =constrain(ripsi1, 0,4);
        delay(100);
      }
      else if(!sel){
      lcd.clear();
      state=100;
      state2=1;
      state3=100;
      EEPROM.write(1,ripsi1);
      }
    delay(100);
  break;
  case 2://timer2
    lcd.clear();
    lcd.print("XRONOS ANAM2 = ");
    lcd.setCursor(0,1);
    lcd.print( anamonh2); 
      if(!up){
        anamonh2++;
        anamonh2 =constrain(anamonh2, 0, 25);
        delay(100);
      }
      else if(!down){
        anamonh2--;
        anamonh2 =constrain(anamonh2, 0,25);
        delay(100);
      }
      else if(!sel){
        lcd.clear();
        state=100;
        state2=2;
        state3=100;
        EEPROM.write(2,anamonh2);
      }
    delay(100);
  break;
  case 3://timer2
    lcd.clear();
    lcd.print("XRONOS RIPSIS2 = ");
    lcd.setCursor(0,1);
    lcd.print( ripsi2); 
      if(!up){
        ripsi2 = ripsi2+0.5;
        ripsi2 =constrain(ripsi2, 0,4);
        delay(100);
      }
      else if(!down){ 
        ripsi2 = ripsi2-0.5;
        ripsi2 =constrain(ripsi2, 0,4);
        delay(100);
      }
      else if(!sel){
        lcd.clear();
        state=100;
        state2=3;
        state3=100;
        EEPROM.write(3,ripsi2); 
      }
  delay(100);
  break;
  case 4://timer3
    lcd.clear();
    lcd.print("XRONOS ANAM3 = ");
    lcd.setCursor(0,1);
    lcd.print( anamonh3); 
      if(!up){
        anamonh3++;
        anamonh3 =constrain(anamonh3, 0, 25);
        delay(100);
      }
      else if(!down){
        anamonh3--;
        anamonh3 =constrain(anamonh3, 0,25);
        delay(100);
      }
      else if(!sel){
        lcd.clear();
        state=100;
        state2=4;
        state3=100;
        EEPROM.write(4,anamonh3);   
      }
     delay(100);
  break;
  case 5://timer3
    lcd.clear();
    lcd.print("XRONOS RIPSIS3 = ");
    lcd.setCursor(0,1);
    lcd.print( ripsi3); 
      if(!up){
        ripsi3 = ripsi3+0.5;
        ripsi3 =constrain(ripsi3, 0, 4);
        delay(100);
      }
      else if(!down){
        ripsi3 = ripsi3-0.5;
        ripsi3 =constrain(ripsi3, 0, 4);
        delay(100);
      }
      else if(!sel){
        lcd.clear();
        state=100;
        state2=5;
        state3=100;
        EEPROM.write(5,ripsi3);
      }
    delay(100);
  break;
  case 6://timer3
  lcd.clear();
  lcd.print("XRONOS AR. RIPSIS = ");
  lcd.setCursor(0,1);
  lcd.print( arips); 
    if(!up){
      arips++;
      arips =constrain(arips, 0, 40);
      delay(100);
    }
    else if(!down){ 
      arips--;
      arips =constrain(arips, 0,40);
      delay(100);
    }
    else if(!sel){
      lcd.clear();
      state=3;
      state2=100;
      state3=100;
      EEPROM.write(6,arips);
    }
   delay(100);
  break;
 }  
}


  

  
