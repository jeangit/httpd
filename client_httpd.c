/* client httpd.

   $$DATE$$ : ven. 02 novembre 2018 (16:53:22)

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

#define MAXLEN 8192
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


int get_stream_beginning( TCPsocket *tcpsock)
{
  int bytes_read = 0;

  /* envoyer un « GET / » pour initier la récup de données */
  char * const get = "GET /\n";
  fprintf(stderr,"receive_tcp\n");
  int bytes_written = SDLNet_TCP_Send( *tcpsock, get, strlen(get));
  fprintf(stderr,"bytes_written: %d\n",bytes_written);

  if (bytes_written) {
    // on reçoit le header http, et le début du flux
    bytes_read = SDLNet_TCP_Recv( *tcpsock,buffer,MAXLEN);

    for ( int i=0 ; i<bytes_read; i++) {
      // interrompre la recherche dès que la signature est trouvée
      if (!strncmp("OggS",&buffer[i],4) ) {
        bytes_read = bytes_read - i; // ajuster la frame lue
        memcpy(buffer, &buffer[i], bytes_read);
        break;
      }
    }

  } // bytes_written

  return bytes_read;
}


int get_stream( TCPsocket *tcpsock)
{

  // reçoit le flux
  int  bytes_read = SDLNet_TCP_Recv( *tcpsock,buffer,MAXLEN);
  fprintf(stderr,"bytes_read: %d\n",bytes_read);

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

    int bytes_read = get_stream_beginning( &tcpsock);
    fwrite( buffer, 1, bytes_read, stdout);

    do {
      bytes_read = get_stream( &tcpsock);
      if ( bytes_read > 0 ) {
        fwrite( buffer, 1, bytes_read, stdout);
      }
    } while(bytes_read > 0);


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

