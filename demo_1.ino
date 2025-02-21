#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <deprecated.h>
#include <require_cpp11.h>
#include <SPI.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <string.h>



#define cols 4
#define rows 4
#define lock 2
#define SS_PIN 5
#define RST_PIN 4

LiquidCrystal_I2C lcd(0x27, 20, 4);
MFRC522 rfid(SS_PIN , RST_PIN);

typedef enum{
  MODE_ID_RFID_ADD,
  MODE_ID_RFID_FIRST,
  MODE_ID_RFID_SECOND,
}MODE_ID_RFID_STATE;

MODE_ID_RFID_STATE MODE_RFID = MODE_ID_RFID_ADD;

char password[6] = "22222";
char pass_def[6] = "12345";
char mode_changePass[6] = "*#01#";
char mode_resetPass[6] = "*#02#";
char mode_hardReset[6] = "*#03#";
char mode_addRFID[6] = "*101#";
char mode_delRFID[6] = "*102#";
char mode_delAllRFID[6] = "*103#";


char data_input[6];
char new_pass1[6];
char new_pass2[6];
unsigned char in_num = 0, error_pass = 0, isMode = 0 , id_rf = 0;
int index_tt;


char hexakeys[rows][cols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[rows] = {32, 33, 25, 26};
byte colPins[cols] = {27, 14, 12, 13};

Keypad keypad = Keypad(makeKeymap(hexakeys), rowPins, colPins, rows, cols);

void eepromWrite(char data[])
{
  for(int i= 0 ; i<5 ; i++)
  {
    EEPROM.write(i,data[i]);
  }
  EEPROM.commit();
}

void eepromRead()
{
  for(int i=0;i<5;i++)
  {
    password[i] = EEPROM.read(i);
  }
}

bool check_data_input(char data[])
{
  for(int i=0;i<5;i++)
  {
    if(data[i] == '\0')
    {
      return false;
    }
  }
  return true;
}

bool compare_data(char data1[] , char data2[])
{
  for(int i=0;i<5;i++)
  {
    if(data1[i] != data2[i])
    {
      return false;
    }
  }
  return true;
}

void copy_data(char data1[] , char data2[]) // gan chuoi 2 vao chuoi 1
{
  for(int i=0;i<5;i++)
  {
    data1[i] = data2[i];
  }
}

void clear_data_input()
{
  for(int i=0;i<6;i++)
  {
    data_input[i] = '\0';
  }
}

void getData()
{
  char key = keypad.getKey();
  if(key)
  {
    delay(100);
    if(in_num == 0)
    {
      data_input[0] = key;
      lcd.setCursor(4,1);
      lcd.print(data_input[0]);
      delay(200);
      lcd.setCursor(4,1);
      lcd.print("*");
    }
    if(in_num == 1)
    {
      data_input[1] = key;
      lcd.setCursor(5,1);
      lcd.print(data_input[1]);
      delay(200);
      lcd.setCursor(5,1);
      lcd.print("*");
    }
    if(in_num == 2)
    {
      data_input[2] = key;
      lcd.setCursor(6,1);
      lcd.print(data_input[2]);
      delay(200);
      lcd.setCursor(6,1);
      lcd.print("*");
    }
    if(in_num == 3)
    {
      data_input[3] = key;
      lcd.setCursor(7,1);
      lcd.print(data_input[3]);
      delay(200);
      lcd.setCursor(7,1);
      lcd.print("*");
    }
    if(in_num == 4)
    {
      data_input[4] = key;
      lcd.setCursor(8,1);
      lcd.print(data_input[4]);
      delay(200);
      lcd.setCursor(8,1);
      lcd.print("*");
    }
    if(in_num == 4)
    {
      in_num = 0;
    }
    else{
      in_num++;
    }
  }
}

void checkPass()
{
  getData();
  if(check_data_input(data_input))
  {
    delay(200);
    if(compare_data(data_input , password))
    {
      index_tt = 1;
      lcd.clear();
      clear_data_input();
    }
    else if(compare_data(data_input , mode_changePass))
    {
      index_tt = 2;
      lcd.clear();
      clear_data_input();
    }
    else if(compare_data(data_input , mode_resetPass))
    {
      index_tt = 3;
      lcd.clear();
      clear_data_input();
    }
    else if(compare_data(data_input , mode_addRFID))
    {
      index_tt = 5;
      lcd.clear();
      clear_data_input();
    }
    else if(compare_data(data_input , mode_delRFID))
    {
      index_tt = 6;
      lcd.clear();
      clear_data_input();
    }
    else if(compare_data(data_input , mode_delAllRFID))
    {
      index_tt = 7;
      lcd.clear();
      clear_data_input();
    }
    else
    {
      if(error_pass == 2)
      {
        index_tt = 4;
      }
      error_pass++;
      lcd.setCursor(0,0);
      lcd.print("WRONG PASSWORD");
      delay(1000);
      lcd.clear();
      clear_data_input();
    }
  }
}

void opendoor()
{
  lcd.setCursor(0,0);
  lcd.print("-open the door-");
  delay(1000);
  digitalWrite(lock , HIGH);
  delay(5000);
  digitalWrite(lock , LOW);
  lcd.clear();
  index_tt = 0;
}

void ChangePass()
{
  lcd.setCursor(0,0);
  lcd.print("--CHANGE PASS--");
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("---- START ----");
  delay(1000);
  lcd.clear();

  while(1)
  {
    lcd.setCursor(0, 0);
    lcd.print("NEW PASSWORD");
    getData();
    if(check_data_input(data_input))
    {
      copy_data(new_pass1 , data_input);
      clear_data_input();
      break;
    }
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("---- AGAIN ----");
  while(1)
  {
    getData();
    if(check_data_input(data_input))
    {
      copy_data(new_pass2 , data_input);
      clear_data_input();
      break;
    }
  }
  delay(1000);

  if(compare_data(new_pass1 , new_pass2))
  {
    lcd.clear();
    copy_data(password , new_pass2);
    eepromWrite(password);
    lcd.setCursor(0,0);
    lcd.print("--- SUCCESS ---");
    delay(1000);
    lcd.clear();
    index_tt = 0;
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("---- WRONG ----");
    delay(1000);
    lcd.clear();
  }
}

void ResetPass()
{
  lcd.setCursor(0,0);
  lcd.print("->PASS TO RESET");

  unsigned char choose = 0;
  getData();
  if(check_data_input(data_input))
  {
    delay(200);
    if(compare_data(data_input , password))
    {
      lcd.clear();
      clear_data_input();
      while(1)
      {
        lcd.setCursor(0,0);
        lcd.print("--RESET PASS--");
        char key = keypad.getKey();
        if(choose == 0)
        {
          lcd.setCursor(0,1);
          lcd.print(">");
          lcd.setCursor(1,1);
          lcd.print("YES");
          lcd.setCursor(6,1);
          lcd.print(" NO");
        }
        else if(choose == 1)
        {
          lcd.setCursor(0,1);
          lcd.print(" YES");
          lcd.setCursor(6,1);
          lcd.print(">");
          lcd.setCursor(7,1);
          lcd.print("NO");
        }
        if(key == '*')
        {
          if(choose == 1) choose = 0;
          else choose++;
        }
        if(key == '#' && choose == 0)
        {
          lcd.clear();
          copy_data(password , pass_def);
          eepromWrite(pass_def);
          lcd.setCursor(0,0);
          lcd.print("---RESET OK---");
          delay(1000);
          lcd.clear();
          index_tt = 0;
          break;
        }
        else if(key == '#' && choose == 1)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("-RESET NOT OK-");
          delay(1000);
          lcd.clear();
          index_tt = 0;
          break;
        }
      }
    }else
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("---- WRONG ----");
      clear_data_input();
      delay(1000);
      lcd.clear();
    }
  }
}


void errorPass()
{
  lcd.setCursor(0,0);
  lcd.print("-WRONG 3 TIMES-");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("-WAIT A MINUTE-");
  delay(1000);
  lcd.clear();
  for(int i = 59 ; i >=0 ; i--)
  {
    lcd.setCursor(0,0);
    lcd.printf("WAIT: %.2d",i);
    delay(500);
  }
  lcd.clear();
  index_tt = 0;
}

unsigned char numberInput()
{
  unsigned char count = 0;
  unsigned char number[3];
  while(count < 2)
  {
    char key = keypad.getKey();
    if(key && key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '*' && key != '#')
    {
      number[count] = key;
      lcd.setCursor(10 + count , 1); lcd.print(key);
      count++;
    }
  }
  return (number[0] - '0') * 10 + number[1] - '0';
}

bool check_id_rfid(byte tag[] , unsigned char id)
{
  byte check = 0;
  if(rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
  {
    for(int i=0;i<4;i++)
    {
      tag[i] = rfid.uid.uidByte[i];
    }

    for(int i=0;i<4;i++)
    {
      if(tag[i] != EEPROM.read(10 + id * 4 + i))
      {
        check = 1;
      }
    }
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
  if(check) return false;
  return true;
}

bool check_rfid_in_eeprom(unsigned char tag[])
{
  unsigned char count = 0;
  for(int i= 10 ; i < 512; i+=4)
  {
    for(int j=0;j<4;j++)
    {
      if(tag[j] == EEPROM.read(i+j))
      {
        count++;
      }
    }
    if(count == 4)
    {
      return true;
    }
  }
  return false;
}

void checkRfid()
{
  if(rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
  {
    unsigned char tag[4];
    Serial.print("UID: ");
    for(int i=0;i<4;i++)
    {
      tag[i] = rfid.uid.uidByte[i];
      Serial.print(tag[i] , HEX);
      Serial.printf(" ");
    }

    if(check_rfid_in_eeprom(tag))
    {
      index_tt = 1;
    }
    else{
      if(error_pass == 2)
      {
        index_tt = 4;
      }
      error_pass++;
      lcd.setCursor(0,3);
      lcd.print("-----WRONG RFID-----");
      delay(1000);
      lcd.clear();
    }
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

void addRfid()
{
  byte count = 0;
  byte rfidtag[4];
  lcd.setCursor(0,0);
  lcd.print("---ADD NEW RFID---");
  switch(MODE_RFID)
  {
    case MODE_ID_RFID_ADD:
    {
      lcd.setCursor(0,1);
      lcd.print("INPUT ID: ");
      id_rf = numberInput();
      delay(1000);
      if(id_rf == 0)
      {
        lcd.setCursor(0,2);
        lcd.print("     ID ERROR      ");
        delay(1000);
        lcd.clear();
      }
      else{
        MODE_RFID = MODE_ID_RFID_FIRST;
      }
    }
    break;
    case MODE_ID_RFID_FIRST:
    {
      lcd.setCursor(0,1);
      lcd.print("     PUT RFID     ");
      if(rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
      {
        for(int i=0;i<4;i++)
        {
          rfidtag[i] = rfid.uid.uidByte[i];
        }
        if(check_rfid_in_eeprom(rfidtag))
        {
          lcd.setCursor(0,2);
          lcd.print("tag added before");
          index_tt = 0;
          delay(2000);
          lcd.clear();
          MODE_RFID = MODE_ID_RFID_ADD;
        }
        else{
          MODE_RFID = MODE_ID_RFID_SECOND;
        }
          rfid.PICC_HaltA();
          rfid.PCD_StopCrypto1();
      }
    }
    break;
    case MODE_ID_RFID_SECOND:
    {
      lcd.setCursor(0,1);
      lcd.print("     PUT AGAIN     ");
      if(rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
      {
        byte tag[4];
        for(int i=0;i<4;i++)
        {
          tag[i] = rfid.uid.uidByte[i];
        }
        //if(check_id_rfid(rfidtag,id_rf) == false)
        //{
          for(int i=0;i<4;i++)
          {
            if(tag[i] == rfidtag[i])
            {
              count++;
            }
          }
       // }
        if(count == 4)
        {
          for(int i=0;i<4;i++)
          {
            EEPROM.write(10 + id_rf * 4 + i , tag[i]);
            EEPROM.commit();
          }
          lcd.setCursor(0,1);
          lcd.print("      SUCCESS      ");
          index_tt = 0;
          delay(2000);
          lcd.clear();
          MODE_RFID = MODE_ID_RFID_ADD;
          rfid.PICC_HaltA();
          rfid.PCD_StopCrypto1();
        }
        else{
          lcd.setCursor(0,1);
          lcd.print("       WRONG       ");
          index_tt = 0;
          delay(2000);
          lcd.clear();
          MODE_RFID = MODE_ID_RFID_ADD;
          rfid.PICC_HaltA();
          rfid.PCD_StopCrypto1();
        }
      }
    }
    break;
  }
}
void delRfid()
{
  byte flag=0;
  byte count = 0;
  byte tag[4];
  lcd.setCursor(0,0);
  lcd.print("START DELETE RFID");
  lcd.setCursor(0,1);
  lcd.print("INPUT ID: ");
  id_rf = numberInput();
  delay(1000);
  if(id_rf == 0)
  {
    lcd.setCursor(0,2);
    lcd.print("     ID ERROR      ");
    delay(1000);
    index_tt = 0;
    lcd.clear();
  }
  else{
    lcd.setCursor(0,2);
    lcd.print("PUT TAG NEED DELETE");
    while(1)
    {
      if(rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
      {
        for(int i=0;i<4;i++)
        {
          tag[i] = rfid.uid.uidByte[i];
        }
        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
        break;
      }
    }
    for(int j=0;j<4;j++)
    {
      if(tag[j] == EEPROM.read(10 + id_rf * 4 + j))
      {
        count++;
      }
    }
    if(count == 4)
    {
      for(int i=0;i<4;i++)
      {
        EEPROM.write(10 + id_rf * 4 + i, '\0');
        EEPROM.commit();
      }
      lcd.setCursor(0,3);
      lcd.print("    DELETED TAG    ");
      delay(1000);
      lcd.clear();
      index_tt = 0;
    }
    else{
      lcd.setCursor(0,3);
      lcd.print("    DELETE FAIL    ");
      delay(1000);
      lcd.clear();
      index_tt = 0;
    }
    count = 0;
  }
}

void delAllRfid()
{
  unsigned char choose = 0;
  lcd.setCursor(0,0);
  lcd.print("  DELETE ALL RFID? ");
  while(1)
  {
    char key = keypad.getKey();
    if(choose == 0)
    {
      lcd.setCursor(0,1);
      lcd.print("   >YES       NO   ");
    }
    else if(choose == 1)
    {
      lcd.setCursor(0,1);
      lcd.print("    YES      >NO   ");
    }
    if(key == '*')
    {
      if(choose == 1)
      {
        choose = 0;
      }
      else{
        choose = 1;
      }
    }
    if(key == '#' && choose == 0)
    {
      for(int i=10;i<512;i+=4)
      {
        EEPROM.write(i,'\0');
        EEPROM.commit();
      }
      lcd.setCursor(0,2);
      lcd.print("DELETED ALL SUCCESS");
      delay(1000);
      lcd.clear();
      index_tt = 0;
      break;
    }
    if(key == '#' && choose == 1)
    {
      lcd.setCursor(0,2);
      lcd.print(" DELETED ALL FAIL ");
      delay(1000);
      lcd.clear();
      index_tt = 0;
      break;
    }
  }
}

void setup() {
  lcd.init();
  lcd.backlight();
  SPI.begin();
  rfid.PCD_Init();
  EEPROM.begin(512);
  eepromRead();
  pinMode(lock , OUTPUT);
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("ENTER PASSWORD");
  checkPass();
  checkRfid();
  while(index_tt == 1)
  {
    opendoor();
  }

  while(index_tt == 2)
  {
    ChangePass();
  }

  while(index_tt == 3)
  {
    ResetPass();
  }

  while(index_tt == 4)
  {
    errorPass();
    error_pass = 0;
  }

  while(index_tt == 5)
  {
    addRfid();
  }

  while(index_tt == 6)
  {
    delRfid();
  }

  while(index_tt == 7)
  {
    delAllRfid();
  }

}
