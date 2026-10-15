#include <math.h>

// ---------- Configuración ----------
const int   PIN_EMG    = 34;
const int   PIN_BOTON  = 0;        // botón BOOT: recalibrar
const float FS         = 1000.0;   // Hz
const uint32_t TS_US   = 1000;     // 1 kHz
const int   N_BLOQUE   = 50;       // 50 ms -> 20 salidas/s
const bool  USAR_NOTCH = true;     // 50 Hz (Chile)
#define MODO_GRAFICO 0             // 1 = imprime "crudo,filtrado" a 1 kHz (para las figuras del informe)

// ---------- Filtro biquad (RBJ cookbook) ----------
struct Biquad {
  float b0=1, b1=0, b2=0, a1=0, a2=0, z1=0, z2=0;
  void config(int tipo, float f0, float Q) {   // 0=pasa-bajos, 1=pasa-altos, 2=notch
    float w = 2*PI*f0/FS, c = cosf(w), al = sinf(w)/(2*Q), a0 = 1 + al;
    if (tipo == 0)      { b0 = (1-c)/2; b1 = 1-c;      b2 = (1-c)/2; }
    else if (tipo == 1) { b0 = (1+c)/2; b1 = -(1+c);   b2 = (1+c)/2; }
    else                { b0 = 1;       b1 = -2*c;     b2 = 1; }
    a1 = -2*c/a0; a2 = (1-al)/a0; b0 /= a0; b1 /= a0; b2 /= a0;
    z1 = z2 = 0;
  }
  float proc(float x) {
    float y = b0*x + z1;
    z1 = b1*x - a1*y + z2;
    z2 = b2*x - a2*y;
    return y;
  }
};
Biquad hp, lp, nt;

// ---------- Estado ----------
enum Estado { ARRANQUE, AVISO, REPOSO, CONTRACCION, LISTO };
Estado estado = ARRANQUE;
int bloquesEstado = 0;
uint32_t tSig;
float sumSq = 0; int nMuestras = 0;
float env = 0;
float mu = 0, m2 = 0, sigma = 0, mvc = 0, umbralON = 0, umbralOFF = 0;
int nRep = 0;
bool calibrado = false, activo = false;
int cntOn = 0, cntOff = 0;

void cambiar(Estado e) {
  estado = e; bloquesEstado = 0;
  if (e == AVISO)       Serial.println("# Preparate: relaja el musculo (2 s)");
  if (e == REPOSO)      { Serial.println("# REPOSO 3 s: no muevas el brazo"); mu = m2 = 0; nRep = 0; }
  if (e == CONTRACCION) { Serial.println("# CONTRAE FUERTE 3 s"); mvc = 0; }
}

void finalizarCalibracion() {
  float rango = mvc - mu;
  calibrado = (mvc > 3*mu) && (rango > 6*sigma);   // criterios iniciales, ajustalos con tus datos
  Serial.printf("# mu=%.2f sigma=%.2f mvc=%.2f razon=%.1f\n", mu, sigma, mvc, mu > 0 ? mvc/mu : 0);
  if (calibrado) {
    umbralON  = mu + 0.30f*rango;
    umbralOFF = mu + 0.15f*rango;
    Serial.println("# CALIBRACION OK");
  } else {
    Serial.println("# CALIBRACION FALLIDA: contrae mas fuerte o revisa electrodos/ganancia. Pulsa BOOT para repetir");
  }
  activo = false; cntOn = cntOff = 0;
  cambiar(LISTO);
}

void detectar() {
  float nivel = 0;
  if (calibrado) {
    if (!activo) { cntOn  = (env > umbralON)  ? cntOn + 1 : 0;  if (cntOn  >= 2) { activo = true;  cntOff = 0; } }
    else         { cntOff = (env < umbralOFF) ? cntOff + 1 : 0; if (cntOff >= 3) { activo = false; cntOn = 0; } }
    nivel = constrain((env - mu) / (mvc - mu), 0.0f, 1.0f);
  }
  Serial.printf("%.2f,%.2f,%d,%.2f\n", env, umbralON, activo ? 1 : 0, nivel);
}

void procesarBloque(float r) {
  env = 0.4f*r + 0.6f*env;
  bloquesEstado++;
  switch (estado) {
    case ARRANQUE:                       // 1 s para que se asienten los filtros
      if (bloquesEstado >= 20) cambiar(AVISO);
      break;
    case AVISO:
      if (bloquesEstado >= 40) cambiar(REPOSO);
      break;
    case REPOSO:
      if (bloquesEstado > 10) {          // ignora el primer 0.5 s
        nRep++; float d = env - mu; mu += d/nRep; m2 += d*(env - mu);
      }
      if (bloquesEstado >= 60) { sigma = sqrtf(m2 / max(nRep - 1, 1)); cambiar(CONTRACCION); }
      break;
    case CONTRACCION:
      if (env > mvc) mvc = env;
      if (bloquesEstado >= 60) finalizarCalibracion();
      break;
    case LISTO:
      detectar();
      break;
  }
}

void setup() {
  Serial.begin(921600);
  pinMode(PIN_BOTON, INPUT_PULLUP);
  analogReadResolution(12);
  analogSetPinAttenuation(PIN_EMG, ADC_11db);
  hp.config(1, 20, 0.7071f);
  lp.config(0, 400, 0.7071f);
  nt.config(2, 50, 30);
  tSig = micros();
}

void loop() {
  if (estado == LISTO && digitalRead(PIN_BOTON) == LOW) cambiar(AVISO);

  if ((int32_t)(micros() - tSig) < 0) return;
  tSig += TS_US;

  float x = analogRead(PIN_EMG);
  float y = lp.proc(hp.proc(x));
  if (USAR_NOTCH) y = nt.proc(y);

#if MODO_GRAFICO
  Serial.print((int)x); Serial.print(','); Serial.println(y);
  return;
#endif

  sumSq += y*y;
  if (++nMuestras < N_BLOQUE) return;
  float r = sqrtf(sumSq / N_BLOQUE);
  sumSq = 0; nMuestras = 0;
  procesarBloque(r);
}