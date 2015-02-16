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

    const char *FILENAME = "/tmp/power_of.txt";
    char *buffer;

    fdout = open(FILENAME, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
    printf("fdout=%d\n", fdout);

    for (i=0; i<100; i++) {
        char *text = (void *) power_of(i);
        n = strlen(text) + 1;
        printf("length=%d\n", n);
        m = write(fdout, text, n);
    }

    close(fdout);

    printf("%d bytes written (length %d)\n", m, n);
    buffer = (char *) malloc(m);
    fdin = open(FILENAME, O_RDONLY);
    printf("fdin=%d\n", fdin);
    k = read(fdin, (void *) buffer, m);
    close(fdin);
    printf("%d byte read:\n%s\n", k, buffer);
    exit(0);
}
