/* (C) IT Sky Consulting GmbH 2014
* http://www.it-sky-consulting.com/
* Author: Karl Brodowsky
* Date: 2014-02-27
* License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

const char * power_of(int i) {
    static char str[6];
    sprintf(str, "%4d\n", i*i);
    return str;
}

int main(int argc, char *argv[]) {
    int k,m,n;
    int fdout;
    int fdin;
    int i=0;

    const char *FILENAME = "/tmp/power_of_h1.txt";
    char *buffer;

    fdout = open(FILENAME, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);

    for (i=99; i>0; i--) {
        char *text = (void *) power_of(i);
        n = strlen(text);
        m = write(fdout, text, n);
    }

    close(fdout);

    buffer = (char *) malloc(m);
    fdin = open(FILENAME, O_RDONLY);
    k = read(fdin, (void *) buffer, m);
    close(fdin);
    exit(0);
}
