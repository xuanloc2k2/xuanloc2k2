
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <time.h>

// ======================================
// CAU HINH WIFI AP
// ======================================

const char* AP_SSID = "DONGHO-SETUP";
const char* AP_PASS = "12345678";

// ======================================
// KHOI TAO SERVER
// ======================================

AsyncWebServer server(80);
DNSServer dnsServer;

// ======================================
// BIEN TRANG THAI WIFI
// ======================================

bool cheDoCauHinh = false;
bool dangKetNoi = false;
bool dangQuetWiFi = false;
bool ntpStarted = false;
bool littleFS_OK = false;

String wifiSSID = "";
String wifiPASS = "";

unsigned long thoiDiemKetNoi = 0;

// ======================================
// CAU HINH NTP
// ======================================

const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;

// ======================================
// KHAI BAO HAM
// ======================================

void batWiFiAP();
void khoiTaoNTP();
void kiemTraKetNoiWiFi();

void xuLyScan(AsyncWebServerRequest *request);
void xuLyTrangThai(AsyncWebServerRequest *request);
void xuLyKetNoi(AsyncWebServerRequest *request);

void dangKyWeb();

// ======================================
// JSON ESCAPE
// ======================================

String jsonEscape(const String &input) {

    String output;

    output.reserve(input.length() + 8);

    for (size_t i = 0; i < input.length(); i++) {

        char c = input[i];

        switch (c) {

            case '\"':
                output += "\\\"";
                break;

            case '\\':
                output += "\\\\";
                break;

            case '\n':
                output += "\\n";
                break;

            case '\r':
                output += "\\r";
                break;

            case '\t':
                output += "\\t";
                break;

            default:

                if ((uint8_t)c >= 0x20) {
                    output += c;
                }

                break;
        }
    }

    return output;
}


// ======================================
// KHOI TAO NTP
// ======================================

void khoiTaoNTP() {

    if (WiFi.status() != WL_CONNECTED) {
        return;
    }

    configTime(
        gmtOffset_sec,
        daylightOffset_sec,
        "pool.ntp.org",
        "time.nist.gov",
        "time.google.com"
    );

    ntpStarted = true;

    Serial.println("[NTP] Da khoi tao dong bo thoi gian!");
}


// ======================================
// BAT WIFI AP CAU HINH
// ======================================

void batWiFiAP() {

    WiFi.mode(WIFI_AP_STA);

    bool ketQua = WiFi.softAP(
        AP_SSID,
        AP_PASS
    );

    if (ketQua) {

        dnsServer.start(
            53,
            "*",
            WiFi.softAPIP()
        );

        cheDoCauHinh = true;

        Serial.println();
        Serial.println("========== WIFI SETUP ==========");

        Serial.print("[AP] Ten WiFi: ");
        Serial.println(AP_SSID);

        Serial.print("[AP] Mat khau: ");
        Serial.println(AP_PASS);

        Serial.print("[AP] Dia chi IP: ");
        Serial.println(WiFi.softAPIP());

        Serial.println("================================");
    }
    else {

        Serial.println(
            "[AP] Khong the khoi dong WiFi AP!"
        );
    }
}


// ======================================
// KIEM TRA KET NOI WIFI
// ======================================

void kiemTraKetNoiWiFi() {

    if (!dangKetNoi) {
        return;
    }

    // ----------------------------------
    // KET NOI THANH CONG
    // ----------------------------------

    if (WiFi.status() == WL_CONNECTED) {

        dangKetNoi = false;

        Serial.println();
        Serial.println("[WIFI] Ket noi thanh cong!");

        Serial.print("[WIFI] SSID: ");
        Serial.println(WiFi.SSID());

        Serial.print("[WIFI] Dia chi IP: ");
        Serial.println(WiFi.localIP());

        khoiTaoNTP();

        // Tat AP cau hinh

        dnsServer.stop();

        WiFi.softAPdisconnect(true);

        WiFi.mode(WIFI_STA);

        cheDoCauHinh = false;

        Serial.println("[AP] Da tat WiFi cau hinh.");

        Serial.println(
            "[SYSTEM] Hay truy cap IP moi cua ESP8266."
        );

        return;
    }

    // ----------------------------------
    // KET NOI THAT BAI SAU 20 GIAY
    // ----------------------------------

    if (millis() - thoiDiemKetNoi >= 20000) {

        dangKetNoi = false;

        Serial.println();
        Serial.println("[WIFI] Ket noi that bai!");

        Serial.println(
            "[WIFI] Quay lai che do cau hinh AP..."
        );

        WiFi.disconnect();

        batWiFiAP();
    }
}


// ======================================
// API QUET WIFI
// ======================================

void xuLyScan(AsyncWebServerRequest *request) {

    // Khong quet khi dang ket noi WiFi
    if (dangKetNoi) {

        request->send(
            409,
            "application/json",
            "{\"done\":false,\"message\":\"Dang ket noi WiFi\"}"
        );

        return;
    }

    int soMang = WiFi.scanComplete();

    // ----------------------------------
    // WIFI DANG QUET
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
    // CHUA QUET HOAC QUET THAT BAI
    // ----------------------------------

    if (soMang == WIFI_SCAN_FAILED) {

        if (dangQuetWiFi) {

            Serial.println(
                "[SCAN] Qua trinh quet that bai!"
            );

            dangQuetWiFi = false;
        }

        Serial.println("[SCAN] Bat dau quet WiFi...");

        WiFi.scanDelete();

        int ketQua = WiFi.scanNetworks(
            true,
            true
        );

        if (ketQua == WIFI_SCAN_RUNNING) {

            dangQuetWiFi = true;

            Serial.println(
                "[SCAN] Da gui lenh quet!"
            );

        }
        else {

            Serial.println(
                "[SCAN] Khong the bat dau quet!"
            );

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

    Serial.println();
    Serial.println("[SCAN] Quet WiFi hoan tat!");

    Serial.print("[SCAN] Tim thay: ");
    Serial.print(soMang);
    Serial.println(" mang WiFi");

    String json =
        "{\"done\":true,\"networks\":[";

    for (int i = 0; i < soMang; i++) {

        String tenWiFi = WiFi.SSID(i);

        int rssi = WiFi.RSSI(i);

        bool baoMat =
            (WiFi.encryptionType(i) != ENC_TYPE_NONE);

        Serial.println("-----------------------------");

        Serial.print("[WIFI] STT: ");
        Serial.println(i + 1);

        Serial.print("[WIFI] SSID: ");
        Serial.println(tenWiFi);

        Serial.print("[WIFI] RSSI: ");
        Serial.print(rssi);
        Serial.println(" dBm");

        Serial.print("[WIFI] Bao mat: ");

        Serial.println(
            baoMat ? "Co mat khau" : "Khong mat khau"
        );

        if (i > 0) {
            json += ",";
        }

        json += "{";

        json += "\"ssid\":\"";
        json += jsonEscape(tenWiFi);
        json += "\",";

        json += "\"rssi\":";
        json += String(rssi);
        json += ",";

        json += "\"secure\":";
        json += baoMat ? "true" : "false";

        json += "}";
    }

    json += "]}";

    if (soMang == 0) {

        Serial.println(
            "[SCAN] Khong tim thay WiFi nao!"
        );
    }

    Serial.println("[SCAN] Da gui danh sach WiFi.");
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
// API TRANG THAI WIFI
// ======================================

void xuLyTrangThai(AsyncWebServerRequest *request) {

    bool daKetNoi =
        (WiFi.status() == WL_CONNECTED);

    String trangThai;

    if (dangKetNoi) {

        trangThai = "Dang ket noi WiFi";
    }
    else if (daKetNoi) {

        trangThai = "Da ket noi WiFi";
    }
    else if (cheDoCauHinh) {

        trangThai = "Chua ket noi WiFi - Dang cau hinh";
    }
    else {

        trangThai = "Chua ket noi WiFi";
    }

    String ip;

    if (daKetNoi) {

        ip = WiFi.localIP().toString();
    }
    else if (cheDoCauHinh) {

        ip = WiFi.softAPIP().toString();
    }
    else {

        ip = "0.0.0.0";
    }

    String json = "{";

    json += "\"connected\":";
    json += daKetNoi ? "true" : "false";

    json += ",\"status\":\"";
    json += jsonEscape(trangThai);

    json += "\",\"ip\":\"";
    json += ip;

    json += "\"}";

    request->send(
        200,
        "application/json; charset=utf-8",
        json
    );
}


// ======================================
// API KET NOI WIFI
// ======================================

void xuLyKetNoi(AsyncWebServerRequest *request) {

    // ----------------------------------
    // KIEM TRA DU LIEU
    // ----------------------------------

    if (!request->hasParam("ssid", true) ||
        !request->hasParam("pass", true)) {

        request->send(
            400,
            "application/json",
            "{\"success\":false,\"message\":\"Thieu thong tin WiFi\"}"
        );

        return;
    }

    wifiSSID =
        request->getParam("ssid", true)->value();

    wifiPASS =
        request->getParam("pass", true)->value();

    wifiSSID.trim();

    if (wifiSSID.length() == 0) {

        request->send(
            400,
            "application/json",
            "{\"success\":false,\"message\":\"Ten WiFi khong hop le\"}"
        );

        return;
    }

    // ----------------------------------
    // KIEM TRA DANG KET NOI
    // ----------------------------------

    if (dangKetNoi) {

        request->send(
            409,
            "application/json",
            "{\"success\":false,\"message\":\"Dang co yeu cau ket noi WiFi\"}"
        );

        return;
    }

    // ----------------------------------
    // PHAN HOI TRUOC
    // ----------------------------------

    request->send(
        200,
        "application/json",
        "{\"success\":true,\"message\":\"Dang ket noi WiFi...\"}"
    );

    Serial.println();
    Serial.println("========== KET NOI WIFI ==========");

    Serial.print("[WIFI] SSID: ");
    Serial.println(wifiSSID);

    // ----------------------------------
    // BAT DAU KET NOI
    // ----------------------------------

    WiFi.mode(WIFI_AP_STA);

    WiFi.begin(
        wifiSSID.c_str(),
        wifiPASS.c_str()
    );

    dangKetNoi = true;

    thoiDiemKetNoi = millis();

    Serial.println("[WIFI] Dang cho ket noi...");
}


// ======================================
// DANG KY CAC DUONG DAN WEB
// ======================================

void dangKyWeb() {

    // ----------------------------------
    // TRANG CHU
    // ----------------------------------

    server.on("/", HTTP_GET,
        [](AsyncWebServerRequest *request) {

            if (!littleFS_OK ||
                !LittleFS.exists("/clock.html")) {

                request->send(
                    500,
                    "text/plain; charset=utf-8",
                    "Khong tim thay clock.html trong LittleFS"
                );

                return;
            }

            request->send(
                LittleFS,
                "/clock.html",
                "text/html; charset=utf-8"
            );
        }
    );


    // ----------------------------------
    // CSS
    // ----------------------------------

    server.on("/clock.css", HTTP_GET,
        [](AsyncWebServerRequest *request) {

            request->send(
                LittleFS,
                "/clock.css",
                "text/css; charset=utf-8"
            );
        }
    );


    // ----------------------------------
    // JAVASCRIPT
    // ----------------------------------

    server.on("/clock.js", HTTP_GET,
        [](AsyncWebServerRequest *request) {

            request->send(
                LittleFS,
                "/clock.js",
                "application/javascript; charset=utf-8"
            );
        }
    );


    // ----------------------------------
    // API QUET WIFI
    // ----------------------------------

    server.on("/scan", HTTP_GET,
        [](AsyncWebServerRequest *request) {

            xuLyScan(request);
        }
    );


    // ----------------------------------
    // API KET NOI WIFI
    // ----------------------------------

    server.on("/connect", HTTP_POST,
        [](AsyncWebServerRequest *request) {

            xuLyKetNoi(request);
        }
    );


    // ----------------------------------
    // API TRANG THAI
    // ----------------------------------

    server.on("/status", HTTP_GET,
        [](AsyncWebServerRequest *request) {

            xuLyTrangThai(request);
        }
    );


    // ----------------------------------
    // KHONG TIM THAY DUONG DAN
    // ----------------------------------

    server.onNotFound(
        [](AsyncWebServerRequest *request) {

            request->send(
                404,
                "text/plain; charset=utf-8",
                "Khong tim thay trang"
            );
        }
    );


    // ----------------------------------
    // KHOI DONG SERVER
    // ----------------------------------

    server.begin();

    Serial.println(
        "[WEB] Web Server da khoi dong!"
    );
}


// ======================================
// SETUP
// ======================================

void setup() {

    Serial.begin(9600);

    delay(100);

    Serial.println();
    Serial.println("================================");
    Serial.println("      ESP8266 WIFI WEB SERVER");
    Serial.println("================================");


    // ----------------------------------
    // KHOI DONG LITTLEFS
    // ----------------------------------

    if (!LittleFS.begin()) {

        littleFS_OK = false;

        Serial.println(
            "[FS] Loi khoi dong LittleFS!"
        );
    }
    else {

        littleFS_OK = true;

        Serial.println("[FS] LittleFS OK!");

        Serial.println("[FS] Danh sach file:");

        Dir dir = LittleFS.openDir("/");

        while (dir.next()) {

            Serial.println(
                dir.fileName()
            );
        }
    }


    // ----------------------------------
    // THU KET NOI WIFI DA LUU
    // ----------------------------------

    WiFi.mode(WIFI_STA);

    WiFi.begin();

    Serial.println(
        "[WIFI] Dang kiem tra WiFi da luu..."
    );

    unsigned long batDau = millis();

    while (
        WiFi.status() != WL_CONNECTED &&
        millis() - batDau < 15000
    ) {

        delay(300);

        Serial.print(".");
    }

    Serial.println();


    // ----------------------------------
    // KIEM TRA KET QUA
    // ----------------------------------

    if (WiFi.status() == WL_CONNECTED) {

        Serial.println(
            "[WIFI] Ket noi WiFi da luu thanh cong!"
        );

        Serial.print("[WIFI] SSID: ");
        Serial.println(WiFi.SSID());

        Serial.print("[WIFI] IP: ");
        Serial.println(WiFi.localIP());

        khoiTaoNTP();
    }
    else {

        Serial.println(
            "[WIFI] Khong co WiFi da luu!"
        );

        batWiFiAP();
    }


    // ----------------------------------
    // KHOI DONG WEB SERVER
    // ----------------------------------

    dangKyWeb();

    Serial.println(
        "[SYSTEM] ESP8266 san sang!"
    );
}


// ======================================
// LOOP
// ======================================

void loop() {

    // Xu ly DNS khi dang o che do AP

    if (cheDoCauHinh) {

        dnsServer.processNextRequest();
    }


    // Kiem tra tien trinh ket noi WiFi

    kiemTraKetNoiWiFi();


    // Khoi tao lai NTP neu chua khoi tao

    if (
        WiFi.status() == WL_CONNECTED &&
        !ntpStarted
    ) {

        khoiTaoNTP();
    }


    yield();
}
