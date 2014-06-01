CXX = gcc
PREFIX=/usr/local/


LIB_VERSION = 1.0.0
LIB_SHORTVER = 1

all: libiplus1.so languages iplus1d iplus1c

build:
	mkdir build/

# libiplus1

LIB_CFLAGS = -fPIC -ggdb -Wall -Werror
LIB_LDFLAGS = -fPIC -ldl -licu

LIB_SRC = $(wildcard src/libiplus1/*.c)
LIB_OSRC = $(patsubst src/libiplus1/%,%,$(LIB_SRC))
LIB_OBJ = $(foreach obj, $(LIB_OSRC:.c=.o), build/libiplus1/$(obj))

build/libiplus1: build
	mkdir -p build/libiplus1

build/libiplus1/%.o: src/libiplus1/%.c
	$(CXX) -c $(LIB_CFLAGS) -o $@ $<

libiplus1.so: build/libiplus1 $(LIB_OBJ) 
	$(CXX) -shared -Wl,-soname,$@.$(LIB_VERSION) -o $@.$(LIB_VERSION) $(LIB_OBJ) $(LIB_LDFLAGS)
	ln -sf libiplus1.so.$(LIB_VERSION) libiplus1.so.$(LIB_SHORTVER)
	ln -sf libiplus1.so.$(LIB_VERSION) libiplus1.so
	

# language plugins

LANG_CFLAGS = -fPIC -ggdb -Wall -Werror -Isrc/libiplus1
LANG_LDFLAGS = -fPIC -L. -liplus1 -lstemmer -licu

LANG_SRC = $(wildcard src/libiplus1/lang/*.c)
LANG_OSRC = $(patsubst src/libiplus1/lang/%,%,$(LANG_SRC))
LANG_OBJ = $(foreach obj, $(LANG_OSRC:.c=.so), lang/$(obj))

lang:
	mkdir -p lang

lang/%.so: src/libiplus1/lang/%.c
	$(CXX) -shared $(LANG_CFLAGS) $(LANG_LDFLAGS) -o $@ $<

languages: lang $(LANG_OBJ)
	

# iplus1d

D_CFLAGS = -ggdb -Wall -Werror -Isrc/libiplus1
D_LDFLAGS = -L. -liplus1

D_SRC = $(wildcard src/iplus1d/*.c)
D_OSRC = $(patsubst src/iplus1d/%,%,$(D_SRC))
D_OBJ = $(foreach obj, $(D_OSRC:.c=.o), build/iplus1d/$(obj))

build/iplus1d: build
	mkdir -p build/iplus1d

build/iplus1d/%.o: src/iplus1d/%.c
	$(CXX) -c $(D_CFLAGS) -o $@ $<

iplus1d: build/iplus1d $(D_OBJ) 
	$(CXX) -o $@ $(D_OBJ) $(D_LDFLAGS)

# iplus1c

C_CFLAGS = -ggdb -Wall -Werror -Isrc/libiplus1
C_LDFLAGS = -L. -liplus1

C_SRC = $(wildcard src/iplus1c/*.c)
C_OSRC = $(patsubst src/iplus1c/%,%,$(C_SRC))
C_OBJ = $(foreach obj, $(C_OSRC:.c=.o), build/iplus1c/$(obj))

build/iplus1c: build
	mkdir -p build/iplus1c

build/iplus1c/%.o: src/iplus1c/%.c
	$(CXX) -c $(C_CFLAGS) -o $@ $<

iplus1c: build/iplus1c $(C_OBJ) 
	$(CXX) -o $@ $(C_OBJ) $(C_LDFLAGS)

clean:
	rm -r build lang
	rm libiplus1.so* iplus1c iplus1d
