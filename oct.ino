#include <FirmataDefines.h>
#include <FirmataConstants.h>
#include <Boards.h>
#include <Firmata.h>
#include <FirmataMarshaller.h>
#include <FirmataParser.h>

int buttonPin = 13;
int old_button ;
int pushed = 0;
int level = 7;
int select = 1;

int data = 0;
int index[40]; // ------------------------------------------ [문제 없음]

// ---------------------------------------------------------- [anode]
// RED
int redData = 9;
int redLatch = 10;
int redClock = 11;

//GREEN
int greenData = A3;  // DATA
int greenLatch = A4; // STCP
int greenClock = A5; // SHCP

//BLUE
int blueData = A0;
int blueLatch = A1;
int blueClock = A2;

int shiftData[3] = { redData, greenData, blueData };
int shiftLatch[3] = { redLatch, greenLatch, blueLatch };
int shiftClock[3] = { redClock, greenClock, blueClock };

// --------------------------------------------------------- [cathode]
int layerPin[6] = { 2, 3, 4, 5, 6, 7 };

byte grid[5];
boolean play = false;

// 5*5 이미지
// byte 1:off  0:On

byte glow[5] = {
  B11100000,
  B11100000,
  B11100000,
  B11100000,
  B11100000
};

byte pat[] = {
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,

  B00001,
  B11111,
  B11111,
  B11111,
  B11111,

  B11111,
  B10001,
  B11101,
  B11111,
  B11111,

  B11111,
  B01111,
  B00011,
  B01111,
  B11111,

  B11110,
  B11110,
  B11110,
  B11110,
  B11111,

  B11111,
  B11111,
  B11111,
  B10011,
  B00111,

  B11111,
  B11111,
  B10001,
  B11101,
  B11111,

  B11111,
  B11111,
  B11111,
  B11111,
  B11000
};

void setup() {
  Serial.begin(9600); // --------------------------------- 시리얼 통신 초기화

  pinMode(buttonPin, INPUT); // --------------------------- PUSH BUTTON

  for (int i = 0; i < 6; i++) {
    pinMode(layerPin[i], OUTPUT); // ---------------------- 각 층별 핀 설정
  }

  for (int i = 0; i < 3; i++) {
    pinMode(shiftData[i], OUTPUT);
    pinMode(shiftLatch[i], OUTPUT);
    pinMode(shiftClock[i], OUTPUT);
  }

  updateShiftRegisterAnode(7);
  updateShiftRegisterCathode(play);
}


// 계속 실행할 무한 루프
void loop() {

  int new_button = digitalRead(buttonPin); // ---------------- 버튼 읽기

  if (new_button > 0 && old_button != 1 && level == 7)
  {
    pushed += 1;
    if (pushed > 1)
      pushed = 0; // ---------------------------------------- 버튼 활성화
  }
  old_button = new_button;
  Serial.print(old_button);
  Serial.print(", ");

  if (pushed == 1) {
    play = true;
    Serial.print(" Pushed,  ");

    level -= 1;
    if (level < 1) {
      level = 1;
    }
    select += 0; // ----------------------------------- select = 선택된 도형 번호
    int tmp = select;
    index[data] = tmp;

    updateShiftRegisterAnode(level); // -------------------- LED 켜지는 층
    updateShiftRegisterCathode(play);

  } else if (pushed == 0) {
    Serial.print("unPushed, ");
    select += 1;
    if (select > 7) {
      select = 1;
    }
  }

  if (level == 1) {
    pushed = 0; // ------------------------------------------ 버튼 활성화
    level = 9;
  } else if (level == 9) {
    data++;
    level = 7;

  }
  Serial.print("[");
  Serial.print(data); // --------------------------------- 저장되는 데이터 갯수
  Serial.print("] : ");
  for (int i = 0; i <= data; i++) {
    Serial.print(index[i]); // ---------------------------- 저장된 도형 번호
    Serial.print(", ");
  }
  Serial.print(" current level is... ");
  Serial.print(level);
  Serial.print(", random selection... : ");
  Serial.println(select);

  for (int i = 0; i < 5; i++) {
    grid[i] |=  ~(pat[((index[data]) * 5) + i]);
    Serial.println(grid[i], BIN);

    if ( level == 7 && grid[0] == pat[0] && grid[1] == pat[1] && grid[2] == pat[2] && grid[3] == pat[3] && grid[4] == pat[4] ) {
      grid[0] = glow[0];
      grid[1] = glow[1];
      grid[2] = glow[2];
      grid[3] = glow[3];
      grid[4] = glow[4];
      for (int j = 0; j < data; j++) {
        index[j] = 0;
      }
      data = 0;
      updateShiftRegisterAnode(level);
    }
  }
}

void gridPattern() {
  for (int i = 0; i < 5; i++) {
    if ( level != 1 && level < 7) {
      int shape = (select * 5) + i;
      if (select == 1) {
        shiftOut(redData, redClock, LSBFIRST, pat[shape] );
        shiftOut(greenData, greenClock, LSBFIRST, pat[i]);
        shiftOut(blueData, blueClock, LSBFIRST, pat[i]);
      } else if (select == 2) {
        shiftOut(redData, redClock, LSBFIRST, pat[shape] );
        shiftOut(greenData, greenClock, LSBFIRST, pat[shape] );
        shiftOut(blueData, blueClock, LSBFIRST, pat[i]);
      } else if (select == 3) {
        shiftOut(redData, redClock, LSBFIRST, pat[i]);
        shiftOut(greenData, greenClock, LSBFIRST, pat[shape] );
        shiftOut(blueData, blueClock, LSBFIRST, pat[i]);
      } else if (select == 4) {
        shiftOut(redData, redClock, LSBFIRST, pat[i]);
        shiftOut(greenData, greenClock, LSBFIRST, pat[shape] );
        shiftOut(blueData, blueClock, LSBFIRST, pat[shape] );
      } else if (select == 5) {
        shiftOut(redData, redClock, LSBFIRST, pat[i]);
        shiftOut(greenData, greenClock, LSBFIRST, pat[i]);
        shiftOut(blueData, blueClock, LSBFIRST, pat[shape] );
      } else if (select == 6) {
        shiftOut(redData, redClock, LSBFIRST, pat[shape] );
        shiftOut(greenData, greenClock, LSBFIRST, pat[i]);
        shiftOut(blueData, blueClock, LSBFIRST, pat[shape] );
      } else if (select == 7) {
        shiftOut(redData, redClock, LSBFIRST, pat[shape] );
        shiftOut(greenData, greenClock, LSBFIRST, pat[shape] );
        shiftOut(blueData, blueClock, LSBFIRST, pat[shape] );
      }
    } else {
      // ------------------------------------------ bitAnd = & , bitOr = |, bitNor = ^, bitNot = ~
      shiftOut(redData, redClock, LSBFIRST, ~(grid[i]));
      shiftOut(greenData, greenClock, LSBFIRST, ~(grid[i]));
      shiftOut(blueData, blueClock, LSBFIRST, pat[i]);
    }
  }
}

void patternV() {
  // -------------------------------------------------------- [ LED OFF ]
  for (int i = 0; i < 5; i++) {
    shiftOut(redData, redClock, LSBFIRST, B11111111);
    shiftOut(greenData, greenClock, LSBFIRST, B11111111);
    shiftOut(blueData, blueClock, LSBFIRST, B11111111);
  }
}

// ------------------------------------------------- 패턴 cathode LOW일 때 켜짐 (-)
void updateShiftRegisterCathode(boolean pattern) {
  digitalWrite(redLatch,  LOW);
  digitalWrite(greenLatch,  LOW);
  digitalWrite(blueLatch,  LOW);

  if (pattern) {
    gridPattern();
  }
  else {
    // -------------------------------------------------------- [ LED OFF ]
    patternV();
  }
  digitalWrite(redLatch,  HIGH);
  digitalWrite(greenLatch,  HIGH);
  digitalWrite(blueLatch,  HIGH);
}

// -------------------------------------------------------- 각 층(layer)
void updateShiftRegisterAnode(int layer) {
  for (int i = 0; i < 6; i++) {
    digitalWrite(layerPin[i],  LOW); // ------------ 레이어 anode HIGH일 때 켜짐 (+)
    delay(1);

    if (play) { // ---------------------------------- play = 1층 모니터용으로 그냥 만든 것
      digitalWrite(layerPin[0],  HIGH);

      switch (layer) {
        case 1:
          digitalWrite(layerPin[0],  HIGH);
          break;
        case 2:
          digitalWrite(layerPin[1],  HIGH);
          break;
        case 3:
          digitalWrite(layerPin[2],  HIGH);
          break;
        case 4:
          digitalWrite(layerPin[3],  HIGH);
          break;
        case 5:
          digitalWrite(layerPin[4],  HIGH);
          break;
        case 6:
          digitalWrite(layerPin[5],  HIGH);
          break;
        case 7:
          for (int i = 0; i < 6; i++) {
            // -------------------------------------------------------- [ LED OFF ]
            digitalWrite(layerPin[i],  LOW);
          }
          break;
      }
    }
  }
}
