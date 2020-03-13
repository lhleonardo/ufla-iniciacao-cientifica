#include <Wire.h>
#include <LiquidCrystal.h>
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_MLX90614.h>
#include <math.h>

// CONFIGURAÇÃO DO SENSOR DE PULSO
const int REPORTING_PERIOD = 500; // ms
PulseOximeter pulseSensor;
uint32_t lastPulseReport = 0; 

// CONFIGURAÇÃO DO SENSOR DE TEMPERATURA
Adafruit_MLX90614 temperatureSensor = Adafruit_MLX90614();
uint32_t lastTemperatureReport = 0;

// CONFIGURAÇÃO DO LCD 
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
const int brightnessPin = 6, brightness = 90;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// CONFIGURAÇÃO DOS BOTOES DE MENU
const int buttonTemperature = 4; // botão 0
const int buttonPulse = 5; // botão 1

// BITMAP DE TERMOMETRO E CORAÇÃO
byte termometerIcon[] = { B00100, B10101, B01110, B11011, B01110, B10101, B00100, B00000 };
byte hearthIcon[] = { B00000, B01010, B10101, B10001, B10001, B01010, B00100, B00000 };

//////////////////////////////////////////////////////////////////////////////////////////

void lcdInitialize() {
  lcd.begin(16, 2);
  pinMode(brightnessPin, OUTPUT);
  analogWrite(brightnessPin, brightness);

  // definição dos caracteres especiais
  lcd.createChar(0, termometerIcon);
  lcd.createChar(1, hearthIcon);
}

void pulseOximeterInitialize() {
  if (not pulseSensor.begin()) {
//    throw -1; // não conseguiu inicializar o sensor de pulso
  }
}

void temperatureInitialize() {
  temperatureSensor.begin();
}

void setup() {
  // inicialização do LCD
  lcdInitialize();
  
  // mapeamento dos botões
  // usando PULLUP interno para evitar componentes externos no circuito
  pinMode(buttonTemperature, INPUT_PULLUP);
  pinMode(buttonPulse, INPUT_PULLUP);

  // inicialização do sensor de pulso e oximetro
  pulseOximeterInitialize();
  
  // inicialização do sensor de temperatura
  temperatureInitialize();
}

//////////////////////////////////////////////////////////////////////////////////////////////

int stateButtonTemperature, stateButtonPulse;
int currentButton = 1;

bool needClear;

void loop() {
    stateButtonTemperature = digitalRead(buttonTemperature);
    stateButtonPulse = digitalRead(buttonPulse);
    
    // não deve pressionar os dois ao mesmo tempo
    if (not stateButtonTemperature and not stateButtonPulse) return;

    // botão de temperatura foi pressionado
    if (not stateButtonTemperature and currentButton != 0) {
      currentButton = 0;
      // limpa lcd ao começar novo estágio
      pulseSensor.shutdown();

      lastTemperatureReport = 0;
    }

    // botão de pulso foi pressionado
    if (not stateButtonPulse and currentButton != 1) {
      currentButton = 1;
      // limpa lcd ao começar novo estágio
      pulseSensor.resume();
      lastPulseReport = 0;
    }

    if (currentButton != -1) {
      lcd.setCursor(0, 0);
      lcd.write(byte(currentButton));
      lcd.setCursor(3, 0);
      lcd.print(currentButton == 0 ? "Temperatura" : "Pulso/SpO3");
      lcd.setCursor(0, 1);
      switch (currentButton) {
        case 0: {
          readTemperature();
          break;
        } case 1: {
          readPulseOximeter();
          break;
        }
      }
    }
}

void readTemperature() {
  if (millis() - lastTemperatureReport > REPORTING_PERIOD) {
    double ambient, object;

    ambient = temperatureSensor.readAmbientTempC();
    object  = temperatureSensor.readObjectTempC();

    lcd.print(String(ambient) + "  ");
    lcd.setCursor(11, 1);
    lcd.print(String(object) + " ");
    
    lastTemperatureReport = millis();
  }
}

void readPulseOximeter() {
  pulseSensor.update();

  if (millis() - lastPulseReport > REPORTING_PERIOD) {
    lcd.print(String(pulseSensor.getHeartRate()) + " ");
    lcd.setCursor(11,1);
    lcd.print("  " + String(pulseSensor.getSpO2()) + " ");
    
    lastPulseReport = millis();
  }
}
