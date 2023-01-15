#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <stdlib.h>
#include <string.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
char words4[32][20] = {"aber", "adel", "affe", "ahoi", "fabrik", "jaguar", "kacken", "radeln", "nachbarn", "sachlage", "facebook", "babyphon", "dabehalten", "debatieren", "yachthafen", "rabenvater", "jackenkragen", "nachbarinsel", "tabakkonzern", "zackenbarsch", "tabakplantagen", "ultramarinblau", "abenteuerreise", "silvesterabend", "tourismusbranche", "mehrfamilienhaus", "liebesgeschichte", "jahrtausendwende", "schaubudenbesitzer", "sicherheitsabstand", "baumwollbekleidung", "hochleistungssport"};

// pins
int sAudioPin = 8;
int joy_x = 1; // analog 
int joy_y = 0; // analog
int joy_press = 34; // digital

// sounds
int NOTE_A5 = 880;
int NOTE_B5 = 988;
int NOTE_C5 = 523;
int NOTE_D5 = 587;
int NOTE_E5 = 659;
int NOTE_SUSTAIN = 70;

// vars
int wordLen = 2; // current word length (gets to 4 before game starts);
int sFirst = -1; // index of first selected else -1;
int correct = -1; // index of correct word in words
char current[18]; // current word on display (maxlength 18) 
int xstart = 0; // left padding before word starts (ui);
int cursor = xstart; // position of cursor
bool bon = false; // for blinking selected char
int winCount = 0; // amount Wins
int tipCount = 9; // amount Tips left

void setup() {
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  Serial.begin(9600);
  getWord();
}

void loop() {
  int x = analogRead(joy_x); // 500 mid; left -> 0; right -> 1000
  int y = analogRead(joy_y); // 500 mid; up -> 0; down -> 1000
  if (sFirst != -1) {
    if (bon) {
      bon = false;
      lcd.setCursor(sFirst + xstart, 1);
      lcd.print(" ");
    } else {
      bon = true;
      lcd.setCursor(sFirst + xstart, 1);
      lcd.print(current[sFirst]);
    }
  }
  if (x > 900) {
    walk(1);
  } else if (x < 100) {
    walk(-1);
  } else if (y < 100) {
    selectPosition();
    delay(300);
    if (isCorrect()) {
      printWin();
      getWord();
    }
  } else if (y > 900) {
    if (cursor > 7 && cursor < 12) {
      getTip();
      if (isCorrect()) {
        printWin();
        getWord();
      } else {
        delay(400);
      }
    } 
  }
  delay(100);
}



void printWin() {
  playWinSound();
  lcd.clear();
  char win[8] = "Gewonnen";
  int curs = 6;
  for (int i = 0; i < 8; i++) {
    lcd.setCursor(curs+i, 1);
    lcd.print(win[i]);
    delay(200);
  }

  if (winCount % 4 == 0) {
    int ccurs = curs - 1;
    char nr[10] = "Level Up!!"; 
    for (int i = 0; i < 10; i++) {
      lcd.setCursor(ccurs+i, 2);
      lcd.print(nr[i]);
      delay(200);
  }

  for (int i = 9; i >= 0; i--) {
      lcd.setCursor(ccurs+i, 2);
      lcd.print(" ");
      delay(200);
    }
  }
  
  for (int i = 7; i >= 0; i--) {
    lcd.setCursor(curs+i, 1);
    lcd.print(" ");
    delay(200);
  }

  delay(1000);
}

void selectPosition() {
  if (sFirst == -1) {
    sFirst = cursor - xstart;
    return;
  } 
  char f = current[sFirst];
  current[sFirst] = current[cursor-xstart];
  current[cursor-xstart] = f;
  lcd.setCursor(sFirst + xstart, 1);
  lcd.print(current[sFirst]);
  lcd.setCursor(cursor, 1);
  lcd.print(current[cursor-xstart]);
  sFirst = -1; 
}

void walk(int d) { // -1 l, 1 r
  if (d == -1) {
    if (cursor <= xstart) return;
    cursor -= 1;
    lcd.setCursor(cursor, 2);
    lcd.print("^ ");
  } else {
    if (cursor >= xstart + wordLen - 1) return;
    lcd.setCursor(cursor, 2);
    lcd.print(" ^");
    cursor += 1;
  }
}

void getWord() {
  if (winCount % 4 == 0) {
    wordLen += 2; 
  }
  correct++;
  strcpy(current, words4[correct]);
  xstart = 20 / 2 - wordLen / 2;
  cursor = xstart;
  printDefault();
  int minDiff = wordLen / 2;
  int diff = 0;
  while (diff <= minDiff) {
    diff = 0;
    shuffle(current, wordLen);
    for (int c = 0; c < wordLen; c++) {
      if (current[c] != words4[correct][c]) {
        diff++;
      }
    } 
  }
  for (int c = 0; c < wordLen; c++) {
      lcd.setCursor(c+xstart, 1);
      lcd.print(current[c]);
  }
}

bool isCorrect() {
  if (sFirst != -1) return false;
  for (int i = 0; i < wordLen; i++) {
    if (current[i] != words4[correct][i]) {
      return false;
    }
  }
  winCount++;
  return true;
}

void getTip() {
  if (tipCount < 0) return;
  tipCount--; 
  int s = 0;
  for (int i = 0; i < wordLen; i++) {
    if (current[i] != words4[correct][i]) {
      s = i;
      break;      
    }
  }
  int lastOcc = 0;
  for (int i = wordLen-1; i >= 0; i--) {
    if (current[i] == words4[correct][s]) {
      lastOcc = i;
      break;
    }
  }
  char temp = current[s];
  current[s] = current[lastOcc];
  current[lastOcc] = temp;
  lcd.setCursor(s + xstart, 1);
  lcd.print(current[s]);
  lcd.setCursor(lastOcc + xstart, 1);
  lcd.print(current[lastOcc]);
  lcd.setCursor(19, 3);
  char tips[2];
  sprintf(tips, "%d", tipCount);
  lcd.print(tips[0]); 
}

void printDefault() {
  lcd.clear();
  char wins[2];
  char score[] = "Score: ";
  int len = 8;
  if (winCount > 10) len++;
  sprintf(wins, "%d", winCount);
  strcat(score, wins);
  for (int i  = 0; i < len; i++) {
    lcd.setCursor(i+5, 0);
    lcd.print(score[i]);
  }
  lcd.setCursor(cursor, 2);
  lcd.print("^");

  char tips[2];
  sprintf(tips, "%d", tipCount);

  lcd.setCursor(8, 3);
  lcd.print("HELP");
  lcd.setCursor(19, 3);
  lcd.print(tips[0]);
}

void shuffle(char *array, size_t n)
{
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          char t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

void playWinSound() {
  for(uint8_t nLoop = 0;nLoop < 2;nLoop ++)
         {
           tone(sAudioPin,NOTE_A5);
           delay(NOTE_SUSTAIN);
           tone(sAudioPin,NOTE_B5);
           delay(NOTE_SUSTAIN);
           tone(sAudioPin,NOTE_C5);
           delay(NOTE_SUSTAIN);
           tone(sAudioPin,NOTE_B5);
           delay(NOTE_SUSTAIN);
           tone(sAudioPin,NOTE_C5);
           delay(NOTE_SUSTAIN);
           tone(sAudioPin,NOTE_D5);
           delay(NOTE_SUSTAIN);
           tone(sAudioPin,NOTE_C5);
           delay(NOTE_SUSTAIN);
           tone(sAudioPin,NOTE_D5);
           delay(NOTE_SUSTAIN);
           tone(sAudioPin,NOTE_E5);
           delay(NOTE_SUSTAIN);
           tone(sAudioPin,NOTE_D5);
           delay(NOTE_SUSTAIN);
           tone(sAudioPin,NOTE_E5);
           delay(NOTE_SUSTAIN);
           tone(sAudioPin,NOTE_E5);
           delay(NOTE_SUSTAIN);
         }
         noTone(sAudioPin);
}




