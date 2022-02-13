#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define RST_PIN 9
#define SS_PIN 10
#define len_block 18
#define nr_block 2
#define ascii_1 49
#define ascii_0 48

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

boolean cod_card = false;
int blockNum = nr_block;
byte blockData [len_block];
byte readBlockData[len_block];
byte bufferLen = len_block;
int master_card1 = 0;
int master_card2 = 0;
int master_card3 = 0;
int master_card4 = 0;
int user_card1 = 0;
int user_card2 = 0;
int user_card3 = 0;
int user_card4 = 0;
char block_str[16] = "0000000000000000";

byte block[8]={
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
  };

byte frame[8]={
    B11111,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111,
  };

void setup()
{
  lcd.init();
  lcd.backlight();
  SPI.begin();
  lcd.createChar(0, block);
  lcd.createChar(1, frame);
  mfrc522.PCD_Init();
  eeprom_read_block(&master_card1,0,2);
  eeprom_read_block(&master_card2,2,2);
  eeprom_read_block(&master_card3,4,2);
  eeprom_read_block(&master_card4,6,2);
  
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
  WriteDataToBlock(2, "D000000000000000");
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
     
    if (readBlockData[0] == 'A') {master_card1 = 1; block_str[0] = 'N'; WriteDataToBlock(nr_block, block_str );}
    if (readBlockData[0] == 'B') {master_card2 = 1; block_str[0] = 'N'; WriteDataToBlock(nr_block, block_str );}
    if (readBlockData[0] == 'C') {master_card3 = 1; block_str[0] = 'N'; WriteDataToBlock(nr_block, block_str );}
    if (readBlockData[0] == 'D') {master_card4 = 1; block_str[0] = 'N'; WriteDataToBlock(nr_block, block_str );}
    if (readBlockData[0] == 'R') {
     master_card1 = 0;
     master_card2 = 0;
     master_card3 = 0;
     master_card4 = 0;
     block_str[0] = 'N';
     WriteDataToBlock(nr_block, block_str );
    }
   block_str[0] = '0';
  
   if (readBlockData[0] != 'A' && readBlockData[0] != 'B' && readBlockData[0] != 'C' && readBlockData[0] != 'D' && readBlockData[0] != 'R' && readBlockData[0] != 'N') {
      cod_card = true;
      if (readBlockData[1] == 'X' || master_card1 == 1) {user_card1 = 1; block_str[1] = 'X';}
      if (readBlockData[2] == 'X' || master_card2 == 1) {user_card2 = 1; block_str[2] = 'X';}
      if (readBlockData[3] == 'X' || master_card3 == 1) {user_card3 = 1; block_str[3] = 'X';}
      if (readBlockData[4] == 'X' || master_card4 == 1) {user_card4 = 1; block_str[4] = 'X';}
      WriteDataToBlock(nr_block, block_str );
      
      block_str[1] = '0';
      block_str[2] = '0';
      block_str[3] = '0';
      block_str[4] = '0';
   }
  
 
  eeprom_write_block(&master_card1,0,2);
  eeprom_write_block(&master_card2,2,2);
  eeprom_write_block(&master_card3,4,2);
  eeprom_write_block(&master_card4,6,2);

  delay(100);
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void main_screen()
{
  
  lcd.setCursor(0, 0);
  lcd.print("ABY ZAKODOWAC POLOZ ");
  lcd.setCursor(0, 1);
  lcd.print("KARTE NA KODERZE");
  lcd.setCursor(14, 2);
  lcd.print("1234");
  lcd.setCursor(0, 3);
  lcd.print("DOSTEPNE:");
  lcd.setCursor(14,3); 
  if (master_card1 == 1) {lcd.write(0);} else {lcd.write(1);}
  lcd.setCursor(15,3);
  if (master_card2 == 1) {lcd.write(0);} else {lcd.write(1);}
  lcd.setCursor(16,3);
  if (master_card3 == 1) {lcd.write(0);} else {lcd.write(1);}
  lcd.setCursor(17,3);
  if (master_card4 == 1) {lcd.write(0);} else {lcd.write(1);}
}

void load_screen()
{
  lcd.clear ();
  lcd.setCursor(0,0);
  lcd.print ("ZSYNCHRONIZOWANO");
  lcd.setCursor(0,1);
  lcd.print ("KARTE DOSTEPU");
  lcd.setCursor (14,2);
  lcd.print("1234");
  lcd.setCursor(0, 3);
  lcd.print("DOSTEPNE:");
  lcd.setCursor(14,3); 
  if (user_card1 == 1) {lcd.write(0);} else {lcd.write(1);}
  lcd.setCursor(15,3);
  if (user_card2 == 1) {lcd.write(0);} else {lcd.write(1);}
  lcd.setCursor(16,3);
  if (user_card3 == 1) {lcd.write(0);} else {lcd.write(1);}
  lcd.setCursor(17,3);
  if (user_card4 == 1) {lcd.write(0);} else {lcd.write(1);}

  delay(5000);
  cod_card = false;
  user_card1 = 0;
  user_card2 = 0;
  user_card3 = 0;
  user_card4 = 0;
}

void loop() {
  //DEF_CARD();
  RFID ();
  main_screen();
  if (cod_card == true){load_screen();}
}
