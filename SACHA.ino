//librerías necesarias
#include <SPI.h>    // incluye libreria interfaz SPI
#include <SD.h>     // incluye libreria para tarjetas SD
//#include <Wire.h>   // incluye libreria para interfaz I2C
#include <RTClib.h>   // incluye libreria para el manejo del modulo RTC
#include <LiquidCrystal_I2C.h>//incluye libreria para el manejo de LCD
//#include "Arduino.h"
#include "PCF8574.h"//i2c sensores hall
#include "HX711.h"
#include <EEPROM.h>
#include "I2CKeyPad.h"
#define SSpin 10    // Slave Select en pin digital 10


//objetos y variables
I2CKeyPad keyPad;
RTC_DS3231 rtc;     // crea objeto del tipo RTC_DS3231
File archivo;     // objeto archivo del tipo File
LiquidCrystal_I2C lcd(0x27,4,20);
PCF8574 pcf8574(0x25);
HX711 balanza;

//const uint8_t KEYPAD_ADDRESS = 0x21;
int bt = 2;
int huevo= 3;
int idx,lastidx= 16;
const int DOUT=A0;
const int CLK=A1;
int paso[] = {6,7,6};
int dosif[] = {8,9,7};
int origen= 2;
int escala = 955;
int c = 0;
int pesoingresado = 0;
float peso, peso_aux =0.0;
float pesohuevo = 0.0;
String dato = "";
String docname="";
String hora = "";
String clase = "";
String pscala = "";
char keys[] = "123A456B789C*0#DNF";  // N = Nokey, F = Fail
int pause = 0;

byte Bluetooth[] = {
  B00110,
  B10101,
  B01110,
  B00100,
  B01110,
  B10101,
  B00110,
  B00000
};
byte sd[] = {
  B00111,
  B01111,
  B11111,
  B11111,
  B10001,
  B10001,
  B11111,
  B00000
};
byte Reloj[] = {
  B01110,
  B10001,
  B10101,
  B10111,
  B10001,
  B01110,
  B00000,
  B00000
};

byte AA[] = {
  B01010,
  B10101,
  B10101,
  B11111,
  B10101,
  B10101,
  B10101,
  B00000
};




void setup() {
  digitalWrite(bt,LOW);
 Serial.begin(9600);    // inicializa comunicacion serie a 9600 bps

 if (! rtc.begin()) {       // si falla la inicializacion del modulo
 //Serial.println("Modulo RTC no encontrado !");
 while (1); 
  }

  //Wire.begin();
  //Wire.setClock(400000);
  if (keyPad.begin(0x21) == false)
  {
    //Serial.println("\nERROR: cannot communicate to keypad.\nPlease reboot.\n");
    while(1);
  }
  
  nameday();
  
  //rtc.adjust(DateTime(__DATE__, __TIME__));
  
  
  //Serial.println("Inicializando tarjeta ...");  // texto en ventana de monitor
  if (!SD.begin(SSpin)) {     // inicializacion de tarjeta SD
  //  Serial.println("fallo en inicializacion !");// si falla se muestra texto correspondiente y
    return;         // se sale del setup() para finalizar el programa
  }
  
 // Serial.println("inicializacion correcta");  // texto de inicializacion correcta


  balanza.begin(DOUT, CLK);
  byte buffer[] = {0x04, 0x1C};
  buffer[0] = EEPROM.read(0);
  buffer[1] = EEPROM.read(1);
  //escala = (byte)buffer[0] << 8 | (byte)buffer[1];
  balanza.set_scale(escala); // Establecemos la escala
  balanza.tare(10);
  lcd.begin(20,4);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SACHA-MODULAR");
  //mov(paso,5);

  leerSD();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("A pesar se dijo");
  delay(1000);
  lcd.clear();
  
  

    
    
      

  Serial.print("total: ");
  lcd.setCursor(0,3);
  lcd.print(c);


  pcf8574.pinMode(0, INPUT);
  pcf8574.pinMode(1, INPUT);
  pcf8574.pinMode(2, INPUT);
  pcf8574.pinMode(4, INPUT);
  pcf8574.pinMode(5, INPUT);
  pcf8574.pinMode(6, INPUT);
  pcf8574.pinMode(7, INPUT);
  
  pinMode(6, OUTPUT);  // pin 4 como salida
  pinMode(7, OUTPUT);   // pin 5 como salida
  pinMode(8, OUTPUT);  // pin 4 como salida
  pinMode(9, OUTPUT);   // pin 5 como salida
  pinMode(4, OUTPUT);  // pin 4 como salida
  pinMode(5, OUTPUT);   // pin 5 como salida
  pinMode(10, OUTPUT);   // pin 5 como salida
  pinMode(bt, OUTPUT);  // pin 4 como salida
  pinMode(huevo, INPUT);   // pin 5 como salida


    lcd.createChar(0, Bluetooth);
  lcd.createChar(1, Reloj);
  lcd.createChar(2, AA);
  lcd.createChar(3, sd);
  lcd.setCursor(17,0);
  lcd.write(0);
  lcd.setCursor(18,0);
  lcd.write(1);
  lcd.setCursor(19,0);
  lcd.write(3);
  lcd.setCursor(0,3);
  
  


}


void loop() { 

 
  pausa();
  if(pause == 1){
    pause =0;
lcd.setCursor(6,0);
  lcd.print("PAUSA");
    while(true){
      
      idx = keyPad.getKey();
      if(keys[idx]=='*'){
            lcd.setCursor(0,0);
            lcd.clear();
            idx,lastidx =16;
            delay(1000);
            break;
        }
        
     if(keys[idx]=='#'){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("peso: ");
      delay(1000);

      while(true){
        
         
          idx = keyPad.getKey();
          delay(20);
          if(keys[idx]=='N'&&keys[lastidx]!='N'){
            if(keys[lastidx]=='#'){
              pesoingresado = pscala.toInt();
              pesohuevo = (float)pesoingresado/10;
              lcd.setCursor(0,1);        
              lcd.print(pesohuevo);
              pscala = "";
              delay(1000);
              balanza.tare();
              mov(dosif,4);
              Serial.println("x");
              delay(300);
              rec(paso); 
              
              escala = (float)balanza.get_value(10)/pesohuevo;
              Serial.println("x");
              
              
               mov(paso,5);
               delay(246);
               lcd.setCursor(0,3);        
              lcd.print(escala);
              EEPROM.write(0,highByte(escala));
              EEPROM.write(1,lowByte(escala));
              
              balanza.set_scale(escala);
              
          
              break;
              }
            else{
            lcd.print(keys[lastidx]);
            pscala +=(keys[lastidx]-'0');
            }
            
          }
          lastidx = idx;
          
          
  
          
          }

          

        
      }
    }


    
  }




 








  while(digitalRead(huevo)){
    //digitalWrite(bt,HIGH);
    }
  peso_aux = balanza.get_units();
  lcd.setCursor(0,0);
  lcd.print("      ");
  lcd.setCursor(0,0);
  lcd.print(peso_aux);
  
while(peso_aux>10){
      lcd.setCursor(2,2);
      lcd.print("                                     ");
      delay(1000);
      lcd.setCursor(8,2);
      lcd.print("ERROR!!");
      delay(1000);
      lcd.setCursor(2,2);
      lcd.print("                                     ");
      delay(1000);
      lcd.setCursor(8,2);
      lcd.print("ERROR!!");
      delay(1000);
      lcd.setCursor(2,2);
      lcd.print("                                     ");
      peso_aux = balanza.get_units();
      peso = 0;
}
if(peso_aux >2 || peso_aux<-2){
      lcd.setCursor(2,2);
      lcd.print("                                     ");
      lcd.setCursor(1,2);
      lcd.print("CALIBRANDO");
      
    balanza.tare();
    peso_aux = balanza.get_units();
      lcd.setCursor(1,2);
      lcd.print("           ");
    }
  
  
     
  mov(dosif,4);
  delay(300);
  


  rec(paso); 
 Serial.println("ya");

  if(peso>41.7)
  {
    escribirSD(peso);
    c++;
    lcd.setCursor(0,3);
    lcd.print(c);

   

      
    
  
    lcd.setCursor(10,1);
    lcd.print(peso,1);
    lcd.setCursor(11,2);
    
    if(clase =="AA"){
    lcd.print("(");
    lcd.setCursor(12,2);
    lcd.write(2);
    lcd.setCursor(13,2);
    lcd.print(")");
    }
    else{
      lcd.print(clase);
      
   
      }
  }
 
 
   Serial.println(peso); 
  
  peso = balanza.get_units()- peso_aux;

  if(peso >=47.6 && peso <53.6){
    clase = "(B)";
    clas(1);}
  else if(peso >=53.6 && peso <59.6){
    clase = "(A)";
    clas(2);
  }
  else if(peso >=59.6 && peso <67.6){
    clase= "AA";
    clas(3);
  }
  else if(peso >=67.6 && peso <73.6){
    clase = "(X)";
    clas(4);
  }
  else if(peso >=41.6 && peso <47.6){
    clase = "(C)";
    clas(0);
  }
  else if(peso >=29.7 && peso <41.7){
    clas(0);  
    clase = "(-)";
  }
         
  else if(peso >=73.7){
    clase = "(J)";
    clas(0); 
  }
  else {
    clase = "(-)";
    clas(2);
  }
   
  lcd.setCursor(2,1);
  lcd.print(peso,1);
  lcd.setCursor(3,2);
    if(clase =="AA"){
  lcd.print("(");
  lcd.setCursor(4,2);
  lcd.write(2);
  lcd.setCursor(5,2);
  lcd.print(")");
  }
  else{
    lcd.print(clase);
    
    }






  DateTime fecha = rtc.now();
if(fecha.hour()<10){hora=0+String(fecha.hour());}
else{hora=String(fecha.hour());}
if(fecha.minute()<10){hora=hora+":"+0+String(fecha.minute());}
else{hora=hora+":"+String(fecha.minute());}
  //Serial.println(peso);
      
  
  lcd.setCursor(15,3);
  lcd.print(hora);

}

void nameday(){
  DateTime fecha = rtc.now();
docname=String(fecha.year())+String(fecha.month())+String(fecha.day());
if(fecha.month()<10){docname=String(fecha.year())+0+String(fecha.month());}
else{docname=String(fecha.year())+String(fecha.month());}
if(fecha.day()<10){docname=docname+0+String(fecha.day());}
else{docname=docname+String(fecha.day())+".txt";}
docname = "hola.txt";
  }



void leerSD(){
  
  archivo = SD.open(docname);    // apertura de archivo prueba.txt
  if (archivo) {
    
    Serial.println("Contenido de prueba.txt:"); // texto en monitor serie
    while (archivo.available()) {   // mientras exista contenido en el archivo
      dato = archivo.read();
      if(dato =="44"){
        c++;}

      //Serial.println(dato); 
      // lectura de a un caracter por vez
    }
    archivo.close();        // cierre de archivo
  }
  
 
  else {
    Serial.println("error en la apertura de prueba.txt");// texto de falla en apertura de archivo
  }
  }


void escribirSD(float n){
archivo = SD.open(docname, FILE_WRITE);  // apertura para lectura/escritura de archivo prueba.txt
    if (archivo) {
      archivo.print(n);  // escritura de una linea de texto en archivo
       archivo.print(",");
      Serial.println("bien"); // texto en monitor serie
             // cierre del archivo
      //Serial.println("escritura correcta"); // texto de escritura correcta en monitor serie
       archivo.close();
    }
     
    else 
    {
      
      Serial.println("XSD");  // texto de falla en apertura de archivo
    }
}




//-----------------------------------inicio control motor ----------------------------------//
void mov(int motor[], int vel)
{
  int c = 0;
  
  int hall = 0;
  int aux = 0;

 // int pasos= map(grados, 0, 360, 0 , 200 );
  digitalWrite(motor[0], LOW);    // giro en un sentido
  while(true){ 
    pausa();


    if(c> 3){
    aux = hall;
    hall = pcf8574.digitalRead(motor[2]);
    c = 0;
    }
    c++;
    
  
    if(hall == 0 && aux == 1){break;}
    
    // 200 pasos para motor de 0.9 grados de angulo de paso
    digitalWrite(motor[1], HIGH);       // nivel alto
    delay(vel);          // por 10 mseg
    digitalWrite(motor[1], LOW);        // nivel bajo
    delay(vel);          // por 10 mseg
  
  }
  
  
}

//--------final motor-----------------------------------//


//---------inicio reponer------------------------------//
void rec(int motor[])
{
  digitalWrite(motor[0], LOW);
  for (int i = 0;i<=30;i++)
  {
    pausa();
    digitalWrite(motor[1], HIGH);       // nivel alto
    delay(2);          // por 10 mseg
    digitalWrite(motor[1], LOW);        // nivel bajo
    delay(2);          // por 10 mseg
    }
  }
  //---------fin reponer----------------------------//


void pausa(){
  if(pause==0){
   idx = keyPad.getKey();
   //Serial.println(idx);
  if(keys[idx]=='*'){
    lcd.setCursor(7,0);
    lcd.print(F("p.."));
    
    pause = 1;
    }
   
}
}





void clas(int fin)
{
  int c_clas = 0;
int hall_clas = 0; 
int aux_clas = 0;
int c_mov= 0;
int hall_mov = 0; 
int aux_mov = 0;
int clas_listo =0;
int paso_listo =0;
int vel = 3;
  digitalWrite(paso[0], LOW);
  int sensor;
  if(fin>2)
  {sensor = fin+1;
    }
    else{
      sensor = fin;}

if(fin-origen ==4 || (fin<origen && fin-origen !=-4)){
  digitalWrite(4, 0);//giro horario
  }
  else if(origen == fin){
    clas_listo = 1;
    vel = 6;}
  else{
    digitalWrite(4, 1);//giro antihorario
    }
      // giro en un sentido





  while(clas_listo ==0||paso_listo ==0){ 

    if(clas_listo ==0){
  if(c_clas>2){
    aux_clas = hall_clas;
    hall_clas = pcf8574.digitalRead(sensor);
    
    c_clas = 0;
    }
    c_clas++;
    
  if(hall_clas==0 && aux_clas ==1){

    origen = fin;
      clas_listo = 1;
      }
  
 
    else{
          digitalWrite(5, HIGH);       // nivel alto
    delay(vel);          // por 10 mseg
    digitalWrite(5, LOW);        // nivel bajo
    delay(vel); 
      
    }
    }

    if(paso_listo==0){


          if(c_mov> 2){
    aux_mov = hall_mov;
    hall_mov = pcf8574.digitalRead(paso[2]);
    Serial.println(hall_mov);
    c_mov = 0;
    }
    c_mov++;
    
  
    if(hall_mov == 0 && aux_mov == 1){
      paso_listo = 1;
      }

      else{
    
    // 200 pasos para motor de 0.9 grados de angulo de paso
    digitalWrite(paso[1], HIGH);       // nivel alto
    delay(vel);          // por 10 mseg
    digitalWrite(paso[1], LOW);        // nivel bajo
    delay(vel);          // por 10 mseg
      
      }
      
      
      }
  
  
  
  }

  
}
