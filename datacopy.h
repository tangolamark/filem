#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <dirent.h>
#include <string.h>

struct wndw{
  WINDOW* wnd;
  char** name;
  int numberOfStrings;
  int indexOfAllotedString;
  char* path;
} *left, *right;

void sort(struct wndw*);
