void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
}

void loop() {
  int debit=0;
  debit=analogRead(A0);
  Serial.print("Debit: ");
  delay(500);
  Serial.print(debit);
  Serial.print("\n");
}
