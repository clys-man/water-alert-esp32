#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

const int potPin = 15;
const int freq = 5000;
const int resolution = 8;

int cont = 0;
int alertPin = 18;
int percentPins[4] = {19, 21, 22, 23};
int size = sizeof(percentPins) / sizeof(percentPins[0]);

void setup() {
  pinMode(alertPin, OUTPUT);

  for (cont = 0; cont < size; cont++) {
    ledcSetup(cont, freq, resolution);
    ledcAttachPin(percentPins[cont], cont);
  }

  Serial.begin(115200);
  SerialBT.begin("ESP32test");
  Serial.println("Bluetooth on!");
}

void sendAlert(int pin, int percent) {
  bool alert = !percent;
  digitalWrite(alertPin, alert);
}

int calcDuty(int percent) {
  return (256 * percent) / 100;
}

int calcPercent(int potency, int channel) {
  return ((100 * potency) / 25) - 104 * channel;
}

void turnAllLeds(int state = HIGH) {
  for (cont = 0; cont < size; cont++) {
    ledcWrite(cont, 0);
  }
}

void sendBtAlert() {
  SerialBT.println("O nível de água está baixo!");
  delay(150);
}

void loop() {
  int potValue = map(analogRead(potPin), 0, 4095, 0, 100);
  int channels = potValue / 26;
  int percent = calcPercent(potValue, channels);
  int duty = calcDuty(percent);

  if (potValue == 0) {
    turnAllLeds(LOW);

    sendBtAlert();
  }


  ledcWrite(channels, duty);
  ledcWrite(channels + 1, 0);
  sendAlert(alertPin, potValue);
}
