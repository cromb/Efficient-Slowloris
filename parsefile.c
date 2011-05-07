
#include "slowloris.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>


int parse_file( const char *filename )
{
    FILE *fp;
    char one_line[1024];
    char buffer[20][1024];
    char *one_word[21]; // similar to const char **
    int len;
    int i, j, k;

    if((fp = fopen( filename, "r" ))==NULL)
    {
        printf("File %s was specified but not found.\n", filename);
        return 0;
    }

    len = fread(one_line, 1, 1024, fp);

    fclose( fp );

    for( i = 1, j = 0; i < 21 && j < len; i++)
    {
        one_word[i] = buffer[i];
        // kill whitespace.
        while( isspace( one_line[j] ) && j < len) j++;
        for( k = 0; !isspace( one_line[j]) && j < len;j++, k++)
        {
            one_word[i][k] = one_line[j];
        }
        //add the NULL at the end;
        one_word[i][++k] = '\0';
    }

    // Parse the file just as you would the command line
    return parse_args( i - 1, (const char **) one_word, global_arg_list, 0 );

}
