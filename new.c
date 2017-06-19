#include <sys/types.h>
#include "datacopy.h"

void sig_winch(int signo){
  struct winsize size;
  ioctl(fileno(stdout), TIOCGWINSZ, (char*)&size);
  resizeterm(size.ws_row, size.ws_col);
}

void init(){
  initscr();
  start_color();
  init_pair(1,COLOR_WHITE,COLOR_BLUE);
  init_pair(2,COLOR_WHITE,COLOR_BLACK);
  signal(SIGWINCH, sig_winch);
  cbreak();
  curs_set(0);
  refresh();
  noecho();
  left = malloc(sizeof(*left));
  right = malloc(sizeof(*right));
  left->path = malloc(255 * sizeof(char));
  right->path = malloc(255 * sizeof(char));
  left->wnd = newwin(50,91,2,5);
  box(left->wnd,'|','-');
  right->wnd = newwin(50,91,2,105);
  box(right->wnd,'|','-');
  left->numberOfStrings = 0, left->indexOfAllotedString = 1;
  right->numberOfStrings = 0, right->indexOfAllotedString = 1;
  keypad(left->wnd,true);
  keypad(right->wnd,true);
  wrefresh(left->wnd);
  wrefresh(right->wnd);
  getch();
}

void changeOpenFile(WINDOW* tmp, char* buff){
  curs_set(1);
  int i = 2, j = 2;
  wprintw(tmp,"%s",buff);
  while(1){
//    wclear(tmp);
    int t = getch();
    if(t == 10){
      break;
    }else if(t == 119 && i > 1){
      --i;
    }else if(t == 115 && i < 47){
      ++i;
    }else if(t == 97 && j > 1){
      --j;
    }else if(t == 100 && j < 86){
      ++j;
    }else{
      wprintw(tmp,"%c",(char)t);
    }
//    wmove(tmp,0,0);
//    wprintw(tmp,"%s",buff);
    wmove(tmp,i,j);
/*    wattron(tmp,COLOR_PAIR(1));
    wprintw(tmp,"  ");
    wattroff(tmp,COLOR_PAIR(1));
    wattron(tmp,COLOR_PAIR(2));
    t = getch();
    wprintw(tmp,"%c",(char)t);
    wattroff(tmp,COLOR_PAIR(1));*/
//    ++j;
    wrefresh(tmp);
  }
  curs_set(0);
}

void openFile(struct wndw* curr){
  WINDOW* tmp = derwin(curr->wnd, 48,88,1,2);
  wrefresh(tmp);
  FILE* file = fopen(curr->path,"r");
  if(!file){
    wprintw(tmp,"ne udalos' otkrit' file\n");
    wrefresh(tmp);
    return;
  }
  fseek(file,0,SEEK_END);
  long size = ftell(file);
  rewind(file);
  char* buff = malloc (sizeof(char) * size);
  if(!buff){
    wprintw(tmp, "ne udalos' videlit' pamyat'\n");
    wrefresh(tmp);
    return;
  }
  size_t sizet = fread(buff, 1, size,file);
  if(sizet != size){
    printf("oshibka chteniya\n");
    return;
  }
  changeOpenFile(tmp,buff);
/*
  int i,j,c=2,r=2,k = 0,f = 0;
  for(i = 0; i < 48;++i){
    for(j = 0; j < 88; ++j,++k){
      wmove(tmp,i,j);
      wattron(tmp,COLOR_PAIR(1));
      wprintw(tmp,"%c",buff[k]);
      wattroff(tmp,COLOR_PAIR(1));
      if(buff[k] == '\n'){
        ++k;
	break;
      }
      if(k == size){
        f = 1;
	break;
      }
    }
    if(f){
      break;
    }
  }

*/
  wrefresh(tmp);
  fclose(file);
  free(buff);
  delwin(tmp);
  return;
}

void changeValue(struct wndw *curr){
  int i;
  WINDOW* tmp = derwin(curr->wnd,48,88,1,2);
  wclear(tmp);
//  wrefresh(tmp);
  for(i=1;i< curr->numberOfStrings; ++i){
    if(i == curr->indexOfAllotedString){
      wattron(tmp,COLOR_PAIR(1));
      wprintw(tmp,"%s\n",(curr->name)[i]);
      wattroff(tmp,COLOR_PAIR(1));
    }else{
      wprintw(tmp,"%s\n",(curr->name)[i]);
    }
  }
  wrefresh(tmp);
  delwin(tmp);
}

int getDataOfDirectory(struct wndw *curr, char* path){
  struct dirent *current;
  DIR* dir = opendir(path);
  if(!dir){
    return 0;
  }
  free(curr->name);
  curr->numberOfStrings = 0;
  curr->name = (char**)malloc(sizeof(char*));
  while(current = readdir(dir)){
    curr->name = realloc(curr->name,(curr->numberOfStrings + 1) * sizeof(char*));
    (curr->name)[curr->numberOfStrings] = (char*)malloc(255 * sizeof(char));
    strcpy(curr->name[curr->numberOfStrings],current->d_name);
    curr->numberOfStrings += 1;
  }
  sort(curr);
  return 1;
}

void sort(struct wndw* curr){
  int i,j;
  for(i = 0; i < curr->numberOfStrings - 1; ++i){
    for(j = 0; j < curr->numberOfStrings - i - 1; ++j){
      if(strcmp(curr->name[j],curr->name[j+1]) > -1){
	char* tmp = curr->name[j];
	curr->name[j] = curr->name[j+1];
	curr->name[j+1] = tmp;
      }
    }
  }
}

void menu(){
  struct wndw* curr = left;
  while(1){
    int code = getch();
    switch(code){
      case 10:
	wclear(curr->wnd);
	char* path;
	path = malloc(255*sizeof(char));
	strcpy(path,curr->path);
        int t = strlen(curr->path);
        curr->path[t++] = '/';
	int i;
	for(i = 0; i < strlen(curr->name[curr->indexOfAllotedString]); ++i){
	  curr->path[t+i] = curr->name[curr->indexOfAllotedString][i];
	}
	curr->path[t + strlen(curr->name[curr->indexOfAllotedString])] = '\0';
	curr->indexOfAllotedString = 1;
	if(!getDataOfDirectory(curr,curr->path)){
	  openFile(curr);
	  curr->path = path;
//	  getDataOfDirectory(curr, curr->path);
	}else{
	  changeValue(curr);
	}
        break;
      case 119:
	if(curr->indexOfAllotedString)
	  curr->indexOfAllotedString -= 1;
	changeValue(curr);
	break;
      case 115:
	if(curr->indexOfAllotedString < curr->numberOfStrings - 1)
	  curr->indexOfAllotedString += 1;
	changeValue(curr);
	break;
      case 97:
	curr = left;
	break;
      case 100:
	curr = right;
	break;
      default:
	printf("asd");
        return;
    }
  }
}

int main(){
  init();
//  char* st = getenv("PWD");
  left->path = getenv("PWD");
  wprintw(left->wnd,"%s",left->path);
 /* getDataOfDirectory(left,left->path);
  changeValue(left);*/

  getDataOfDirectory(left,left->path);
  changeValue(left);
  right->path = getenv("HOME");
  getDataOfDirectory(right,right->path);
  changeValue(right);
  menu();

  endwin();
  return 0;
}
