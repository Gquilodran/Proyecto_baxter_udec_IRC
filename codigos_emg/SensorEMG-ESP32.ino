// const int pinEMG=34; // el pin en la ESP32 
// int signalEMG=0; // de 0 a 1095
// float emgFiltrado = 0.0; // Valor EMG filtrado
// float alpha = 0.2; // Coeficiente de filtro


// void setup() {
//   Serial.begin(115200); // baudios
//   //Serial.begin(9600); 
//   analogReadResolution(12);
// }

// void loop() {
//   signalEMG = analogRead(pinEMG); // lectura y almacenamiento
//   //Serial.println(signalEMG); // impresion de las lecturas
//   emgFiltrado = (alpha * signalEMG) + ((1 - alpha) * emgFiltrado);
//   //delay(15);
//   Serial.println(emgFiltrado);
//   delay(80); // Ajusta el retardo según sea necesario
// }

const int pinEMG = 34;
const int muestrasBuffer = 50;
float bufferEMG[muestrasBuffer];
int indiceBuffer = 0;

// Parámetros de filtrado y procesamiento
float emgFiltrado = 0.0;
float alpha = 0.2;  // Filtro paso-bajo (reducido para mejor suavizado)
float baseline = 0.0; // Para offset removal
float offset = 0.0;  // Valor DC
float ganancia = 15.0; // AMPLIFICACIÓN DE SOFTWARE (ajusta según necesario)

// Para monitoreo
unsigned long ultimoTiempo = 0;
float maxValor = 0;
float minValor = 4095;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  
  // Calibración inicial: obtener baseline
  Serial.println("Iniciando calibración... NO MUEVAS EL BRAZO");
  delay(2000);
  
  float suma = 0;
  for (int i = 0; i < 200; i++) {
    suma += analogRead(pinEMG);
    delay(10);
  }
  offset = suma / 200.0;
  Serial.print("Offset calculado: ");
  Serial.println(offset);
  Serial.println("¡Calibración completa! Puedes comenzar a mover el brazo.");
}

void loop() {
  int signalEMG = analogRead(pinEMG);
  
  // Remover offset (componente DC)
  float emgSinOffset = signalEMG - offset;
  
  // Valor absoluto (EMG es bipolar)
  float emgAbs = abs(emgSinOffset);
  
  // Amplificación de software (crucial para valores pequeños)
  float emgAmplificado = emgAbs * ganancia;
  
  // Filtro paso-bajo (exponential moving average)
  emgFiltrado = (alpha * emgAmplificado) + ((1 - alpha) * emgFiltrado);
  
  // Almacenar en buffer para RMS
  bufferEMG[indiceBuffer] = emgFiltrado;
  indiceBuffer = (indiceBuffer + 1) % muestrasBuffer;
  
  // Calcular RMS (mejor métrica para EMG que el promedio)
  float rms = calcularRMS();
  
  // Monitoreo de rango dinámico
  if (rms > maxValor) maxValor = rms;
  if (rms < minValor && rms > 0) minValor = rms;
  
  // Mostrar en Serial (puedes visualizar en Serial Plotter)
  Serial.print(rms);
  Serial.print(",");
  Serial.print(maxValor * 0.5); // Referencia visual
  Serial.print(",");
  Serial.println(0); // Línea de referencia en 0
  
  delay(80); // Tasa de muestreo: 100 Hz
}

// Calcular RMS del buffer
float calcularRMS() {
  float suma = 0;
  for (int i = 0; i < muestrasBuffer; i++) {
    suma += bufferEMG[i] * bufferEMG[i];
  }
  return sqrt(suma / muestrasBuffer);
}