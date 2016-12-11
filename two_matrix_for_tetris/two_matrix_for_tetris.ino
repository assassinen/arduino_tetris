#include <MsTimer2.h>
#include <TimerOne.h>

//*****************************
// список обоев,задержек
byte arrdisplay1[19][16] = {
  {192, 192, 48, 48, 12, 12, 3, 3, 192, 192, 48, 48, 12, 12, 3, 3},
  {64, 128, 16, 32, 4, 8, 1, 2, 64, 128, 16, 32, 4, 8, 1, 2},
  {128, 64, 32, 16, 8, 4, 2, 1, 128, 64, 32, 16, 8, 4, 2, 1},

  {34, 85, 34, 0, 68, 170, 68, 0, 34, 85, 34, 0, 68, 170, 68, 0},
  {0, 120, 0, 0, 68, 68, 68, 0, 0, 120, 0, 0, 68, 68, 68, 0},
  {34, 34, 34, 0, 0, 239, 0, 0, 34, 34, 34, 0, 0, 239, 0, 0},

  {24, 24, 24, 24, 48, 24, 48, 48, 24, 24, 48, 48, 24, 24, 24, 48},
  {24, 24, 24, 48, 24, 48, 48, 24, 24, 48, 48, 24, 24, 24, 48, 24},
  {24, 24, 48, 24, 48, 48, 24, 24, 48, 48, 24, 24, 24, 48, 24, 24},
  {24, 48, 24, 48, 48, 24, 24, 48, 48, 24, 24, 24, 48, 24, 24, 24},
  {48, 24, 48, 48, 24, 24, 48, 48, 24, 24, 24, 48, 24, 24, 24, 24},
  {24, 48, 48, 24, 24, 48, 48, 24, 24, 24, 48, 24, 24, 24, 24, 48},

  {255, 129, 129, 129, 129, 129, 129, 255, 255, 129, 129, 129, 129, 129, 129, 255},
  {255, 255, 195, 195, 195, 195, 255, 255, 255, 255, 195, 195, 195, 195, 255, 255},
  {255, 255, 255, 231, 231, 255, 255, 255, 255, 255, 255, 231, 231, 255, 255, 255},
  {0, 0, 0, 24, 24, 0, 0, 0, 0, 0, 0, 24, 24, 0, 0, 0},
  {255, 255, 255, 231, 231, 255, 255, 255, 255, 255, 255, 231, 231, 255, 255, 255},
  {255, 255, 195, 195, 195, 195, 255, 255, 255, 255, 195, 195, 195, 195, 255, 255},
  {255, 129, 129, 129, 129, 129, 129, 255, 255, 129, 129, 129, 129, 129, 129, 255}

};

byte displayArray[24] = {
  0, 0, 0, 0,
  170, 85, 170, 85, 170, 85, 170, 85, 170, 85, 170, 85, 170, 85, 170, 85,
  0, 0, 0, 0
};

//Пин подключен к ST_CP (12) входу 74HC595
int latch_pin = 8;
//Пин подключен к SH_CP (11) входу 74HC595
int clock_pin = 12;
//Пин подключен к DS (14) входу 74HC595
int data_pin = 11;

int green_data_pin = 3;
int red_data_pin = 4;
int reset_pin = 2;
int number_of_columns = 16;
int number_of_string = 8;
int len_delay = 1;

void setup() {
  pinMode(latch_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);
  pinMode(green_data_pin, OUTPUT);
  pinMode(red_data_pin, OUTPUT);
  pinMode(reset_pin, OUTPUT);
  Serial.begin(9600);
  digitalWrite(reset_pin, LOW);
  digitalWrite(reset_pin, HIGH);
  MsTimer2::set(2, showDisplay); //
  MsTimer2::start();
}

void push_string(int i, int led) {
  for (byte y = 0; y < 8; y++) {
    //включение строки
    digitalWrite(data_pin, !bitRead(B10000000 >> y, i));
    //вывод столбцов правой матрицы (крайний левый бит - первый)
    byte bhigh = displayArray[(y + led * 8 + 16) % 16 + 4];
    //byte bhigh = arrdisplay1[14][(y + led * 8 + 16) % 16];
    digitalWrite(green_data_pin, bitRead(bhigh, i));
    digitalWrite(clock_pin, HIGH);
    digitalWrite(clock_pin, LOW);
  }
}

void showDisplay() {
  for (byte i = 0; i < 8; i++) {
    push_string(i, 1);
    push_string(i, 0);
    digitalWrite(latch_pin, HIGH);
    digitalWrite(latch_pin, LOW);
    delayMicroseconds(1000);
  }
}

// the loop function runs over and over again forever
void loop() {

}

