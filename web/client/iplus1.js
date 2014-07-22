var iplus1 = new Worker("./worker.iplus1.js");


var ankifile = document.getElementById("ankifile");
var submit = document.getElementById("submit");
var content = document.getElementById("content");
var nlang = document.getElementById("nativelang");
var tlang = document.getElementById("targetlang");


function iplus1_start()
{
    iplus1.postMessage({action: "start", file: ankifile.files[0], nativelang: nlang.value, targetlang: tlang.value});
}

submit.addEventListener("click", iplus1_start, true);





























