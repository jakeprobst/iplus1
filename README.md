iplus1 is a pretty cool thing to do stuff to words

## Requirements
* icu
* [snowball-stemmer](http://snowball.tartarus.org/index.php)
* mecab
* sqlite3

## Build
Change PREFIX variable in Makefile if you want, defaults to /usr/local
```
make
make install
```


#### Build on Ubuntu
```
sudo apt-get install libicu-dev libstemmer-dev libmecab-dev sqlite3
make
```
