#include <TFT.h>
#include <SPI.h>
#include "pitches.h"

#define CS   10
#define DC   9
#define RESET  8

#define PIN_BUTTON_LEFT 4
#define PIN_BUTTON_MIDDLE 3
#define PIN_BUTTON_RIGHT 5

#define PIN_SPEAKER 3

#define BLOCK_SIZE 6

#define X -256

#define GRID_SKINNY 10
#define GRID_LONG 22

TFT screen = TFT(CS, DC, RESET);

bool leftWasPressed, rightWasPressed, middleWasPressed;

unsigned int tick = 0;
unsigned int score = 0;
unsigned int highScore = 0;

short dropX = GRID_LONG;
short dropY = GRID_SKINNY / 2;

char dropIndex = 0;
byte dropRotation = 0;

char nextDrop = 0;

short lastDropX = dropX;
short lastDropY = dropY;
byte lastDropRotation = dropRotation;

char grid[GRID_LONG][GRID_SKINNY];

short screenWidth, screenHeight, leftEdge, rightEdge, bottomEdge;

#define startDropDelay 900
short dropDelay = startDropDelay;

unsigned long time;
unsigned long lastTime;
unsigned long lastNoteTime;

#define pieceI 0
#define pieceO 1
#define pieceT 2
#define pieceS 3
#define pieceZ 4
#define pieceJ 5
#define pieceL 6

char pieceList[4][7][8] = {
  { //Rotated 0
    { -1, 0, 0, 0, 1, 0, 2, 0}, //I SAME as 180
    {0, 0, 1, 0, 0, 1, 1, 1}, //O SAME as 180
    {0, -1, -1, 0, 0, 0, 1, 0}, //T
    {0, 0, 1, 0, -1, 1, 0, 1}, //S SAME as 180
    { -1, 0, 0, 0, 0, 1, 1, 1}, //Z SAME as 180
    { -1, 0, -1, 1, 0, 1, 1, 1}, //J
    {1, 0, -1, 1, 0, 1, 1, 1} //L
  }, { //Rotated 90 Counter-Clockwise
    {0, -1, 0, 0, 0, 1, 0, 2}, //I SAME as 270
    {0, 0, 1, 0, 0, 1, 1, 1}, //O SAME as 270
    {1, 0, 0, -1, 0, 0, 0, 1}, //T
    {0, 0, 0, -1, 1, 1, 1, 0}, //S SAME as 270
    {0, 1, 0, 0, 1, 0, 1, -1}, //Z SAME as 270
    {0, -1, -1, -1, -1, 0, -1, 1}, //J
    {0, 1, -1, -1, -1, 0, -1, 1} //L
  }, { //Rotated 180 Counter-Clockwise
    { -1, 0, 0, 0, 1, 0, 2, 0}, //I SAME as 0
    {0, 0, 1, 0, 0, 1, 1, 1}, //O SAME as 0
    {0, 1, 1, 0, 0, 0, -1, 0}, //T
    {0, 0, 1, 0, -1, 1, 0, 1}, // S SAME as 0
    { -1, 0, 0, 0, 0, 1, 1, 1}, // Z SAME as 0
    {1, 0, 1, -1, 0, -1, -1, -1}, //J
    { -1, 0, 1, -1, 0, -1, -1, -1} //L
  }, { //Rotated 270 Counter-Clockwise
    {0, -1, 0, 0, 0, 1, 0, 2}, //I SAME as 90
    {0, 0, 1, 0, 0, 1, 1, 1}, //O SAME as 90
    { -1, 0, 0, -1, 0, 0, 0, 1}, //T
    {0, 0, 0, -1, 1, 1, 1, 0}, //S SAME as 90
    {0, 1, 0, 0, 1, 0, 1, -1}, //Z SAME as 90
    {0, 1, 1, 1, 1, 0, 1, -1}, //J
    {0, -1, 1, 1, 1, 0, 1, -1} //L
  }
};

#define noteCount 63
short notes[noteCount] = {
NOTE_NONE,
NOTE_NONE,
NOTE_NONE,
NOTE_NONE,
NOTE_NONE,
NOTE_E6,NOTE_E6, 
NOTE_NONE, 
NOTE_B5,NOTE_B5,  NOTE_C6,NOTE_C6,  NOTE_D6,NOTE_D6,
NOTE_NONE,
NOTE_C6,NOTE_C6,  NOTE_B5,NOTE_B5,  NOTE_A5,NOTE_A5,
NOTE_NONE,
NOTE_A5,NOTE_A5,  NOTE_C6,NOTE_C6,  NOTE_E6,NOTE_E6,
NOTE_NONE,
NOTE_D6,NOTE_D6,  NOTE_C6,NOTE_C6,  NOTE_B5,NOTE_B5,
NOTE_NONE,
NOTE_B5,NOTE_B5,  NOTE_C6,NOTE_C6,  NOTE_D6,NOTE_D6,  NOTE_E6,NOTE_E6,
NOTE_NONE,
NOTE_NONE,
NOTE_C6,NOTE_C6,NOTE_C6,  NOTE_NONE,  
NOTE_A5,NOTE_A5,NOTE_A5,  NOTE_NONE,  
NOTE_A5,NOTE_A5,NOTE_A5,  NOTE_NONE,  
NOTE_NONE,
NOTE_NONE,
NOTE_NONE,
NOTE_NONE,
NOTE_NONE,

};
short toneDelay = 100;


byte colorsR[7] = {0  , 255, 255, 0  , 255, 0  , 255};
byte colorsG[7] = {255, 255, 0  , 255, 0  , 0  , 127};
byte colorsB[7] = {255, 0  , 255, 0  , 0  , 255, 0  };

void setup() {
  screen.begin();
  screen.background(0, 0, 0);
  Serial.begin(9600);
  screenWidth = screen.width();
  screenHeight = screen.height();

  pinMode(PIN_BUTTON_LEFT, INPUT);
  pinMode(PIN_BUTTON_MIDDLE, INPUT);
  pinMode(PIN_BUTTON_RIGHT, INPUT);

  for (short i = 0; i < GRID_LONG; i++) {
    for (short j = 0; j < GRID_SKINNY; j++) {
      grid[i][j] = -1;
    }
  }

  randomSeed(analogRead(A0));

  dropIndex = random(0, 7);
  nextDrop = random(0, 7);

  leftEdge = (screenHeight - (GRID_SKINNY * BLOCK_SIZE)) / 2;
  rightEdge = screenHeight - leftEdge;
  bottomEdge = screenWidth - (GRID_LONG * BLOCK_SIZE);


  drawBG();

}

void loop() {
  
  bool changed = false;

  ///////////////////////////////////////////////////////
  if (digitalRead(PIN_BUTTON_LEFT)) {
    if (!leftWasPressed) {
      changed = true;
      dropY--;
      if (dropCollidesAnywhere()) {
        dropY++;
      }
    }
  }
  leftWasPressed = digitalRead(PIN_BUTTON_LEFT);

  if (digitalRead(PIN_BUTTON_RIGHT)) {
    if (!rightWasPressed) {
      changed = true;
      dropY++;
      if (dropCollidesAnywhere()) {
        dropY--;
      }
    }
  }
  rightWasPressed = digitalRead(PIN_BUTTON_RIGHT);

  if (digitalRead(PIN_BUTTON_MIDDLE)) {
    if (!middleWasPressed) {
      changed = true;
      dropRotation++;
      if (dropRotation > 3) {
        dropRotation = 0;
      }
      if (dropCollidesAnywhere()) {
        dropRotation--;
        if (dropRotation < 0) {
          dropRotation = 3;
        }
      }
    }
  }
  middleWasPressed = digitalRead(PIN_BUTTON_MIDDLE);
  ///////////////////////////////////////////////////////
  
  time = millis();
  
  bool newDrop = false;

  short x = bottomEdge;
  short y = leftEdge;
  
  if((time - lastNoteTime) >= toneDelay){
    tick++;
    if(notes[tick%noteCount] == NOTE_NONE){
      noTone(PIN_SPEAKER);
    }else{
      tone(PIN_SPEAKER, notes[tick%noteCount]);
    }
    lastNoteTime = time;
  }
  
  

  if ((time - lastTime) >= dropDelay) {
    
    lastTime = time;
    
    dropX--;
    
    changed = true;

    //delay(10);


    if (dropCollidesOnBottom()) {
    
      if(dropDelay > 100){
        dropDelay -= 1;
      }
      
      if (dropX + 1 == GRID_LONG) {
        dropX = GRID_LONG;
        dropY = GRID_SKINNY / 2;
        dropIndex = random(0, 7);
        nextDrop = random(0, 7);
        dropRotation = 0;
        drawBG();
        for (short i = 0; i < GRID_LONG; i++) {
          for (short j = 0; j < GRID_SKINNY; j++) {
            grid[i][j] = -1;
          }
        }
        highScore = max(highScore, score);
        score = 0;
        dropDelay = startDropDelay;

        screen.fill(0, 0, 0);
        screen.rect(x, y, screenWidth - bottomEdge, rightEdge - leftEdge);
        return;
      }
      dropX++;
      placeDrop();

      dropX = GRID_LONG;
      dropY = GRID_SKINNY / 2;

      //lastDropX = dropX;
      //lastDropY = dropY;
      //lastDropRotation = dropRotation;

      dropIndex = nextDrop;
      nextDrop = random(0, 7);
      dropRotation = 0;

      drawBG();
      //screen.fill(colorsR[nextDrop], colorsG[nextDrop], colorsB[nextDrop]);
      //drawPiece(screenWidth-(4*BLOCK_SIZE), (2*BLOCK_SIZE), nextDrop, 0);

      newDrop = true;

      screen.fill(0, 0, 0);
      screen.rect(x, y, screenWidth - bottomEdge, rightEdge - leftEdge);
    }

    byte rows[22];
    bool rowDone = false;

    for (short i = 0; i < GRID_LONG; i++) {
      rows[i] = 0;
      for (short j = 0; j < GRID_SKINNY; j++) {
        char pieceIndex = grid[i][j];
        if (pieceIndex > -1) {
          screen.fill(colorsR[pieceIndex], colorsG[pieceIndex], colorsB[pieceIndex]);
          screen.rect(x + (i * BLOCK_SIZE), y + (j * BLOCK_SIZE), BLOCK_SIZE, BLOCK_SIZE);
          rows[i]++;
        }
      }
      if (rows[i] == 10) {
        rowDone = true;
      }
    }

    if (rowDone) {
      for (byte i = 0; i < 3; i++) {
        drawNeededRowsWhite(x, y, rows);
        delay(200);
        drawNeededRowsNormal(x, y, rows);
        delay(200);
      }
      removeNeededRows(rows);

      screen.fill(0, 0, 0);
      screen.rect(x, y, screenWidth - bottomEdge, rightEdge - leftEdge);

      drawBG();
    }
    
  }
  if(changed) {

    if (!newDrop) {
      screen.fill(0, 0, 0);
      drawPiece(x + (lastDropX * BLOCK_SIZE), y + (lastDropY * BLOCK_SIZE), dropIndex, lastDropRotation);
    }

    screen.fill(colorsR[dropIndex], colorsG[dropIndex], colorsB[dropIndex]);
    drawPiece(x + (dropX * BLOCK_SIZE), y + (dropY * BLOCK_SIZE), dropIndex, dropRotation);

    lastDropX = dropX;
    lastDropY = dropY;
    lastDropRotation = dropRotation;



  }
}

void removeNeededRows(byte rows[22]) {

  for (byte i = 0; i < GRID_LONG; i++) {
    if (rows[i] == GRID_SKINNY) {
      for (byte j = 0; j < GRID_SKINNY; j++) {
        grid[i][j] = -1;
      }
    }
  }

  bool rieatoain = false;

  do {
    rieatoain = false;
    byte row = 0;
    short x = 0;
    for (byte i = 0; i < GRID_LONG; i++) {
      for (byte j = 0; j < GRID_SKINNY; j++) {
        char pieceIndex = grid[i][j];
        if (grid[i][j] == -1 && rows[i] == GRID_SKINNY) {
          row++;
        }
      }
      if (row == 10) {
        x = i;
        break;
      }
    }

    if (row == 10) {
      rieatoain = true;
      for (byte i = x; i < GRID_LONG - 1; i++) {
        for (byte j = 0; j < GRID_SKINNY; j++) {
          grid[i][j] = grid[i + 1][j];
        }
      }
      score++;

    }

  } while (rieatoain);



}

void drawNeededRowsWhite(short x, short y, byte rows[22]) {
  screen.fill(255, 255, 255);
  for (byte i = 0; i < GRID_LONG; i++) {
    if (rows[i] == GRID_SKINNY) {
      screen.rect(x + (i * BLOCK_SIZE), y, BLOCK_SIZE, BLOCK_SIZE * 10);
    }
  }
}

void drawNeededRowsNormal(short x, short y, byte rows[22]) {
  for (byte i = 0; i < GRID_LONG; i++) {
    for (short j = 0; j < GRID_SKINNY; j++) {
      char pieceIndex = grid[i][j];
      if (pieceIndex > -1) {
        screen.fill(colorsR[pieceIndex], colorsG[pieceIndex], colorsB[pieceIndex]);
        screen.rect(x + (i * BLOCK_SIZE), y + (j * BLOCK_SIZE), BLOCK_SIZE, BLOCK_SIZE);
      }
    }
  }
}

void placeDrop() {
  for (short i = 0; i < 8; i += 2) {
    grid[(dropX + (pieceList[dropRotation][dropIndex][i]))][(dropY + (pieceList[dropRotation][dropIndex][i + 1]))] = dropIndex;
  }
}

bool dropCollidesOnBottom() {
  bool c = false;
  for (short i = 0; i < GRID_LONG && !c; i++) {
    for (short j = 0; j < GRID_SKINNY && !c; j++) {
      for (short k = 0; k < 8 && !c; k += 2) {
        if (gridOcupied(dropX + pieceList[dropRotation][dropIndex][k], dropY + pieceList[dropRotation][dropIndex][k + 1]) || dropX + pieceList[dropRotation][dropIndex][k] < 0) {
          c = true;
        }
      }
    }
  }
  return c;
}

bool gridOcupied(short x, short y) {
  if (x < 0 || y < 0 || x >= GRID_LONG || y >= GRID_SKINNY) {
    return false;
  }
  return grid[x][y] > -1;
}

bool dropCollidesAnywhere() {
  bool c = false;
  for (short i = 0; i < GRID_LONG && !c; i++) {
    for (short j = 0; j < GRID_SKINNY && !c; j++) {
      for (short k = 0; k < 8 && !c; k += 2) {
        if (gridOcupiedOrOutOfBounds(dropX + pieceList[dropRotation][dropIndex][k], dropY + pieceList[dropRotation][dropIndex][k + 1])) {
          c = true;
        }
      }
    }
  }
  return c;
}

bool gridOcupiedOrOutOfBounds(short x, short y) {
  if (x < 0 || y < 0 || x >= GRID_LONG || y >= GRID_SKINNY) {
    return true;
  }
  return grid[x][y] > -1;
}

void drawBG() {
  screen.fill(64, 64, 64);
  screen.rect(0, 0, screenWidth, leftEdge);
  screen.rect(0, rightEdge, screenWidth, screenWidth - rightEdge);
  screen.rect(0, 0, bottomEdge, screenHeight);
  screen.noFill();
  screen.stroke(127, 127, 127);
  screen.rect(bottomEdge - 1, leftEdge - 1, screenWidth - bottomEdge + 2, rightEdge - leftEdge + 2);
  screen.noStroke();

  screen.fill(colorsR[nextDrop], colorsG[nextDrop], colorsB[nextDrop]);
  drawPiece(screenWidth - (4 * BLOCK_SIZE), (2 * BLOCK_SIZE), nextDrop, 0);

  screen.stroke(255, 255, 255);
  screen.text(String(score).c_str(), 2, 2);
  if (score != highScore) {
    screen.text("Highscore", 2, screenHeight - 20);
    screen.text(String(highScore).c_str(), 2, screenHeight - 10);
  }
  screen.noStroke();

}

void drawPiece(short x, short y, char pieceIndex, byte rotation) {
  for (short i = 0; i < 8; i += 2) {
    screen.rect((x + (pieceList[rotation][pieceIndex][i]*BLOCK_SIZE)), (y + (pieceList[rotation][pieceIndex][i + 1]*BLOCK_SIZE)), BLOCK_SIZE, BLOCK_SIZE);
  }
  //screen.fill(127, 127, 127);
  //screen.rect(x, y, BLOCK_SIZE/2, BLOCK_SIZE/2);
}
