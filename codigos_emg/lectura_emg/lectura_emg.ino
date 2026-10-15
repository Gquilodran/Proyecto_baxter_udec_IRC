int emg_signal=0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  emg_signal = analogRead(A0);
  Serial.println(emg_signal);
  delay(100);
}



