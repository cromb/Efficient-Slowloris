#ifndef __SLOWLORIS_H__
#define __SLOWLORIS_H__

#ifdef WIN32
#include<windows.h>
#endif

typedef struct
{
    const char *argstr;
    const int  type;
    void       *ptr;
    const char *help;
} T_ARGS_ARRAY;


int parse_args( int argc, const char **argv, T_ARGS_ARRAY *arg_list, int reset_args );
int parse_file( const char *filename );
void print_args( void );
void doconnections( int num);
void domultithreading( int num );
#ifndef WIN32
void *dothread( void *args );
#else
DWORD WINAPI dothread( void *args );
#endif
int check_num( const char *string );

#define NONE 	0
#define SWITCH 	1
#define STRING  2
#define INT     3

extern char cache;
extern char httpready;
extern char https;
extern char test;
extern char version;
extern char dns[1024];
extern char filename[1024];
extern int num;
extern int port;
extern int tcpto;
extern int timeout;
extern char help;
extern char nopayload;

extern T_ARGS_ARRAY global_arg_list[]; 
#endif
