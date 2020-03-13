#include <Wire.h>
// bibliotecas a serem utilizadas. 
// pulseoximeter para sensor MAX30100
// Adafruit para sensor MCP9808
#include "Adafruit_MCP9808.h"
#include "MAX30100_PulseOximeter.h"
#define REPORTING_PERIOD_MS     700 // configuração para sensor de pulso


PulseOximeter pox;
uint32_t tsLastReport = 0;
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();


void setup() {
  Serial.begin(115200);
  while (not Serial);
  Serial.println("Configurações iniciais realizadas.");
  
  // inicialização do sensor de pulso
  if (not pox.begin()) {
    Serial.println("Falha ao inicializar o sensor de pulso. Verifique!");
    while(true);
  } else {
    Serial.println("Sensor de pulso inicializado!");
  }

  // inicialização do sensor de temperatura
  if (not tempsensor.begin(0x18)) {
    Serial.println("Falha ao inicializar o sensor de temperatura. Verifique!");
    while (true);
  } else {
    Serial.println("Sensor de temperatura inicializado!");
  }

  // configuração de funcionamento do sensor de temperatura. 
  // nivel 3 significa mais precisão com tempo de resposta maior
  tempsensor.setResolution(3);

  tsLastReport = 0;
}

unsigned semaforo = 0;

void loop() {
  pox.update();
 
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    Serial.print("Batimento cardiaco:");
    Serial.print(pox.getHeartRate());
    Serial.print("bpm / SpO2:");
    Serial.print(pox.getSpO2());
    Serial.println("%");

    pox.shutdown();
    delay(100);
    executaLeituraTemperatura();

    pox.resume();
    tsLastReport = millis();
  }  
}

void executaLeituraTemperatura() {
  //Serial.println("\"Levantando\" o sensor de temperatura...");
  tempsensor.wake();

  float celsius = tempsensor.readTempC();
  Serial.print("Temperatura atual (em ºC): ");
  Serial.println(celsius);

  //Serial.println("Desligando temporariamente o sensor de temperatura...");
  delay(100);
  tempsensor.shutdown_wake(1);
  delay(100);
  
}
