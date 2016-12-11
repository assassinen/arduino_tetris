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

//Интервал считывания данных с аналогового входа
int read_delya = 100;
unsigned long start_point = millis();

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
}

byte displayArray[24] = {
0,0,0,0,
170,85,170,85,170,85,170,85,170,85,170,85,170,85,170,85,
0,0,0,0
};

static byte letters[] = {
  B00000000,
  B01100000,
  B01010110,
  B01101000,
  B01010100,
  B01010010,
  B00001100,
  B00000000,

  B00000000,
  B01100110,
  B10011001,
  B10000001,
  B01000010,
  B00100100,
  B00011000,
  B00000000
};

int str[][22] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0},
  {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0},
  {1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0},
  {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0},
  {1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

int str_[][16] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0},
  {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0},
  {1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0},
  {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0},
  {1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

int step = 0;
void output_to_screen() {
  for (int j = 0; j < number_of_string; j++) {
    for (int g = number_of_columns / number_of_string - 1; g >= 0; g--) {
      for (int i = step; i < number_of_string + step; i++) {
        if (i == number_of_string - 1 + step - j) {
          digitalWrite(data_pin, 0);   // turn the LED on (HIGH is the voltage level)
        } else {
          digitalWrite(data_pin, 1);
        }
        digitalWrite(red_data_pin, 0);
        digitalWrite(green_data_pin, str[j][g * number_of_string + i]);
        digitalWrite(clock_pin, HIGH);
        digitalWrite(clock_pin, LOW);
      }
    }
    digitalWrite(latch_pin, HIGH);
    digitalWrite(latch_pin, LOW);
    delay(len_delay);
  }
}

void push_string(int i, int led) {
  for (byte y = 0; y < 8; y++) {
    //включение строки
    digitalWrite(data_pin, !bitRead(B10000000 >> y, i));
    //вывод столбцов правой матрицы (крайний левый бит - первый)
    byte bhigh = displayArray[(y + led * 8 + 16) % 16+4];
    digitalWrite(green_data_pin, bitRead(bhigh, i));
    digitalWrite(clock_pin, HIGH);
    digitalWrite(clock_pin, LOW);
  }
}

void output_to_screen_bit() {
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
  //if (millis() - start_point >= read_delya) {
    /*
      //if (step == number_of_string-2) {
      if (step == 24) {
      step = 0;
      } else {
      step++;
      }
    */
  //  start_point = millis();
  //}
  //output_to_screen();
  output_to_screen_bit();
}

