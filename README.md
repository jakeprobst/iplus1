iplus1 is a pretty cool thing to do stuff to words

This is designed to be part of [Tatoeba](http://tatoeba.org)/[Pytoeba](https://github.com/loolmeh/pytoeba-dev).
While I suppose it is possible to link this into any project, I don't think I would recommend it.
I might later on write a version that doesn't depend on redis to keep track of data (maybe local sqlite file, the reason it doesnt use this already is cause lool0 was crying about disk access on the server). If someone seriously wants it I`ll do it.
But if you really want to:

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

See example.py
