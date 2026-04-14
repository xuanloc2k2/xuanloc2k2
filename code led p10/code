#define BLYNK_PRINT Serial 
#define BLYNK_TEMPLATE_ID "TMPL6BUnrCnJG"
#define BLYNK_TEMPLATE_NAME "p10"
#define BLYNK_AUTH_TOKEN "yGJmnImyIbwzdBnL9QnZwytk_Xjf9jY4"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <BlynkSimpleEsp8266.h>
int pixel_h;
int pixel_c;
int value_R;
int value_G;
int value_B;

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
WiFiUDP udp;

#define ARTNET_PORT 6454
#define MAX_PACKET 600

byte packetBuffer[MAX_PACKET];
byte dmxData[512];

//void printMatrix16x8(uint16_t universe);

#define A_PIN 1
#define B_PIN 15
//#define C_PIN A0
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

// Mỗi màu 1 ma trận riêng
byte R[ROWS][COLS] = {};
byte G[ROWS][COLS] = {};
byte B[ROWS][COLS] = {};
int quetled = 128;
int x,y,byteindex,bitindex,row;


//------------------- SET MÀU -------------------
void setPixel(int i, int row) {
  // Tắt hết LED trước
  digitalWrite(R1_PIN, LOW); digitalWrite(G1_PIN, LOW); digitalWrite(B1_PIN, LOW);
  digitalWrite(R2_PIN, LOW); digitalWrite(G2_PIN, LOW); digitalWrite(B2_PIN, LOW);

  // Tìm vị trí x,y từ mapping
  for (int r = 0; r < 2; r++) {
    for (int c = 0; c < 64; c++) {
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



void getdata(){
  R[pixel_h][pixel_c] = value_R;
  G[pixel_h][pixel_c] = value_G;
  B[pixel_h][pixel_c] = value_B;
  if(value_R == 300) { memset(R, 0, sizeof(matrix));}
  if(value_G == 300) { memset(G, 0, sizeof(matrix));}
  if(value_B == 300) { memset(B, 0, sizeof(matrix));}
}

void scanDisplay() { for (row = 0; row < 4; row++) { 
  
  digitalWrite(OE_PIN, HIGH); // tắt LED khi nạp dữ liệu 
 
  for (int i = 0; i < quetled; i++) 
  
  { setPixel(i, row); 
    digitalWrite(CLK_PIN, HIGH); 
    digitalWrite(CLK_PIN, LOW); } 
    digitalWrite(A_PIN, row & 0x01); 
    digitalWrite(B_PIN, (row >> 1) & 0x01);
    digitalWrite(LAT_PIN, HIGH);
    digitalWrite(LAT_PIN, LOW);
    digitalWrite(OE_PIN, LOW); // bật LED hiển thị
    delayMicroseconds(1000);
    }}



//------------------- QUÉT HÀNG -------------------
/*
void convertDMXtoRGB() {

  // clear mảng
  memset(R, 0, sizeof(R));
  memset(G, 0, sizeof(G));
  memset(B, 0, sizeof(B));

  // 16x8 = 128 pixel
  for (int i = 0; i < 128; i++) {

    int index = i * 3;

    byte r = dmxData[index];
    byte g = dmxData[index + 1];
    byte b = dmxData[index + 2];

    // ===== CHUYỂN SANG 16x8 =====
    int y = i / 8;   // row (0-15)
    int x = i % 8;   // byte (0-7)

    int bit = 7 - (y % 8);  // bit trong byte

    // ===== SET BIT =====
    if (r > 127) bitSet(R[y][x], bit);
    if (g > 127) bitSet(G[y][x], bit);
    if (b > 127) bitSet(B[y][x], bit);
  }
}
  */
 void clearMatrix() {
  for (int h = 0; h < 16; h++) {
    for (int c = 0 ; c < 8; c++) {
      matrix[h][c] = 0;
    }
  }
}
unsigned long lastTime = 0;
unsigned long interval = 100;
//------------------- SETUP -------------------
void setup() {

  Serial.begin(9600);
  
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
}

//------------------- LOOP -------------------
void loop() {
  scanDisplay();  // luôn quét hiển thị
  Blynk.run();
  getdata();
}




  

 
