CXX = gcc
PREFIX=/usr/local


LIB_VERSION = 1.0.0
LIB_SHORTVER = 1

all: libiplus1.so languages pyplus1

build:
	mkdir build/

# libiplus1

LIB_CFLAGS = -fPIC -ggdb -Wall -Werror `pkg-config --cflags icu-uc` -DPREFIX=\"$(PREFIX)\"
LIB_LDFLAGS = -fPIC -ldl `pkg-config --libs icu-uc` -lhiredis

LIB_SRC = $(wildcard src/iplus1/*.c)
LIB_OSRC = $(patsubst src/iplus1/%,%,$(LIB_SRC))
LIB_OBJ = $(foreach obj, $(LIB_OSRC:.c=.o), build/iplus1/$(obj))

build/iplus1: build
	mkdir -p build/iplus1

build/iplus1/%.o: src/iplus1/%.c
	$(CXX) -c $(LIB_CFLAGS) -o $@ $<

libiplus1.so: build/iplus1 $(LIB_OBJ) 
	$(CXX) -shared -Wl,-soname,$@.$(LIB_VERSION) -o $@.$(LIB_VERSION) $(LIB_OBJ) $(LIB_LDFLAGS)
	ln -sf libiplus1.so.$(LIB_VERSION) libiplus1.so.$(LIB_SHORTVER)
	ln -sf libiplus1.so.$(LIB_VERSION) libiplus1.so

# language plugins

LANG_CFLAGS = -fPIC -ggdb -Wall -Werror -Isrc/iplus1 `pkg-config --cflags icu-uc` -DPREFIX=\"$(PREFIX)\"
LANG_LDFLAGS = -fPIC -L. -liplus1 -lstemmer `pkg-config --libs icu-uc` -lmecab

LANG_SRC = $(wildcard src/iplus1/lang/*.c)
LANG_OSRC = $(patsubst src/iplus1/lang/%,%,$(LANG_SRC))
LANG_OBJ = $(foreach obj, $(LANG_OSRC:.c=.so), lang/$(obj))

lang:
	mkdir -p lang

lang/%.so: src/iplus1/lang/%.c
	$(CXX) -shared $(LANG_CFLAGS) $(LANG_LDFLAGS) -o $@ $<

languages: lang $(LANG_OBJ)

pyplus1:
	python setup.py build

install:
	mkdir -p $(PREFIX)/include/iplus1
	mkdir -p $(PREFIX)/share/iplus1
	mkdir -p $(PREFIX)/share/iplus1/lang
	mkdir -p $(PREFIX)/share/iplus1/data
	install $(wildcard src/iplus1/*.h) -m 644 -t $(PREFIX)/include/iplus1
	install libiplus1.so* -m 755 -t $(PREFIX)/lib
	install lang/* -m 755 -t $(PREFIX)/share/iplus1/lang
	install data/* -m 644 -t $(PREFIX)/share/iplus1/data
	python setup.py install

clean:
	rm -r build lang
	rm libiplus1.so*
