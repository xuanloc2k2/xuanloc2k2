
let dangQuet = false;
let daChuyenGiaoDien = false;
let dangKiemTraTrangThai = false;

let timerGui = null;

// ======================================
// KHOI TAO BO CHON MAU IRO
// ======================================
document.getElementById("ip").innerText = "IP: " + ip;
const picker = new iro.ColorPicker("#colorPicker", {
    width: 280,
    color: "#ff0000",

    layout: [
        {
            component: iro.ui.Wheel
        },
        {
            component: iro.ui.Slider,
            options: {
                sliderType: "value"
            }
        }
    ]
});

// ======================================
// CAP NHAT MAU RGB
// ======================================

function capNhatMau(color) {

    document.getElementById("red").value = color.rgb.r;
    document.getElementById("green").value = color.rgb.g;
    document.getElementById("blue").value = color.rgb.b;

    document.getElementById("hex").value = color.hexString;

    document.getElementById("preview").style.background =
        color.hexString;

    guiDieuKhien();
}

picker.on("color:change", capNhatMau);


// ======================================
// GUI DU LIEU DEN ESP8266
// ======================================

function guiDieuKhien() {

    clearTimeout(timerGui);

    timerGui = setTimeout(async function() {

        const data = new URLSearchParams({

            r: document.getElementById("red").value,
            g: document.getElementById("green").value,
            b: document.getElementById("blue").value,

            brightness: document.getElementById("brightness").value,
            speed: document.getElementById("speed").value

        });

        try {

            const response = await fetch("/set", {

                method: "POST",

                headers: {
                    "Content-Type": "application/x-www-form-urlencoded"
                },

                body: data

            });

            if (!response.ok) {

                console.log("ESP8266 khong nhan duoc du lieu!");

            }

        } catch (error) {

            console.log("Loi gui dieu khien:", error);

        }

    }, 80);
}


// ======================================
// NHAP RGB THU CONG
// ======================================

["red", "green", "blue"].forEach(id => {

    document.getElementById(id).addEventListener("input", function() {

        const redInput = document.getElementById("red").value;
        const greenInput = document.getElementById("green").value;
        const blueInput = document.getElementById("blue").value;

        if (
            redInput === "" ||
            greenInput === "" ||
            blueInput === ""
        ) return;

        const r = Number(redInput);
        const g = Number(greenInput);
        const b = Number(blueInput);

        if ([r, g, b].some(v =>
            !Number.isInteger(v) || v < 0 || v > 255
        )) return;

        picker.color.rgb = { r, g, b };

    });

});


// ======================================
// DO SANG
// ======================================

document.getElementById("brightness").addEventListener("input", function() {

    document.getElementById("brightnessValue").innerText = this.value;

    guiDieuKhien();

});


// ======================================
// TOC DO
// ======================================

document.getElementById("speed").addEventListener("input", function() {

    document.getElementById("speedValue").innerText = this.value;

    guiDieuKhien();

});


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

            const response = await fetch("/scan?t=" + Date.now(), {
                cache: "no-store"
            });

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
                w.ssid + " (" + w.rssi + " dBm)" +
                (w.secure ? " 🔒" : "");

            select.appendChild(option);

        });

        status.innerText = networks.length
            ? "Tìm thấy " + networks.length + " mạng WiFi"
            : "Không tìm thấy WiFi";

    } catch (error) {

        status.innerText =
            "Lỗi quét WiFi: " + error.message;

    } finally {

        dangQuet = false;

    }

}


// ======================================
// KET NOI WIFI
// ======================================


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

        if (!response.ok) return;

        const data = await response.json();


    } catch (error) {

        // Không thay đổi giao diện khi mất kết nối tạm thời

    } finally {

        dangKiemTraTrangThai = false;

    }

}


// ======================================
// KHOI DONG
// ======================================



document.getElementById("wifiForm").addEventListener("submit", async function(e) {

    e.preventDefault();

    const ssid = document.getElementById("wifi").value.trim();
    const pass = document.getElementById("pass").value;
    const status = document.getElementById("status");
    const btn = this.querySelector('[type="submit"]');

    if (!ssid) {
        status.innerText = "Vui lòng chọn WiFi!";
        return;
    }

    // Khóa nút tránh gửi nhiều lần
    btn.disabled = true;

    status.innerText = "Đang gửi thông tin WiFi...";

    try {

        const response = await fetch("/connect", {

            method: "POST",

            headers: {
                "Content-Type": "application/x-www-form-urlencoded"
            },

            body:
                "ssid=" + encodeURIComponent(ssid) +
                "&pass=" + encodeURIComponent(pass)

        });

        if (!response.ok) {
            throw new Error("HTTP " + response.status);
        }

        const data = await response.json();

        if (data.success === false) {
            throw new Error(data.message || "Không thể gửi thông tin WiFi");
        }

        status.innerText = "Đã gửi thông tin. Đang chờ kết nối WiFi...";

        // Chờ ESP8266 bắt đầu kết nối
        setTimeout(() => {
            kiemTraTrangThai();
        }, 1500);

    }
    catch (error) {

        status.innerText = "Lỗi: " + error.message;

    }
    finally {

        btn.disabled = false;

    }

});



document.addEventListener("DOMContentLoaded", async function() {

    // Luôn hiển thị cả hai giao diện

    hienGiaoDienWiFi();

    // Kiểm tra WiFi hiện tại

    await kiemTraTrangThai();

    // Nếu chưa kết nối thì tự động quét WiFi

    if (!daChuyenGiaoDien) {

        quetWiFi();

    }

    // Kiểm tra WiFi mỗi 2 giây

    setInterval(kiemTraTrangThai, 2000);

});