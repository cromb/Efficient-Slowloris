
#include "slowloris.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

char cache;
char httpready;
char https;
char test;
char version;
char nopayload;
char help;
char dns[1024];
char filename[1024];
int num;
int port;
int tcpto;
int timeout;

T_ARGS_ARRAY global_arg_list[] =
{
    { "cache"    ,SWITCH    , &cache        , "Attempt Cache Avoidance, experimental"               },
    { "httpready",SWITCH    , &httpready    , "Attempt to bypass httpready protection"              },
//    { "https"    ,SWITCH    , &https        ,                                                       },
//    { "test"     ,SWITCH    , &test         ,                                                       },
    { "version"  ,SWITCH    , &version      , "Prints the version"                                  },
    { "dns"      ,STRING    , &dns          , "Specify a target.  -dns www.target.com"              },
    { "num"      ,INT       , &num          , "Specify the number of connections"                   },
    { "port"     ,INT       , &port         , "Specify the port to connect too"                     },
//    { "tcpto"    ,INT       , &tcpto        ,                                                       },
    { "timeout"  ,INT       , &timeout      , "Time delay for threads to sleep after connections"   },
    { "file"     ,STRING    , &filename     , "Specify configureation from a file"                  },
    { "nopayload",SWITCH    , &nopayload    , "Connect, but don't send payload data"                },
    { "help"     ,SWITCH    , &help         , "Print this help data"                                },
    { ""         ,NONE      , NULL          , ""                                                    },
};

int parse_args( int argc, const char **argv, T_ARGS_ARRAY *arg_list, int reset_args )
{
    int i, j;
    char *str_ptr, *str_ptr2;
    char *a_char;
    int *an_int;

    if( reset_args )
    {
        // Loop through the list and reset the args data
        for( i = 0; arg_list[i].ptr != NULL; i++ )
        {
            switch( arg_list[i].type )
            {
                case SWITCH:
                case STRING:
                    a_char = (char *)arg_list[i].ptr;
                    *a_char = 0;
                    break;
                case INT:
                    an_int = (int *) arg_list[i].ptr;
                    *an_int = 0;
                    break;
                default:
                    break;
            }
        }
    }

    for( j = 1; j < argc; j++ )
    {
       str_ptr = (char *) argv[j];
       if( *str_ptr != '-' )
       {
           printf( "'%s' not recognized, need argument flag\n", str_ptr );
           print_args();
           return 0;
       }
       str_ptr++;
       for( i = 0; arg_list[i].ptr != NULL; i++ )
       {
            if(strncasecmp( str_ptr, (char *) arg_list[i].argstr, strlen( (char *) arg_list[i].argstr )) == 0)
            {
                switch( arg_list[i].type )
                {
                    case SWITCH:
                        a_char = (char *)arg_list[i].ptr;
                        *a_char = '\1';
                        printf("%s set to true\n", str_ptr);
                        break;
                    case STRING:
                        j++;
                        if( j >= argc || argv[j][0] == '\0')
                        {
                            printf( "argument -%s needs a string value\n", str_ptr );
                            return 0;
                        }
                        a_char = (char *)arg_list[i].ptr;
                        str_ptr2 = (char *)argv[j];
                        strncpy( a_char, str_ptr2, 1024 );
                        printf("%s set to %s\n", str_ptr, a_char );
                        break;
                    case INT:
                        j++;
                        if( j >= argc || !check_num((const char *)argv[j]))
                        {
                            printf( "argument -%s needs an interger value\n", str_ptr );
                            return 0;
                        }
                        an_int = (int *) arg_list[i].ptr;
                        *an_int = atoi( argv[j]);
                        break;
                    default:
                        break;
                }
                break;
            }
       }
       if( arg_list[i].ptr == NULL )
       {
           printf("Command %s wasn't found...\n", str_ptr);
           print_args();
       }
    }
    return 1;
}
void print_args( void )
{
    int i;
    for(i = 0; global_arg_list[i].ptr != NULL; i++)
    {
        printf( "\t%s - %s\n",
                global_arg_list[i].argstr,
                global_arg_list[i].help);
    }
    return;
}

int check_num( const char *string )
{
    int i;
    int test = 1;

    // Non-existant strings can't be numbers.
    if( string == NULL || *string == '\0' )
    {
        return 0;
    }
    
    for( i = 0; string[i] != '\0'; i++ )
    {
        if(!isdigit(string[i]))
        {
            test = 0;
            break;
        }
    }

    return test;
}

