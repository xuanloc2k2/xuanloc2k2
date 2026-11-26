
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <FastLED.h>
#include <LittleFS.h>
#include <time.h>
#include <EEPROM.h>

// ======================================
// CAU HINH LED
// ======================================

#define LED_PIN         2
#define NUM_LEDS        120
#define LED_PER_SEG     4

#define COLON_START     56
#define GRADIENT_SPEED  25
#define GRADIENT_STEP   2

CRGB leds[NUM_LEDS];

const int digitStart[4] = {92, 64, 28, 0};
 
const byte numberMap[10][7] = {
  {1,1,1,1,1,1,0},
  {1,1,0,0,0,0,0},
  {0,1,1,0,1,1,1},
  {1,1,1,0,0,1,1},
  {1,1,0,1,0,0,1},
  {1,0,1,1,0,1,1},
  {1,0,1,1,1,1,1},
  {1,1,1,0,0,0,0},
  {1,1,1,1,1,1,1},
  {1,1,1,1,0,1,1}
};

// ======================================
// WIFI
// ======================================

const char* AP_SSID = "DONGHO-SETUP";
const char* AP_PASS = "12345678";

AsyncWebServer server(80);
DNSServer dnsServer;

bool cheDoCauHinh = false;
bool dangKetNoi = false;
bool daKetNoi = false;
bool dangQuetWiFi = false;

String wifiSSID = "";
String wifiPASS = "";

unsigned long thoiDiemKetNoi = 0;
unsigned long lanKiemTraWiFi = 0;

// ======================================
// THOI GIAN
// ======================================

const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;

bool ntpStarted = false;

unsigned long lanDocGio = 0;

int gioHienTai = 0;
int phutHienTai = 0;

bool chamHienTai = false;

// Luu trang thai da hien thi

int gioDaHienThi = -1;
int phutDaHienThi = -1;
bool chamDaHienThi = false;

// ======================================
// CHE DO MAU LED
// ======================================

// 0 = Gradient
// 1 = RGB co dinh

uint8_t mode = 0;

uint8_t baseHue = 0;

unsigned long lanGradient = 0;

uint8_t speed = 25;

uint8_t r = 255;
uint8_t g = 0;
uint8_t b = 0;

uint8_t brightness = 50;

// ======================================
// KHOI TAO NTP
// ======================================
// ======================================
// LAY MAU LED THEO CHE DO
// ======================================

CRGB layMauLED(int ledIndex) {

  switch (mode) {

    // MODE 0: GRADIENT

    case 0: {

      uint8_t hue = baseHue + ledIndex * GRADIENT_STEP;

      return CHSV(hue, 255, 255);
    }

    // MODE 1: RGB CO DINH

    case 1: {

      return CRGB(r, g, b);
    }

    default: {

      return CRGB::Black;
    }
  }
}

// ======================================
// BAT LED
// ======================================

void batLED(int index) {

  leds[index] = layMauLED(index);
}

// ======================================
// HIEN THI DOAN LED
// ======================================

void hienThiDoan(int digit, int segment, bool bat) {

  int start = digitStart[digit] + segment * LED_PER_SEG;

  for (int i = 0; i < LED_PER_SEG; i++) {

    int index = start + i;

    if (bat) {
      batLED(index);
    }
    else {
      leds[index] = CRGB::Black;
    }
  }
}

// ======================================
// HIEN THI SO
// ======================================

void hienThiSo(int digit, int number) {

  if (number < 0 || number > 9) return;

  for (int s = 0; s < 7; s++) {

    hienThiDoan(
      digit,
      s,
      numberMap[number][s]
    );
  }
}

// ======================================
// HIEN THI DAU HAI CHAM
// ======================================

void hienThiCham(bool bat) {

  for (int i = 0; i < 8; i++) {

    int index = COLON_START + i;

    if (bat) {
      batLED(index);
    }
    else {
      leds[index] = CRGB::Black;
    }
  }
}

// ======================================
// CAP NHAT DONG HO
// ======================================

void capNhatDongHo(int gio, int phut, bool cham) {

  FastLED.clear();

  hienThiSo(0, gio / 10);
  hienThiSo(1, gio % 10);

  hienThiCham(cham);

  hienThiSo(2, phut / 10);
  hienThiSo(3, phut % 10);

  FastLED.show();

  // Luu trang thai da hien thi

  gioDaHienThi = gio;
  phutDaHienThi = phut;
  chamDaHienThi = cham;
}


void khoiTaoNTP() {

  configTime(
    gmtOffset_sec,
    daylightOffset_sec,
    "pool.ntp.org"
  );

  ntpStarted = true;

  Serial.println("Dang dong bo thoi gian...");
}

// ======================================
// WIFI AP
// ======================================

void batWiFiAP() {

  WiFi.mode(WIFI_AP_STA);

  WiFi.softAP(AP_SSID, AP_PASS);

  dnsServer.start(53, "*", WiFi.softAPIP());

  cheDoCauHinh = true;

  Serial.println("=== WIFI SETUP ===");
  Serial.println(AP_SSID);
  Serial.println(WiFi.softAPIP());
}

// ======================================
// KIEM TRA KET NOI WIFI
// ======================================

void kiemTraKetNoiWiFi() {

  if (!dangKetNoi) return;

  if (WiFi.status() == WL_CONNECTED) {

    dangKetNoi = false;
    daKetNoi = true;

    Serial.println("Da ket noi WiFi!");
    Serial.println(WiFi.localIP());

    khoiTaoNTP();

    WiFi.softAPdisconnect(true);
    dnsServer.stop();

    cheDoCauHinh = false;

    return;
  }

  if (millis() - thoiDiemKetNoi > 20000) {

    dangKetNoi = false;

    Serial.println("Ket noi WiFi that bai!");

    WiFi.disconnect();

    batWiFiAP();
  }
}

// ======================================
// QUET WIFI
// ======================================


void xuLyScan(AsyncWebServerRequest *request) {

  Serial.println();
  Serial.println("========== QUET WIFI ==========");

  // KIEM TRA TRANG THAI QUET
  int soMang = WiFi.scanComplete();

  // ----------------------------------
  // TRUONG HOP WIFI DANG QUET
  // ----------------------------------

  if (soMang == WIFI_SCAN_RUNNING) {

    if (!dangQuetWiFi) {
      Serial.println("[SCAN] WiFi dang quet...");
      dangQuetWiFi = true;
    }

    request->send(
      200,
      "application/json",
      "{\"done\":false}"
    );

    return;
  }

  // ----------------------------------
  // TRUONG HOP CHUA QUET HOAC QUET LOI
  // ----------------------------------

  if (soMang == WIFI_SCAN_FAILED) {

    if (dangQuetWiFi) {
      Serial.println("[SCAN] Quet WiFi that bai!");
      dangQuetWiFi = false;
    }

    Serial.println("[SCAN] Bat dau quet WiFi...");

    WiFi.scanDelete();

    int ketQua = WiFi.scanNetworks(true, true);

    if (ketQua == WIFI_SCAN_RUNNING) {

      dangQuetWiFi = true;

      Serial.println("[SCAN] Da gui lenh quet WiFi!");
      Serial.println("[SCAN] Dang cho ket qua...");

    }
    else if (ketQua == WIFI_SCAN_FAILED) {

      Serial.println("[SCAN] Khong the bat dau quet!");

    }

    request->send(
      200,
      "application/json",
      "{\"done\":false}"
    );

    return;
  }

  // ----------------------------------
  // QUET WIFI HOAN TAT
  // ----------------------------------

  Serial.println("[SCAN] Quet WiFi hoan tat!");

  Serial.print("[SCAN] Tim thay: ");
  Serial.print(soMang);
  Serial.println(" mang WiFi");

  String json = "{\"done\":true,\"networks\":[";

  // ----------------------------------
  // DOC DANH SACH WIFI
  // ----------------------------------

  for (int i = 0; i < soMang; i++) {

    String tenWiFi = WiFi.SSID(i);

    int rssi = WiFi.RSSI(i);

    bool baoMat =
      (WiFi.encryptionType(i) != ENC_TYPE_NONE);

    // IN THONG TIN RA SERIAL

    Serial.println("-----------------------------");

    Serial.print("[WIFI] STT: ");
    Serial.println(i + 1);

    Serial.print("[WIFI] SSID: ");
    Serial.println(tenWiFi);

    Serial.print("[WIFI] RSSI: ");
    Serial.print(rssi);
    Serial.println(" dBm");

    Serial.print("[WIFI] Bao mat: ");
    Serial.println(baoMat ? "Co mat khau" : "Khong mat khau");

    // ----------------------------------
    // TAO JSON
    // ----------------------------------

    if (i > 0) json += ",";

    tenWiFi.replace("\\", "\\\\");
    tenWiFi.replace("\"", "\\\"");
    tenWiFi.replace("\n", "");
    tenWiFi.replace("\r", "");

    json += "{";

    json += "\"ssid\":\"" + tenWiFi + "\",";

    json += "\"rssi\":" + String(rssi) + ",";

    json += "\"secure\":";

    json += baoMat ? "true" : "false";

    json += "}";
  }

  json += "]}";

  // ----------------------------------
  // KET THUC QUET
  // ----------------------------------

  if (soMang == 0) {

    Serial.println("[SCAN] Khong tim thay WiFi nao!");

  }

  Serial.println("-----------------------------");
  Serial.println("[SCAN] Gui danh sach WiFi ve trinh duyet");
  Serial.println("================================");

  dangQuetWiFi = false;

  WiFi.scanDelete();

  request->send(
    200,
    "application/json; charset=utf-8",
    json
  );
}


// ======================================
// TRANG THAI WIFI
// ======================================

void xuLyTrangThai(AsyncWebServerRequest *request) {

  String json = "{";

  json += "\"connected\":";
  json += (WiFi.status() == WL_CONNECTED) ? "true" : "false";

  json += ",\"status\":\"";

  if (dangKetNoi) {
    json += "Dang ket noi WiFi";
  }
  else if (WiFi.status() == WL_CONNECTED) {
    json += "Da ket noi WiFi";
  }
  else {
    json += "Chua ket noi WiFi";
  }

  json += "\",\"ip\":\"";

  json += (WiFi.status() == WL_CONNECTED)
          ? WiFi.localIP().toString()
          : WiFi.softAPIP().toString();

  json += "\"}";

  request->send(
    200,
    "application/json; charset=utf-8",
    json
  );
}

// ======================================
// KET NOI WIFI
// ======================================

void xuLyKetNoi(AsyncWebServerRequest *request) {

  if (!request->hasParam("ssid", true) ||
      !request->hasParam("pass", true)) {

    request->send(
      400,
      "application/json",
      "{\"success\":false,\"message\":\"Thieu thong tin WiFi\"}"
    );

    return;
  }

  wifiSSID = request->getParam("ssid", true)->value();
  wifiPASS = request->getParam("pass", true)->value();

  if (wifiSSID.length() == 0) {

    request->send(
      400,
      "application/json",
      "{\"success\":false,\"message\":\"Ten WiFi khong hop le\"}"
    );

    return;
  }

  request->send(
    200,
    "application/json",
    "{\"success\":true,\"message\":\"Dang ket noi WiFi...\"}"
  );

  WiFi.mode(WIFI_AP_STA);

  WiFi.begin(wifiSSID.c_str(), wifiPASS.c_str());

  dangKetNoi = true;
  thoiDiemKetNoi = millis();

  Serial.println("Dang ket noi WiFi...");
  Serial.println(wifiSSID);
}

// ======================================
// DIEU KHIEN LED TU WEB
// ======================================

void xuLyDieuKhien(AsyncWebServerRequest *request) {

  if (request->hasParam("r", true))
    r = constrain(
      request->getParam("r", true)->value().toInt(),
      0, 255
    );

  if (request->hasParam("g", true))
    g = constrain(
      request->getParam("g", true)->value().toInt(),
      0, 255
    );

  if (request->hasParam("b", true))
    b = constrain(
      request->getParam("b", true)->value().toInt(),
      0, 255
    );

  if (request->hasParam("brightness", true))
    brightness = constrain(
      request->getParam("brightness", true)->value().toInt(),
      0, 255
    );

  if (request->hasParam("speed", true))
    speed = constrain(
      request->getParam("speed", true)->value().toInt(),
      1, 100
    );

  if (request->hasParam("mode", true))
    mode = constrain(
      request->getParam("mode", true)->value().toInt(),
      0, 1
    );

  FastLED.setBrightness(brightness);

  // Cap nhat LED ngay lap tuc

  capNhatDongHo(
    gioHienTai,
    phutHienTai,
    chamHienTai
  );

  request->send(
    200,
    "application/json",
    "{\"success\":true}"
  );
}

// ======================================
// DANG KY WEB
// ======================================

void dangKyWeb() {

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(
      LittleFS,
      "/clock.html",
      "text/html; charset=utf-8"
    );
  });

  server.on("/clock.css", HTTP_GET, [](AsyncWebServerRequest *request) {

    request->send(
      LittleFS,
      "/clock.css",
      "text/css; charset=utf-8"
    );

  });

  server.on("/clock.js", HTTP_GET, [](AsyncWebServerRequest *request) {

    request->send(
      LittleFS,
      "/clock.js",
      "application/javascript; charset=utf-8"
    );

  });

  server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request) {
    xuLyScan(request);
  });

  server.on("/connect", HTTP_POST, [](AsyncWebServerRequest *request) {
    xuLyKetNoi(request);
  });

  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    xuLyTrangThai(request);
  });

  server.on("/set", HTTP_POST, [](AsyncWebServerRequest *request) {
    xuLyDieuKhien(request);
  });

  server.onNotFound([](AsyncWebServerRequest *request) {

    request->send(
      404,
      "text/plain",
      "Khong tim thay trang"
    );

  });

  server.begin();

  Serial.println("Web server da khoi dong!");
}


// ======================================
// SETUP
// ======================================

void setup() {

  Serial.begin(9600);

  // KHOI TAO LED

  FastLED.addLeds<WS2812B, LED_PIN, GBR>(leds, NUM_LEDS);

  FastLED.setBrightness(brightness);

  FastLED.clear(true);

  // KHOI TAO LITTLEFS

  if (!LittleFS.begin()) {

    Serial.println("Loi khoi dong LittleFS!");

  }
  else {

    Serial.println("LittleFS OK!");

  }

  // KET NOI WIFI DA LUU

  WiFi.mode(WIFI_STA);

  WiFi.begin();

  Serial.println("Dang kiem tra WiFi da luu...");

  unsigned long batDau = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - batDau < 15000) {

    delay(300);

    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {

    daKetNoi = true;

    Serial.println("Da ket noi WiFi!");
    Serial.println(WiFi.localIP());

    khoiTaoNTP();
  }
  else {

    Serial.println("Khong co WiFi da luu!");

    batWiFiAP();
  }

  // KHOI DONG WEB

  dangKyWeb();
}

// ======================================
// LOOP
// ======================================

void loop() {

  // ======================================
  // XU LY WIFI AP
  // ======================================

  if (cheDoCauHinh) {

    dnsServer.processNextRequest();

  }

  kiemTraKetNoiWiFi();

  unsigned long hienTai = millis();

  // ======================================
  // KHOI TAO NTP KHI KET NOI LAI
  // ======================================

  if (WiFi.status() == WL_CONNECTED && !ntpStarted) {

    khoiTaoNTP();

  }

  // ======================================
  // DOC THOI GIAN
  // ======================================

  if (WiFi.status() == WL_CONNECTED &&
      hienTai - lanDocGio >= 200) {

    lanDocGio = hienTai;

    time_t now = time(nullptr);

    if (now > 100000) {

      struct tm timeinfo;

      localtime_r(&now, &timeinfo);

      gioHienTai = timeinfo.tm_hour;
      phutHienTai = timeinfo.tm_min;

      chamHienTai = (timeinfo.tm_sec % 2 == 0);

    }
  }

  // ======================================
  // DIEU KHIEN HIEN THI LED
  // ======================================

  switch (mode) {

    // ----------------------------------
    // MODE 0: GRADIENT CAU VONG
    // ----------------------------------

    case 0:

      if (hienTai - lanGradient >= speed) {

        lanGradient = hienTai;

        baseHue++;

        capNhatDongHo(
          gioHienTai,
          phutHienTai,
          chamHienTai
        );

      }

      break;

    // ----------------------------------
    // MODE 1: RGB CO DINH
    // ----------------------------------

    case 1:

      // Chi cap nhat khi gio, phut hoac
      // dau hai cham thay doi.

      if (gioHienTai != gioDaHienThi ||
          phutHienTai != phutDaHienThi ||
          chamHienTai != chamDaHienThi) {

        capNhatDongHo(
          gioHienTai,
          phutHienTai,
          chamHienTai
        );

      }

      break;

    // ----------------------------------
    // MODE MAC DINH
    // ----------------------------------

    default:

      break;
  }

  delay(2);
}
