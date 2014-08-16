importScripts("sql.js");
importScripts("jszip.min.js");
importScripts("apkg.js");

//var content = document.getElementById("content");

var valid_deck = false;

function clean_string(instr)
{
    var s = instr.split("\x1f")[0];
    var out = s.replace(/{{.*?::(.*?)}}/g, "$1");
    var out = out.replace(/(<.*?>)/g, "$1");
    return out;
}

function read_file(path)
{
    postMessage({action:"progress", progress:"reading file"});
    var r = new FileReaderSync();
    var result = r.readAsArrayBuffer(path);
    var Uints = new Uint8Array(result);
    postMessage({action:"progress", progress:""});
    return new SQL.Database(Uints);
}

function parse_database(db, decks)
{
    postMessage({action:"progress", progress:"parsing db"});
    
    var dids = decks[0];
    for(var i = 1; i < decks.length; i++) {
        dids += ", " + decks[i];
    }
    
    // if the interval is greater than 21 the card is mature and should be known
    var cards = db.exec("SELECT DISTINCT nid FROM cards WHERE did IN ("+ dids +") AND ivl > 21;");
    console.log(cards);
    if (cards.length == 0) {
        postMessage({action:"progress", progress:"no valid cards in selected deck(s)"});
        return "";
    }
    
    var nids = "'" + cards[0]["values"][0] + "'";
    for(var i = 1; i < cards[0]["values"].length; i++) {
        nids += ",'" + cards[0]["values"][i] + "'";
    }
    
    var res = db.exec("SELECT flds FROM notes WHERE id in ("+ nids +");");

    out = [];
    res[0]["values"].map( function(i) {
        out.push(clean_string(i.toString()));
    });
    return out;
}

function send_request(nlang, tlang, sentences)
{
    postMessage({action:"progress", progress:"sending request"});
    var req_data = {"native_lang":nlang, "target_lang": tlang, "deck": sentences};
    
    var conn = new XMLHttpRequest();
    conn.open("POST", "/api/iplus1/?format=json", false);
    conn.setRequestHeader("Content-Type", "application/json");
    
    postMessage({action:"progress", progress:"waiting for server"});
    conn.send(JSON.stringify(req_data));
    
    postMessage({action:"progress", progress:""});
    return JSON.parse(conn.responseText);
}

function iplus1_start(path, nlang, tlang, decks)
{
    if (!valid_deck) {
        return;
    }
    if (decks.length == 0) {
        postMessage({action:"progress", progress:"select deck(s) to get cards from"});
        return;
    }
    
    var db = read_file(path);
    var sentences = parse_database(db, decks);
    if (sentences == "") {
        return;
    }
    var response = send_request(nlang, tlang, sentences);
    
    postMessage({action:"result", result: response});
}

// TODO: actually make apkg? a csv is probably better/easier
// apkgs dont really offer anything special...
function make_csv(sents)
{
    out = []
    sents.map( function (i) {
        out.push(i[0] + "\t" + i[1]);
    });
    postMessage({action:"csv", buffer:out.join('\n')});
}

function make_apkg(sents)
{
    apkg = apkg_new();
    sents.map( function (i) {
        apkg_add_card(apkg, i[0], i[1]);
    });
    postMessage({action:"apkg", buffer:apkg_blob(apkg)});
}

function get_decks(path)
{
    var db = read_file(path);
    try {
        var decks = db.exec("SELECT decks FROM col;");
        valid_deck = true;
    }
    catch (err) {
        console.log(err);
        postMessage({action:"progress", progress:"select a valid anki deck"});
        postMessage({action:"decklist", decklist:[]});
        valid_deck = false;
        return;
    }
    
    decks = JSON.parse(decks[0]["values"][0]);
    
    var dl = [];
    for(var d in decks) {
        dl.push([decks[d]["name"], decks[d]["id"]]);
    }
    
    postMessage({action:"decklist", decklist:dl});
}



function handle_message(event)
{
    switch(event.data['action']) {
        case "decklist":
            get_decks(event.data['file']);
            break;
        case "start":
            iplus1_start(event.data['file'], event.data['nativelang'], event.data['targetlang'], event.data["decks"]);
            break;
        case "apkg":
            make_apkg(event.data['sentences']);
            break;
        case "csv":
            make_csv(event.data['sentences']);
            break;
    }
}

addEventListener("message", handle_message, false);
