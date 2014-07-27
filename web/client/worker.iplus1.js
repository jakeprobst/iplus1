importScripts("sql.js");

//var content = document.getElementById("content");

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
    return new SQL.Database(Uints);
}

function parse_database(db)
{
    postMessage({action:"progress", progress:"parsing db"});
    var res = db.exec("SELECT flds FROM notes;");

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
    
    return JSON.parse(conn.responseText);
}

function iplus1_start(path, nlang, tlang)
{
    var db = read_file(path);
    var sentences = parse_database(db);
    console.log(sentences);
    var response = send_request(nlang, tlang, sentences);
    
    console.log(response);
    postMessage({action:"result", result: response});
    
    
    
}
// TODO: actually make apkg? a csv is probably better/easier
// apkgs dont really offer anything special...
function make_apkg(sents)
{
    out = []
    sents.map( function (i) {
        out.push(i[0] + "\t" + i[1]);
    });
    
    postMessage({action:"apkg", buffer:out.join('\n')});
}



function handle_message(event)
{
    switch(event.data['action']) {
        case "start":
            iplus1_start(event.data['file'], event.data['nativelang'], event.data['targetlang']);
            break;
        case "apkg":
            make_apkg(event.data['sentences']);
            break;
    }
}







addEventListener("message", handle_message, false);
