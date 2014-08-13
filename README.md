iplus1 is a library for language learning where it takes a list of words you know
and a list of sentences and tries to find sentences where exactly one new word
is present.


Currently it is rather integrated with how
[Tatoeba](http://tatoeba.org)/[Pytoeba](https://github.com/loolmeh/pytoeba-dev)
handles things. I'm not sure I can recommend it for use in anything else at the
moment (but if you are interested in using it in your own project, feel free
to message me).



## Requirements
* icu
* [snowball-stemmer](http://snowball.tartarus.org/index.php)
* mecab
* sqlite3
* redis

## Build
Change PREFIX variable in Makefile if you want, defaults to /usr/local
```
make
make install
```


#### Build on Ubuntu
```
sudo apt-get install libicu-dev libstemmer-dev libmecab-dev libsqlite3-dev libhiredis-dev
make
make install
```

## Initialization

Retrieve fresh data:

```
./preparefiles.py
```

Populate redis db

```
redis-server
./fillredis.py
./insertlang.py
```

# Usage

See example.py for basic usage.  
example.c for a demo taking an anki deck as an argument  
./web/server/ contains the code used on tatoeba
