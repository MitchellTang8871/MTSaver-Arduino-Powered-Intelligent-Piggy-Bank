#include <LiquidCrystal_I2C.h>
#define NOTE_C4 262
#define NOTE_G3 196
#define NOTE_A3 220
#define NOTE_B3 247
#define NOTE_A4 440
int seconds = 0;
int ir1 = 5;
int ir2 = 4;
int ir3 = 3;
int led1 = 11;
int led2 = 12;
int led3 = 13;
int buzzer = 8;
int button = 2;
int trigPin = 10;
int echoPin = 9;
int ldrPin = A0;
int isObstacle1 = HIGH;
int isObstacle2 = HIGH;
int isObstacle3 = HIGH;
float saving = 0;
int previousSaving = 0;
int melody[] = { NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4 };
int ledMelody[] = { 11, 12, 13, 11, 12, 0, 13, 0 };
int noteDurations[] = { 4, 8, 8, 4, 4, 4, 4, 4 };
int backlightState = 1;
int pot = A1;
int threshold = 512;
int buzzerStatus = 1;
int storage=300;
int ldrStatus=0;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);
  pinMode(ir3, INPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ldrPin, INPUT);
  lcd.backlight();
  lcd.display();
  Serial.begin(9600);
  Serial.println("Welcome");
}

void loop() {
  //For lcd light
  buttonCheck();

  //For buzzer on/off
  potCheck();

  //For ldr alarm
  ldrCheck();

  //For coin counter
  irCheck();

  //For status checking
  printStatus();

  // If saved another Ringgit
  if (saving - previousSaving >= 1) {
    lcd.clear();
    lcd.print("Nice, another");
    lcd.setCursor(0, 1);
    lcd.print("1 Ringgit");
    previousSaving = saving;
    music();
    lcd.clear();

    //check storage
    udCheck();
  }
}

void udCheck(){
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  int duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  storage = duration*0.034/2;
}

void irCheck(){
  isObstacle1 = digitalRead(ir1);
  isObstacle2 = digitalRead(ir2);
  isObstacle3 = digitalRead(ir3);

  if (isObstacle1 == LOW) {
    insertSound();
    lcd.clear();
    lcd.print("Nah just 10 sen");
    saving += 0.1;
    for (int a = 1; a <= 2; a++) {
      digitalWrite(led1, HIGH);  // Turn the LED on
      delay(500);
      digitalWrite(led1, LOW);  // Turn the LED off
    }
    lcd.clear();
  } else if (isObstacle2 == LOW) {
    insertSound();
    lcd.clear();
    lcd.print("Good job,");
    lcd.setCursor(0, 1);
    lcd.print("thats 20 sen");
    saving += 0.2;
    for (int a = 1; a <= 2; a++) {
      digitalWrite(led2, HIGH);  // Turn the LED on
      delay(500);
      digitalWrite(led2, LOW);  // Turn the LED off
    }
    lcd.clear();
  } else if (isObstacle3 == LOW) {
    insertSound();
    lcd.clear();
    lcd.print("Excellent you");
    lcd.setCursor(0, 1);
    lcd.print("saved 50 sen");
    saving += 0.5;
    for (int a = 1; a <= 3; a++) {
      digitalWrite(led3, HIGH);  // Turn the LED on
      delay(500);
      digitalWrite(led3, LOW);  // Turn the LED off
    }
    lcd.clear();
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Saving = ");
    lcd.print(saving);
    lcd.setCursor(0,1);
    lcd.print("Storage: ");
    if (storage <= 5){
      lcd.print("Full");
    } else if (storage <= 10){
      lcd.print("Half");
    } else{
      lcd.print("Empty");
    }
  }
}

void ldrCheck(){
  ldrStatus = analogRead(ldrPin);
  if (ldrStatus > 200) {
    danger();
    delay(2000);
  }
}

void potCheck(){
  int potValue = analogRead(pot);
  if (potValue > threshold) {
    buzzerStatus = 1;  // Turn the buzzer on
  } else {
    buzzerStatus = 0;   // Turn the buzzer off
  }
}

void buttonCheck(){
  if (digitalRead(button) == 0) {  // Check if button is pressed
    toggleBacklight();             // Toggle the backlight
  }
}

void printStatus(){
  Serial.print(isObstacle1);
  Serial.print(", ");
  Serial.print(isObstacle2);
  Serial.print(", ");
  Serial.print(isObstacle3);
  Serial.print(", ");
  Serial.print(buzzerStatus);
  Serial.print(", ");
  Serial.print(backlightState);
  Serial.print(", ");
  Serial.print(storage);
  Serial.print(", ");
  Serial.println(ldrStatus);
}

void music() {
  // Iterate over the notes of the melody
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    // Calculate the note duration, take one second. Divided by the note type
    int noteDuration = 1000 / noteDurations[thisNote];
    if (buzzerStatus == 1) {
      tone(buzzer, melody[thisNote], noteDuration);
    }

    // Distinguish the notes, set a minimum time between them
    // The note's duration +30% seems to work well
    int pauseBetweenNotes = noteDuration * 1.30;
    
    // Blink the LED
    if (thisNote < 7) {
      digitalWrite(ledMelody[thisNote], HIGH);  // Turn the corresponding LED on
    } else {
      // Last Node All LEDs blink together
      for (int i = 11; i <= 13; i++) {
        digitalWrite(i, HIGH);
      }
    }
    delay(pauseBetweenNotes);

    // Stop the tone playing
    noTone(buzzer);

    for (int i = 11; i <= 13; i++) {
      digitalWrite(i, LOW);
    }
  }
}

void insertSound() {
  if (buzzerStatus == 1) {
    tone(buzzer, NOTE_A4, 80);
  }
}

void toggleBacklight() {
  if (backlightState == 1) {  // Set backlight according to the state
    lcd.noBacklight();
    backlightState = 0;
  } else {
    lcd.backlight();
    backlightState = 1;
  }
}

void danger() {
  while (true) {
    lcd.clear();
    lcd.print("ALERT: Danger!");
    lcd.setCursor(0, 1);
    lcd.print("Please check.");
  
    // Play warning sound on the buzzer
    for (int i = 0; i < 3; i++) {
      tone(buzzer, NOTE_A4, 200);
      delay(250);
      noTone(buzzer);
      delay(250);
    }
    
    // Check for a button press to exit the loop
    if (digitalRead(button) == 0) {
      lcd.clear();
      break; // Exit the loop if the button is pressed
    }
  }
}