// Code for testing wireless transmission of signals from 
// one channel

// input signal goes to pin A6
int input = A6;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // read voltage signal and map to mV
  double mV = map(analogRead(input), 0, 1023, 0, 5000);

  // print to serial monitor
  Serial.println(mV);

  // delay 20ms
  delay(20);
}
