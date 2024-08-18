function clock()
{
let giay = document.getElementById("giay");
let phut = document.getElementById("phut");
let gio = document.getElementById("gio");

let ngay = new Date();
var x = ngay.getHours();
var y = ngay.getMinutes();
var z = ngay.getSeconds();
var m = ngay.getMonth()+1;
var d = ngay.getDay();
var n = ngay.getFullYear();

var giay1="";
for(var i=1 ; i< 61 ; i++)
{
var xoay = i*6;
if(i==z){
giay1 += '<div class="dot active" style ="transform:rotate('+xoay+'deg)"> </div>';
}
else{
giay1 += '<div class="dot" style ="transform:rotate('+xoay+'deg)"> </div>';
}
}


var phut1="";
for (var i = 1; i < 61; i++)
{
var xoay = i * 6;
if (i == y) {
phut1 += '<div class="dot active" style ="transform:rotate(' + xoay + 'deg)"> </div>';
}
else {
phut1 += '<div class="dot" style ="transform:rotate(' + xoay + 'deg)"> </div>';
}
}

var gio1 = "";
for (var i = 1; i < 25; i++)
{
var xoay = i * 15;
if (i == x) {
gio1 += '<div class="dot active" style ="transform:rotate(' + xoay + 'deg)"> </div>';
}
else {
gio1 += '<div class="dot" style ="transform:rotate(' + xoay + 'deg)"> </div>';
}
}

gio.innerHTML= gio1 + '<h2>' + zero(x) + '</h2>';
phut.innerHTML=phut1 + '<h2>' + zero(y) + '</h2>';
giay.innerHTML=giay1 + '<h2>' + zero(z) + '</h2>';

day.innerHTML='<h2>' + zero(d) +"-" + '</h2>';
mon.innerHTML='<h2>' + zero(m)+ "-" + '</h2>';
year.innerHTML='<h2>' + n + '</h2>';

function zero(h){
if(h<10)
return '0'+h;
else{
return h;
}
}
};

setInterval(clock,1000);