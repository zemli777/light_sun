#include <Adafruit_NeoPixel.h>
#include <iarduino_RTC.h>
iarduino_RTC time(RTC_DS1307);  //разобраться с интерфейсами и включить в рпз код библиотеки 
#include <LiquidCrystal.h>
//#include <inttypes.h>
#include <math.h>
#include "params.h"

// #define but_up_pin A0;
// #define but_down_pin A1;
// #define but_left_pin A2;
// #define but_right_pin A3;

const int lamp[24][3]= {
      {3, 255, 128}, 
      {7, 255, 0}, 
      {15, 254, 0}, 
      {31, 252, 0}, 
      {63,248, 0}, 
      {127,240,0},
      {255,224,0},
      {255,192,1},
      {255,128,3},
      {255,0,7},
      {254,0,15},
      {252,0,31},
      {248,0,63},
      {240,0,127},
      {224,0,255},
      {192,1,255},
      {128,3,255},
      {0,7,255},
      {0,15,254},
      {0,31,252},
      {0,63,248},
      {0,127,240},
      {0,255,224},
      {1,255,192}
};

const int NeoPin = 9;
// количество светодиодов 
const int NeoCount = 24;
//Пин подключен к ST_CP входу 74HC595
const int latchPin = 2;
//Пин подключен к SH_CP входу 74HC595
const int clockPin = 12;
//Пин подключен к DS входу 74HC595
const int dataPin = 10; 
constexpr uint8_t PIN_RS = 4;
constexpr uint8_t PIN_EN = 3;
constexpr uint8_t PIN_DB4 = 8;
constexpr uint8_t PIN_DB5 = 7;
constexpr uint8_t PIN_DB6 = 6;
constexpr uint8_t PIN_DB7 = 5;

 int r,g,b;
 int c = -1;
 int brn;
 int data=0;
 bool bup;
 bool bdown;
 bool bleft;
 bool bright;
bool flag_up, flag_right, flag_left, flag_down = false;
LiquidCrystal lcd(PIN_RS, PIN_EN, PIN_DB4, PIN_DB5, PIN_DB6, PIN_DB7);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NeoCount, NeoPin, NEO_GRB + NEO_KHZ800); //first number change does distance between colors
 
 


struct MENUS { // основная структура, в которой сохраняются текущие настройки
     int brn;
     //int mode;
     String components = "ARD.Uno,DS1307,LM044L,WS2812_8,74HC595";
     int menu_count=1000;
     bool speed_bool= false;
     bool mode_sun_bool = true;
     bool mode_lamp_bool = true;
     bool info_bool = false;

      MENUS(){
         brn = 10;
     };

     void button(int type){
       switch (type){
         case 0: //up
         if ((menu_count % 1000) == 0 ){ 
                    } 
                else {
                    if((menu_count % 100) == 0 ) {
                        if ((menu_count / 100) > 11){
                            menu_count = menu_count - 100;
                        } else { menu_count= menu_count + 300;}
                    }  else if ((menu_count % 100) > 10) {
                                        menu_count = menu_count - 10;
                                    } else {  
                                        menu_count= menu_count + 10;}
              };
              if ((menu_count % 1000) == 410) {
                if (brn < 9){brn = brn + 1;} else {brn = 1;}}
          break;
         case 1: //down
         if ((menu_count % 1000) == 0 ){ 
                  } 
              else {
                  if((menu_count % 100) == 0 ) {
                      if ((menu_count / 100) < 14){
                          menu_count = menu_count + 100;
                      } else { menu_count= menu_count - 300;}
                  }  else if ((menu_count % 100) < 20) {
                                      menu_count = menu_count + 10;
                                  } else { menu_count= menu_count - 10;}
            };
            if ((menu_count % 1000) == 410) {
                if (brn > 1){brn = brn - 1;} else {brn = 10;}}
              
             break;
         case 2: //left
          if ((menu_count % 1000)==0){ } else {if ((menu_count % 100) == 0){//проверить на 1000
            menu_count= menu_count/1000;
            menu_count = menu_count*1000;
         } else if ((menu_count % 10) == 0) {
              menu_count= menu_count/100;
              menu_count = menu_count*100;  
         };}
         break;
         case 3: //right
          if((menu_count % 1000) ==0){
              menu_count = menu_count + 100;
         } else if ((menu_count % 100) == 0){
            menu_count= menu_count+10;
         } else if((menu_count % 1000) == 110){ 
            speed_bool = false;
            //режим реального времени
            //выбор действие (изменить что-то) из меню 
         } else if((menu_count % 1000) == 120){
            speed_bool = true;
            //режим ускоренный х100
         } else if((menu_count % 1000) == 210){
            mode_lamp_bool = ! mode_lamp_bool;
            //режим без ламп
         } else if((menu_count % 1000) == 220){
            mode_sun_bool = ! mode_sun_bool;
            //режим без солнца
         } else if((menu_count % 1000)== 410){
            //яркость 100%
            brn = 10;
         } else if((menu_count % 1000)== 420){
           //яркость 50%
            brn = 7;
         }
        
         break;
       }
     };

};
     
MENUS m1;

void setup() {

   pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pixels.begin();
  pixels.show(); // Устанавливаем все светодиоды в состояние "Выключено"

  lcd.createChar(SYMBOL_UP_ROOT,charUpRoot);
  lcd.createChar(SYMBOL_UP_SUB,charUpSub);
  lcd.createChar(SYMBOL_DOWN_ROOT,charDownRoot);
  lcd.createChar(SYMBOL_DOWN_SUB,charDownSub);
  lcd.createChar(SYMBOL_BOTH_ROOT,charBothRoot);
  lcd.createChar(SYMBOL_BOTH_SUB,charBothSub);
  lcd.createChar(SYMBOL_NOT_ACTIVE,charNotActive);
    
   // переделать меню
// int state_level = 0;
// int point_[4] = [0,0,0,0];

// void menu_level_1(){
//   if()
//     point_[state_level]++
//   if()
//     state_level++


  lcd.begin(20,4); // посмотреть размер экрана 4 строк??
  lcd.setCursor(1,1);
  lcd.print("Menu");
 
 
  }


//}
void menu_type(MENUS m1,int k){
  switch (k){
    case 1000:
      lcd.clear();
      lcd.print("Menu");
    //lcd.print(1000);
      break;
    case 1100:
      lcd.clear();
      lcd.print("Menu -> Speed");
      lcd.print("               Mode");
      //lcd.print(1100);
      break;
    case 1200: 
      
      lcd.clear();
      //lcd.print("Menu    Speed");
      lcd.print("Menu -> Mode");
      lcd.print("                Info");
      //lcd.print("Mode");
      //lcd.print(1200);
      break;
     case 1300: 
      lcd.clear();
      //lcd.print("Menu    Speed");
      lcd.print("Menu    Mode");
      lcd.print("             -> Info");
      break;

      case 1400:
      lcd.clear();
      lcd.print("Menu    Info");
      lcd.print("             -> Brightness");
      break;

      case 1110:
  
      lcd.clear();
      lcd.print("Speed -> RTC");
      lcd.print("                 X100");
      
      break;
    case 1120: 
      lcd.clear();
      lcd.print("Speed    RTC");
      lcd.print("              -> X100");
      
      break;
    case 1210: 
      lcd.clear();
      lcd.print("Mode -> Lamp");
      lcd.print("                Sun");
      
      break;
    case 1220: 
      
      lcd.clear();
      lcd.print("Mode    Lamp");
      lcd.print("             -> Sun");
      break;
    case 1310:
      lcd.clear();
      lcd.print(m1.components);
      //lcd.print();
     break;
    case 1410:
      int buf_down =m1.brn -1;
      if (buf_down <2) {buf_down = 10;};
      lcd.clear();
      lcd.print("Brightness -> ");
      lcd.print("10");
      //lcd.print(m1.brn * 10);
      lcd.print("                  ");
      lcd.print(buf_down * 10); 
      break;
    
  }

}

float times( int h, int m, int c){

  //выразить в секундах, чтобы потом умножать, ускорять
    float t=0;
    float m1 = m;
    float  c1 = c;
    t = (h + m1/60 + c1 /3600) ;
  
    return t;
}
void loop() {

    time.gettime();
     int h = time.Hours;
     int m = time.minutes ;
     int s = time.seconds ;
     if (m1.speed_bool)
     { h = s % 24;// ускоренный режим теста 
       s = (m +30) % 60;
      }
    float time = times(h,m,s);
    for( int i =0 ; i< NeoCount; i++){
      float temp;
      float l= times((h+i)%24, m, s);
      temp = collor(l);
      r = Red(temp)* brightness(l) * brightness(m1.brn) ;
      g = Green(temp) * brightness(l) * brightness(m1.brn);
      b = Blue(temp) * brightness(l) * brightness(m1.brn) ;
  
      pixels.setPixelColor(i, pixels.Color(r,g,b)); // выяснить как устроено внутри и по какому интерфейсу работает диодная лента
    }
  pixels.show();
  int nesting =0;
  // rainbow(30);
  bup = digitalRead(A0);//but_up_pin);
  bdown = digitalRead(A1);//but_down_pin);
  bleft = digitalRead(A2);//but_left_pin);
  bright = digitalRead(A3);//but_right_pin);
  if (bup && !flag_up) {
    flag_up = true; 
    m1.button(0);
    menu_type(m1, m1.menu_count);
  };
  if (!bup && flag_up) {
    flag_up = false;
  };
  if (bdown && !flag_down) {
    flag_down = true; 
    m1.button(1);
    menu_type(m1, m1.menu_count);
    
  };
  if (!bdown && flag_down) {
    flag_down = false;
  };
  if (bleft && !flag_left) {
    flag_left = true; 
    m1.button(2);
    menu_type(m1, m1.menu_count);
  };
  if (!bleft && flag_left) {
    flag_left = false;
  };
  if (bright && !flag_right) {
    flag_right = true; 

    m1.button(3);
    menu_type(m1, m1.menu_count);
  };
  if (!bright && flag_right) {
    flag_right = false;
  };
 
    digitalWrite(latchPin, LOW);
  int d1 = 0;
  int d2 = 0;
  int d3 = 0;
  
 
// передаем последовательно на вход данных
  if (!m1.mode_lamp_bool){
      shiftOut(dataPin, clockPin, MSBFIRST, 0);
      shiftOut(dataPin, clockPin, MSBFIRST, 0); 
      shiftOut(dataPin, clockPin, MSBFIRST, 0); 
     
  } else {
      shiftOut(dataPin, clockPin, MSBFIRST, lamp[h][1]);
      shiftOut(dataPin, clockPin, MSBFIRST, lamp[h][2]); 
      shiftOut(dataPin, clockPin, MSBFIRST, lamp[h][3]); 
  }
        //"защелкиваем" регистр, устанавливаем значения на выходах
      digitalWrite(latchPin, HIGH);
  
   
   
}


float brightness(float temp){
  float f=0;
  f = 0 - pow(((0.2 *temp) -2.4),4) + 10; 
  if (f<0){ f =0;};
  f = f / 10;
  return f;
}

float collor(float data){ 
    float f=0;
     if (data < 8.9) {
       f= 30*tan(0.15*data -0.2); 
     } else if (data > 15.01) {
       f= 30*tan(-0.15*data +0.2);
     } else if (data <= 9.4) {
       f = 10*(pow(M_E,((-1)/(7*(data - 8.8)))+6.2));
     } else if (data >=14.7){
       f = 10*(pow(M_E, ((1)/(7*(data-15.2)))+6.2));
     } else {f = 10*(sin(0.4*data-3.23)+6.5);}
         return f;
}

float Red(float temp){
  float reds;
  if (temp <= 66 ) {  reds = 255; } 
  else {
    reds = temp - 60 ;
    reds = 329.698727446 * (pow(reds,(-0.1332047592)));
    if (reds < 0){reds = 0;} 
    else if (reds > 255){reds = 255;};
  } 
  return reds;
}

float Green(float temp){
  float gr=0;
  if (temp <= 66 ) { 
     
    gr = temp ;
    gr = 99.4708025861 *log(gr); 
    gr = gr - 161.1195681661;
    if (gr < 0){gr = 0;} 
    else if (gr > 255){gr = 255;};
   } 
  else {
    gr = temp - 60 ;
    gr = 288.1221695283 * (pow(gr,(-0.0755148492)));
    if (gr < 0){gr = 0;} 
    else if (gr > 255){gr = 255;};
  } 
  return gr;

}
float Blue(float temp) {
   float bl=0; 
  if (temp >= 66 ) {  bl = 255; } 
  else {
    if (temp <=19 ) { bl =0;} 
    else {
      bl =  temp - 10; 
      bl =  138.5177312231 * log(bl) - 305.0447927307;
      if (bl < 0) {bl = 0;}
      else if(bl > 255){bl = 255;}
    }
  }
  return bl;
}
