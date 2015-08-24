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

#define SPEED_PACMAN 1
#define SPEED_GHOST 1

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
{0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 1, 5, 5, 5, 5, 5, 5, 1, 0, 0, 0, 3, 0, 0, 0, 0, 0,  },
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
byte ghostMode[4] = {0, 0, 0, 0};

int score = 0;
byte lives = 0;
int timer = 0;
long lastMillis = 0;

void setup() {
  Serial.begin(9600);

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
  lives = 3;
  timer = 0;
  
  pacX = (13*4)+2;
  pacY = 23*4;
  pacDir = DIR_NONE;
  pacLastX = pacX;
  pacLastY = pacY;
  pacFrame = 0;
  
  ghostX[GHOST_RED] = (13*4)+2;
  ghostY[GHOST_RED] = 11*4;
  
  ghostX[GHOST_BLUE] = (11*4)+2;
  ghostY[GHOST_BLUE] = 14*4;
  
  ghostX[GHOST_PINK] = (13*4)+2;
  ghostY[GHOST_PINK] = 14*4;
  
  ghostX[GHOST_ORANGE] = (15*4)+2;
  ghostY[GHOST_ORANGE] = 14*4;
  
  for(byte i=0;i<4;i++){
    ghostLastX[i] = ghostX[i];
    ghostLastY[i] = ghostY[i];
    ghostDir[i] = DIR_NONE;
    ghostMode[i] = 2;
  }
  
  ghostDir[GHOST_RED] = DIR_LEFT;
  ghostDir[GHOST_PINK] = DIR_LEFT;
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
  
  delay(10);
  
  if(tick % 15 == 0){
    pacFrame++;
    if(pacFrame == 3){
      pacFrame = 1;
    }
  }
  
  //Scared = ?1 = 1 or 3
  //Chase = b10 = 2
  //Scatter = b00 = 0

  //mode | 2 = (00 or 01) to (10 or 11)
  //mode & 1 = (10 or 11) to (00 or 01)
  
  switch(timer){
    case 0:{
      for(byte i=0;i<4;i++){
        ghostMode[i] = ghostMode[i] | 2;
      }
      break;
    }
    case (7):{
      for(byte i=0;i<4;i++){
        ghostMode[i] = ghostMode[i] & 1;
        ghostDir[GHOST_BLUE] = DIR_RIGHT;
      }
      break;
    }
    case (7+20):{
      for(byte i=0;i<4;i++){
        ghostMode[i] = ghostMode[i] | 2;
        ghostDir[GHOST_ORANGE] = DIR_LEFT;
      }
      break;
    }
    case (7+20+7):{
      for(byte i=0;i<4;i++){
        ghostMode[i] = ghostMode[i] & 1;
      }
      break;
    }
    case (7+20+7+20):{
      for(byte i=0;i<4;i++){
        ghostMode[i] = ghostMode[i] | 2;
      }
      break;
    }
    case (7+20+7+20+5):{
      for(byte i=0;i<4;i++){
        ghostMode[i] = ghostMode[i] & 1;
      }
      break;
    }
    case (7+20+7+20+5+20):{
      for(byte i=0;i<4;i++){
        ghostMode[i] = ghostMode[i] | 2;
      }
      break;
    }
    case (7+20+7+20+5+20+5):{
      for(byte i=0;i<4;i++){
        ghostMode[i] = ghostMode[i] & 1;
      }
      break;
    }
  }

  if(millis() - lastMillis > 1000){
    timer++;
    lastMillis = millis();
  }
  
  if(tick % 5 == 0){

    if(pacX == 0){
      pacX = (LEVEL_WIDTH-1)*4;
    }else if(pacX == (LEVEL_WIDTH-1)*4){
      pacX = 0;
    }
    
    for(byte i = 0;i<4;i++){
      if(ghostX[i] == 0){
        ghostX[i] = (LEVEL_WIDTH-1)*4;
      }else if(ghostX[i] == (LEVEL_WIDTH-1)*4){
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

    ghostAi(0);
    ghostAi(1);
    ghostAi(2);
    ghostAi(3);
    
    if(tile(pacLastX >> 2, pacLastY >> 2) == TILE_PELLET){
      tile(pacLastX >> 2, pacLastY >> 2) = TILE_EMPTY;
      score++;
    }
    
    if(tile(pacLastX >> 2, pacLastY >> 2) == TILE_POWER_PELLET){
      tile(pacLastX >> 2, pacLastY >> 2) = TILE_EMPTY;
      
      score += 10;
    }
  }
  
}

void ghostAi(byte i){
  
  if(ghostDir[i] == DIR_NONE){
    return;
  }
  
  byte targetX;
  byte targetY;
  
  if(ghostMode[i] == 2){ //not scared and chase mode
    targetX = pacX;
    targetY = pacY;
  }else if(ghostMode[i] == 0){ //net scared and scatter mode
    switch(i){
      case GHOST_RED:{
        targetX = LEVEL_WIDTH * 4;
        targetY = 0;
      }
      case GHOST_BLUE:{
        targetX = LEVEL_WIDTH * 4;
        targetY = LEVEL_HEIGHT * 4;
      }
      case GHOST_PINK:{
        targetX = 0;
        targetY = 0;
      }
      case GHOST_ORANGE:{
        targetX = 0;
        targetY = LEVEL_HEIGHT * 4;
      }
    }
  }else{
    //Scared mode TODO
  }
    
  //Empty directions
  bool left = !solid((ghostX[i]-1) >> 2, ghostY[i] >> 2);
  bool right = !solid((ghostX[i] >> 2)+1, ghostY[i] >> 2);
  bool up = !cageUpSolid(ghostX[i] >> 2, (ghostY[i]-1) >> 2);
  bool down = !solid(ghostX[i] >> 2, (ghostY[i] >> 2)+1);
  
  if(tile(ghostX[i] >> 2, ghostY[i] >> 2) == TILE_CAGE || tile(ghostX[i] >> 2, ghostY[i] >> 2) == TILE_GATE){
    if(ghostX[i] < LEVEL_WIDTH/2){
      ghostDir[i] = DIR_RIGHT;
    }else if(ghostX[i] < LEVEL_WIDTH/2){
      ghostDir[i] = DIR_LEFT;
    }else{
      ghostDir[i] = DIR_UP;
    }
    
  }else{
    
    if(ghostX[i]%4==0 && ghostY[i]%4==0){
    
      if((ghostDir[i] == DIR_LEFT) || (ghostDir[i] == DIR_RIGHT)){
        short upDist = distSqr(ghostX[i], ghostY[i]-4, targetX, targetY);
        short downDist = distSqr(ghostX[i], ghostY[i]+4, targetX, targetY);
        short straitDist = distSqr(ghostX[i]+(ghostDir[i] == DIR_LEFT?-4:4), ghostY[i], targetX, targetY);
        if((ghostDir[i] == DIR_LEFT?left:right) && straitDist < upDist && straitDist < downDist){
          
        }else{
          if(up && down){
            ghostDir[i] = upDist < downDist ? DIR_UP : DIR_DOWN;
          }else if(up){
            ghostDir[i] = DIR_UP;
          }else if(down){
            ghostDir[i] = DIR_DOWN;
          }
        }
      } else if((ghostDir[i] == DIR_UP) || (ghostDir[i] == DIR_DOWN)){
        short leftDist = distSqr(ghostX[i]-4, ghostY[i], targetX, targetY);
        short rightDist = distSqr(ghostX[i]+4, ghostY[i], targetX, targetY);
        short straitDist = distSqr(ghostX[i], ghostY[i]+(ghostDir[i] == DIR_UP?-4:4), targetX, targetY);
        if((ghostDir[i] == DIR_UP?up:down) && straitDist < leftDist && straitDist < rightDist){
          
        }else{
          if(left && right){
            ghostDir[i] = leftDist < rightDist ? DIR_LEFT : DIR_RIGHT;
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

short distSqr(short x1, short y1, short x2, short y2){
  return ((x2-x1)*(x2-x1))+((y2-y1)*(y2-y1));
}

void drawGhost(byte i){
  screen.noStroke();
  
  if(ghostMode[i] & 1){
    screen.fill(0, 0, 255);
  }else{
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
  }
  
  screen.rect(ghostX[i], ghostY[i], 4, 4);
  screen.stroke(0, 0, 0);
  screen.point(ghostX[i], ghostY[i]);
  screen.point(ghostX[i]+3, ghostY[i]);
  screen.point(ghostX[i]+((tick>>4)&1), ghostY[i]+3);
  screen.point(ghostX[i]+((tick>>4)&1)+2, ghostY[i]+3);
  
}

void drawPacman(){
  
  screen.noStroke();
  screen.fill(COLOR_R_PACMAN, COLOR_G_PACMAN, COLOR_B_PACMAN);
  screen.rect(pacX, pacY, 4, 4);
  screen.stroke(0, 0, 0);
  screen.point(pacX, pacY);
  screen.point(pacX+3, pacY);
  screen.point(pacX, pacY+3);
  screen.point(pacX+3, pacY+3);
  
  switch(pacFrame){
    case 2:{
      if(pacX > pacLastX){
        screen.point(pacX+2, pacY+1);
        screen.point(pacX+3, pacY+1);
        screen.point(pacX+2, pacY+2);
        screen.point(pacX+3, pacY+2);
      }else if(pacX < pacLastX){
        screen.point(pacX, pacY+1);
        screen.point(pacX+1, pacY+1);
        screen.point(pacX, pacY+2);
        screen.point(pacX+1, pacY+2);
      }else if(pacY > pacLastY){
        screen.point(pacX+1, pacY+2);
        screen.point(pacX+1, pacY+3);
        screen.point(pacX+2, pacY+2);
        screen.point(pacX+2, pacY+3);
      }else if(pacY < pacLastY){
        screen.point(pacX+1, pacY);
        screen.point(pacX+1, pacY+1);
        screen.point(pacX+2, pacY);
        screen.point(pacX+2, pacY+1);
      }
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

