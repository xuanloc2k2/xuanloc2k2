
function send(v1, v2, v3, v4, v5, v6, v7, v8)
{
    fetch("/set?v1=" + v1 +
            "&v2=" + v2 +
            "&v3=" + v3 +
            "&v4=" + v4 +
            "&v5=" + v5 +
            "&v6=" + v6 +
            "&v7=" + v7 +
            "&v8=" + v8
        );
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
send(
document.getElementById("x").value,
document.getElementById("y").value,
document.getElementById("r").value,
document.getElementById("g").value,
document.getElementById("b").value,
document.getElementById("mode").value,
document.getElementById("speed").value,
document.getElementById("bright").value
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


