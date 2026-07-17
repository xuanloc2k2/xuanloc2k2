// Bảng ký tự -> ID

const FONT_ID = {
    "0":0,  "1":1,  "2":2,  "3":3,  "4":4,
    "5":5,  "6":6,  "7":7,  "8":8,  "9":9,

    "A":10,  "Ă":11,  "Â":12,  "B":13,  "C":14,
    "D":15,  "Đ":16,  "E":17,  "Ê":18,  "G":19,
    "H":20,  "I":21,  "K":22,  "L":23,  "M":24,
    "N":25,  "O":26,  "Ô":27,  "Ơ":28,  "P":29,
    "U":30,  "Ư":31,  "V":32,  "X":33,  "Y":34,
    "Z":35,

    "À":36,  "Á":37,  "Ả":38,  "Ã":39,  "Ạ":40,
    "Ằ":41,  "Ắ":42,  "Ẳ":43,  "Ẵ":44,  "Ặ":45,
    "Ầ":46,  "Ấ":47,  "Ẩ":48,  "Ẫ":49,  "Ậ":50,
    "È":51,  "É":52,  "Ẻ":53,  "Ẽ":54,  "Ẹ":55,
    "Ề":56,  "Ế":57,  "Ể":58,  "Ễ":59,  "Ệ":60,
    "Ì":61,  "Í":62,  "Ỉ":63,  "Ĩ":64,  "Ị":65,
    "Ò":66,  "Ó":67,  "Ỏ":68,  "Õ":69,  "Ọ":70,
    "Ồ":71,  "Ố":72,  "Ổ":73,  "Ỗ":74,  "Ộ":75,
    "Ờ":76,  "Ớ":77,  "Ở":78,  "Ỡ":79,  "Ợ":80,
    "Ù":81,  "Ú":82,  "Ủ":83,  "Ũ":84,  "Ụ":85,
    "Ừ":86,  "Ứ":87,  "Ử":88,  "Ữ":89,  "Ự":90,
    "Ỳ":91,  "Ý":92,  "Ỷ":93,  "Ỹ":94,  "Ỵ":95,

    " ":96, 
    "!":97, 
    "?":98, 
    "(":99, 
    ")":100,
    ".":101, 
    ",":102, 
    ":":103, 
    "-":104,
    "/":105, 
    "+":106, 
    "%":107, 
    "&":108
};

function textToIDs(text) {
    text = text.toLocaleUpperCase("vi-VN");

    const ids = [];

    for (const ch of text) {
        ids.push(FONT_ID[ch] ?? 85);
    }

    document.getElementById("idOutput").value = ids.join(",");

    return ids;
}

function send(v1, v2, v3, v4, v5, v6, v7, v8, v9)
{
    fetch("/set?v1=" + v1 +
            "&v2=" + v2 +
            "&v3=" + v3 +
            "&v4=" + v4 +
            "&v5=" + v5 +
            "&v6=" + v6 +
            "&v7=" + v7 +
            "&v8=" + v8 +
            "&v9=" + v9 
        );
}



function updateRGB(r, g, b)
{
    // Ô nhập số
    document.getElementById("rNum").value = r;
    document.getElementById("gNum").value = g;
    document.getElementById("bNum").value = b;

    // Thanh trượt
    document.getElementById("rSlider").value = r;
    document.getElementById("gSlider").value = g;
    document.getElementById("bSlider").value = b;
}





document.addEventListener("keydown", function(e)
{
    if(e.key === "Enter")
    {
        e.preventDefault();
        sendData();
    }
});

function sendData()
{
    textToIDs(document.getElementById('text').value)
    send(
        document.getElementById("x").value,
        document.getElementById("y").value,
        document.getElementById("rNum").value,
        document.getElementById("gNum").value,
        document.getElementById("bNum").value,
        document.getElementById("mode").value,
        document.getElementById("speed").value,
        document.getElementById("bright").value,
        document.getElementById("idOutput").value
    );
}
function clearDisplay()
{
    document.getElementById("mode").value = 1; // Cập nhật ô mode
    sendData()
}
function drawLine()
{
    document.getElementById("mode").value = 0; // Cập nhật ô mode
    sendData()
}
function drawPixel()
{
    document.getElementById("mode").value = 2; // Cập nhật ô mode
    sendData()
}

