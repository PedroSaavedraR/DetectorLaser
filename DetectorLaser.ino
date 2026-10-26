#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "Pantalla.h"
#include "Sensores.h"

// Hardware e instancias
GestorPantalla pantalla;
GestorSensores sensores;

const int PIN_BOTON = 12;

// Estados
enum ModoOperacion { BUSCANDO, COLABORATIVO, AUTONOMO };
ModoOperacion modoActual = AUTONOMO;

// Tiempos y red
const unsigned long TIMEOUT_BUSQUEDA_MS = 3000;
unsigned long tInicioBusqueda = 0;
bool parDetectado = false;
uint8_t macPar[6];
uint8_t macPropia[6];
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

typedef struct __attribute__((packed)) {
  uint8_t tipo;
  uint8_t macOrigen[6];
} MensajeDescubrimiento;

void alRecibirDatos(const esp_now_recv_info_t *info, const uint8_t *datos, int len) {
  if (len == sizeof(MensajeDescubrimiento)) {
    MensajeDescubrimiento msg;
    memcpy(&msg, datos, sizeof(MensajeDescubrimiento));
    if (msg.tipo == 0 && !parDetectado) {
      memcpy(macPar, msg.macOrigen, 6);
      parDetectado = true;
    }
  }
}

void registrarPeer(const uint8_t* mac) {
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, mac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (!esp_now_is_peer_exist(mac)) {
    esp_now_add_peer(&peerInfo);
  }
}

void enviarBalizaBusqueda() {
  MensajeDescubrimiento msg;
  msg.tipo = 0;
  memcpy(msg.macOrigen, macPropia, 6);
  esp_now_send(broadcastAddress, (uint8_t*)&msg, sizeof(MensajeDescubrimiento));
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_BOTON, INPUT_PULLUP);
  sensores.begin();
  pantalla.begin();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_get_mac(WIFI_IF_STA, macPropia);

  if (esp_now_init() == ESP_OK) {
    registrarPeer(broadcastAddress);
    esp_now_register_recv_cb(alRecibirDatos);
  }

  pantalla.mostrar("AUTONOMO", false, 0);
}

void loop() {
  // Pulsación del botón para forzar descubrimiento
  if (digitalRead(PIN_BOTON) == LOW && modoActual != BUSCANDO) {
    modoActual = BUSCANDO;
    parDetectado = false;
    tInicioBusqueda = millis();
    sensores.setBuzzer(false);
  }

  switch (modoActual) {
    case BUSCANDO: {
      enviarBalizaBusqueda();
      pantalla.mostrar("BUSCANDO", false, 0);

      if (parDetectado) {
        modoActual = COLABORATIVO;
        registrarPeer(macPar);
        delay(400);
      } else if (millis() - tInicioBusqueda >= TIMEOUT_BUSQUEDA_MS) {
        modoActual = AUTONOMO;
        delay(400);
      }
      delay(80);
      break;
    }

    case COLABORATIVO: {
      int lecturaLDR = 0;
      bool alarma = sensores.verificarCorteLaser(lecturaLDR);
      sensores.setBuzzer(alarma);
      pantalla.mostrar("COLABORATIVO", alarma, lecturaLDR);
      delay(60);
      break;
    }

    case AUTONOMO: {
      long distancia = sensores.medirDistanciaUltrasonidos();
      bool alarma = (distancia < sensores.UMBRAL_DISTANCIA_CM);
      sensores.setBuzzer(alarma);
      pantalla.mostrar("AUTONOMO", alarma, (int)distancia);
      delay(80);
      break;
    }
  }
}