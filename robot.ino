// ════════════════════════════════════════════════════════════════
// OBSTACLE AVOIDING ROBOT — CLEAN PIN VERSION (extension board)
// No shared pins between LEDs and motor driver
// ════════════════════════════════════════════════════════════════

// ── Sensor
const int TRIG  = 32;
const int ECHO  = 33;

// ── LEDs + Buzzer (unchanged)
const int LED_G = 13;
const int LED_Y = 12;
const int LED_R = 14;
const int BUZ   = 27;

// ── L293D — now all dedicated pins, no sharing
const int EN1 = 26;   // left motor ENABLE
const int IN1 = 25;   // left motor direction A
const int IN2 = 19;   // left motor direction B   ← NEW
const int EN2 = 18;   // right motor ENABLE        ← NEW
const int IN3 = 4;    // right motor direction A   ← NEW
const int IN4 = 2;    // right motor direction B   ← NEW

// ── Thresholds
const float DIST_CLEAR    = 40.0;
const float DIST_WARNING  = 20.0;
const float DIST_CRITICAL = 10.0;
const int   TURN_TIME     = 650;

// ════════════════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  int outputs[] = {LED_G, LED_Y, LED_R, BUZ,
                   EN1, IN1, IN2, EN2, IN3, IN4};
  for (int p : outputs) pinMode(p, OUTPUT);

  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);

  // Startup self-test
  Serial.println("=== ROBOT STARTING ===");
  digitalWrite(LED_G, HIGH); delay(250); digitalWrite(LED_G, LOW);
  digitalWrite(LED_Y, HIGH); delay(250); digitalWrite(LED_Y, LOW);
  digitalWrite(LED_R, HIGH); delay(250); digitalWrite(LED_R, LOW);
  digitalWrite(BUZ,   HIGH); delay(100); digitalWrite(BUZ,   LOW);
  Serial.println("All good. Flip switch ON to run.");
}

// ════════════════════════════════════════════════════════════════
float getDistance() {
  digitalWrite(TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long dur = pulseIn(ECHO, HIGH, 30000);
  if (dur == 0) return 999;
  return (dur * 0.0343) / 2.0;
}

void allOff() {
  digitalWrite(LED_G, LOW); digitalWrite(LED_Y, LOW);
  digitalWrite(LED_R, LOW); digitalWrite(BUZ,   LOW);
  digitalWrite(IN1,   LOW); digitalWrite(IN2,   LOW);
  digitalWrite(IN3,   LOW); digitalWrite(IN4,   LOW);
}

void goForward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void goStop() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

void turnRight() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}

void turnLeft() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

// ════════════════════════════════════════════════════════════════
void loop() {
  allOff();
  float dist = getDistance();

  Serial.print("Distance: ");
  Serial.print(dist, 1);
  Serial.print(" cm  ->  ");

  if (dist > DIST_CLEAR) {
    digitalWrite(LED_G, HIGH);
    goForward();
    Serial.println("SAFE - forward");

  } else if (dist > DIST_WARNING) {
    digitalWrite(LED_Y, HIGH);
    goForward();
    Serial.println("WARNING - obstacle ahead");

  } else {
    digitalWrite(LED_R, HIGH);
    digitalWrite(BUZ,   HIGH);
    goStop();
    Serial.println("CRITICAL - stopping!");
    delay(500);
    digitalWrite(BUZ, LOW);
    Serial.println("  -> turning to avoid");
    turnRight();
    delay(TURN_TIME);
  }

  delay(80);
}