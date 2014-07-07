var worker = new Worker("./worker.sql.js");

var ankifile = document.getElementById("ankifile");
var submit = document.getElementById("submit");
var content = document.getElementById("content");

function clean_string(instr)
{
    var s = instr.split("\x1f")[0];
    var out = s.replace(/{{.*?::(.*?)}}/g, "$1");
    console.log(out);
    return out;
}

function read_deck()
{
    worker.onmessage = function(event) {
        var results = event.data.results;
        
        console.log(event.data);
        console.log("length: " + results.length);
        for(var i = 0; i < results.length; i++) {
            for(var v = 0; v < results[i].values.length; v++) {
                var s = document.createElement("div");
                s.innerHTML += clean_string(results[i].values[v][0]);
                content.appendChild(s);
            }
        }
    }
    
    worker.postMessage({action:"exec", sql:"SELECT flds FROM notes;"});
}


ankifile.onchange = function () {
    var f = new FileReader();
    f.onload = function () {
        console.log("opening: " + f);
        worker.postMessage({action:"open", buffer:f.result}, [f.result]);
    }
    f.readAsArrayBuffer(ankifile.files[0]);    
}
    


submit.addEventListener("click", read_deck, true);


