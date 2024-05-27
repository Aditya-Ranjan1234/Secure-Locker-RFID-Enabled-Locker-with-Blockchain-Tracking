#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10
#define RELAY_PIN 3
#define GREEN_LED_PIN 2
#define RED_LED_PIN 4
#define BUZZER_PIN 5

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Define the desired RFID card UID
byte desiredUID[] = {0x49, 0x34, 0x95, 0x0F};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  resetComponents();  // Initialize components to initial states

  Serial.println("RFID Ready");
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.print("Card UID: ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
    }

    if (compareUID(mfrc522.uid.uidByte, desiredUID, mfrc522.uid.size)) {
      Serial.println("\nAccess granted - Turning on the RELAY, GREEN LED, and BUZZER");

      digitalWrite(RELAY_PIN, LOW);  // Turn on the RELAY (lock position)
      digitalWrite(GREEN_LED_PIN, HIGH);  // Turn on the GREEN LED
      tone(BUZZER_PIN, 1000, 200);  // Play a sound for 200 milliseconds

      delay(3000);  // Keep the components on for 3 seconds

      digitalWrite(RELAY_PIN, HIGH);  // Turn off the RELAY (unlock position)
      digitalWrite(GREEN_LED_PIN, LOW);  // Turn off the GREEN LED
      noTone(BUZZER_PIN);  // Turn off the BUZZER

      Serial.println("Access complete");
      resetArduino();  // Reset the Arduino after each run
    } else {
      Serial.println("\nAccess denied - Turning on the RED LED and a different sound with the BUZZER");

      digitalWrite(RED_LED_PIN, HIGH);  // Turn on the RED LED
      tone(BUZZER_PIN, 2000, 1500);  // Play a different sound for 200 milliseconds

      delay(3000);  // Keep the RED LED and BUZZER on for 3 seconds

      digitalWrite(RED_LED_PIN, LOW);  // Turn off the RED LED
      noTone(BUZZER_PIN);  // Turn off the BUZZER

      Serial.println("Access denied");
      resetArduino();  // Reset the Arduino after each run
    }

    mfrc522.PICC_HaltA();
    delay(1000);  // Adjust the delay as needed
  }
}

bool compareUID(byte* UID1, byte* UID2, byte size) {
  for (byte i = 0; i < size; i++) {
    if (UID1[i] != UID2[i]) {
      return false;
    }
  }
  return true;
}

void resetComponents() {
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  noTone(BUZZER_PIN);
}

void resetArduino() {
  asm volatile ("  jmp 0");
}
