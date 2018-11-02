# $$DATE$$ : ven. 02 novembre 2018 (19:14:33)

sdl2_cflags=$(shell sdl2-config --cflags)
sdl2_lflags=$(shell sdl2-config --libs)

all: httpd_play.elf

httpd_play.elf: client_httpd.o mixer.o
	gcc -o $@ $^ $(sdl2_lflags) -lSDL2_net -lSDL2_mixer

%.o: %.c
	gcc -Wall -W $(sdl2_cflags) -c -o $@ $<

clean:
	@find -regex ".*\(\.o\|\.elf\)$$" -delete 
	@echo Killing is my business… and business is good !
