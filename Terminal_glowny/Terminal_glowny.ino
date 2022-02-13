#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define RST_PIN 9
#define SS_PIN 10
#define len_block 18
#define nr_block 2
#define ascii_1 49

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

int blockNum = nr_block;
byte blockData [len_block];
byte readBlockData[len_block];
byte bufferLen = len_block;
int master_card1 = 0;
int master_card2 = 0;
int master_card3 = 0;
int master_card4 = 0;

void setup()
{
  lcd.init();
  lcd.backlight();
  SPI.begin();
  mfrc522.PCD_Init();
}

void ReadDataFromBlock(int blockNum, byte readBlockData[])
{
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
}

void WriteDataToBlock(int blockNum, byte blockData[])
{
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  status = mfrc522.MIFARE_Write(blockNum, blockData, 16);
}
void DEF_CARD()
{
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  ReadDataFromBlock(blockNum, readBlockData);
  WriteDataToBlock(2, "100001              ");
}

void RFID ()
{
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  ReadDataFromBlock(blockNum, readBlockData);

  if (readBlockData[0] == ascii_1) {
    if (readBlockData[5] != ascii_1) {
      if (readBlockData[1] == ascii_1) {
        master_card1 = 1;
      }
      if (readBlockData[2] == ascii_1) {
        master_card2 = 1;
      }
      if (readBlockData[3] == ascii_1) {
        master_card3 = 1;
      }
      if (readBlockData[4] == ascii_1) {
        master_card4 = 1;
      }
    }
    if (readBlockData[5] == ascii_1) {
      lcd.clear();
      master_card1 = 0;
      master_card2 = 0;
      master_card3 = 0;
      master_card4 = 0;
    }
  }
  delay(100);
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
void screen()
{
  lcd.setCursor(0, 0);
  lcd.print("KODUJ KARTE DOSTEPU");
  lcd.setCursor(0, 1);
  lcd.print("SKALIBROWANE");
  lcd.setCursor(0, 2);
  lcd.print("FRAGMENTY KODU");
  if (master_card1 == 1) {
    lcd.setCursor(0, 3);
    lcd.print("XXXXX");
  }
  if (master_card2 == 1) {
    lcd.setCursor(5, 3);
    lcd.print("XXXXX");
  }
  if (master_card3 == 1) {
    lcd.setCursor(10, 3);
    lcd.print("XXXXX");
  }
  if (master_card4 == 1) {
    lcd.setCursor(15, 3);
    lcd.print("XXXXX");
  }
}
void loop() {
  //DEF_CARD();
  RFID ();
  screen();
}
