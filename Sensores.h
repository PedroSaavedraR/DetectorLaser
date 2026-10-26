#pragma once
#include <Arduino.h>

class GestorSensores {
public:
  const int PIN_LDR     = 4;
  const int PIN_BUZZER  = 15;
  const int PIN_US_TRIG = 19;
  const int PIN_US_ECHO = 23;

  const int UMBRAL_LASER        = 4000;
  const int UMBRAL_DISTANCIA_CM = 80;

  void begin() {
    pinMode(PIN_LDR, INPUT);
    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_US_TRIG, OUTPUT);
    pinMode(PIN_US_ECHO, INPUT);

    digitalWrite(PIN_BUZZER, LOW);
    digitalWrite(PIN_US_TRIG, LOW);
  }

  bool verificarCorteLaser(int &valorLeido) {
    valorLeido = analogRead(PIN_LDR);
    return (valorLeido < UMBRAL_LASER);
  }

  long medirDistanciaUltrasonidos() {
    digitalWrite(PIN_US_TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(PIN_US_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_US_TRIG, LOW);

    long duracion = pulseIn(PIN_US_ECHO, HIGH, 25000);
    if (duracion == 0) return 999;
    return duracion * 0.034 / 2;
  }

  void setBuzzer(bool encendido) {
    digitalWrite(PIN_BUZZER, encendido ? HIGH : LOW);
  }
};