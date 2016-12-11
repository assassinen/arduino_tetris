 // Программа управлением светодиодной матрицей
 // 8x16
 // + управление клавиатурой 6 кнопок
 
 /*  * CS - to digital pin 8  (SS pin)
  * SDI - to digital pin 11 (MOSI pin)
  * CLK - to digital pin 13 (SCK pin) 
*/


// inslude the SPI library:
#include <SPI.h>
// set pin 8 as the slave select for the digital pot:
const int displayPin = 8;

// библиотека для прерываний по таймеру
#include <MsTimer2.h> 

byte displayArray[24] = {
0,0,0,0,
170,85,170,85,170,85,170,85,170,85,170,85,170,85,170,85,
0,0,0,0
};
// текущая программа для вывода

//*****************************
// список обоев,задержек
byte arrdisplay1[19][16]={
   {192,192,48,48,12,12,3,3,192,192,48,48,12,12,3,3},
   {64,128,16,32,4,8,1,2,64,128,16,32,4,8,1,2},
   {128,64,32,16,8,4,2,1,128,64,32,16,8,4,2,1},
   
   {34,85,34,0,68,170,68,0,34,85,34,0,68,170,68,0},
   {0,120,0,0,68,68,68,0,0,120,0,0,68,68,68,0},
   {34,34,34,0,0,239,0,0,34,34,34,0,0,239,0,0},
 
   {24,24,24,24,48,24,48,48,24,24,48,48,24,24,24,48},
   {24,24,24,48,24,48,48,24,24,48,48,24,24,24,48,24},   
   {24,24,48,24,48,48,24,24,48,48,24,24,24,48,24,24},
   {24,48,24,48,48,24,24,48,48,24,24,24,48,24,24,24},
   {48,24,48,48,24,24,48,48,24,24,24,48,24,24,24,24},
   {24,48,48,24,24,48,48,24,24,24,48,24,24,24,24,48},

   {255,129,129,129,129,129,129,255,255,129,129,129,129,129,129,255},
   {255,255,195,195,195,195,255,255,255,255,195,195,195,195,255,255},
   {255,255,255,231,231,255,255,255,255,255,255,231,231,255,255,255},
   {0,0,0,24,24,0,0,0,0,0,0,24,24,0,0,0},
   {255,255,255,231,231,255,255,255,255,255,255,231,231,255,255,255},
   {255,255,195,195,195,195,255,255,255,255,195,195,195,195,255,255},
   {255,129,129,129,129,129,129,255,255,129,129,129,129,129,129,255}

 };
long arrtimes1[19]={200,200,200,100,100,100,50,50,50,50,50,50,80,80,80,150,80,80,80};
byte startpozdisplay1[]={0,3,6,12,19};
int tekpoz1=3;
int tekindex1=12;
long endmillis11=0;
int tekgame=0;
//****** подключение клавиатуры, джойстика
const int keyboardPin = A5;

struct KEYS    // структура для хранения статусов клавиш
{
  int button;      // нажатая кнопка
  int joystickX;      // x
  int joystickY;      // y
  int joystickZ;      // z
  long millisbutton[7];      // millis для button 
  long millis2;      // millis для joystick 
};
struct OFFSET_DISPLAY    // структура для хранения смещений экранов
{
  int offsetX;      // x
  int offsetY;      // y
  int deltaX;      // dX
  int deltaY;      // dy
};
struct FIGURA    // структура для хранения фигуры на экране
{
  int driving;
  int offsetX;
  int offsetY;
  int dX;      // dX
  int dY;      // dy
  int dT;      // поворот 90 градусов
  int figura;
  int length;
  long millis1;
};

// для тетриса фигуры
byte arrFigure[21][5]={
     {0,0,0,0,0},{15,0,0,0,4},{1,1,1,1,1},{3,3,0,0,2}, 
     {7,1,0,0,3},{7,4,0,0,3},{1,7,0,0,3},{4,7,0,0,3}, 
     {3,1,1,0,2},{3,2,2,0,2},{1,1,3,0,2},{2,2,3,0,2},
     {7,2,0,0,3},{2,3,2,0,2},{2,7,0,0,3},{1,3,1,0,2},
     {7,5,0,0,3},{3,1,3,0,2},{5,7,0,0,3},{3,2,3,0,2},
     {2,7,2,0,3}};
byte transFigure[21]={0,2,1,3,9,8,10,11,5,7,4,6,13,14,15,12,17,18,19,16,20};

int speed2=200;

KEYS KEYS1={0,0,0,0,{0,0,0,0,0,0,0},0};
OFFSET_DISPLAY OFFSET_DISPLAY1={0,0,0,0};
FIGURA FIGURA1={0,3,20,0,0,0,0,0,0};


void setup() {
  // set the slaveSelectPin as an output:
  pinMode (displayPin, OUTPUT);
  // initialize SPI:
  SPI.begin(); 
  // запуск прерывания по таймеру
  MsTimer2::set(2, showDisplay); //  
  MsTimer2::start(); 
  clearDisplay();
  
  Serial.begin(9600);
}

void loop() {
  int valbutton;
  // go through the six channels of the digital pot:
  switch(tekgame)
   {
   case 0: game1();
      break;
   case 1: if(!game2()) 
             {clearDisplay();tekgame=1;}
      break;
   case 2: game3();
      break;
   default: break;
   }
   // опрос клавиатуры
   valbutton=analogRead(keyboardPin);
   if(valbutton<1000)
     {buttonClick2(buttonClick1(valbutton));}

}

//////****************************
// обработка прырывания по таймеру
// динамическая индикация
void showDisplay() {
  // take the SS pin low to select the chip:
  for(byte i=0;i<8;i++)
   {
   digitalWrite(displayPin,LOW);
   // 4,3
   // из-за ошибки пайки 
   byte bhigh=displayArray[(i+OFFSET_DISPLAY1.offsetY+8+16)%16+4];
   bhigh=circleShift(bhigh,OFFSET_DISPLAY1.offsetX);
   SPI.transfer(correctError(255-bhigh));
   SPI.transfer(B00000001<<i);
   // 2,1
   byte blow=255-displayArray[(i+OFFSET_DISPLAY1.offsetY+16)%16+4];
   blow=circleShift(blow,OFFSET_DISPLAY1.offsetX);
   SPI.transfer(blow);
   SPI.transfer(B00000001<<i);
   // take the SS pin high to de-select the chip:
   digitalWrite(displayPin,HIGH); 
   //digitalWrite(slaveSelectPin,LOW);
   ////****************
   delayMicroseconds(1000); 
   }
}
// коррекция ошибки пайки
byte correctError(byte a)
 {
 int arr[]={7,6,5,4,3,2,1,0};
 byte correct=B00000000;
 for(int i=0;i<8;i++)
   bitWrite(correct,i,bitRead(a,arr[i]));
  return correct; 
  }
// циклический сдвиг байта
byte circleShift(byte var,int offset)
 {
 byte varnew;
 for(int i=0;i<8;i++)
   bitWrite(varnew,i,bitRead(var,(i+offset+8)%8));
 return varnew;  
 } 
// очистка дисплея
void clearDisplay() {
  for(int i=0;i<sizeof(displayArray);i++)
     displayArray[i]=0;
}
//////****************************
// режим1 - обои
void game1() {
  if((millis()-endmillis11)>arrtimes1[tekindex1])
   {
    OFFSET_DISPLAY1.offsetX=(OFFSET_DISPLAY1.offsetX+OFFSET_DISPLAY1.deltaX)%8;
    OFFSET_DISPLAY1.offsetY=(OFFSET_DISPLAY1.offsetY+OFFSET_DISPLAY1.deltaY)%8;
    Serial.print("offsetX=");Serial.print(OFFSET_DISPLAY1.offsetX);
    Serial.print(" offsetY=");Serial.print(OFFSET_DISPLAY1.offsetY);
    Serial.println(""); 
    changeDisplay1(); 
    endmillis11=millis();
   }
}
// новый экран
void changeDisplay1() {
  tekindex1++; 
  if(tekindex1==startpozdisplay1[tekpoz1+1])  
    tekindex1=startpozdisplay1[tekpoz1];
  for(int i=4;i<sizeof(displayArray)-4;i++)
    displayArray[i]=arrdisplay1[tekindex1][i-4];
}
//////******************************
// обработка нажатия кнопки
int buttonClick1(int val)
 {
  if(val>650) {KEYS1.button=1;return 1;} 
  if(val>600) {KEYS1.button=2;return 2;} 
  if(val>530) {KEYS1.button=3;return 3;} 
  if(val>450) {KEYS1.button=4;return 4;} 
  if(val>300) {KEYS1.button=5;return 5;} 
  if(val>200) {KEYS1.button=6;return 6;}
  return 0;  
  }
  
   void buttonClick2(int val)
 {
   if(millis()-KEYS1.millisbutton[val]<100)
     return;
   KEYS1.millisbutton[val]=millis();
   KEYS1.button=val;
   switch(val) {
    case 1: OFFSET_DISPLAY1.deltaX=1;OFFSET_DISPLAY1.deltaY=0;
        FIGURA1.dX=FIGURA1.dX-1;                                        // 
        Serial.println("left");
        break; 
    case 2: 
        OFFSET_DISPLAY1.deltaX=0;OFFSET_DISPLAY1.deltaY=-1;  // game1
        FIGURA1.dT=1;                                        // game2 (поворот 90)
        Serial.println("up");
       break; 
    case 3: OFFSET_DISPLAY1.deltaX=0;OFFSET_DISPLAY1.deltaY=1;
        Serial.println("down");
       break; 
    case 4: OFFSET_DISPLAY1.deltaX=-1;OFFSET_DISPLAY1.deltaY=0;
        FIGURA1.dX=FIGURA1.dX+1;                                        // 
       Serial.println("right");
        break; 
    case 5: OFFSET_DISPLAY1.deltaX=0;OFFSET_DISPLAY1.deltaY=0;
        // переключение на другую программу
        tekpoz1=(tekpoz1+1)%(sizeof(startpozdisplay1)-1);
        tekindex1=startpozdisplay1[tekpoz1];
        speed2=max(speed2-25,5);
        Serial.println("white");
        Serial.println(speed2);
       break; 
    case 6: 
        OFFSET_DISPLAY1.deltaX=0;OFFSET_DISPLAY1.deltaY=0;
        OFFSET_DISPLAY1.offsetX=0;OFFSET_DISPLAY1.offsetY=0;
        clearDisplay();
        tekgame=(tekgame+1)%3;
        Serial.println("red");
        break; 
    default: 
        break; 
   }   
 }
 ////**********************
 // игра тетрис
 boolean game2()
  {
   if(FIGURA1.driving==0)
     add_figura(); 
   if((millis()-FIGURA1.millis1)>speed2 && FIGURA1.driving>0)
     {
     //figura_clear(FIGURA1.offsetY);  
     // по X
     if(FIGURA1.dX!=0)
        {
        int dxpr=FIGURA1.dX;
        int offxpr=FIGURA1.offsetX+dxpr;
        offxpr=max(offxpr,0);
        offxpr=min(offxpr,8-FIGURA1.length);       
        if(!collision1(FIGURA1.offsetY,FIGURA1.figura,FIGURA1.figura,FIGURA1.offsetX,offxpr))
         {
        figura_clear(FIGURA1.offsetY,FIGURA1.offsetX);
        FIGURA1.offsetX=FIGURA1.offsetX+FIGURA1.dX;
        FIGURA1.offsetX=max(FIGURA1.offsetX,0);
        FIGURA1.offsetX=min(FIGURA1.offsetX,8-FIGURA1.length);       
        FIGURA1.dX=0;
        Serial.println(FIGURA1.offsetX);    
        figura_driving(FIGURA1.offsetY,0,FIGURA1.figura);
         }
        
        //figura_clear(FIGURA1.offsetY);
       }
     // поворот 90 градусов
     if(FIGURA1.dT>0)
        {
        if(!collision1(FIGURA1.offsetY,transFigure[FIGURA1.figura],FIGURA1.figura,FIGURA1.offsetX,FIGURA1.offsetX))
         {
         figura_clear(FIGURA1.offsetY,FIGURA1.offsetX);
         figura_driving(FIGURA1.offsetY,0,transFigure[FIGURA1.figura]);
         FIGURA1.figura=transFigure[FIGURA1.figura];
         FIGURA1.length=arrFigure[FIGURA1.figura][4];
         }
        FIGURA1.dT=0;
        //figura_clear(FIGURA1.offsetY);
       }
     // вниз по времени
     if(collision2(FIGURA1.offsetY-1,FIGURA1.figura,FIGURA1.figura,FIGURA1.offsetX,FIGURA1.offsetX))
       {FIGURA1.millis1=millis();FIGURA1.driving=0;
       if(FIGURA1.offsetY==20)
         return false;
       else
        return true;
       }
     figura_clear(FIGURA1.offsetY,FIGURA1.offsetX);  
     figura_driving(FIGURA1.offsetY-1,0,FIGURA1.figura);
     FIGURA1.offsetY=FIGURA1.offsetY-1;
     if(FIGURA1.offsetY<5)
       FIGURA1.driving=0;
    //
     
        
    FIGURA1.dX=0;FIGURA1.dY=0;FIGURA1.dT=0;
    FIGURA1.millis1=millis();
    }
  return true;
     
  }
 // контроль столкновения с кирпичиками поворот,в сторону
 boolean collision1(int yd,int figura,int figurapr,int dx,int ddx)
  {
  byte opand,disp;
  boolean collision=false;
  for(int i=0;i<4;i++) // по высоте фигуры - 4
    {
    disp=displayArray[yd+i];
    //if(i>0)
    disp=disp-(arrFigure[figurapr][i]<<dx);  
    opand=disp & (arrFigure[figura][i]<<ddx); 
    Serial.print("dx=");Serial.print(dx); 
    Serial.print(" ddx=");Serial.print(ddx); 
    Serial.print("  f=");Serial.print(arrFigure[figura][i]<<dx); 
    Serial.print("  ff=");Serial.print(arrFigure[figura][i]<<(ddx)); 
    Serial.print(" opand=");Serial.println(opand); 
    if(opand>0)
      {collision=true;Serial.println("$$$$$$$$$$$$$$");}
    } 
   Serial.println("*********************"); 
   return collision; 
  }
  // контроль столкновения с кирпичиками вниз
 boolean collision2(int yd,int figura,int figurapr,int dx,int ddx)
  {
  byte opand,disp;
  boolean collision=false;
  for(int i=0;i<4;i++) // по высоте фигуры - 4
    {
    disp=displayArray[yd+i];
    if(i>0)
      disp=disp-(arrFigure[figurapr][i-1]<<dx);  
    opand=disp & (arrFigure[figura][i]<<dx); 
    if(opand>0)
      {collision=true;Serial.println("$$$$$$$$$$$$$$");}
    } 
   return collision; 
  } 
 // добавить фигуру на поле
 void add_figura()
  {
  FIGURA1.figura=random(1,sizeof(transFigure));
  FIGURA1.dX=0;FIGURA1.dY=0;FIGURA1.dT=0;
  FIGURA1.offsetX=3;FIGURA1.offsetY=20;
  FIGURA1.millis1=millis();
  FIGURA1.length=arrFigure[FIGURA1.figura][4];
  for(int i=0;i<4;i++)
    {displayArray[FIGURA1.offsetY+i]=arrFigure[FIGURA1.figura][i]<<FIGURA1.offsetX;}
  FIGURA1.driving=1;  
  }
// очищение изображения фигуры
void  figura_clear(int y,int x)
  {
   for(int i=0;i<4;i++)
    displayArray[y+i]=displayArray[y+i]-(arrFigure[FIGURA1.figura][i]<<x);
   }
// перемещение фигуры
void  figura_driving(int y,int x,int figura)
  {
   for(int i=0;i<4;i++)
    displayArray[y+i]=displayArray[y+i]+(arrFigure[figura][i]<<FIGURA1.offsetX);
  }

