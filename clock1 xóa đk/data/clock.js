
let dangQuet = false;
let dangKiemTraTrangThai = false;

// ======================================
// QUET WIFI
// ======================================

async function quetWiFi() {

    if (dangQuet) return;

    dangQuet = true;

    const select = document.getElementById("wifi");
    const status = document.getElementById("status");

    select.innerHTML =
        '<option value="">Đang quét WiFi...</option>';

    status.innerText = "Đang tìm mạng WiFi...";

    try {

        let data;
        let hoanTat = false;

        for (let i = 0; i < 25; i++) {

            const response = await fetch(
                "/scan?t=" + Date.now(),
                { cache: "no-store" }
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

        status.innerText = networks.length
            ? "Tìm thấy " + networks.length + " mạng WiFi"
            : "Không tìm thấy WiFi";

    }
    catch (error) {

        status.innerText =
            "Lỗi quét WiFi: " + error.message;

    }
    finally {

        dangQuet = false;

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
            { cache: "no-store" }
        );

        if (!response.ok) return;

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

        console.log("Lỗi kiểm tra WiFi:", error);

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

    const ssid = document.getElementById("wifi").value.trim();
    const pass = document.getElementById("pass").value;

    const status = document.getElementById("status");

    const btn = this.querySelector('[type="submit"]');

    if (!ssid) {

        status.innerText = "Vui lòng chọn WiFi!";

        return;
    }

    // Khoa nut ket noi
    btn.disabled = true;

    status.innerText = "Đang gửi thông tin WiFi...";

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

            throw new Error("HTTP " + response.status);

        }

        const result = await response.json();

        if (!result.success) {

            throw new Error(
                result.message || "Không thể kết nối WiFi"
            );

        }

        status.innerText =
            "Đã gửi thông tin. Đang kết nối WiFi...";

        // Bat dau kiem tra trang thai
        setTimeout(kiemTraTrangThai, 1000);

    }
    catch (error) {

        status.innerText =
            "Lỗi kết nối: " + error.message;

    }
    finally {

        btn.disabled = false;

    }

});


// ======================================
// KHOI DONG
// ======================================

document.addEventListener("DOMContentLoaded", async function() {

    // Kiem tra WiFi hien tai
    await kiemTraTrangThai();

    // Tu dong quet WiFi
    quetWiFi();

    // Kiem tra trang thai moi 2 giay
    setInterval(kiemTraTrangThai, 2000);

});
