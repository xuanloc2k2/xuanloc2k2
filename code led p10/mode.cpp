
#define BLYNK_PRINT Serial 
#define BLYNK_TEMPLATE_ID "TMPL6BUnrCnJG"
#define BLYNK_TEMPLATE_NAME "p10"
#define BLYNK_AUTH_TOKEN "yGJmnImyIbwzdBnL9QnZwytk_Xjf9jY4"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "time.h"

String tu  = "HELLO WORLD      ";
const char* ntpServer = "pool.ntp.org"; 
byte hour_i, minute_i;
uint8_t oldc = 0;
uint8_t oldh = 0;
uint8_t old_state = 0;

uint8_t mode = 50;

int pixel_h;
int pixel_c;
int value_R = 1;
int value_G;
int value_B;
int brightness = 1000;
int q = 0;           
int e = 0;
int tam = 0;


unsigned long lastTime = 0;
unsigned long interval = 50;
int ha;
int co;

#define A_PIN 1
#define B_PIN 15
#define R1_PIN 4
#define G1_PIN 5
#define B1_PIN 16
#define R2_PIN 13
#define G2_PIN 12
#define B2_PIN 14

#define CLK_PIN 0
#define LAT_PIN 2
#define OE_PIN 3

#define ROWS 16
#define COLS 8  // 8 byte = 64 cột

int quetled = 128;
int x,y,byteindex,bitindex,row;

byte chu[][16] = {
{0x00,0x00,0x00,0x00, 0x18,0x3C,0x66,0x66,0x7E,0x66,0x66,0x66, 0x00,0x00,0x00,0x00},//A
{0x00,0x00,0x00,0x00, 0x78,0x64,0x68,0x78,0x64,0x66,0x66,0x7C, 0x00,0x00,0x00,0x00},//B
{0x00,0x00,0x00,0x00, 0x3C,0x62,0x60,0x60,0x60,0x62,0x62,0x3C, 0x00,0x00,0x00,0x00},//C
{0x00,0x00,0x00,0x00, 0x78,0x64,0x66,0x66,0x66,0x66,0x64,0x78, 0x00,0x00,0x00,0x00},//D
{0x00,0x00,0x00,0x00, 0x7E,0x60,0x60,0x7C,0x60,0x60,0x60,0x7E, 0x00,0x00,0x00,0x00},//E
{0x00,0x00,0x00,0x00, 0x7E,0x60,0x60,0x7C,0x60,0x60,0x60,0x60, 0x00,0x00,0x00,0x00},//F
{0x00,0x00,0x00,0x00, 0x3C,0x62,0x60,0x60,0x66,0x62,0x62,0x3C, 0x00,0x00,0x00,0x00},//G
{0x00,0x00,0x00,0x00, 0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x66, 0x00,0x00,0x00,0x00},//H
{0x00,0x00,0x00,0x00, 0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x7E, 0x00,0x00,0x00,0x00},//I
{0x00,0x00,0x00,0x00, 0x7E,0x18,0x18,0x18,0x18,0x18,0x1A,0x0C, 0x00,0x00,0x00,0x00},//J
{0x00,0x00,0x00,0x00, 0x62,0x64,0x68,0x70,0x70,0x68,0x64,0x62, 0x00,0x00,0x00,0x00},//K
{0x00,0x00,0x00,0x00, 0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x7E, 0x00,0x00,0x00,0x00},//L
{0x00,0x00,0x00,0x00, 0xC3,0xE7,0xDB,0xDB,0xC3,0xC3,0xC3,0xC3, 0x00,0x00,0x00,0x00},//M
{0x00,0x00,0x00,0x00, 0x62,0x62,0x52,0x52,0x4A,0x4A,0x46,0x46, 0x00,0x00,0x00,0x00},//N
{0x00,0x00,0x00,0x00, 0x3C,0x66,0x66,0x66,0x66,0x66,0x66,0x3C, 0x00,0x00,0x00,0x00},//O
{0x00,0x00,0x00,0x00, 0x7C,0x62,0x62,0x7C,0x60,0x60,0x60,0x60, 0x00,0x00,0x00,0x00},//P
{0x00,0x00,0x00,0x00, 0x38,0x64,0x64,0x64,0x64,0x6C,0x64,0x3A, 0x00,0x00,0x00,0x00},//Q
{0x00,0x00,0x00,0x00, 0x7C,0x62,0x62,0x7C,0x70,0x68,0x64,0x62, 0x00,0x00,0x00,0x00},//R
{0x00,0x00,0x00,0x00, 0x1C,0x22,0x30,0x18,0x0C,0x46,0x46,0x3C, 0x00,0x00,0x00,0x00},//S
{0x00,0x00,0x00,0x00, 0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x18, 0x00,0x00,0x00,0x00},//T
{0x00,0x00,0x00,0x00, 0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x3C, 0x00,0x00,0x00,0x00},//U
{0x00,0x00,0x00,0x00, 0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x18, 0x00,0x00,0x00,0x00},//V
{0x00,0x00,0x00,0x00, 0x81,0x81,0x81,0x81,0x81,0x99,0x99,0x66, 0x00,0x00,0x00,0x00},//W
{0x00,0x00,0x00,0x00, 0x42,0x42,0x24,0x18,0x18,0x24,0x42,0x42, 0x00,0x00,0x00,0x00},//X
{0x00,0x00,0x00,0x00, 0xC3,0x66,0x3C,0x18,0x18,0x18,0x18,0x18, 0x00,0x00,0x00,0x00},//Y
{0x00,0x00,0x00,0x00, 0x7E,0x02,0x04,0x08,0x10,0x20,0x40,0x7E, 0x00,0x00,0x00,0x00},//Z

// 0 - 9
{0x00,0x00,0x00,0x00, 0x3C,0x66,0x66,0x6E,0x76,0x66,0x66,0x3C, 0x00,0x00,0x00,0x00},//0
{0x00,0x00,0x00,0x00, 0x18,0x38,0x58,0x18,0x18,0x18,0x18,0x7E, 0x00,0x00,0x00,0x00},//1
{0x00,0x00,0x00,0x00, 0x3C,0x66,0x66,0x0C,0x18,0x30,0x7E,0x7E, 0x00,0x00,0x00,0x00},//2
{0x00,0x00,0x00,0x00, 0x7E,0x0C,0x18,0x3C,0x06,0x06,0x46,0x3C, 0x00,0x00,0x00,0x00},//3
{0x00,0x00,0x00,0x00, 0x0C,0x18,0x30,0x6C,0x6C,0x7E,0x0C,0x0C, 0x00,0x00,0x00,0x00},//4
{0x00,0x00,0x00,0x00, 0x7E,0x60,0x60,0x7C,0x06,0x06,0x46,0x3C, 0x00,0x00,0x00,0x00},//5
{0x00,0x00,0x00,0x00, 0x04,0x08,0x10,0x38,0x6C,0x66,0x66,0x3C, 0x00,0x00,0x00,0x00},//6
{0x00,0x00,0x00,0x00, 0x7E,0x46,0x0C,0x18,0x18,0x18,0x18,0x18, 0x00,0x00,0x00,0x00},//7
{0x00,0x00,0x00,0x00, 0x3C,0x66,0x66,0x3C,0x66,0x66,0x66,0x3C, 0x00,0x00,0x00,0x00},//8
{0x00,0x00,0x00,0x00, 0x3C,0x66,0x66,0x36,0x1C,0x08,0x10,0x20, 0x00,0x00,0x00,0x00},//9
 

// Space (khoảng trắng)
{0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00},//space
{0x00,0x00,0x00,0x00, 0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00, 0x00,0x00,0x00,0x00},//:
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x1C,0x08}, // ,
{0x00,0x18,0x18,0x10,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // '
{0x00,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00,0x00,0x18,0x18,0x00,0x00,0x00}, // !

};

byte so_nho[][16]={
{0x00,0x00,0x00,0x00,0x7C,0x44,0x44,0x44,0x44,0x44,0x44,0x7C,0x00,0x00,0x00,0x00}, // 0
{0x00,0x00,0x00,0x00,0x10,0x30,0x50,0x10,0x10,0x10,0x10,0x7C,0x00,0x00,0x00,0x00}, // 1
{0x00,0x00,0x00,0x00,0x7C,0x04,0x04,0x7C,0x40,0x40,0x40,0x7C,0x00,0x00,0x00,0x00}, // 2
{0x00,0x00,0x00,0x00,0x7C,0x04,0x04,0x7C,0x04,0x04,0x04,0x7C,0x00,0x00,0x00,0x00}, // 3
{0x00,0x00,0x00,0x00,0x08,0x10,0x20,0x40,0x48,0x7C,0x08,0x08,0x00,0x00,0x00,0x00}, // 4
{0x00,0x00,0x00,0x00,0x7C,0x40,0x40,0x7C,0x04,0x04,0x04,0x7C,0x00,0x00,0x00,0x00}, // 5
{0x00,0x00,0x00,0x00,0x7C,0x40,0x40,0x7C,0x44,0x44,0x44,0x7C,0x00,0x00,0x00,0x00}, // 6
{0x00,0x00,0x00,0x00,0x7C,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00}, // 7
{0x00,0x00,0x00,0x00,0x7C,0x44,0x44,0x7C,0x44,0x44,0x44,0x7C,0x00,0x00,0x00,0x00}, // 8
{0x00,0x00,0x00,0x00,0x7C,0x44,0x44,0x44,0x7C,0x04,0x04,0x7C,0x00,0x00,0x00,0x00}  // 9
};


byte dau[][3]={
{0b00000100,0b00001000,0b00001000},
{0b00100000,0b00010000,0b00001000},   

};

char character[]={65,'B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','0','1','2','3','4','5','6','7','8','9',' ',':',',',39,33};

BLYNK_WRITE(V0)
{
  pixel_h = param.asInt();
}
BLYNK_WRITE(V1)
{
  pixel_c = param.asInt();
}
BLYNK_WRITE(V2)
{
  value_R = param.asInt();
}
BLYNK_WRITE(V3)

{
  value_G = param.asInt();
}
BLYNK_WRITE(V4)
{
  value_B = param.asInt();
}
BLYNK_WRITE(V5)
{
  interval = param.asInt();
}
BLYNK_WRITE(V6)
{
  tu = param.asStr();
}
BLYNK_WRITE(V7)
{
  mode = param.asInt();
}
BLYNK_WRITE(V8)
{
  brightness = param.asInt();
}
BLYNK_WRITE(V9)
{
  if (param.asInt())
  {
    pixel_h--;   // lên
    pixel_h = constrain(pixel_h, 0, 15);
  }
}
BLYNK_WRITE(V10)
{
  if (param.asInt())
  {
    pixel_h++;   // xuống
    pixel_h = constrain(pixel_h, 0, 15);
  }
}
BLYNK_WRITE(V11)
{
  if (param.asInt())
  {
    pixel_c--;   // trái
    pixel_c = constrain(pixel_c, 0, 63);
  }
}
BLYNK_WRITE(V12)
{
  if (param.asInt())
  {
    pixel_c++;   // phải
    pixel_c = constrain(pixel_c, 0, 63);
  }
}

int matrix[2][64] = {
  {
    7,6,5,4,3,2,1,0,23,22,21,20,19,18,17,16,
    39,38,37,36,35,34,33,32,55,54,53,52,51,50,49,48,
    71,70,69,68,67,66,65,64,87,86,85,84,83,82,81,80,
    103,102,101,100,99,98,97,96,119,118,117,116,115,114,113,112
  },
  {
    8,9,10,11,12,13,14,15,24,25,26,27,28,29,30,31,
    40,41,42,43,44,45,46,47,56,57,58,59,60,61,62,63,
    72,73,74,75,76,77,78,79,88,89,90,91,92,93,94,95,
    104,105,106,107,108,109,110,111,120,121,122,123,124,125,126,127
  }
};
char ssid[] = "PhuongLien";
char pass[] = "LyLy97ok";



byte led_ao[16][1] = {};
byte G[ROWS][COLS] = {
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																															
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																															
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																															
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																															
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																															
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																															
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																															
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																															
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																															
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																															
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																															
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																															
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																															
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																															
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																															
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																															
};
byte R[ROWS][COLS] = {
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																																
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																																
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																																
0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																																
0b00010000,0b00100000,0b00000100,0b00001000,0b00000000,0b00000000,0b00000000,0b00000000,																																																																
0b00110000,0b01100001,0b10001100,0b00011000,0b00000000,0b00000000,0b00000000,0b00000000,																																																																
0b01010000,0b10100001,0b10010100,0b00101000,0b00000000,0b00000000,0b00000000,0b00000000,																																																																
0b00010000,0b00100000,0b00000100,0b00001000,0b00000000,0b00000000,0b00000000,0b00000000,																																																																
0b00010000,0b00100000,0b00000100,0b00001000,0b00000000,0b00000000,0b00000000,0b00000000,																																																																
0b00010000,0b00100001,0b10000100,0b00001000,0b00000000,0b00000000,0b00000000,0b00000000,																																																																
0b00010000,0b00100001,0b10000100,0b00001000,0b00000000,0b00000000,0b00000000,0b00000000,																																																																
0b01111100,0b11111000,0b00011111,0b00111110,0b00000000,0b00000000,0b00000000,0b00000000,																																																																
0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																																
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																																
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																																
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,																																																																
};
byte B[ROWS][COLS] = {};
byte MAU[ROWS][COLS] = {};


//------------------- SET MÀU -------------------
void setPixel(int i, int row) {
  // Tắt hết LED trước
  digitalWrite(R1_PIN, LOW); digitalWrite(G1_PIN, LOW); digitalWrite(B1_PIN, LOW);
  digitalWrite(R2_PIN, LOW); digitalWrite(G2_PIN, LOW); digitalWrite(B2_PIN, LOW);

  // Tìm vị trí x,y từ mapping
  for (int r = 0; r < 2; r++) {
    for (int c = 0; c < quetled/2; c++) {
      if (matrix[r][c] == i) { x = r; y = c; goto found; }
    }
  }
  return; // không tìm thấy
found:

  byteindex = y / 8;
  bitindex  = y % 8;

  // Bảng ánh xạ row → vị trí trong ma trận
  const byte mapR1[4][2] = { {0,4}, {1,5}, {2,6}, {3,7} };
  const byte mapR2[4][2] = { {8,12}, {9,13}, {10,14}, {11,15} };

  byte idx1 = (x == 0) ? mapR1[row][0] : mapR1[row][1];
  byte idx2 = (x == 0) ? mapR2[row][0] : mapR2[row][1];

  // đọc bit trong từng ma trận
  bool r1 = bitRead(R[idx1][byteindex], 7 - bitindex);
  bool g1 = bitRead(G[idx1][byteindex], 7 - bitindex);
  bool b1 = bitRead(B[idx1][byteindex], 7 - bitindex);

  bool r2 = bitRead(R[idx2][byteindex], 7 - bitindex);
  bool g2 = bitRead(G[idx2][byteindex], 7 - bitindex);
  bool b2 = bitRead(B[idx2][byteindex], 7 - bitindex);

  // bật LED theo từng màu
  if (r1) digitalWrite(R1_PIN, HIGH);
  if (g1) digitalWrite(G1_PIN, HIGH);
  if (b1) digitalWrite(B1_PIN, HIGH);

  if (r2) digitalWrite(R2_PIN, HIGH);
  if (g2) digitalWrite(G2_PIN, HIGH);
  if (b2) digitalWrite(B2_PIN, HIGH);
}
void getdata(byte matrix[16][8], bool value) {
  if (pixel_c < 0 || pixel_c >= 64 || pixel_h < 0 || pixel_h >= 16) return;

  int byteIndex = pixel_c / 8;
  int bitIndex  = pixel_c % 8;
  
  if (millis() - lastTime >= interval) {
    lastTime = millis();
    R[pixel_h][byteIndex] ^= (1 << (7 - bitIndex));     // đảo bit để tạo hiệu ứng nhấp nháy
  }

  if (value){ matrix[pixel_h][byteIndex] |= (1 << (7 - bitIndex));}   // bật bit 
        else{ matrix[pixel_h][byteIndex] &= ~(1 << (7 - bitIndex));}  // tắt bit }

  if(pixel_c != oldc || pixel_h != oldh || mode != 100) { // chỉ cập nhật nếu có thay đổi
    memcpy(R, MAU, sizeof(R));   
    oldc = pixel_c;
    oldh = pixel_h;                                                                       
  }
}



void rain(byte M[ROWS][COLS]) {
  // ===== DỊCH XUỐNG =====
  for (int r = ROWS - 1; r > 0; r--) {
    for (int c = 0; c < COLS; c++) {
      M[r][c] = M[r - 1][c];  
      // hàng dưới nhận dữ liệu từ hàng trên
    }
  }

  // ===== TẠO HẠT MƯA MỚI Ở HÀNG 0 =====
  for (int c = 0; c < COLS; c++) {
    byte newByte = 0;

    for (int b = 0; b < 8; b++) {
      if (random(0, 10) > 7) {  
        // xác suất xuất hiện hạt mưa (~30%)
        newByte |= (1 << b);
      }
    }

    M[0][c] = newByte;  
    // gán hàng trên cùng
  }
}

void gradientRainbow(byte R[16][8], byte G[16][8], byte B[16][8]) {
  int t = millis() / 20;   // tốc độ chạy

  for (int y = 0; y < 16; y++) {
    for (int x = 0; x < 64; x++) {

      int byteIndex = x / 8;
      int bitIndex  = 7 - (x % 8);

      int pos = (x + t) % 192;   // chia 3 vùng màu

      bool r = 0, g = 0, b = 0;

      if (pos < 64) {           // đỏ -> vàng
        r = 1;
        g = (pos > 32);
      }
      else if (pos < 128) {     // xanh lá
        g = 1;
        b = (pos > 96);
      }
      else {                    // xanh dương -> tím
        b = 1;
        r = (pos > 160);
      }

      // set bit
      if (r) R[y][byteIndex] |=  (1 << bitIndex);
      else   R[y][byteIndex] &= ~(1 << bitIndex);

      if (g) G[y][byteIndex] |=  (1 << bitIndex);
      else   G[y][byteIndex] &= ~(1 << bitIndex);

      if (b) B[y][byteIndex] |=  (1 << bitIndex);
      else   B[y][byteIndex] &= ~(1 << bitIndex);
    }
  }
}
void scanDisplay() { for (int   = 0; xxx < 4; xxx++) { 
  
  digitalWrite(OE_PIN, HIGH); // tắt LED khi nạp dữ liệu 
 
  for (int i = 0; i < quetled; i++) 
  
  { setPixel(i, xxx); 
    digitalWrite(CLK_PIN, HIGH); 
    digitalWrite(CLK_PIN, LOW); } 

    digitalWrite(A_PIN, xxx & 0x01); 
    digitalWrite(B_PIN, (xxx >> 1) & 0x01);
    digitalWrite(LAT_PIN, HIGH);
    digitalWrite(LAT_PIN, LOW);
    digitalWrite(OE_PIN, LOW); // bật LED hiển thị
    delayMicroseconds(brightness); // điều chỉnh độ sáng
    }}
void shiftLeft(byte M[][COLS])   {
  for (int ha = 0; ha < ROWS; ha++) {

    byte firstBit = (M[ha][0] >> 7);

    for (int co = 0; co < COLS - 1; co++) {
      M[ha][co] = (M[ha][co] << 1) | (M[ha][co + 1] >> 7);
    }

   M[ha][COLS - 1] = (M[ha][COLS - 1] << 1) | firstBit;
  }
}
void shiftRight(byte M[][COLS])  {
  for (int ha = 0; ha < ROWS; ha++) {

    byte lastBit = M[ha][COLS - 1] & 0x01;  // lấy bit cuối

    for (int co = COLS - 1; co > 0; co--) {
      M[ha][co] = (M[ha][co] >> 1) | ((M[ha][co - 1] & 0x01) << 7);
    }
    if(mode != 9 ){
    M[ha][0] = (M[ha][0] >> 1) | (lastBit << 7);}
  }
}
void shiftDown(byte M[][COLS]) {
  byte temp[COLS];

  // lưu hàng cuối (để xoay vòng)
  for(int i = 0; i < COLS; i++){
    temp[i] = M[ROWS - 1][i];
  }

  // dịch xuống
  for(int ha = ROWS - 1; ha > 0; ha--){
    for(int co = 0; co < COLS; co++){
      M[ha][co] = M[ha - 1][co];
    }
  }

  // đưa hàng cuối lên đầu (xoay vòng)
  if(mode != 9 ){
  for(int i = 0; i < COLS; i++){
    M[0][i] = temp[i];
  }
}}
void shiftUp(byte M[][COLS]) {
  byte temp[COLS];

  // lưu hàng đầu
  for(int i = 0; i < COLS; i++){
    temp[i] = M[0][i];
  }

  // dịch lên
  for(int ha = 0; ha < ROWS - 1; ha++){
    for(int co = 0; co < COLS; co++){
      M[ha][co] = M[ha + 1][co];
    }
  }

  // đưa hàng đầu xuống cuối (xoay vòng)
  for(int i = 0; i < COLS; i++){
    M[ROWS - 1][i] = temp[i];
  }
}
//------------------- QUÉT HÀNG -------------------

void off() {
  memset(R, 0, sizeof(R));
  memset(G, 0, sizeof(G));
  memset(B, 0, sizeof(B));
}
void dichbit_napbit(){          // cho chương trình chạy chữ
  for (int hang = 0; hang < ROWS; hang++) {

      for (int cot = 0; cot < COLS - 1; cot++) {
        MAU[hang][cot] =
          (MAU[hang][cot] << 1) |
          (MAU[hang][cot + 1] >> 7);
      }

      MAU[hang][COLS - 1] =
        (MAU[hang][COLS - 1] << 1) |
        ((led_ao[hang][0] >> (7 - tam)) & 1);
    }
}
void hienthi(String tukhoa) {

  // tìm index e của ký tự
  for (int i = 0; i < sizeof(character)/sizeof(character[0]); i++) {
    if (tukhoa.charAt(q) == character[i]) {
      e = i;
      break;
    }
  }

  // millis điều khiển tốc độ
  if (millis() - lastTime >= interval) {
    lastTime = millis();

    // nạp chữ khi bắt đầu
    if (tam == 0) {
      for (int cott = 0; cott < 16; cott++) {
        led_ao[cott][0] = chu[e][cott];
      }
    }
      dichbit_napbit();
      (value_R) ? memcpy(R, MAU, sizeof(R)) : memset(R, 0, sizeof(R));
      (value_G) ? memcpy(G, MAU, sizeof(G)) : memset(G, 0, sizeof(G));
      (value_B) ? memcpy(B, MAU, sizeof(B)) : memset(B, 0, sizeof(B));

    tam++;

    // hết 1 ký tự
    if (tam >= 8) {   // hoặc 16 nếu font 16 cột
      tam = 0;
      q++;

      if (q >= tukhoa.length()) {
        q = 0;
      }
    }
  }

  scanDisplay(); // luôn gọi liên tục
}
void on_mau() {
  memset(MAU, 255, sizeof(MAU));
}
void off_mau() {
  memset(MAU, 0, sizeof(MAU));
} 
void setup() {
//hienthi("ABC", G);
 Serial.begin(9600);
  configTime(7 * 3600, 0, "pool.ntp.org");
 //printMatrix();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(A_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
  //pinMode(C_PIN, OUTPUT);

  pinMode(R1_PIN, OUTPUT);
  pinMode(G1_PIN, OUTPUT);
  pinMode(B1_PIN, OUTPUT);
  pinMode(R2_PIN, OUTPUT);
  pinMode(G2_PIN, OUTPUT);
  pinMode(B2_PIN, OUTPUT);

  pinMode(CLK_PIN, OUTPUT);
  pinMode(LAT_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);
  digitalWrite(OE_PIN, LOW);
  //digitalWrite(C_PIN, LOW);
  //memset(R, 255, sizeof(R));      // set all
}
void duplicateModule(byte M[ROWS][COLS]) {
  for (int r = 0; r < ROWS; r++) {
    for (int c = 0; c < COLS / 2; c++) {
      M[r][c + COLS/2] = M[r][c];
    }
  }
}
void hienthi_no_shift(String tukhoa) {          //  code

  // tìm index e của ký tự
  for (int i = 0; i < sizeof(character)/sizeof(character[0]); i++) {
    if (tukhoa.charAt(q) == character[i]) {
      e = i;
      break;
    }
  }

  // millis điều khiển tốc độ
  if (millis() - lastTime >= interval) {
    lastTime = millis();

    // nạp chữ khi bắt đầu
    if (tam == 0) {
      for (int cott = 0; cott < 16; cott++) {
        led_ao[cott][0] = so_nho[e][cott];
      }
    }
      (value_R) ? memcpy(R, MAU, sizeof(R)) : memset(R, 0, sizeof(R));
      (value_G) ? memcpy(G, MAU, sizeof(G)) : memset(G, 0, sizeof(G));
      (value_B) ? memcpy(B, MAU, sizeof(B)) : memset(B, 0, sizeof(B));

    tam++;

    // hết 1 ký tự
    if (tam >= 8) {   // hoặc 16 nếu font 16 cột
      tam = 0;
      q++;

      if (q >= tukhoa.length()) {
        q = 0;
      }
    }
  }
}
//------------------- LOOP -------------------
void loop() {
    //scanDisplay();
    Blynk.run();
    switch(mode)
    {
      case 0:                                                      // tắt
        off();
        break;
      case 1:
        hienthi(tu);                                            // chạy chữ
        break;
    
      case 100:                                                    // mode 1: VẼ ĐIỂM ẢNH
        getdata(MAU,value_R);
        getdata(G,value_G);
        getdata(B,value_B);
        scanDisplay();
        break;
      case 3:     
        if (millis() - lastTime >= interval) {
          lastTime = millis();
          shiftLeft(R);                                       // mode 2: hiển thị điểm ảnh từ blynk + DỊCH phải -> trái
          shiftLeft(G);
          shiftLeft(B);   
        }     
        scanDisplay();
        break;
      case 4:
        if (millis() - lastTime >= interval) {
          lastTime = millis();
          shiftRight(R);                                      // mode 3: hiển thị điểm ảnh từ blynk +  DỊCH trái-> phải
          shiftRight(G);
          shiftRight(B);   
        }
        scanDisplay();
        break;
        case 5:
        if (millis() - lastTime >= interval) {
          lastTime = millis();
          shiftDown(R);                                        // mode 4: hiển thị điểm ảnh từ blynk + DỊCH trên -> dưới
          shiftDown(G);
          shiftDown(B);   
        }
        scanDisplay();
        break;
        case 6:
        if (millis() - lastTime >= interval) {                // mode 5: hiển thị điểm ảnh từ blynk + DỊCH dưới -> trên
          lastTime = millis();
          shiftUp(R);                    
          shiftUp(G);
          shiftUp(B);   
        }
        scanDisplay();
        break;
        case 7:
        if (millis() - lastTime >= interval) {                // nháy 
          lastTime = millis();
          on_mau();}
          else {
          off_mau();
          }
        (value_R) ? memcpy(R, MAU, sizeof(R)) : memset(R, 0, sizeof(R));
        (value_G) ? memcpy(G, MAU, sizeof(G)) : memset(G, 0, sizeof(G));
        (value_B) ? memcpy(B, MAU, sizeof(B)) : memset(B, 0, sizeof(B));
        scanDisplay();
        break;

        case 2:
        memcpy(MAU, R, sizeof(MAU));  
        mode = 100;
        break;

        case 8:
        if(millis() - lastTime >= interval) {                // mưa
        lastTime = millis();
        if(value_R) { rain(R); } else { memset(R, 0, sizeof(R)); }
        if(value_G) { rain(G); } else { memset(G, 0, sizeof(G)); }
        if(value_B) { rain(B); } else { memset(B, 0, sizeof(B)); }
        }
        scanDisplay();
        
        break;

        

        case 10: {
        struct tm timeinfo; 
        if (!getLocalTime(&timeinfo)) {
          hienthi(" ERROR TIME         ");
        }
        else {
          char buffer[9];
          strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);
          hienthi(String(buffer)+"        ");
          //Serial.println(buffer);
        }
          scanDisplay();
        break;}


        case 11:
        if(millis() - lastTime >= interval) {                // time display no shift
        lastTime = millis();}
          off_mau();
          { MAU[6][1]=0b00000001;       MAU[9][1]=0b00000001;
            MAU[7][1]=0b00000001;	      MAU[10][1]=0b00000001;
            MAU[6][2]=0b10000000;       MAU[9][2]=0b10000000;
            MAU[7][2]=0b10000000;				MAU[10][2]=0b10000000;
          }
          (value_R) ? memcpy(R, MAU, sizeof(R)) : memset(R, 0, sizeof(R));
          (value_G) ? memcpy(G, MAU, sizeof(G)) : memset(G, 0, sizeof(G));
          (value_B) ? memcpy(B, MAU, sizeof(B)) : memset(B, 0, sizeof(B));
          scanDisplay();
          break;

        case 50:  {
          off_mau();
          struct tm timeinfo;
          if (getLocalTime(&timeinfo)) {
          hour_i   = timeinfo.tm_hour;
          minute_i = timeinfo.tm_min;
         }
          uint8_t h1 = hour_i / 10;   // hàng chục giờ  → 1
          uint8_t h2 = hour_i % 10;   // hàng đơn vị giờ → 4

          uint8_t m1 = minute_i / 10; // hàng chục phút → 3
          uint8_t m2 = minute_i % 10; // hàng đơn vị phút → 5

            for(int row = 0; row < 16; row++) {
              MAU[row][0] = so_nho[h1][row];
              MAU[row][1] = so_nho[h2][row]<<1 | ((row==5 || row==6 || row==9 || row==10) ? 0b00000001 : 0);           
              MAU[row][2] = so_nho[m1][row]>>2 | ((row==5 || row==6 || row==9 || row==10) ? 0b10000000 : 0);
              MAU[row][3] = so_nho[m2][row]>>1;}
          //duplicateModule(MAU); // nhân đôi dữ liệu sang nửa phải
          (value_R) ? memcpy(R, MAU, sizeof(R)) : memset(R, 0, sizeof(R));
          (value_G) ? memcpy(G, MAU, sizeof(G)) : memset(G, 0, sizeof(G));
          (value_B) ? memcpy(B, MAU, sizeof(B)) : memset(B, 0, sizeof(B));
          scanDisplay();
          }
        break;

        case 200:
        gradientRainbow(R, G, B);
        scanDisplay();
        break;
    }

  }
