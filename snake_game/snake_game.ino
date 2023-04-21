#include <stdint.h>
#include <stdlib.h>
#include <ili9341_8_bits_fast.h>
#include <string.h>

// Pines usados
//#define LCD_RST PD_0
//#define LCD_CS PD_1
//#define LCD_RS PD_2
//#define LCD_WR PD_3
//#define LCD_RD PA_6
// hay que modificar los pines en la librería
// DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};

#include "bitmaps.h"
#define sGREY 0x7BEF
#define sDARKGREEN 0x0485
#define sLIGHTGREEN 0x8684
#define sBLUE 0x1BBF

void setup() {
  Serial.begin(9600);
  lcdInit();

  interrupts();
   pinMode(37, INPUT_PULLUP);//PB2 como input y interrupt
  attachInterrupt(digitalPinToInterrupt(37), up, RISING);
  pinMode(36, INPUT_PULLUP);//PB2 como input y interrupt
  attachInterrupt(digitalPinToInterrupt(36), right, RISING);
   pinMode(35, INPUT_PULLUP);//PB2 como input y interrupt
  attachInterrupt(digitalPinToInterrupt(35), left, RISING);
  pinMode(34, INPUT_PULLUP);//PB2 como input y interrupt
  attachInterrupt(digitalPinToInterrupt(34), down, RISING);
  
  game();
  
  //LCD_Bitmap(1,1, 320, 240, game_screen);
}

char coordHead; //coordenadas de la cabeza
char direction; //0 = derecha, 1 = arriba, 2 = izquierda, 3 = abajo
char snake_length; //cuerpo + cabeza
char occupiedCoords[4] = {0x77, 0x76, 0x75, 0x74};// posiciones inciales
bool snakedead = 1;
char coordApple;

void game() {
  lcdClear(sGREY);
  FillRect(0, 0, 240, 240, sDARKGREEN);
  FillRect(24, 24, 192, 192, sLIGHTGREEN);
  direction = 0; //hacia la derecha
  coordHead = 0x77; //centro de la escena
  snake_length = 3;
  snakedead = 0;
  paintApple();
  

  paintHead(coordHead);
  for (int x = 1; x < 4; x++) {
    paintBody(0x77 - x);
  }

  while (1) {
    if (snakedead == 0) snakeMove();
    //digitalWrite(30, HIGH);
  }
  
  
}

  bool inmove = 0;
  
void snakeMove() {
  bool newApple = 0;
  inmove = 0;
  delay(300);
  bool collision = 0;
  char occupiedCoordsTemp[snake_length]; //creamos un array temproal para cambiar los valores de que coordenadas estan ocupadas
  char coordHeadTemp;

  switch (direction) { //revisar si el snake 
    case 0: //derecha
    if ((coordHead & 0x0F) == 0x0F) snakedead = 1;
    else    coordHeadTemp = coordHead + 1;
    break;
    
    case 1: //arriba
    if ((coordHead & 0xF0) == 0x00) snakedead = 1;
    else coordHeadTemp = coordHead - 0x10;
    break;

    case 2: //izquierda
    if ((coordHead & 0x0F) == 0x00) snakedead = 1;
    else coordHeadTemp = coordHead - 1;
    break;

    case 3: //abajo
    if ((coordHead & 0xF0) == 0xF0) snakedead = 1;
    else coordHeadTemp = coordHead + 0x10;
    break;

    default:
    break;

}

if (coordHeadTemp == coordApple) {newApple = 1;
snake_length++;}

  for (int i = 0;  i < snake_length; i++) { //copiar los valores del array al nuev 
    occupiedCoordsTemp[i+1] = occupiedCoords[i];
  }

  for (int i = 1; i <= snake_length; i++) {
    if (coordHeadTemp == occupiedCoordsTemp[i]) {
      collision = 1;
      break;
    }
  }

  if (collision == 1) snakedead = 1;

  if (snakedead == 1) {
    paintHeadDead(coordHead);
  return;}

  paintTile(occupiedCoords[snake_length]); //pintar sobre el ultimo segmento del cuerpo con un tile cuerpo
  paintBody(occupiedCoords[0]);
  paintHead(coordHeadTemp);

  occupiedCoordsTemp[0] = coordHeadTemp;
  coordHead = coordHeadTemp; 

  for (int i = 0; i <= snake_length; i++)
  occupiedCoords[i] = occupiedCoordsTemp[i];

  if (newApple == 1) paintApple();
  }

int coordToLcdX(unsigned char c) { //funcion para transformar medidas del sistema de coordenadas del juego a medidas para el juego
  int x = ((c & 0x0F) + 2) * 12;
  return x;
}

int coordToLcdY(unsigned char c) { //funcion para transformar medidas del sistema de coordenadas del juego a medidas para el juego
  int y = ((c >> 4) + 2) * 12;
  return y;
}

void paintHead(unsigned char coord) { //funcion para colocar el sprite de la cabeza en su direccion correcta
  LCD_Sprite(coordToLcdX(coord), coordToLcdY(coord), 12, 12, snake, 4, direction, 0, 0);
}

void paintHeadDead(unsigned char coord) { //funcion para colocar el sprite de la cabeza muerta en su direccion correcta
  LCD_Sprite(coordToLcdX(coord), coordToLcdY(coord), 12, 12, snakeDead, 4, direction, 0, 0);
}

void paintBody(unsigned char coord) { //funcion para colocar el cuerpo
  FillRect(coordToLcdX(coord), coordToLcdY(coord), 12, 12, sBLUE);
}

void paintTile(unsigned char coord) { //funcion para pintar sobre algo con el fondo
  FillRect(coordToLcdX(coord), coordToLcdY(coord), 12, 12, sLIGHTGREEN);
}

void paintApple() {
  char coord = rand() % 0xFF;
  for (int i = 1; i <= snake_length; i++) {
    if (coord == occupiedCoords[i]) coord = rand() % 0xFF;
   }

  coordApple = coord;
  
  paintScore();
  LCD_Bitmap(coordToLcdX(coord), coordToLcdY(coord), 12, 12, apple);
  
}

void paintScore() {
  int score = snake_length - 3;
  int hundreds = score/100 % 10;
  int tens = score/10 % 10;
  int ones = score % 10;
  
  LCD_Sprite(240, 96, 24, 24, numbers, 10, hundreds, 0, 0);
  LCD_Sprite(264, 96, 24, 24, numbers, 10, tens, 0, 0);
  LCD_Sprite(288, 96, 24, 24, numbers, 10, ones, 0, 0);
}

void right() {if ((direction != 2) && (inmove == 0)) 
{direction = 0;
inmove = 1;}}
void up() {if ((direction != 3) && (inmove == 0)) 
{direction = 1;
inmove = 1;}}
void left() {if ((direction != 0) && (inmove == 0)) 
{direction = 2;
inmove = 1;}}
void down() {if ((direction != 1) && (inmove == 0)) 
{direction = 3;
inmove = 1;}}



void loop() {


}
