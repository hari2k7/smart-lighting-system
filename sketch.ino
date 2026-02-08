
const int LDR_PIN          = 34;   // LDR analog input
const int POT_PIN          = 35;   // Potentiometer analog input
const int BUTTON_PIN       = 2;    // Manual override switch (active LOW)
const int POWER_SWITCH_PIN = 21;   // Main power switch (active LOW)
const int PIR_PIN          = 19;   // PIR motion sensor digital output

const int LED1_SENSOR      = 33;   // Light indicator (YELLOW)
const int LED2_POWER       = 5;    // Power status LED (GREEN)
const int LED3_MANUAL      = 4;    // Manual override LED (BLUE)


int ldrValue = 0;
int potValue = 0;

bool manualMode   = false;
bool systemOn     = true;
bool motionActive = false;

unsigned long lastMotionTime = 0;

// Auto-OFF if no motion for this long (in ms)
const unsigned long MOTION_TIMEOUT = 10000UL;  // 10 seconds

void setup() {
  Serial.begin(115200);

  pinMode(LDR_PIN, INPUT);
  pinMode(POT_PIN, INPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(POWER_SWITCH_PIN, INPUT_PULLUP);

  pinMode(PIR_PIN, INPUT);

  pinMode(LED1_SENSOR, OUTPUT);
  pinMode(LED2_POWER, OUTPUT);
  pinMode(LED3_MANUAL, OUTPUT);

  digitalWrite(LED1_SENSOR, LOW);
  digitalWrite(LED2_POWER, LOW);
  digitalWrite(LED3_MANUAL, LOW);

  Serial.println("Classroom Energy-Saver 2.0 Started...\n");
}

void loop() {

  systemOn = !digitalRead(POWER_SWITCH_PIN);   // LOW = switch ON → systemOn = true

  if (!systemOn) {
    digitalWrite(LED1_SENSOR, LOW);
    digitalWrite(LED2_POWER, LOW);
    digitalWrite(LED3_MANUAL, LOW);

    Serial.println("MAIN POWER OFF → System Sleeping");
    delay(200);
    return;
  }


  digitalWrite(LED2_POWER, HIGH);


  ldrValue   = analogRead(LDR_PIN);
  potValue   = analogRead(POT_PIN);
  manualMode = !digitalRead(BUTTON_PIN);  // LOW = pressed → manualMode = true

  int pirState = digitalRead(PIR_PIN);    // HIGH when motion detected

  if (pirState == HIGH) {
    motionActive   = true;
    lastMotionTime = millis();
  } else {
    // If no motion for a long time, consider room empty
    if (millis() - lastMotionTime > MOTION_TIMEOUT) {
      motionActive = false;
    }
  }

  bool isDark = (ldrValue > potValue);

  if (manualMode) {
    // force light ON
    digitalWrite(LED3_MANUAL, HIGH);  // Manual mode indicator
    digitalWrite(LED1_SENSOR, HIGH);  // Light ON
    Serial.println("MANUAL MODE → Teacher control: Light FORCED ON");
  } else {
    // Automatic mode
    digitalWrite(LED3_MANUAL, LOW);

    if (isDark && motionActive) {
      // Dark + People inside => turn light ON
      digitalWrite(LED1_SENSOR, HIGH);
      Serial.println("AUTO MODE → Dark + Motion: Light ON");
    } else {
      // Either bright OR no motion => light OFF
      digitalWrite(LED1_SENSOR, LOW);

      if (!isDark) {
        Serial.println("AUTO MODE → Bright room: Light OFF");
      } else if (!motionActive) {
        Serial.println("AUTO MODE → No motion for timeout: Light OFF");
      }
    }
  }


  Serial.print("LDR = ");
  Serial.print(ldrValue);
  Serial.print(" | POT = ");
  Serial.print(potValue);
  Serial.print(" | Dark = ");
  Serial.print(isDark ? "YES" : "NO");
  Serial.print(" | Manual = ");
  Serial.print(manualMode ? "YES" : "NO");
  Serial.print(" | Motion = ");
  Serial.print(motionActive ? "YES" : "NO");
  Serial.print(" | Power = ");
  Serial.println(systemOn ? "ON" : "OFF");

  delay(200);
}
