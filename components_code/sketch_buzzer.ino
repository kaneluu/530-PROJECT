#define BUZZER_PIN 9

void setup()
{
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop()
{
  buzz(BUZZER_PIN);
}

void buzz(int BUZZZ) {
  tone(BUZZZ, 1000);
  delay(100);
  noTone(BUZZZ);
  delay(100);
}
