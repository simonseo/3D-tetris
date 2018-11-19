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
int height = 0;

int data = 0;
int index[40]; // ------------------------------------------- [문제 없음]

//-------------------------------------------------------------[anode]
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

//------------------------------------------------------------[cathode]
int layerPin[6] = { 2, 3, 4, 5, 6, 7 };

byte imageG[5];
byte grid[5];
boolean play = false;

// 5*5 이미지
// byte 1:off  0:On

byte image6[5] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

byte pat[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,

  B11111,
  B11111,
  B11001,
  B10011,
  B11111,

  B11111,
  B11111,
  B10011,
  B10011,
  B11111,

  B11111,
  B11011,
  B11011,
  B11011,
  B11011,

  B11111,
  B11111,
  B11011,
  B10001,
  B11111,

  B11111,
  B11111,
  B10001,
  B11101,
  B11111
};

void setup() {
  Serial.begin(9600); // ---------------------------------- 시리얼 통신 초기화

  pinMode(buttonPin, INPUT); // ---------------------------- PUSH BUTTON

  for (int i = 0; i < 6; i++) {
    pinMode(layerPin[i], OUTPUT); // ----------------------- 각 층별 핀 설정
  }

  pinMode(greenData, OUTPUT);
  pinMode(greenLatch, OUTPUT);
  pinMode(greenClock, OUTPUT);

  pinMode(blueData, OUTPUT);
  pinMode(blueLatch, OUTPUT);
  pinMode(blueClock, OUTPUT);

  pinMode(redData, OUTPUT);
  pinMode(redLatch, OUTPUT);
  pinMode(redClock, OUTPUT);

  updateShiftRegisterAnode(7);
  updateShiftRegisterCathode(play);
}


// 계속 실행할 무한 루프
void loop() {

  int new_button = digitalRead(buttonPin); // ----------------- 버튼 읽기

  if (new_button > 0 && old_button != new_button && old_button != 1 )
  {
    pushed += 1;
    if (pushed > 1)
      pushed = 0; // ----------------------------------------- 버튼 활성화
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
    delay(100); // ------------------------------------------  떨어지는 속도

    select += 0; // ------------------------------------ select = 선택된 도형 번호
    int tmp = select;
    index[data] = tmp;

    updateShiftRegisterAnode(level); // --------------------- LED 켜지는 층
    updateShiftRegisterCathode(play);

  } else if (pushed == 0) {
    Serial.print("unPushed, ");
    select += 1;
    if (select > 5) {
      select = 1;
    }
  }

  if (level == 1) {
    pushed = 0; // ------------------------------------------- 버튼 활성화
    level = 9;
  } else if (level == 9) {
    data++;
    level = 7;

  }
  //  if(data%4 == 2){ // ------------------------------------ 누적 높이
  //    height++;
  //    if(height > 1){
  //      height = 1;
  //    }
  //  }
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
  Serial.print(select);
  Serial.print(", ");
  Serial.println(height);

  for (int i = 0; i < 5; i++) {
    grid[i] |=  ~(pat[((index[data]) * 5) + i]);
    Serial.println(grid[i], BIN);
  }
}

void gridPattern() {
  if (level == 1) {
    for (int i = 0; i < 5; i++) {
      digitalWrite(redLatch,  LOW); // ----------- bitAnd = & , bitOr = |, bitNor = ^,
      shiftOut(redData, redClock, LSBFIRST, ~(grid[i]));
      digitalWrite(redLatch,  HIGH);
    }
    for (int i = 0; i < 5; i++) {
      digitalWrite(greenLatch, LOW);
      shiftOut(greenData, greenClock, LSBFIRST, image6[i]);
      digitalWrite(greenLatch,  HIGH);
    }
    for (int i = 0; i < 5; i++) {
      digitalWrite(blueLatch, LOW);
      shiftOut(blueData, blueClock, LSBFIRST, image6[i]);
      digitalWrite(blueLatch,  HIGH);
    }
  } else {
    for (int i = 0; i < 5; i++) {
      digitalWrite(redLatch,  LOW);
      shiftOut(redData, redClock, LSBFIRST, image6[i]);
      digitalWrite(redLatch,  HIGH);
    }
    for (int i = 0; i < 5; i++) {
      digitalWrite(greenLatch, LOW);
      shiftOut(greenData, greenClock, LSBFIRST, pat[(select * 5) + i] );
      digitalWrite(greenLatch,  HIGH);
    }
    for (int i = 0; i < 5; i++) {
      digitalWrite(blueLatch, LOW);
      shiftOut(blueData, blueClock, LSBFIRST, image6[i]);
      digitalWrite(blueLatch,  HIGH);
    }
  }
}

void patternV() {
  // ----------------------------------------------------[ LED OFF ]
  for (int i = 0; i < 5; i++) {
    digitalWrite(redLatch,  LOW);
    shiftOut(redData, redClock, LSBFIRST, image6[i]);
    digitalWrite(redLatch,  HIGH);
  }
  for (int i = 0; i < 5; i++) {
    digitalWrite(greenLatch,  LOW);
    shiftOut(greenData, greenClock, LSBFIRST, image6[i]);
    digitalWrite(greenLatch,  HIGH);
  }
  for (int i = 0; i < 5; i++) {
    digitalWrite(blueLatch,  LOW);
    shiftOut(blueData, blueClock, LSBFIRST, image6[i]);
    digitalWrite(blueLatch,  HIGH);
  }
}

// ------------------------------------------------ 패턴 cathode LOW일 때 켜짐(-)
void updateShiftRegisterCathode(boolean pattern) {
  digitalWrite(redLatch,  LOW);
  digitalWrite(greenLatch,  LOW);
  digitalWrite(blueLatch,  LOW);
  if (pattern) {
    gridPattern();
  }
  else {
    patternV();
  }
  digitalWrite(redLatch,  HIGH);
  digitalWrite(greenLatch,  HIGH);
  digitalWrite(blueLatch,  HIGH);
}

// ------------------------------------------------------ 각 층(layer) 제어
void updateShiftRegisterAnode(int layer) {
  for (int i = 0; i < 6; i++) {
    digitalWrite(layerPin[i],  LOW); // ------------ 레이어 anode HIGH일 때 켜짐(+)
    if (play) {
      delay(1);
      digitalWrite(layerPin[0],  HIGH); // ----------------- 1층 모니터용
    }
  }

  switch (layer) {
    case 1:
      delay(1);
      digitalWrite(layerPin[0],  HIGH);
      break;
    case 2:
      delay(1);
      digitalWrite(layerPin[1],  HIGH);
      break;
    case 3:
      delay(1);
      digitalWrite(layerPin[2],  HIGH);
      break;
    case 4:
      delay(1);
      digitalWrite(layerPin[3],  HIGH);
      break;
    case 5:
      delay(1);
      digitalWrite(layerPin[4],  HIGH);
      break;
    case 6:
      delay(1);
      digitalWrite(layerPin[5],  HIGH);
      break;
    case 7:
      for (int i = 0; i < 6; i++) {
        delay(1);
        digitalWrite(layerPin[i],  LOW);
      }
      break;

      //   -----------------------------------------------------------[ LED OFF ]
  }
  //      for (int i = 0; i < height; i++) {// -------------------------누적 높이
  //        delay(1);
  //        digitalWrite(layerPin[i],  HIGH);
  //      }
}
