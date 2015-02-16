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
#include <errno.h>
#include <dirent.h>

int main(int argc, char *argv[]) {
  int r;

  int fdin;
  const char *DIRNAME = "/tmp/test";

  struct dirent *entry;
  fdin = open(DIRNAME, O_RDONLY);
  printf("fdin=%d\n", fdin);
  DIR *dir = fdopendir(fdin);
  while (1) {
    entry = readdir(dir);
    if (entry == NULL) {
      int myerrno = errno;
      if (myerrno != 0) {
          printf("error\n");
      } else {
        break;
      }
    }
    printf("inode=%12ld reclen=%6ld type=%4ld name=%s\n", (long) entry->d_ino, (long) entry->d_reclen, (long) entry->d_type, entry->d_name);
  }
  r = closedir(dir);
  printf("r=%d\n", r);
  r = close(fdin);
  printf("r=%d\n", r);
  exit(0);
}
