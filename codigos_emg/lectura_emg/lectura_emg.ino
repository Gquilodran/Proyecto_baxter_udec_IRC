int emg_signal;
int voltage;

void setup() {
  Serial.begin(9600);
}

void loop() {
  emg_signal = analogRead(A0);
  //voltage = emg_signal*(5.0/1023.0);
  Serial.println(emg_signal);
  delay(50);
}