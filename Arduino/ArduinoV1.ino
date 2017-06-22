int baud = 9600;
unsigned long EndClock = 1999;
unsigned long TemponClock;
unsigned long StartClock;
int TimeGame = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(baud);
  StartClock = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  TemponClock = millis();
  if(TemponClock - StartClock > EndClock){
    TimeGame++;
    Serial.print("0/Soleil/1/Soleil/");
    Serial.println(TimeGame);
    StartClock = millis();
  } 
}

