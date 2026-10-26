#pragma once
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA  64
#define DIRECCION_OLED 0x3C 

class GestorPantalla {
public:
  GestorPantalla() : _display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1) {}

  bool begin() {
    Wire.begin(21, 22); // Pines nativos I2C
    return _display.begin(SSD1306_SWITCHCAPVCC, DIRECCION_OLED);
  }

  void mostrar(const char* modo, bool alarma, int valorSensor) {
    _display.clearDisplay();
    _display.setTextColor(SSD1306_WHITE);

    // Cabecera: Modo actual
    _display.setTextSize(1);
    _display.setCursor(0, 0);
    _display.print("MODO: ");
    _display.println(modo);
    _display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    // Estado central: ALARMA o VIGILANDO
    _display.setCursor(0, 18);
    _display.setTextSize(2);
    if (alarma) {
      _display.println("ALARMA!");
    } else {
      _display.println("VIGILANDO");
    }

    // Datos del sensor según modo
    _display.setTextSize(1);
    _display.setCursor(0, 48);
    if (strcmp(modo, "COLABORATIVO") == 0) {
      _display.print("LDR: ");
      _display.print(valorSensor);
    } else if (strcmp(modo, "AUTONOMO") == 0) {
      _display.print("Distancia: ");
      _display.print(valorSensor);
      _display.print(" cm");
    } else {
      _display.println("Buscando red...");
    }

    _display.display();
  }

private:
  Adafruit_SSD1306 _display;
};