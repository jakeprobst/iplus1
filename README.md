iplus1 is a pretty cool thing to do stuff to words

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
```

## Usage

# Server

First, retrieve fresh data:

```
./preparefiles.py
```

Then start the server (no initial output, just let it sit in its own terminal for instance):

```
./iplus1d
```

# Client

```
LD_LIBRARY_PATH=. ./iplus1c
```
