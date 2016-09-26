/*
 *  Making libraries is just too painful under Win32/BCC 5.5
 *
 *  The TLIB syntax is just PITA, and the stupid thing insists on
 *  interpreting 'game\load-rst.obj' as 'game\load.obj' plus an
 *  option '-rst.obj'. And we're even talking about file names with
 *  forward slashes here.
 *
 *  This is used as 'mklib path\to\tlib libname.lib obj1.obj obj2.obj...',
 *  it will make an appropriate response file and call tlib.
 *
 *  Public Domain.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "mklib000.$$$"

int main(int argc, char** argv)
{
    int i;
    char cmdbuf[1000];
    char* p;
    FILE* fp;

    if (argc < 2) {
        printf("usage: mklib <path-to-tlib> <libfile> <objfile>...\n");
        return 1;
    }

    fp = fopen(FILENAME, "w");
    if (!fp) {
        perror(FILENAME);
        return 1;
    }
    for (i = 3; argv[i]; ++i) {
        for (p = argv[i]; *p; ++p)
            if (*p == '/')
                *p = '\\';
        fprintf(fp, "+\"%s\" ", argv[i]);
        /* printf("r - %s\n", argv[i]); */
    }
    fclose(fp);
    sprintf(cmdbuf, "%s %s @%s", argv[1], argv[2], FILENAME);
    printf("> %s\n", cmdbuf);
    remove(argv[2]);
    i = system(cmdbuf);
    printf("> exit value is %d\n", i);
    remove(FILENAME);

    return 0;
}
