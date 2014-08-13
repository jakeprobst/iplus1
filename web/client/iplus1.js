var iplus1 = new Worker("./worker.iplus1.js");


var ankifile = document.getElementById("ankifile");
var submit = document.getElementById("submit");
var content = document.getElementById("content");
var deckselect = document.getElementById("deckselect");
var nlang = document.getElementById("nativelang");
var tlang = document.getElementById("targetlang");



iplus1.onmessage = function(event)
{
    switch(event.data['action']) {
        case "result":
            handle_result(event.data['result']);
            break;
        case "decklist":
            handle_decklist(event.data['decklist']);
            break;
        case "progress":
            handle_progress(event.data['progress']);
            break
        case "apkg":
            handle_apkg(event.data["buffer"]);
            break;
    }
}

function get_selected_pairs()
{
    var out = []
    var results = document.getElementsByClassName("result");
    for (var i = 0; i < results.length; i++) {
        var div = results[i];
        
        if (div.getElementsByTagName("input")[0].checked) {
            var span = div.getElementsByTagName("span");
            
            var target = span[0].textContent;
            var native = span[1].textContent;
            out.push([target, native]);
        }
    }
    return out;
}

function get_selected_decks()
{
    var out = [];
    var decks = document.getElementsByClassName("deck");
    for (var i = 0; i < decks.length; i++) {
        var div = decks[i];
        
        var cb = div.getElementsByTagName("input")[0]
        if (cb.checked) {
            out.push(cb.value);
        }
    }
    
    return out;
}

function make_deck()
{
    var sents = get_selected_pairs();
    iplus1.postMessage({action:"apkg", sentences:sents});
}



function handle_result(res)
{
    res['deck'].map( function (i) {
        content.innerHTML += '<div class="result">' +
                            '<input type="checkbox">' +
                            "<span><strong>" + i[1] + "</strong></span><br />" +
                            "<span>" + i[0] + "</span>" +
                            '</div><hr />';
    });
    
    var makedeck = document.createElement("input");
    makedeck.setAttribute("type","button");
    makedeck.setAttribute("id","makedeck");
    makedeck.setAttribute("value","make deck");
    content.appendChild(makedeck);
    makedeck.addEventListener("click", make_deck, true);
}

function handle_decklist(list)
{
    deckselect.innerHTML = "";
    for(var l in list) {
        deckselect.innerHTML += '<div class="deck"><input type="checkbox" value="' + list[l][1] + '">' + list[l][0] + "</div>";
    }
}


function handle_progress(prog)
{
    content.innerHTML = prog;
}


function handle_apkg(buffer)
{
    var blob = new Blob([buffer], {type: 'text/csv'});
    var a = document.createElement("a");
    a.style = "display: none;";
    document.body.appendChild(a);
    a.href = window.URL.createObjectURL(blob);
    a.download = "tatoeba.csv";
    a.click();
}

ankifile.onchange = function ()
{
    iplus1.postMessage({action:"decklist", file: ankifile.files[0]});
}

function iplus1_start()
{
    iplus1.postMessage({action: "start", file: ankifile.files[0], nativelang: nlang.value, targetlang: tlang.value, decks:get_selected_decks()});
}

submit.addEventListener("click", iplus1_start, true);




























