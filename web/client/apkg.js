
// http://decks.wikia.com/wiki/Anki_APKG_format_documentation
// https://github.com/dae/anki
// uses template collection.anki2, will note when values are hardcoded in

function intTime()
{
    var d = new Date();
    return d.getTime();
}

function apkg_new()
{
    var conn = new XMLHttpRequest();
    conn.open("GET", "./collection.anki2", false);
    conn.responseType = "arraybuffer";
    conn.send();
    
    console.log("col: " + conn.response.length + " " + conn.response);
    ///console.log("col: " + conn.responseText.slice(conn.responseText-500, conn.responseText));
    var db = new SQL.Database(new Uint8Array(conn.response));
    console.log(db.exec("select * from col;")[0]);
    //db.handleError = console.log;
    return db;
}

function timestampID(db, table)
{
    var t = intTime();
    while (true) {
        var r = db.exec("select count() from "+table+" where id = ?", [t]);
        if (r[0]["values"][0] == 0) {
            break;
        }
        t++;
    }
    return t;
}

// anki uses a base91 function (0x20 -> 0x7e minus -\')
var base91 = ' !"#$%&()*+,./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}';
function guid64()
{
    out = "";
    for(var i = 0; i < 10; i++) {
        out += base91[Math.floor(Math.random() * base91.length)];
    }
    return out;
}


function apkg_add_card(db, front, back)
{
    var mid = 1408038028461; // 'Basic' card type, hardcoded
    var did = 1; // default deck, hardcoded
    
    //var fields = '\x1f'.join([front, back]);
    var fields = front + '\x1f' + back;
    var nid = timestampID(db, "notes");
    db.run("insert into notes values(?,?,?,?,?,?,?,?,?,?,?);", 
           [nid, guid64(), mid, intTime(), 0, "tatoeba", fields, front, 0, 0, ""]);
    
    db.run("insert into cards values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);",
            [timestampID(db, "cards"), nid, did, 0, intTime(), 0, 0,0,0,0,0,0,0,0,0,0,0,""]);
}


function apkg_blob(db)
{
    var zip = new JSZip();
    zip.file("collection.anki2", db.export());
    zip.file("media","{}");
    return zip.generate({type:"blob"});
}
