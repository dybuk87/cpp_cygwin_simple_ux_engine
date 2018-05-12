CC=g++
CPPFLAGS=-mwindows -lgdi32
SOURCES=$(wildcard src/*.cpp) $(wildcard src/threads/*.cpp) $(wildcard src/core/*.cpp) 
OBJECTS=$(patsubst src/%.cpp,bin/%.o,$(SOURCES))

all: directories app

bin/%.o: src/%.cpp
	$(CC) $(CPPFLAGS) -c -o $@ $<

app: $(OBJECTS)	
	$(CC) $(CPPFLAGS) $(OBJECTS) -o main.exe

directories: bin bin/threads bin/core

bin: 
	mkdir -p bin
	
bin/threads:
	mkdir -p  bin/threads
	
bin/core:
	mkdir -p bin/core

clean:
	rm bin/*.o
	rm main.exe