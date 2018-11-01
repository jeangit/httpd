/* client httpd.

   $$DATE$$ : jeu. 01 novembre 2018 (19:40:58)

   jseb@finiderire.com

   gcc -o client_httpd.{elf,c} $(sdl2-config --cflags --libs) -Wall -W -lSDL2_net

*/

/* tiré des sources SDL2_net-2.0.1 (SDLnetTCP.c)
struct _TCPsocket {
    int ready;
    SOCKET channel;
    IPaddress remoteAddress;
    IPaddress localAddress;
    int sflag;
};
*/

/* tester avec :
  while true; do nc -l -p 1234 -q 1 < ~/ui.ini; done
*/


#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL_net.h>

#define MAXLEN 16384
char buffer[MAXLEN];



int resolve( char *machine, int port, IPaddress *ip)
{
  int is_err = 0;

  is_err = SDLNet_ResolveHost( ip, machine, port);
  if (is_err) {
    fprintf(stderr,"SDLNet_ResolveHost: %s (%d)\n", SDLNet_GetError(), ip->host);
  }

  return is_err;
}


int init_net( char *machine, int port, IPaddress *ip)
{
  int is_err = 0;
  if (SDL_Init(0) == -1) {
    fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
    is_err = 1;
  }

  if (!is_err && SDLNet_Init() == -1) {
    fprintf(stderr,"SDLNet_Init: %s\n", SDLNet_GetError());
    is_err = 1;
  }

  if (!is_err) {
    is_err = resolve( machine, port, ip);
  }
  return is_err;
}


int init_tcp( IPaddress *ip, TCPsocket *tcpsock)
{
  int is_err = 0;
  *tcpsock = SDLNet_TCP_Open(ip);
  if(!*tcpsock) {
    fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
    is_err = 1;
  }

  return is_err;
}


int receive_tcp( TCPsocket *tcpsock)
{
  int bytes_read = 0;

  bytes_read = SDLNet_TCP_Recv(*tcpsock,buffer,MAXLEN);
  return bytes_read;
}


int main (int argc, char **argv)
{
  int is_err = 0;
  IPaddress ip;
  TCPsocket tcpsock; //attention, TCPsocket est un pointeur vers _TCPsocket

  if (argc >= 2) {

    char *machine = argv[1];
    int port = atoi(argv[2]);

    is_err = init_net( machine, port, &ip);
    if (!is_err) {
      is_err = init_tcp( &ip, &tcpsock);
    }

    int bytes_read = 0;
    do {
      bytes_read = receive_tcp( &tcpsock);
      if ( bytes_read > 0 ) {
        printf("%s\n",buffer);
      }

    } while(bytes_read >= 0);


    if (!is_err) {
      if (tcpsock) {
        fprintf(stderr,"closing tcpsock\n");
        SDLNet_TCP_Close(tcpsock);
      }
      SDLNet_Quit();
    }

  } else {

    fprintf(stderr,"%s [machine] [port]\n",argv[0]);
  }

  return is_err;
}

