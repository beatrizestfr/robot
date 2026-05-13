// ── All 3 LEDs + Ultrasonic — full visual state machine ──────────
const int TRIG      = 32;
const int ECHO      = 33;
const int LED_GREEN  = 13;
const int LED_YELLOW = 12;
const int LED_RED    = 14;
const int BUZZER     = 27;


void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LED_GREEN,  OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED,    OUTPUT);
  pinMode(BUZZER,     OUTPUT);

}
  
float getDistance() {
  digitalWrite(TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  return (pulseIn(ECHO, HIGH, 30000) * 0.0343) / 2.0;
}void allOff() {
  digitalWrite(LED_GREEN,  LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED,    LOW);
  digitalWrite(BUZZER,     LOW);

}

void loop() {
  float dist = getDistance();
  allOff();  

  Serial.print("Dist: "); Serial.print(dist); Serial.print(" cm  State: ");

  if (dist > 40) {
    digitalWrite(LED_GREEN, HIGH);
    Serial.println("SAFE ");
  } else if (dist > 20) {
    digitalWrite(LED_YELLOW, HIGH);
    Serial.println("WARNING ");
  } else {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(BUZZER,  HIGH);

    Serial.println("CRITICAL ");
  }
  delay(300);
}