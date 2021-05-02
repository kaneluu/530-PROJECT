const int buzzer = 4; 
const int sensor = 2;

int state = 0; // 0 close - 1 open wwitch

void setup()
{
  pinMode(sensor, INPUT_PULLUP);
}

void loop()
{
  state = digitalRead(sensor);
  
  if (state == HIGH){
    tone(buzzer, 1000);
  }
  else{
    noTone(buzzer);
  }
  delay(200);
}
