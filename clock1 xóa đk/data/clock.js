
let dangQuet = false;
let dangKiemTraTrangThai = false;
let dangGuiKetNoi = false;

let timerTrangThai = null;

// ======================================
// HAM HIEN THI TRANG THAI
// ======================================

function hienThiTrangThai(noiDung) {

    const status = document.getElementById("status");

    if (status) {
        status.innerText = noiDung;
    }
}


// ======================================
// QUET WIFI
// ======================================

async function quetWiFi() {

    if (dangQuet || dangGuiKetNoi) return;

    dangQuet = true;

    const select = document.getElementById("wifi");
    const btnScan = document.getElementById("btnScan");

    btnScan.disabled = true;

    select.innerHTML =
        '<option value="">Đang quét WiFi...</option>';

    hienThiTrangThai("Đang tìm mạng WiFi...");

    try {

        let data;
        let hoanTat = false;

        for (let i = 0; i < 25; i++) {

            const response = await fetch(
                "/scan?t=" + Date.now(),
                {
                    cache: "no-store"
                }
            );

            if (!response.ok) {
                throw new Error("Không thể quét WiFi");
            }

            data = await response.json();

            if (data.done) {

                hoanTat = true;

                break;
            }

            await new Promise(resolve =>
                setTimeout(resolve, 1000)
            );
        }

        if (!hoanTat) {
            throw new Error("Quét WiFi quá thời gian");
        }

        select.innerHTML =
            '<option value="">-- Chọn WiFi --</option>';

        const networks = data.networks || [];

        networks.forEach(w => {

            if (!w.ssid) return;

            const option = document.createElement("option");

            option.value = w.ssid;

            option.textContent =
                w.ssid +
                " (" + w.rssi + " dBm)" +
                (w.secure ? " 🔒" : "");

            select.appendChild(option);

        });

        hienThiTrangThai(
            networks.length
                ? "Tìm thấy " + networks.length + " mạng WiFi"
                : "Không tìm thấy WiFi"
        );

    }
    catch (error) {

        hienThiTrangThai(
            "Lỗi quét WiFi: " + error.message
        );

    }
    finally {

        dangQuet = false;

        btnScan.disabled = false;

    }
}


// ======================================
// KIEM TRA TRANG THAI WIFI
// ======================================

async function kiemTraTrangThai() {

    if (dangKiemTraTrangThai) return;

    dangKiemTraTrangThai = true;

    try {

        const response = await fetch(
            "/status?t=" + Date.now(),
            {
                cache: "no-store"
            }
        );

        if (!response.ok) {
            throw new Error("Không nhận được phản hồi");
        }

        const data = await response.json();

        const status = document.getElementById("status");
        const ipElement = document.getElementById("ip");

        if (status) {
            status.innerText = data.status;
        }

        if (ipElement) {
            ipElement.innerText = "IP: " + data.ip;
        }

    }
    catch (error) {

        console.log(
            "Lỗi kiểm tra WiFi:",
            error
        );

    }
    finally {

        dangKiemTraTrangThai = false;

    }
}


// ======================================
// KET NOI WIFI
// ======================================

document.getElementById("wifiForm")
.addEventListener("submit", async function(e) {

    e.preventDefault();

    if (dangGuiKetNoi) return;

    const ssid = document.getElementById("wifi").value.trim();

    const pass = document.getElementById("pass").value;

    const btn = document.getElementById("btnConnect");

    if (!ssid) {

        hienThiTrangThai("Vui lòng chọn WiFi!");

        return;
    }

    dangGuiKetNoi = true;

    btn.disabled = true;

    hienThiTrangThai("Đang gửi thông tin WiFi...");

    try {

        const data = new URLSearchParams();

        data.append("ssid", ssid);
        data.append("pass", pass);

        const response = await fetch("/connect", {

            method: "POST",

            headers: {
                "Content-Type":
                    "application/x-www-form-urlencoded"
            },

            body: data

        });

        if (!response.ok) {

            const errorData = await response.json();

            throw new Error(
                errorData.message || "HTTP " + response.status
            );

        }

        const result = await response.json();

        if (!result.success) {

            throw new Error(
                result.message || "Không thể kết nối WiFi"
            );

        }

        hienThiTrangThai(
            "Đã gửi thông tin. Đang kết nối WiFi..."
        );

        // Kiem tra lai trang thai sau 1 giay
        setTimeout(kiemTraTrangThai, 1000);

    }
    catch (error) {

        hienThiTrangThai(
            "Lỗi kết nối: " + error.message
        );

        dangGuiKetNoi = false;

        btn.disabled = false;

    }

});


// ======================================
// KHOI DONG
// ======================================

document.addEventListener("DOMContentLoaded", async function() {

    // Kiem tra WiFi hien tai
    await kiemTraTrangThai();

    // Tu dong quet WiFi neu chua ket noi
    quetWiFi();

    // Kiem tra trang thai moi 2 giay
    timerTrangThai = setInterval(
        kiemTraTrangThai,
        2000
    );

});
