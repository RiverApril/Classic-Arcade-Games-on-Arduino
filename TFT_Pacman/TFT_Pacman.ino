#include <TFT.h>
#include <SPI.h>

#define PIN_CS 10
#define PIN_DC 9
#define PIN_RESET 8

#define PIN_BUTTON_STICK 2
#define PIN_BUTTON_RIGHT 3
#define PIN_BUTTON_UP 4
#define PIN_BUTTON_DOWN 5
#define PIN_BUTTON_LEFT 6

#define PIN_AXIS_X A0
#define PIN_AXIS_Y A1

#define HEIGHT 128
#define WIDTH 160

TFT screen = TFT(PIN_CS, PIN_DC, PIN_RESET);

#define COLOR_R_WALL 0
#define COLOR_G_WALL 0
#define COLOR_B_WALL 255

#define COLOR_R_GATE 255
#define COLOR_G_GATE 255
#define COLOR_B_GATE 255

#define COLOR_R_PELLET 255
#define COLOR_G_PELLET 255
#define COLOR_B_PELLET 255

#define COLOR_R_POWER_PELLET 255
#define COLOR_G_POWER_PELLET 255
#define COLOR_B_POWER_PELLET 255

#define COLOR_R_PACMAN 255
#define COLOR_G_PACMAN 255
#define COLOR_B_PACMAN 0

#define TILE_EMPTY 0
#define TILE_WALL 1
#define TILE_GATE 2
#define TILE_PELLET 3
#define TILE_POWER_PELLET 4
#define TILE_CAGE 5

#define LEVEL_WIDTH 28
#define LEVEL_HEIGHT 31

#define JOYSTICK_BOUNDRY 128

#define JOYSTICK_UPPER_BOUNDRY (512+JOYSTICK_BOUNDRY)
#define JOYSTICK_LOWER_BOUNDRY (512-JOYSTICK_BOUNDRY)

#define tile(x, y) grid[y][x]

#define solid(x, y) ((tile(x, y)==TILE_WALL)||(tile(x, y)==TILE_GATE))

#define cageUpSolid(x, y) ((tile(x, y)==TILE_WALL))

#define DIR_LEFT 0
#define DIR_RIGHT 1
#define DIR_UP 2
#define DIR_DOWN 3
#define DIR_NONE 4

#define GHOST_RED 0
#define GHOST_BLUE 1
#define GHOST_PINK 2
#define GHOST_ORANGE 3

#define GHOST_TIME_SCARED 100
#define GHOST_TIME_FLASHING 20

#define LOOP_DELAY 8


#define SPEED_PACMAN 1
#define SPEED_GHOST 1

char textBuffer[10];

byte grid[LEVEL_HEIGHT][LEVEL_WIDTH] = {
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1},
{1, 3, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 3, 1, 1, 3, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 3, 1},
{1, 4, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 3, 1, 1, 3, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 4, 1},
{1, 3, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 3, 1, 1, 3, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 3, 1},
{1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1},
{1, 3, 1, 1, 1, 1, 3, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 3, 1, 1, 1, 1, 3, 1},
{1, 3, 1, 1, 1, 1, 3, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 3, 1, 1, 1, 1, 3, 1},
{1, 3, 3, 3, 3, 3, 3, 1, 1, 3, 3, 3, 3, 1, 1, 3, 3, 3, 3, 1, 1, 3, 3, 3, 3, 3, 3, 1},
{1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1},
{0, 0, 0, 0, 0, 1, 3, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 3, 1, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 1, 3, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 3, 1, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 1, 3, 1, 1, 0, 1, 1, 1, 2, 2, 1, 1, 1, 0, 1, 1, 3, 1, 0, 0, 0, 0, 0},
{1, 1, 1, 1, 1, 1, 3, 1, 1, 0, 1, 5, 5, 5, 5, 5, 5, 1, 0, 1, 1, 3, 1, 1, 1, 1, 1, 1},
{0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 1, 5, 5, 5, 5, 5, 5, 1, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0},
{1, 1, 1, 1, 1, 1, 3, 1, 1, 0, 1, 5, 5, 5, 5, 5, 5, 1, 0, 1, 1, 3, 1, 1, 1, 1, 1, 1},
{0, 0, 0, 0, 0, 1, 3, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 3, 1, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 1, 3, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 3, 1, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 1, 3, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 3, 1, 0, 0, 0, 0, 0},
{1, 1, 1, 1, 1, 1, 3, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 3, 1, 1, 1, 1, 1, 1},
{1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1},
{1, 3, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 3, 1, 1, 3, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 3, 1},
{1, 3, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 3, 1, 1, 3, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 3, 1},
{1, 4, 3, 3, 1, 1, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 1, 1, 3, 3, 4, 1},
{1, 1, 1, 3, 1, 1, 3, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 3, 1, 1, 3, 1, 1, 1},
{1, 1, 1, 3, 1, 1, 3, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 3, 1, 1, 3, 1, 1, 1},
{1, 3, 3, 3, 3, 3, 3, 1, 1, 3, 3, 3, 3, 1, 1, 3, 3, 3, 3, 1, 1, 3, 3, 3, 3, 3, 3, 1},
{1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1},
{1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1},
{1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

int tick = 0;

byte pacX = 0;
byte pacY = 0;
byte pacLastX = 0;
byte pacLastY = 0;
byte pacDir = 0;
byte pacFrame = 0;

byte ghostX[4] = {0, 0, 0, 0};
byte ghostY[4] = {0, 0, 0, 0};
byte ghostLastX[4] = {0, 0, 0, 0};
byte ghostLastY[4] = {0, 0, 0, 0};
byte ghostDir[4] = {0, 0, 0, 0};
byte ghostScaredTime[4] = {0, 0, 0, 0};
bool ghostChase = false;

byte ghostKillStreak = 0;

unsigned int score;
unsigned int lastScore;
unsigned int dotsEaten;
byte lives;
byte lastLives;
unsigned int timer;
char deathTimer;

void setup() {

  pinMode(PIN_BUTTON_STICK, INPUT);
  digitalWrite(PIN_BUTTON_STICK, HIGH);
  
  pinMode(PIN_BUTTON_RIGHT, INPUT);
  digitalWrite(PIN_BUTTON_RIGHT, HIGH);
  
  pinMode(PIN_BUTTON_UP, INPUT);
  digitalWrite(PIN_BUTTON_UP, HIGH);
  
  pinMode(PIN_BUTTON_DOWN, INPUT);
  digitalWrite(PIN_BUTTON_DOWN, HIGH);
  
  pinMode(PIN_BUTTON_LEFT, INPUT);
  digitalWrite(PIN_BUTTON_LEFT, HIGH);
  
  screen.begin();
  screen.background(0, 0, 0);
  
  drawMap();
  
  score = 0;
  lastScore = 255;
  dotsEaten = 0;
  lives = 2;
  lastLives = 0;
  deathTimer = -1;
  
  resetPositions();
  
}

void loop() {
  
  short xAxis = analogRead(PIN_AXIS_X);
  short yAxis = 1023-analogRead(PIN_AXIS_Y);

  /*screen.stroke(0, 0, 0);
  screen.line(WIDTH-1, HEIGHT-1, 0, HEIGHT-1);
  screen.line(WIDTH-1, HEIGHT-1, WIDTH-1, 0);
  screen.stroke(255, 0, 0);
  screen.line(WIDTH-1, HEIGHT-1, map(xAxis, 0, 1023, 0, WIDTH-1), HEIGHT-1);
  screen.line(WIDTH-1, HEIGHT-1, WIDTH-1, map(yAxis, 0, 1023, 0, HEIGHT-1));*/
  
  char inputX = xAxis > JOYSTICK_UPPER_BOUNDRY ? 1 : (xAxis < JOYSTICK_LOWER_BOUNDRY ? -1 : 0);
  char inputY = yAxis > JOYSTICK_UPPER_BOUNDRY ? 1 : (yAxis < JOYSTICK_LOWER_BOUNDRY ? -1 : 0);
  
  
  tick++;
  
  if((inputX == -1 || !digitalRead(PIN_BUTTON_LEFT)) && !solid((pacX-1) >> 2, pacY >> 2)){
    pacDir = DIR_LEFT;
  }else if((inputX == 1 || !digitalRead(PIN_BUTTON_RIGHT)) && !solid((pacX >> 2)+1, pacY >> 2)){
    pacDir = DIR_RIGHT;
  }else if((inputY == -1 || !digitalRead(PIN_BUTTON_UP)) && !solid(pacX >> 2, (pacY-1) >> 2)){
    pacDir = DIR_UP;
  }else if((inputY == 1 || !digitalRead(PIN_BUTTON_DOWN)) && !solid(pacX >> 2, (pacY >> 2)+1)){
    pacDir = DIR_DOWN;
  }
  
  delay(LOOP_DELAY);
  
  if(tick % 15 == 0){
    pacFrame++;
    if(pacFrame == 3){
      pacFrame = 1;
    }
  }
  
  switch(timer){
    case 0:{
      ghostChase = false;
      break;
    }
    case (7):{
      ghostChase = true;
      break;
    }
    case (7+20):{
      ghostChase = false;
      break;
    }
    case (7+20+7):{
      ghostChase = true;
      break;
    }
    case (7+20+7+20):{
      ghostChase = false;
      break;
    }
    case (7+20+7+20+5):{
      ghostChase = true;
      break;
    }
    case (7+20+7+20+5+20):{
      ghostChase = false;
      break;
    }
    case (7+20+7+20+5+20+5):{
      ghostChase = true;
      break;
    }
  }
  
  if(dotsEaten > 30 && ghostDir[GHOST_BLUE] == DIR_NONE){
    ghostDir[GHOST_BLUE] = DIR_LEFT;
  }
  
  if(dotsEaten > (240/3) && ghostDir[GHOST_ORANGE] == DIR_NONE){
    ghostDir[GHOST_ORANGE] = DIR_LEFT;
  }
  

  /*if(millis() - lastMillis > 1000){
    timer++;
    lastMillis = millis();
  }*/
  
  if(lastScore != score){
    screen.noStroke();
    screen.fill(0, 0, 0);
    screen.rect((LEVEL_WIDTH*4)+2, 10, 4*7, 5);
    screen.stroke(255, 255, 255);
    drawNumber(score, (LEVEL_WIDTH*4)+2, 10);
  }
  lastScore = score;
  
  if(lastLives != lives){
    screen.noStroke();
    screen.fill(0, 0, 0);
    screen.rect((LEVEL_WIDTH*4), 2, WIDTH-(LEVEL_WIDTH*4), 4);
    for(byte i=0;i<lives;i++){
      byte x = (LEVEL_WIDTH*4) + i*6;
      byte y = 2;
      screen.noStroke();
      screen.fill(COLOR_R_PACMAN, COLOR_G_PACMAN, COLOR_B_PACMAN);
      screen.rect(x, y, 4, 4);
      screen.stroke(0, 0, 0);
      screen.point(x, y);
      screen.point(x+3, y);
      screen.point(x, y+3);
      screen.point(x+3, y+3);
      
      screen.point(x, y+1);
      screen.point(x+1, y+1);
      screen.point(x, y+2);
      screen.point(x+1, y+2);
    }
  }
  lastLives = lives;
  
  if(tick % 15 == 0){
    timer++;
  }
  
  if(tick % 5 == 0){

    if(pacX == 0 && pacDir == DIR_LEFT){
      pacX = (LEVEL_WIDTH-1)*4;
    }else if(pacX == (LEVEL_WIDTH-1)*4 && pacDir == DIR_RIGHT){
      pacX = 0;
    }
    
    for(byte i = 0;i<4;i++){
      if(ghostX[i] == 0 && pacDir == DIR_LEFT){
        ghostX[i] = (LEVEL_WIDTH-1)*4;
      }else if(ghostX[i] == (LEVEL_WIDTH-1)*4 && pacDir == DIR_RIGHT){
        ghostX[i] = 0;
      }
    }
    
    drawTile(pacLastX >> 2, pacLastY >> 2);
    drawTile((pacLastX >> 2)+1, pacLastY >> 2);
    drawTile(pacLastX >> 2, (pacLastY >> 2)+1);
    drawTile((pacLastX >> 2)+1, (pacLastY >> 2)+1);
    
    
    for(byte i=0;i<4;i++){
      drawTile(ghostLastX[i] >> 2, ghostLastY[i] >> 2);
      drawTile((ghostLastX[i] >> 2)+1, ghostLastY[i] >> 2);
      drawTile(ghostLastX[i] >> 2, (ghostLastY[i] >> 2)+1);
      drawTile((ghostLastX[i] >> 2)+1, (ghostLastY[i] >> 2)+1);
    }
      
    for(byte i=0;i<4;i++){
      drawGhost(i);
      ghostLastX[i] = ghostX[i];
      ghostLastY[i] = ghostY[i];
    }

    
    drawPacman();
    pacLastX = pacX;
    pacLastY = pacY;
    
    if(deathTimer < 0){
  
      if(pacDir == DIR_LEFT && !solid((pacX-1) >> 2, pacY >> 2)){
        pacX -= SPEED_PACMAN;
        pacY = (pacY >> 2) << 2;
      }else if(pacDir == DIR_RIGHT && !solid((pacX >> 2)+1, pacY >> 2)){
        pacX += SPEED_PACMAN;
        pacY = (pacY >> 2) << 2;
      }else if(pacDir == DIR_UP && !solid(pacX >> 2, (pacY-1) >> 2)){
        pacY -= SPEED_PACMAN;
        pacX = (pacX >> 2) << 2;
      }else if(pacDir == DIR_DOWN && !solid(pacX >> 2, (pacY >> 2)+1)){
        pacY += SPEED_PACMAN;
        pacX = (pacX >> 2) << 2;
      }
    
      for(byte i=0;i<4;i++){
        if(abs(pacX - ghostX[i]) < 2 && abs(pacY - ghostY[i]) < 2){
          if(ghostScaredTime[i] > 0){
            resetGhost(i);
            ghostKillStreak++;
            switch(ghostKillStreak){
              case 1:{
                score += 200;
              }
              case 2:{
                score += 400;
              }
              case 3:{
                score += 800;
              }
              case 4:{
                score += 1600;
              }
            }
          }else{
            deathTimer = 20;
            pacFrame = 4;
          }
        }
      }
      
      for(byte i=0;i<4;i++){
        if(ghostScaredTime[i] > 1){
          ghostScaredTime[i]--;
        }else if(ghostScaredTime[i] == 1){
          ghostScaredTime[i] = 0;
        }
        if(ghostScaredTime[i] == 0 || ghostScaredTime[i] % 2 == 0){
          ghostAi(i);
        }
      }
    
    } else {
      if(deathTimer == 0){
        if(lives == 0){
          gameOver();
        }else{
          lives--;
        }
        resetPositions();
      }
      deathTimer--;
    }
    
    if(tile(pacLastX >> 2, pacLastY >> 2) == TILE_PELLET){
      tile(pacLastX >> 2, pacLastY >> 2) = TILE_EMPTY;
      dotsEaten++;
      score+=10;
    }
    
    if(tile(pacLastX >> 2, pacLastY >> 2) == TILE_POWER_PELLET){
      tile(pacLastX >> 2, pacLastY >> 2) = TILE_EMPTY;
      ghostKillStreak = 0;
      score += 50;
      for(byte i=0;i<4;i++){
        ghostScaredTime[i] = GHOST_TIME_SCARED;
      }
    }
  }
  
}

void resetPositions(){
  timer = 0;
  
  pacX = (13*4)+2;
  pacY = 23*4;
  pacDir = DIR_NONE;
  pacFrame = 0;
  
  for(byte i=0;i<4;i++){
    resetGhost(i);
  }
  
}

void resetGhost(byte i){
  switch(i){
    case GHOST_RED:{
      ghostX[GHOST_RED] = (13*4)+2;
      ghostY[GHOST_RED] = 11*4;
      ghostDir[GHOST_RED] = DIR_LEFT;
      break;
    }
    case GHOST_BLUE:{
      ghostX[GHOST_BLUE] = (11*4)+2;
      ghostY[GHOST_BLUE] = 14*4;
      ghostDir[GHOST_BLUE] = DIR_NONE;
      break;
    }
    case GHOST_PINK:{
      ghostX[GHOST_PINK] = (13*4)+2;
      ghostY[GHOST_PINK] = 14*4;
      ghostDir[GHOST_PINK] = DIR_LEFT;
      break;
    }
    case GHOST_ORANGE:{
      ghostX[GHOST_ORANGE] = (15*4)+2;
      ghostY[GHOST_ORANGE] = 14*4;
      ghostDir[GHOST_ORANGE] = DIR_NONE;
      break;
    }
  }
  
  ghostScaredTime[i] = 0;
  
}

void gameOver(){
  screen.fill(0, 0, 0);
  screen.stroke(255, 255, 0);
  screen.rect(0, 0, WIDTH, HEIGHT);
  short x = (WIDTH-(4*11))/2;
  short y = (HEIGHT/2)+6;
  
  drawChar('P', x, y);x+=4;
  drawChar('R', x, y);x+=4;
  drawChar('E', x, y);x+=4;
  drawChar('S', x, y);x+=4;
  drawChar('S', x, y);x+=4;
  x+=4;
  drawChar('R', x, y);x+=4;
  drawChar('E', x, y);x+=4;
  drawChar('S', x, y);x+=4;
  drawChar('E', x, y);x+=4;
  drawChar('T', x, y);x+=4;
  
  x = (WIDTH-(4*9))/2;
  y = (HEIGHT/2)-6;
  
  drawChar('G', x, y);x+=4;
  drawChar('A', x, y);x+=4;
  drawChar('M', x, y);x+=4;
  drawChar('E', x, y);x+=4;
  x+=4;
  drawChar('O', x, y);x+=4;
  drawChar('V', x, y);x+=4;
  drawChar('E', x, y);x+=4;
  drawChar('R', x, y);x+=4;
  
  
  while(true){
    
  }
}

void drawChar(char c, short x, short y){
  switch(c){
    case 'A':{
      screen.point(x, y+1);
      screen.point(x, y+2);
      screen.point(x, y+3);
      screen.point(x, y+4);
      
      screen.point(x+1, y);
      screen.point(x+1, y+2);
      
      screen.point(x+2, y+1);
      screen.point(x+2, y+2);
      screen.point(x+2, y+3);
      screen.point(x+2, y+4);
      break;
    }
    case 'E':{
      screen.point(x, y);
      screen.point(x, y+1);
      screen.point(x, y+2);
      screen.point(x, y+3);
      screen.point(x, y+4);
      
      screen.point(x+1, y);
      screen.point(x+1, y+2);
      screen.point(x+1, y+4);
      
      screen.point(x+2, y);
      screen.point(x+2, y+4);
      break;
    }
    case 'G':{
      screen.point(x, y);
      screen.point(x, y+1);
      screen.point(x, y+2);
      screen.point(x, y+3);
      screen.point(x, y+4);
      
      screen.point(x+1, y);
      screen.point(x+1, y+4);
      
      screen.point(x+2, y);
      screen.point(x+2, y+2);
      screen.point(x+2, y+3);
      screen.point(x+2, y+4);
      break;
    }
    case 'M':{
      screen.point(x, y);
      screen.point(x, y+1);
      screen.point(x, y+2);
      screen.point(x, y+3);
      screen.point(x, y+4);
      
      screen.point(x+1, y+1);
      
      screen.point(x+2, y);
      screen.point(x+2, y+1);
      screen.point(x+2, y+2);
      screen.point(x+2, y+3);
      screen.point(x+2, y+4);
      break;
    }
    case 'O':{
      screen.point(x, y);
      screen.point(x, y+1);
      screen.point(x, y+2);
      screen.point(x, y+3);
      screen.point(x, y+4);
      
      screen.point(x+1, y);
      screen.point(x+1, y+4);
      
      screen.point(x+2, y);
      screen.point(x+2, y+1);
      screen.point(x+2, y+2);
      screen.point(x+2, y+3);
      screen.point(x+2, y+4);
      break;
    }
    case 'P':{
      screen.point(x, y);
      screen.point(x, y+1);
      screen.point(x, y+2);
      screen.point(x, y+3);
      screen.point(x, y+4);
      
      screen.point(x+1, y);
      screen.point(x+1, y+2);
      
      screen.point(x+2, y);
      screen.point(x+2, y+1);
      screen.point(x+2, y+2);
      break;
    }
    case 'R':{
      screen.point(x, y);
      screen.point(x, y+1);
      screen.point(x, y+2);
      screen.point(x, y+3);
      screen.point(x, y+4);
      
      screen.point(x+1, y);
      screen.point(x+1, y+2);
      
      screen.point(x+2, y);
      screen.point(x+2, y+1);
      screen.point(x+2, y+3);
      screen.point(x+2, y+4);
      break;
    }
    case 'S':{
      screen.point(x, y);
      screen.point(x, y+1);
      screen.point(x, y+2);
      screen.point(x, y+4);
      
      screen.point(x+1, y);
      screen.point(x+1, y+2);
      screen.point(x+1, y+4);
      
      screen.point(x+2, y);
      screen.point(x+2, y+2);
      screen.point(x+2, y+3);
      screen.point(x+2, y+4);
      break;
    }
    case 'T':{
      screen.point(x, y);
      
      screen.point(x+1, y);
      screen.point(x+1, y+1);
      screen.point(x+1, y+2);
      screen.point(x+1, y+3);
      screen.point(x+1, y+4);
      
      screen.point(x+2, y);
      break;
    }
    case 'V':{
      screen.point(x, y);
      screen.point(x, y+1);
      screen.point(x, y+2);
      screen.point(x, y+3);
      
      screen.point(x+1, y+4);
      
      screen.point(x+2, y);
      screen.point(x+2, y+1);
      screen.point(x+2, y+2);
      screen.point(x+2, y+3);
      break;
    }
    case '0':{
      screen.point(x, y);
      screen.point(x+1, y);
      screen.point(x+2, y);
      
      screen.point(x, y+1);
      screen.point(x, y+2);
      screen.point(x, y+3);
      
      screen.point(x+2, y+1);
      screen.point(x+2, y+2);
      screen.point(x+2, y+3);
      
      screen.point(x, y+4);
      screen.point(x+1, y+4);
      screen.point(x+2, y+4);
      break;
    }
    case '1':{
      screen.point(x+2, y);
      screen.point(x+2, y+1);
      screen.point(x+2, y+2);
      screen.point(x+2, y+3);
      screen.point(x+2, y+4);
      break;
    }
    case '2':{
      screen.point(x, y);
      screen.point(x+1, y);
      screen.point(x+2, y);
      
      screen.point(x+2, y+1);
      
      screen.point(x, y+2);
      screen.point(x+1, y+2);
      screen.point(x+2, y+2);
      
      screen.point(x, y+3);
      
      screen.point(x, y+4);
      screen.point(x+1, y+4);
      screen.point(x+2, y+4);
      break;
    }
    case '3':{
      screen.point(x, y);
      screen.point(x+1, y);
      screen.point(x+2, y);
      
      screen.point(x+2, y+1);
      
      screen.point(x+1, y+2);
      screen.point(x+2, y+2);
      
      screen.point(x+2, y+3);
      
      screen.point(x, y+4);
      screen.point(x+1, y+4);
      screen.point(x+2, y+4);
      break;
    }
    case '4':{
      screen.point(x, y);
      screen.point(x, y+1);
      screen.point(x, y+2);
      
      screen.point(x+2, y);
      screen.point(x+2, y+1);
      screen.point(x+2, y+2);
      
      screen.point(x+1, y+2);
      
      screen.point(x+2, y+3);
      screen.point(x+2, y+4);
      break;
    }
    case '5':{
      screen.point(x, y);
      screen.point(x+1, y);
      screen.point(x+2, y);
      
      screen.point(x, y+1);
      
      screen.point(x, y+2);
      screen.point(x+1, y+2);
      screen.point(x+2, y+2);
      
      screen.point(x+2, y+3);
      
      screen.point(x, y+4);
      screen.point(x+1, y+4);
      screen.point(x+2, y+4);
      break;
    }
    case '6':{
      screen.point(x, y);
      screen.point(x+1, y);
      screen.point(x+2, y);
      
      screen.point(x, y+1);
      
      screen.point(x, y+2);
      screen.point(x+1, y+2);
      screen.point(x+2, y+2);
      
      screen.point(x, y+3);
      screen.point(x+2, y+3);
      
      screen.point(x, y+4);
      screen.point(x+1, y+4);
      screen.point(x+2, y+4);
      break;
    }
    case '7':{
      screen.point(x, y);
      screen.point(x+1, y);
      screen.point(x+2, y);
      
      screen.point(x+2, y);
      screen.point(x+2, y+1);
      screen.point(x+2, y+2);
      screen.point(x+2, y+3);
      screen.point(x+2, y+4);
      break;
    }
    case '8':{
      screen.point(x, y);
      screen.point(x+1, y);
      screen.point(x+2, y);
      
      screen.point(x, y+1);
      screen.point(x+2, y+1);
      
      screen.point(x, y+2);
      screen.point(x+1, y+2);
      screen.point(x+2, y+2);
      
      screen.point(x, y+3);
      screen.point(x+2, y+3);
      
      screen.point(x, y+4);
      screen.point(x+1, y+4);
      screen.point(x+2, y+4);
      break;
    }
    case '9':{
      screen.point(x, y);
      screen.point(x+1, y);
      screen.point(x+2, y);
      
      screen.point(x, y+1);
      screen.point(x+2, y+1);
      
      screen.point(x, y+2);
      screen.point(x+1, y+2);
      screen.point(x+2, y+2);
      screen.point(x+2, y+3);
      screen.point(x+2, y+4);
      break;
    }
  }
}

void ghostAi(byte i){
  
  if(ghostDir[i] == DIR_NONE){
    return;
  }
  
  byte targetX;
  byte targetY;
  
  if(ghostScaredTime[i] > 0){
      targetX = 255-pacX;
      targetY = 255-pacY;
      
  }else if(ghostChase){
    switch(i){
      case GHOST_RED:{
        targetX = pacX;
        targetY = pacY;
        break;
      }
      case GHOST_BLUE:{
        switch(pacDir){
          case DIR_LEFT:{
            targetX = pacX-8;
            targetY = pacY;
            break;
          }
          case DIR_RIGHT:{
            targetX = pacX+8;
            targetY = pacY;
            break;
          }
          case DIR_UP:{
            targetX = pacX-8;
            targetY = pacY-8;
            break;
          }
          case DIR_DOWN:{
            targetX = pacX;
            targetY = pacY+8;
            break;
          }
        }
        
        targetX = targetX - ghostX[GHOST_RED];
        targetY = targetY - ghostY[GHOST_RED];
        
        targetX = ghostX[GHOST_RED] + (targetX * 2);
        targetY = ghostY[GHOST_RED] + (targetY * 2);
        
        break;
      }
      case GHOST_PINK:{
        switch(pacDir){
          case DIR_LEFT:{
            targetX = pacX-16;
            targetY = pacY;
            break;
          }
          case DIR_RIGHT:{
            targetX = pacX+16;
            targetY = pacY;
            break;
          }
          case DIR_UP:{
            targetX = pacX-16;
            targetY = pacY-16;
            break;
          }
          case DIR_DOWN:{
            targetX = pacX;
            targetY = pacY+16;
            break;
          }
        }
        break;
      }
      case GHOST_ORANGE:{
        if(sqrt(dist(ghostX[i], ghostY[i], pacX, pacY)) > 8*4){
          targetX = pacX;
          targetY = pacY;
        }else{
          targetX = 0;
          targetY = (LEVEL_HEIGHT-1) * 4;
        }
        break;
      }
    }
  }else if(!ghostChase){
    switch(i){
      case GHOST_RED:{
        targetX = (LEVEL_WIDTH-1) * 4;
        targetY = 0;
        break;
      }
      case GHOST_BLUE:{
        targetX = (LEVEL_WIDTH-1) * 4;
        targetY = (LEVEL_HEIGHT-1) * 4;
        break;
      }
      case GHOST_PINK:{
        targetX = 0;
        targetY = 0;
        break;
      }
      case GHOST_ORANGE:{
        targetX = 0;
        targetY = (LEVEL_HEIGHT-1) * 4;
        break;
      }
    }
  }
  
  /*screen.noStroke();
  switch(i){
    case GHOST_RED:{
      screen.fill(255, 0, 0);
      break;
    }
    case GHOST_BLUE:{
      screen.fill(0, 255, 255);
      break;
    }
    case GHOST_PINK:{
      screen.fill(255, 127, 127);
      break;
    }
    case GHOST_ORANGE:{
      screen.fill(255, 127, 0);
      break;
    }
  }
  screen.rect(targetX, targetY, 4, 4);*/
    
  //Empty directions
  bool left = !solid((ghostX[i]-1) >> 2, ghostY[i] >> 2);
  bool right = !solid((ghostX[i] >> 2)+1, ghostY[i] >> 2);
  bool up = !cageUpSolid(ghostX[i] >> 2, (ghostY[i]-1) >> 2);
  bool down = !solid(ghostX[i] >> 2, (ghostY[i] >> 2)+1);
  
  if(tile(ghostX[i] >> 2, ghostY[i] >> 2) == TILE_CAGE || tile(ghostX[i] >> 2, ghostY[i] >> 2) == TILE_GATE){
    if(ghostX[i] < LEVEL_WIDTH*4/2){
      ghostDir[i] = DIR_RIGHT;
    }else if(ghostX[i] > LEVEL_WIDTH*4/2){
      ghostDir[i] = DIR_LEFT;
    }else{
      ghostDir[i] = DIR_UP;
    }
    
  }else{
    
    if(ghostX[i]%4==0 && ghostY[i]%4==0){
    
      if((ghostDir[i] == DIR_LEFT) || (ghostDir[i] == DIR_RIGHT)){
        short upDist = dist(ghostX[i], ghostY[i]-4, targetX, targetY);
        short downDist = dist(ghostX[i], ghostY[i]+4, targetX, targetY);
        short straitDist = dist(ghostX[i]+(ghostDir[i] == DIR_LEFT?-4:4), ghostY[i], targetX, targetY);
        
        if((ghostDir[i] == DIR_LEFT?left:right) && (straitDist < upDist || !up) && (straitDist < downDist || !down)){
          
        }else{
          if(up && down){
            ghostDir[i] = (upDist < downDist) ? DIR_UP : DIR_DOWN;
          }else if(up){
            ghostDir[i] = DIR_UP;
          }else if(down){
            ghostDir[i] = DIR_DOWN;
          }
        }
      } else if((ghostDir[i] == DIR_UP) || (ghostDir[i] == DIR_DOWN)){
        short leftDist = dist(ghostX[i]-4, ghostY[i], targetX, targetY);
        short rightDist = dist(ghostX[i]+4, ghostY[i], targetX, targetY);
        short straitDist = dist(ghostX[i], ghostY[i]+(ghostDir[i] == DIR_UP?-4:4), targetX, targetY);
        if((ghostDir[i] == DIR_UP?up:down) && (straitDist < leftDist || !left) && (straitDist < rightDist || !right)){
          
        }else{
          
          if(left && right){
            ghostDir[i] = (leftDist < rightDist) ? DIR_LEFT : DIR_RIGHT;
          }else if(left){
            ghostDir[i] = DIR_LEFT;
          }else if(right){
            ghostDir[i] = DIR_RIGHT;
          }
        }
      }
    
    }
  
  }
  
  if(ghostDir[i] == DIR_LEFT && left){
    ghostX[i] -= SPEED_GHOST;
    ghostY[i] = (ghostY[i] >> 2) << 2;
  }else if(ghostDir[i] == DIR_RIGHT && right){
    ghostX[i] += SPEED_GHOST;
    ghostY[i] = (ghostY[i] >> 2) << 2;
  }else if(ghostDir[i] == DIR_UP && up){
    ghostY[i] -= SPEED_GHOST;
    ghostX[i] = (ghostX[i] >> 2) << 2;
  }else if(ghostDir[i] == DIR_DOWN && down){
    ghostY[i] += SPEED_GHOST;
    ghostX[i] = (ghostX[i] >> 2) << 2;
  }
  
}

short dist(short x1, short y1, short x2, short y2){
  return ((abs(x2-x1)*abs(x2-x1))+(abs(y2-y1)*abs(y2-y1)));
}

void drawGhost(byte i){
  screen.noStroke();
  
  if(ghostScaredTime[i] > 0){
    if(ghostScaredTime[i] < GHOST_TIME_FLASHING && ghostScaredTime[i] % 4 == 0){
      screen.stroke(255, 255, 255);
    }else{
      screen.stroke(0, 0, 255);
    }
  }else{
    switch(i){
      case GHOST_RED:{
        screen.stroke(255, 0, 0);
        break;
      }
      case GHOST_BLUE:{
        screen.stroke(0, 255, 255);
        break;
      }
      case GHOST_PINK:{
        screen.stroke(255, 127, 127);
        break;
      }
      case GHOST_ORANGE:{
        screen.stroke(255, 127, 0);
        break;
      }
    }
  }
  
  //screen.rect(ghostX[i], ghostY[i], 4, 4);
  //screen.stroke(0, 0, 0);
  
  //screen.point(ghostX[i], ghostY[i]);
  screen.point(ghostX[i]+1, ghostY[i]);
  screen.point(ghostX[i]+2, ghostY[i]);
  //screen.point(ghostX[i]+3, ghostY[i]);
  screen.point(ghostX[i], ghostY[i]+1);
  screen.point(ghostX[i]+1, ghostY[i]+1);
  screen.point(ghostX[i]+2, ghostY[i]+1);
  screen.point(ghostX[i]+3, ghostY[i]+1);
  screen.point(ghostX[i], ghostY[i]+2);
  screen.point(ghostX[i]+1, ghostY[i]+2);
  screen.point(ghostX[i]+2, ghostY[i]+2);
  screen.point(ghostX[i]+3, ghostY[i]+2);
  //screen.point(ghostX[i], ghostY[i]+3);
  //screen.point(ghostX[i]+1, ghostY[i]+3);
  //screen.point(ghostX[i]+2, ghostY[i]+3);
  //screen.point(ghostX[i]+3, ghostY[i]+3);
  
  screen.point(ghostX[i]+((tick>>4)%2), ghostY[i]+3);
  screen.point(ghostX[i]+((tick>>4)%2)+2, ghostY[i]+3);
  
}

void drawPacman(){
  if(pacFrame >= 9){
    return;
  }
  
  screen.noStroke();
  screen.stroke(COLOR_R_PACMAN, COLOR_G_PACMAN, COLOR_B_PACMAN);
  //screen.rect(pacX, pacY, 4, 4);
  //screen.stroke(0, 0, 0);
  
  //screen.point(pacX, pacY);
  //screen.point(pacX+3, pacY);
  //screen.point(pacX, pacY+3);
  //screen.point(pacX+3, pacY+3);
  
  
  switch(pacFrame){
    case 0:
    case 1:
    case 3:
    case 4:{
        ////screen.point(pacX, pacY);
        screen.point(pacX+1, pacY);
        screen.point(pacX+2, pacY);
        ////screen.point(pacX+3, pacY);
        screen.point(pacX, pacY+1);
        screen.point(pacX+1, pacY+1);
        screen.point(pacX+2, pacY+1);
        screen.point(pacX+3, pacY+1);
        screen.point(pacX, pacY+2);
        screen.point(pacX+1, pacY+2);
        screen.point(pacX+2, pacY+2);
        screen.point(pacX+3, pacY+2);
        ////screen.point(pacX, pacY+3);
        screen.point(pacX+1, pacY+3);
        screen.point(pacX+2, pacY+3);
        ////screen.point(pacX+3, pacY+3);
        break;
    }
    case 2:{
      if(pacX > pacLastX){
        
        ////screen.point(pacX, pacY);
        screen.point(pacX+1, pacY);
        screen.point(pacX+2, pacY);
        ////screen.point(pacX+3, pacY);
        screen.point(pacX, pacY+1);
        screen.point(pacX+1, pacY+1);
        //screen.point(pacX+2, pacY+1);
        //screen.point(pacX+3, pacY+1);
        screen.point(pacX, pacY+2);
        screen.point(pacX+1, pacY+2);
        //screen.point(pacX+2, pacY+2);
        //screen.point(pacX+3, pacY+2);
        ////screen.point(pacX, pacY+3);
        screen.point(pacX+1, pacY+3);
        screen.point(pacX+2, pacY+3);
        ////screen.point(pacX+3, pacY+3);
        
        
      }else if(pacX < pacLastX){
        
        ////screen.point(pacX, pacY);
        screen.point(pacX+1, pacY);
        screen.point(pacX+2, pacY);
        ////screen.point(pacX+3, pacY);
        //screen.point(pacX, pacY+1);
        //screen.point(pacX+1, pacY+1);
        screen.point(pacX+2, pacY+1);
        screen.point(pacX+3, pacY+1);
        //screen.point(pacX, pacY+2);
        //screen.point(pacX+1, pacY+2);
        screen.point(pacX+2, pacY+2);
        screen.point(pacX+3, pacY+2);
        ////screen.point(pacX, pacY+3);
        screen.point(pacX+1, pacY+3);
        screen.point(pacX+2, pacY+3);
        ////screen.point(pacX+3, pacY+3);
        
        
      }else if(pacY > pacLastY){
        
        ////screen.point(pacX, pacY);
        screen.point(pacX+1, pacY);
        screen.point(pacX+2, pacY);
        ////screen.point(pacX+3, pacY);
        screen.point(pacX, pacY+1);
        screen.point(pacX+1, pacY+1);
        screen.point(pacX+2, pacY+1);
        screen.point(pacX+3, pacY+1);
        screen.point(pacX, pacY+2);
        //screen.point(pacX+1, pacY+2);
        //screen.point(pacX+2, pacY+2);
        screen.point(pacX+3, pacY+2);
        ////screen.point(pacX, pacY+3);
        //screen.point(pacX+1, pacY+3);
        //screen.point(pacX+2, pacY+3);
        ////screen.point(pacX+3, pacY+3);
        
        
      }else if(pacY < pacLastY){
        
        ////screen.point(pacX, pacY);
        //screen.point(pacX+1, pacY);
        //screen.point(pacX+2, pacY);
        ////screen.point(pacX+3, pacY);
        screen.point(pacX, pacY+1);
        //screen.point(pacX+1, pacY+1);
        //screen.point(pacX+2, pacY+1);
        screen.point(pacX+3, pacY+1);
        screen.point(pacX, pacY+2);
        screen.point(pacX+1, pacY+2);
        screen.point(pacX+2, pacY+2);
        screen.point(pacX+3, pacY+2);
        ////screen.point(pacX, pacY+3);
        screen.point(pacX+1, pacY+3);
        screen.point(pacX+2, pacY+3);
        ////screen.point(pacX+3, pacY+3);
        
        
      }else{
        ////screen.point(pacX, pacY);
        screen.point(pacX+1, pacY);
        screen.point(pacX+2, pacY);
        ////screen.point(pacX+3, pacY);
        screen.point(pacX, pacY+1);
        screen.point(pacX+1, pacY+1);
        screen.point(pacX+2, pacY+1);
        screen.point(pacX+3, pacY+1);
        screen.point(pacX, pacY+2);
        screen.point(pacX+1, pacY+2);
        screen.point(pacX+2, pacY+2);
        screen.point(pacX+3, pacY+2);
        ////screen.point(pacX, pacY+3);
        screen.point(pacX+1, pacY+3);
        screen.point(pacX+2, pacY+3);
        ////screen.point(pacX+3, pacY+3);
      }
      break;
    }
    case 5:{
      screen.point(pacX+2, pacY);
      screen.point(pacX+2, pacY+3);
    }
    case 6:{
      screen.point(pacX+1, pacY);
      screen.point(pacX+1, pacY+3);
    }
    case 7:{
      screen.point(pacX, pacY+1);
      screen.point(pacX, pacY+2);
    }
    case 8:{
      screen.point(pacX+1, pacY+1);
      screen.point(pacX+1, pacY+2);
      break;
    }
  }
}

void drawMap(){
  
  for(byte i=0;i<LEVEL_WIDTH;i++){
    for(byte j=0;j<LEVEL_HEIGHT;j++){
      drawTile(i, j);
    }
  }
  
}

void drawTile(byte x, byte y){

  if(x >= LEVEL_WIDTH){
    return;
  }
  
  byte tile = tile(x, y);
  byte tileLeft = x>0?tile(x-1, y):TILE_EMPTY;
  byte tileRight = x<LEVEL_WIDTH-1?tile(x+1, y):TILE_EMPTY;
  byte tileUp = y>0?tile(x, y-1):TILE_EMPTY;
  byte tileDown = y<LEVEL_HEIGHT-1?tile(x, y+1):TILE_EMPTY;
  
  byte x4 = x*4;
  byte y4 = y*4;
  
  
  screen.noStroke();
  screen.fill(0, 0, 0);
  screen.rect(x4, y4, 4, 4);
  
  switch(tile){
    case TILE_EMPTY:{
      break;
    }
    case TILE_WALL:{
      screen.stroke(COLOR_R_WALL, COLOR_G_WALL, COLOR_B_WALL);
      if(tileLeft != TILE_WALL){
          screen.point(x4, y4+1);
          screen.point(x4, y4+2);
          if(tileUp == TILE_WALL){
            screen.point(x4, y4);
          }
          if(tileDown == TILE_WALL){
            screen.point(x4, y4+3);
          }
      }
      if(tileRight != TILE_WALL){
          screen.point(x4+3, y4+1);
          screen.point(x4+3, y4+2);
          if(tileUp == TILE_WALL){
            screen.point(x4+3, y4);
          }
          if(tileDown == TILE_WALL){
            screen.point(x4+3, y4+3);
          }
      }
      if(tileUp != TILE_WALL){
          screen.point(x4+1, y4);
          screen.point(x4+2, y4);
          if(tileLeft == TILE_WALL){
            screen.point(x4, y4);
          }
          if(tileRight == TILE_WALL){
            screen.point(x4+3, y4);
          }
      }
      if(tileDown != TILE_WALL){
          screen.point(x4+1, y4+3);
          screen.point(x4+2, y4+3);
          if(tileLeft == TILE_WALL){
            screen.point(x4, y4+3);
          }
          if(tileRight == TILE_WALL){
            screen.point(x4+3, y4+3);
          }
      }
      break;
    }
    case TILE_GATE:{
      screen.noStroke();
      screen.fill(COLOR_R_GATE, COLOR_G_GATE, COLOR_B_GATE);
      screen.rect(x4, y4+1, 4, 2);
      break;
    }
    case TILE_PELLET:{
      screen.noStroke();
      screen.fill(COLOR_R_PELLET, COLOR_G_PELLET, COLOR_B_PELLET);
      screen.rect(x4+1, y4+1, 2, 2);
      break;
    }
    case TILE_POWER_PELLET:{
      screen.noStroke();
      screen.fill(COLOR_R_POWER_PELLET, COLOR_G_POWER_PELLET, COLOR_B_POWER_PELLET);
      screen.rect(x4, y4, 4, 4);
      screen.stroke(0, 0, 0);
      screen.point(x4, y4);
      screen.point(x4+3, y4);
      screen.point(x4, y4+3);
      screen.point(x4+3, y4+3);
      break;
    }
    case TILE_CAGE:{
      break;
    }
  }
  
}

void drawNumber(int a, short x, short y){
  int p = -1;
  byte i;
  int a2 = (a/10)+1;
  for(i=0;p<a2;i++){
    p = pow(10, i);
  }
  short mx = (i-1)*4;
  p = -1;
  for(i=0;p<a2;i++){
    p = pow(10, i);
    drawChar( '0'+((a/p)%10), x+mx-(4*i), y);
  }
}

