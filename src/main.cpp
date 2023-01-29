#include <Arduino.h>
#include <Wire.h>              
#include <LiquidCrystal_I2C.h>
#include <CTBot.h>
#include <token.h>

#define touch GPIO_NUM_35
#define motorA GPIO_NUM_15
#define motorB GPIO_NUM_2
#define velA GPIO_NUM_0
#define velB GPIO_NUM_4
#define trigger GPIO_NUM_16
#define echo GPIO_NUM_17

#define COLUMS 16
#define ROWS   2

#define PAGE   ((COLUMS) * (ROWS))

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

int velocidad;
float longitud = 10;
float papel;
float tiempo = 0;
float t;
float d;

CTBot miBot;

void setup() {
  pinMode(touch,INPUT);
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);
  pinMode(velA, OUTPUT);
  pinMode(velB, OUTPUT);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT); 
  digitalWrite(trigger, LOW);
  Serial.begin(9600);

  miBot.wifiConnect(ssid,password);
  miBot.setTelegramToken(token);

  if (miBot.testConnection()) {
    Serial.println("\n Conectado");
  }
  else {
    Serial.println("\n Problemas Auxilio");
  }

  lcd.begin(COLUMS, ROWS);

  lcd.print(F("Dispensadora"));
  lcd.setCursor(0,1);
  lcd.print("de papel");
  delay(2000);

  lcd.clear();
}

void activar_motor(int motor, int ena, int vel, int time){
  analogWrite(ena,vel);
  digitalWrite(motor, HIGH);
  delay(time*1000);
  digitalWrite(motor,LOW);
}

void loop() {
  TBMessage msg;

  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);      
  digitalWrite(trigger, LOW);
  
  t = pulseIn(echo, HIGH);
  d = t/59;            
  
  papel = (1 - ((5 - (longitud - d))/5)) * 100;

  lcd.setCursor(0,0);	//columna - fila
  lcd.print("Cantidad: ");
  lcd.setCursor(0,1);
  lcd.print((String)papel+'%');
  delay(1000);
  lcd.clear();
    

  while (papel<=10){
  
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);      
    digitalWrite(trigger, LOW);
  
    t = pulseIn(echo, HIGH);
    d = t/59;            
  
    papel = (1 - ((5 - (longitud - d))/5)) * 100;
    
    if (millis() - tiempo > 5000){
      miBot.sendMessage(IDchat, "Papel por debajo del 20%, favor recargar");
      lcd.setCursor(0,0);
      lcd.print("Sin papel!");
      delay(2000);
      tiempo = millis();
    }
  }

  lcd.clear();

  if (digitalRead(touch)==1 && papel>10){
    lcd.setCursor(0,0);	//columna - fila
    lcd.print("Dispensando... ");
    activar_motor(motorA, velA, 128, 5);
    delay(1000);
    activar_motor(motorB, velB, 192, 1);
    lcd.clear();
  };
}