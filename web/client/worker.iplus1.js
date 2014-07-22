importScripts("sql.js");

function clean_string(instr)
{
    var s = instr.split("\x1f")[0];
    var out = s.replace(/{{.*?::(.*?)}}/g, "$1");
    return out;
}


function parse_database(db)
{
    var res = db.exec("SELECT flds FROM notes;");

    out = [];
    res[0]["values"].map( function(i) {
        out.push(clean_string(i.toString()));
    });
    return out;    
}

function read_file(path)
{
    var r = new FileReaderSync();
    var result = r.readAsArrayBuffer(path);
    var Uints = new Uint8Array(result);
    return new SQL.Database(Uints);
}

function iplus1_start(path, nlang, tlang)
{
    var db = read_file(path);
    var sentences = parse_database(db);
    
    var req_data = {"native_lang":nlang, "target_lang": tlang, "deck": sentences};
    
    
    var conn = new XMLHttpRequest();
    conn.open("POST", "/api/iplus1/?format=json", false);
    conn.setRequestHeader("Content-Type", "application/json");
    conn.send(JSON.stringify(req_data));
    //conn.send('{"native_lang":"eng","target_lang":"jpn","deck":["…だったら尾行なんて姑息な真似、最初からしない方がよかったのに…","といっても、勿論本当に投函する訳じ ゃあ、露骨に話題を逸らした…。"]}');
    
    console.log(conn.responseText);


}






function handle_message(event)
{
    switch(event.data['action']) {
        case "start":
            iplus1_start(event.data['file'], event.data['nativelang'], event.data['targetlang']);
            break;
    }
}







addEventListener("message", handle_message, false);
