#if 0
gcc=/mingw33/bin/gcc.exe
src=resize.c
out=resize.e
$gcc -o $out $src
exit 0
#else
#include <stdio.h>
#define FF   fflush(stdout);
//#define ESC  "\e"
#define CSI  "\e["

void print2i(int row, int col, char *format, int p1, int p2) {
  printf(CSI "s");
  printf(CSI "%d;%dH", row, col);
  printf(format, p1, p2);
  printf(CSI "u"); FF
}
void print0i(int row, int col, char *format) {
  printf(CSI "s");
  printf(CSI "%d;%dH", row, col);
  printf(format);
  printf(CSI "u"); FF
}

int main(int argc, char **argv) {
  int lines = 0;
  int cols = 0;
  FILE *fsize = popen("stty size", "r");
  if (!fsize) { printf("no fsize\n"); return 1; }
  char cs = 'l';
  int ch = '\0';
  while (ch == ch) { // != 'R') {
    ch = fgetc(fsize); //getchar();
    if (ch == EOF) { printf("EOF\n"); break; }
    else if (ch == 10) { break; } // stop on \n
    else if (ch >= '0' && ch <= '9') {
      if (cs == 'l') { lines = lines * 10 + ch - '0'; }
      if (cs == 'c') { cols  =  cols * 10 + ch - '0'; }
    } else if (ch == ' ') {
      if (cs == 'l') { cs = 'c'; }
    } else {
      printf("ch: %d\n", ch);
    }
  }
  FF // im case of logged evemts
  pclose(fsize);
  // ok now read keys ....
  system("stty raw");
  char escch = '\0';
  char actch = '\0';
  while (1) {
    ch = getchar();
    if    (ch == EOF) { break; } // should break on ctrl+z
    else if (ch == 3) { break; } // should break on ctrl+c
    else if (ch == '\x1b') {
      //printf("1B\n"); FF
      escch = ch;
    } else if (escch == 27) {
      if (ch == '[') { escch = '['; }
      else { escch = '\0'; }
    } else if (escch == '[') {
      if (ch == 'A') { actch = 'y'; escch = '\0'; } // UP
      if (ch == 'B') { actch = 'Y'; escch = '\0'; } // DOWN
      if (ch == 'C') { actch = 'X'; escch = '\0'; } // RIGHT
      if (ch == 'D') { actch = 'x'; escch = '\0'; } // LEFT
    } else {
      printf("KEYPRESS %2x\n", ch); FF 
    }
    //if (actch == 'y') { lines--; actch = 'S'; } // printf("\e[B"); FF actch = 'S'; }
    if (actch == 'y') { lines--; printf("\e[B"); FF actch = 'S'; }
    if (actch == 'Y') { lines++; printf("\e[A"); FF actch = 'S'; }
    if (actch == 'x') { cols--;  printf("\e[C"); FF actch = 'S'; }
    if (actch == 'X') { cols++;  printf("\e[D"); FF actch = 'S'; }
    if (actch == 'S') {
      actch = '\0';
      print0i(1, cols - 10, "          ");
      printf("\e[8;%d;%dt", lines, cols); FF
      print2i(1, cols - 10, "%3d x %3d", lines, cols);
    }
  }
  FF
  system("stty -raw");
  return 0;
}

#endif

