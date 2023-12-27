/*
 * TeslaChargeDoorOpener
 * 
 * This sketch will send a signal that will open the charge port door of a Tesla car. 
 * It is similar to the button of a Tesla charge cable when not plugged into the car.
 * It will send the signal when powered on, then do nothing. Suited for battery-powered
 * operation using a push button.
 * 
 * Pin 11 must be connected to the signal pin of an ASK STX882 433.92MHz transmitter 
 * that can be bought on eBay for a low price.
 * 
 * The message has been grabbed by using an SRX882 receiver to pick up the data sent
 * by a Tesla charging cable with built in push button.
 * The cable uses this signal to open the charge door when pushing the button not being plugged in.
 * When plugged in, the button on the cable can unlock the cable too. This is not done by RF, so
 * this sketch will not unlock the cable when plugged in.
 * The signal will be sent 5 times repeatedly, just like the charge cable button does.
 *
 * Author: Fred Larsen
 * Github: www.github.com/fredilarsen
 * 
 * License: Apache
 */

// Pins
const uint8_t signalPin = GPIO_NUM_32;                   // The number of the pin with the output signal

// The signal to send
const uint16_t pulseWidth = 400;                // Microseconds
const uint16_t messageDistance = 23;            // Millis
const uint8_t transmissions = 10;                // Number of repeated transmissions
// const uint8_t messageLength = 43;
// const uint8_t sequence[messageLength] = { 
//   0x02,0xAA,0xAA,0xAA,  // Preamble of 26 bits by repeating 1010
//   0x2B,                 // Sync byte
//   0x2C,0xCB,0x33,0x33,0x2D,0x34,0xB5,0x2B,0x4D,0x32,0xAD,0x2C,0x56,0x59,0x96,0x66,
//   0x66,0x5A,0x69,0x6A,0x56,0x9A,0x65,0x5A,0x58,0xAC,0xB3,0x2C,0xCC,0xCC,0xB4,0xD2,
//   0xD4,0xAD,0x34,0xCA,0xB4,0xA0};
const uint8_t sequence[] = { // based on https://github.com/akrutsinger/tesla-charge-port-signal
  0xAA,0xAA,0xAA,0x8A,0xCB,0x32,0xCC,0xCC,0xCB,0x4D,0x2D,
  0x4A,0xD3,0x4C,0xAB,0x4B,0x15,0x96,0x65,0x99,0x99,0x96,
  0x9A,0x5A,0x95,0xA6,0x99,0x56,0x96,0x2B,0x2C,0xCB,0x33,
  0x33,0x2D,0x34,0xB5,0x2B,0x4D,0x32,0xAD,0x28,
};
const uint8_t messageLength = sizeof(sequence) / sizeof(uint8_t);

void setup() {
  Serial.begin(115200);
  // pinMode(LED_BUILTIN, OUTPUT);
  pinMode(signalPin, OUTPUT);
  digitalWrite(signalPin, LOW);

  sendSignals();
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    if (input.length() > 0) { // Check if the string is not empty
        Serial.printf("Transmitting...\n");
        sendSignals();
    }
  }
  delay(1000);
  sendSignals();
}

void sendSignals() {
  // digitalWrite(LED_BUILTIN, HIGH);
  for (uint8_t t=0; t<transmissions; t++) {
    for (uint8_t i=0; i<messageLength; i++) sendByte(sequence[i]);
    digitalWrite(signalPin, LOW);
    delay(messageDistance);
  }
  // digitalWrite(LED_BUILTIN, LOW);
}

void sendByte(uint8_t dataByte) {
  for (int8_t bit=7; bit>=0; bit--) { // MSB
    digitalWrite(signalPin, (dataByte & (1 << bit)) != 0 ? HIGH : LOW);
    delayMicroseconds(pulseWidth);
  }
}
