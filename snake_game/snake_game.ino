#include <SPI.h>

#include <SD.h>

#include <stdint.h>
#include <stdlib.h>
#include <ili9341_8_bits_fast.h>
#include <string.h>
#include <tm4c123gh6pm.h>

// Pines usados
//#define LCD_RST PD_0
//#define LCD_CS PD_1
//#define LCD_RS PD_2
//#define LCD_WR PD_3
//#define LCD_RD PA_6
// hay que modificar los pines en la librería
// DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};

#include "bitmaps.h"
//definicion de colores utilizados para pantalla y fotos
#define sGREY 0x7BEF
#define sDARKGREEN 0x0485
#define sLIGHTGREEN 0x8684
#define sBLUE 0x1BBF

//File myFile;

int mainMenuPin = 37;
int gamePin = 36;
int stopPin = 35;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);//comunicación con control

  pinMode(10, OUTPUT);
  pinMode(mainMenuPin, OUTPUT);
  pinMode(gamePin, OUTPUT);
  pinMode(stopPin, OUTPUT);
  digitalWrite(10, HIGH);
  digitalWrite(mainMenuPin, LOW);
  digitalWrite(gamePin, LOW);
  digitalWrite(stopPin, LOW);
  pinMode(16, INPUT);
  SPI.setModule(0);

  lcdInit();

   if (!SD.begin(12)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  mainMenu();
  game();

}

char coordHead; //coordenadas de la cabeza
char direction; //0 = derecha, 1 = arriba, 2 = izquierda, 3 = abajo
char snake_length; //cuerpo + cabeza
char occupiedCoords[4] = {0x77, 0x76, 0x75, 0x74};// posiciones inciales
bool snakedead = 1;
char coordApple;
bool optionMM = 0;
char data = 0; //utilizado para la lectura de uart en el controlador
char olddata = 0;

//Controlan los opciones del juego
bool wraparoundOn = 0;
bool soundOn = 1;
bool speedHigh = 0;

bool dataReceived = 0;

void mainMenu() { //menu principal 
  lcdClear(BLACK);
  if (soundOn) {digitalWrite(mainMenuPin, HIGH);
  digitalWrite(gamePin, LOW);
  digitalWrite(stopPin, LOW);}

  //myFile = SD.open("logo.txt");

  LCD_Bitmap_SD(0, 0, 320, 64, "logo.bin"); //una funcion que jala datos del SD y lo desplega en la pantalla
  //LCD_Print("Snake", 100, 50, 2, WHITE, BLACK);
  //LCD_Bitmap(42, 142, 90, 24, start);
  LCD_Print("START", 42, 142, 2, WHITE, BLACK);
  //LCD_Bitmap(173, 142, 126, 24, options);
  LCD_Print("OPTIONS", 173, 142, 2, WHITE, BLACK);

  paintArrow();


  
  while (1) { //controla en donde se posiciona la flecha de seleccion y si se va al juego o al menu de opciones
    receiveController();

    if (dataReceived) {
    if (data == 1 || data == 4) optionMM = !optionMM;
    //if ((data == 4) && data != olddata) optionMM = optionMM - 1;
    paintArrow();
    if (data == 16 && optionMM == 0) game();
    if (data == 16 && optionMM == 1) optionMenu();
    olddata = data;
    dataReceived = 0;}


  }
}


char optionOM = 0;
char pastOptionOM = 0;

void optionMenu() { //En este menu los jugadores puedan escoger si quieren jugar con wraparound, incrementar la rapidez del juego, y apagar el sonido
  data = 0;
  optionOM = 0;
  lcdClear(BLACK);
  LCD_Sprite_Zoom(143, 50, 33, 33, speed, 2, speedHigh, 0, 0, WHITE);
  LCD_Sprite_Zoom(231, 50, 33, 33, sound, 2, !soundOn, 0, 0, WHITE);
  LCD_Print("BACK", 143, 175, 2, WHITE, BLACK);

 unsigned int wrapOptionFrame = 0;
  
  //Rect(53,48, 70, 70, GREEN);
  //Rect(52,47, 72, 72, GREEN);
  while(1) {
    

    if (wraparoundOn) {LCD_Sprite_Zoom(55, 50, 33, 33, wraparound, 5, (wrapOptionFrame/5) % 4, 0, 0, WHITE);
    wrapOptionFrame++;
    delay(10); }
    else LCD_Sprite_Zoom(55, 50, 33, 33, wraparound, 5, 4, 0, 0, WHITE);
    
  receiveController();
  if(dataReceived) {
  if ((data == 1) && (optionOM != 0) && (optionOM != 3)) optionOM = optionOM + 1;//avanzar si se apacha el boton derecho
  if ((data == 4) && (optionOM != 0) && (optionOM != 1)) optionOM = optionOM - 1;//retroceder si se apacha el boton derecho
  if ((data == 8) && (optionOM != 0)) optionOM = 0; //para abajo = back
  if ((data == 2) && (optionOM == 0)) optionOM = 2; //para arriba = speed

  selectRect(pastOptionOM, BLACK);
  selectRect(optionOM, sBLUE);

  pastOptionOM = optionOM;
  dataReceived = 0;

  if (data == 16) {
    switch (optionOM) {
      case 0: mainMenu();
      break;
      case 1: wraparoundOn = !wraparoundOn;
      break;
      case 2: speedHigh = !speedHigh;
      break;
      case 3: soundOn = !soundOn;
      if (soundOn) {digitalWrite(mainMenuPin, HIGH);
  digitalWrite(gamePin, LOW);
  digitalWrite(stopPin, LOW);}
  else {
    digitalWrite(mainMenuPin, LOW);
  digitalWrite(gamePin, LOW);
  digitalWrite(stopPin, HIGH);
  }
      break;
      
    }
  LCD_Sprite_Zoom(143, 50, 33, 33, speed, 2, speedHigh, 0, 0, WHITE);
  LCD_Sprite_Zoom(231, 50, 33, 33, sound, 2, !soundOn, 0, 0, WHITE);
  }
  };

  olddata = data;
  //LCD_Sprite_Zoom(10,100, 33, 33, wraparoundbw);
}}

void selectRect (char option, char COLOR) {
  switch (option) {
    case 0:
    Rect(141,173, 70, 20, COLOR);
    Rect(140,172, 72, 21, COLOR);
    break;
    
    case 1:
    Rect(53, 48, 70, 70, COLOR);
    Rect(52, 47, 72, 72, COLOR);
    break;

    case 2:
    Rect(141,48, 70, 70, COLOR);
    Rect(140,47, 72, 72, COLOR);
    break;

    case 3:
    Rect(229,48, 70, 70, COLOR);
    Rect(228,47, 72, 72, COLOR);
    break;
  }
  return;
}

void gameOver() {
  data = 0;
  lcdClear(sDARKGREEN);
  LCD_Bitmap_SD(24, 24, 272, 192, "gameover.bin");
  //paintScoreGameOver();
  LCD_Print(String(snake_length - 3), 180, 36, 2, BLACK, sLIGHTGREEN);
  while(1) {
    receiveController();
    if (dataReceived) {
      dataReceived = 0;
      if (data == 16) digitalWrite(10, LOW); //resetear juego;
    }
    
  }
  
}



void paintArrow() { //Funcion para colocar el arrow que indica que opcion se ha seleccionado en el menu principal
  if (optionMM == 0) {
    LCD_Bitmap(75, 168, 21, 15, arrow);
    FillRect(224, 168, 21, 15, BLACK);
  }
  else {
    LCD_Bitmap(224, 168, 21, 15, arrow);
    FillRect(75, 168, 21, 15, BLACK);
  }
}

void game() {
  srand(millis());
  if (soundOn) {digitalWrite(mainMenuPin, LOW);
  digitalWrite(gamePin, HIGH);
  digitalWrite(stopPin, LOW);}
  //delete[] occupiedCoords;
  //occupiedCoords = {0x77, 0x76, 0x75, 0x74};
  
  //dibujar pantalla de juego
  lcdClear(sGREY);
  FillRect(0, 0, 240, 240, sDARKGREEN);
  FillRect(24, 24, 192, 192, sLIGHTGREEN);

  //Dibujar snake y manzana
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
    else gameOver();
  }


}

bool inmove = 0;



void snakeMove() {
  receiveController();

  switch (data) {
    case 1:
      if (direction != 2) direction = 0;
      break;
    case 2:
      if (direction != 3) direction = 1;
      break;
    case 4:
      if (direction != 0) direction = 2;
      break;
    case 8:
      if (direction != 1) direction = 3;
      break;
    default:
      direction = direction;
      break;
  }


  bool newApple = 0;
  inmove = 0;
  delay(300 - speedHigh*150);
  bool collision = 0;
  char occupiedCoordsTemp[snake_length]; //creamos un array temproal para cambiar los valores de que coordenadas estan ocupadas
  char coordHeadTemp;

  if (wraparoundOn == 0) {
  switch (direction) { //revisar si el snake esta apuntado a la direccion correcta
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

  }}
  else {
    switch (direction) { //revisar si el snake
    case 0: //derecha
      if ((coordHead & 0x0F) == 0x0F) coordHeadTemp = coordHead + 1 - 0x10;
      else    coordHeadTemp = coordHead + 1;
      break;

    case 1: //arriba
      if ((coordHead & 0xF0) == 0x00) coordHeadTemp = coordHead - 0x10;
      else coordHeadTemp = coordHead - 0x10;
      break;

    case 2: //izquierda
      if ((coordHead & 0x0F) == 0x00) coordHeadTemp = coordHead - 1 + 0x10;
      else coordHeadTemp = coordHead - 1;
      break;

    case 3: //abajo
      if ((coordHead & 0xF0) == 0xF0) coordHeadTemp = coordHead + 0x10;
      else coordHeadTemp = coordHead + 0x10;
      break;

    default:
      break;
  }}

  if (coordHeadTemp == coordApple) {
    newApple = 1;
    snake_length++;
  }

  for (int i = 0;  i < snake_length; i++) { //copiar los valores del array al nuev
    occupiedCoordsTemp[i + 1] = occupiedCoords[i];
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
    return;
  }

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

void paintApple() { //funcion para colocar la manzana que el serpiente busca
  char coord = rand() % 0xFF;
  for (int i = 1; i <= snake_length; i++) {
    if (coord == occupiedCoords[i]) coord = rand() % 0xFF;
  }

  coordApple = coord;

  paintScore();
  LCD_Bitmap(coordToLcdX(coord), coordToLcdY(coord), 12, 12, apple);

}

void paintScore() { //pinta el score durante el juego en letras negras sober un fondo gris
  int score = snake_length - 3;
  int hundreds = score / 100 % 10;
  int tens = score / 10 % 10;
  int ones = score % 10;

  LCD_Sprite_Zoom(240, 96, 12, 12, numbers, 10, hundreds, 0, 0, sGREY);
  LCD_Sprite_Zoom(264, 96, 12, 12, numbers, 10, tens, 0, 0, sGREY);
  LCD_Sprite_Zoom(288, 96, 12, 12, numbers, 10, ones, 0, 0, sGREY);
}

void paintScoreGameOver() { //pinta el score al final de juego en letras azules sobre un fondo verde
  int score = snake_length - 3;
  int hundreds = score / 100 % 10;
  int tens = score / 10 % 10;
  int ones = score % 10;

  LCD_Sprite_SD(180, 36, 36, 36, "greenbignumbers.bin", 10, hundreds, 0);
  LCD_Sprite_SD(216, 36, 36, 36, "greenbignumbers.bin", 10, tens, 0);
  LCD_Sprite_SD(252, 36, 36, 36, "greenbignumbers.bin", 10, ones, 0);
}

void receiveController() { //recibir datos del control utilizando uart
  if (Serial2.available()) {data = Serial2.read();
  Serial.println(data, BIN);
  dataReceived = 1;}
}

void loop() {


}
