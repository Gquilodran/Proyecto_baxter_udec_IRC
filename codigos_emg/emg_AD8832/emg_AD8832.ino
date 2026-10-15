#define MODO_GRAFICO 1
const int pinEMG = 34;
const uint32_t Ts_us = 1000;   // 1000 us -> 1 kHz
uint32_t t_next;

void setup() {
  Serial.begin(921600);
  analogReadResolution(12);                 // 0..4095
  analogSetPinAttenuation(pinEMG, ADC_11db); // rango ~0-3.3 V
  t_next = micros();
}

void loop() {
  if ((int32_t)(micros() - t_next) >= 0) {
    t_next += Ts_us;
    Serial.println(analogRead(pinEMG));
  }
}
// const int pinEMG=34; // el pin en la ESP32 
// int signalEMG=0; // de 0 a 1095

// void setup() {
//   Serial.begin(115200); // baudios
//   //Serial.begin(9600); 
//   analogReadResolution(12);
// }

// void loop() {
//   signalEMG = analogRead(pinEMG); // lectura y almacenamiento
//   Serial.println(signalEMG); // impresion de las lecturas
//   //delay(15);
//   //Serial.println(analogRead(A0));
//   delay(100);
// }



// int emgPin = A0; // Entrada analogica arduino
//   float emgFiltrado = 0.0; // Valor EMG filtrado
//   float alpha = 0.2; // Coeficiente de filtro

// void setup() {
//   Serial.begin(9600);
// }
// void loop() {
//   int emg_señal = analogRead(emgPin);
//   // Aplicar el filtro de paso bajo
//   emgFiltrado = (alpha * emg_señal) + ((1 - alpha) * emgFiltrado);

//   Serial.println(emgFiltrado);
//   delay(80); // Ajusta el retardo según sea necesario
// }

