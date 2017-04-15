#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define MAKE_MENU_ITEM(A,B) attron(COLOR_PAIR(1)); printw("%2s", A); attroff(COLOR_PAIR(1)); addch(' '); printw("%-10s", B);
#define HISTORY_CAPACITY 10
#define PATH_CAPACITY 10

/**************************************************
* HEADER
**************************************************/
char* history[HISTORY_CAPACITY];
int history_length = 0;
void history_push(char* path);

int width, height;
int ch = -1;

char* path;
int path_length = 0;
int path_capacity = PATH_CAPACITY;
void path_append(char* string);
void path_append_char(char c);
void path_delete();

typedef struct ContentNode {
	
	char* path;
	struct ContentNode* next;
	
} ContentNode;
ContentNode* ContentNode_create(char* path);
void ContentNode_destroy(ContentNode* node);

typedef struct DirCacheNode {
	
	char* dir;
	ContentNode* first;
	ContentNode* last;
	time_t last_modified; // st_mtime
	struct DirCacheNode* next;
	
} DirCacheNode;
DirCacheNode* dir_cache_first = NULL;
DirCacheNode* dir_cache_last = NULL;
DirCacheNode* dir_cache_get(char* dir);
void dir_cache_push_content(ContentNode* node);
DirCacheNode* dir_cache_queue(char* dir, time_t last_modified);
void dir_cache_dequeue();

void interact();

void update();
void list_dir();

void free_all();
void terminate();

/**************************************************
* MAIN FUNCTION
**************************************************/
int main() {
	initscr();
	clear();
	scrollok(stdscr, 1);
	keypad(stdscr, 1);
	noecho();
	cbreak();
	
	start_color();
	init_color(COLOR_WHITE, 1000, 1000, 1000);
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	use_default_colors();
	
	path = (char*)malloc(path_capacity);
	path[0] = '/';
	path[1] = 0;
	path_length = 1;
	
	do {
		interact();
		update();
	} while (ch = getch());
	
	endwin();
	return 0;
}

/**************************************************
* HISTORY FUNCTIONS
**************************************************/
void history_push(char* path) {
	if(history_length == HISTORY_CAPACITY) {
		history_length = 0;	
		history[0] = path;
	} else {
		history[history_length++] = path;
	}
}

/**************************************************
* PATH FUNCTIONS
**************************************************/
void path_append(char* string) {
	int len = strlen(string);
	if(path_length + len >= path_capacity) {
		path_capacity += len;
		path = (char*)realloc(path, path_capacity);
	}
	strcat(path, string);
	path_length += len;
	path[path_length] = '\0';
}

void path_append_char(char c) {
	if(path_length + 1 > path_capacity) {
		path_capacity *= 2;
		path = (char*)realloc(path, path_capacity);
	}
	path[path_length++] = c;
	path[path_length] = 0;
}

void path_delete() {
	if(path_length < 1) return;
	path[--path_length] = 0;	
}

/**************************************************
* CONTENT NODE
**************************************************/
ContentNode* ContentNode_create(char* path) {
	ContentNode* node = (ContentNode*)malloc(sizeof(ContentNode));
	node->path = strdup(path);
	node->next = NULL;
	return node;
}
void ContentNode_destroy(ContentNode* node) {
	free(node);
}

/**************************************************
* DIR CACHE
**************************************************/
DirCacheNode* dir_cache_get(char* dir) {
	DirCacheNode* node = dir_cache_first;
	int i = 0;
	while(node != NULL) {
		//mvprintw(0,50,"%s %s %i",node->dir, dir, i++);
		if(strcmp(node->dir, dir) == 0) {
			return node;
		} else {
			node = node->next;
		}
	}
	return NULL;
}
DirCacheNode* DirCacheNode_push_content(DirCacheNode* node, ContentNode* contentNode) {
	if(node->last == NULL) {
		node->first = node->last = contentNode;
	} else {
		node->last = node->last->next = contentNode;
	}
}
DirCacheNode* dir_cache_queue(char* dir, time_t last_modified) {
	DirCacheNode* node = (DirCacheNode*)malloc(sizeof(DirCacheNode));
	node->dir = strdup(dir);
	node->first = node->last = NULL;
	node->last_modified = last_modified;
	if(dir_cache_last == NULL) {
		dir_cache_first = dir_cache_last = node;
	} else {
		dir_cache_last = dir_cache_last->next = node;
	}
	return node;
}
void dir_cache_dequeue() {
	DirCacheNode* node = dir_cache_first->next;
	ContentNode* contentNode = dir_cache_first->first;
	ContentNode* nextContentNode;
	while(contentNode != NULL) {
		nextContentNode = contentNode->next;
		free(contentNode);
		contentNode = nextContentNode;
	}
	free(dir_cache_first->dir);
	free(dir_cache_first);
	dir_cache_first = node;
}

/**************************************************
* KEYBOARD INTERACTION
**************************************************/
void interact() {
	if(ch == -1) {
		return;
	}
	
	const char* string = keyname(ch);
	if(strcmp(string, "KEY_BACKSPACE") == 0) {
		path_delete();
	} else if(strlen(string) == 2 && string[0] == '^') {
		if(string[1] == 'N') {
			
		} else if(string[1] == 'D') {
			
		} else if(string[1] == 'W') {
			
		} else if(string[1] == 'C') {
			
		} else if(string[1] == 'V') {
			
		} else if(string[1] == 'R') {
			
		} else if(string[1] == 'X') {
			terminate();
		} else if(string[1] == 'O') {
			
		} else if(string[1] == 'L') {
			
		}
	} else if (strlen(string) == 1) {
		if (string[0] == ',') {
		
		} else if (string[0] == '.') {
		
		} else if (string[0] == '?') {
		
		} else {
			path_append_char(string[0]);
			ch = -1;
		}
	}
}

/**************************************************
* DRAW FUNCTIONS
**************************************************/
void update() {
	attron(COLOR_PAIR(1));
	for(int i = 0; i < COLS; i++) {
		mvaddch(0, i, ' ');
	}
	mvprintw(0, 2, "%s", path);
	attroff(COLOR_PAIR(1));
	
	move(LINES - 2, 0);
	MAKE_MENU_ITEM("^N", "New File");
	MAKE_MENU_ITEM("^D", "New Dir");
	MAKE_MENU_ITEM("^W", "Where Is");
	MAKE_MENU_ITEM("^C", "Copy");
	MAKE_MENU_ITEM("^V", "Paste");
	MAKE_MENU_ITEM("^R", "Delete");
	addch('\n');
	MAKE_MENU_ITEM("^X", "Exit");
	MAKE_MENU_ITEM("^O", "Open File");
	MAKE_MENU_ITEM("^L", "Open Dir");
	MAKE_MENU_ITEM(",", "Back");
	MAKE_MENU_ITEM(".", "Forward");
	MAKE_MENU_ITEM("?", "Up");
	
	list_dir();
	
	move(0, 0);
	refresh();
}

void list_dir() {
	DIR* dir = opendir(path);
	if(dir == NULL) return;
	
	struct stat dir_stat;
	stat(path, &dir_stat);
	
	DirCacheNode* node = dir_cache_get(path);
	if(node == NULL) {
		node = dir_cache_queue(path, dir_stat.st_mtime);
		struct dirent* dp;
		while((dp = readdir(dir)) != NULL) {
			//mvprintw(0,16,"%.20s", dp->d_name);
			DirCacheNode_push_content(node, ContentNode_create(dp->d_name));
		}
	} else if (dir_stat.st_mtime > node->last_modified) {
		// TODO UPDATE CONTENT NODES
		node->last_modified = dir_stat.st_mtime;
	} else {
	}
	
	int columns = 0;
	int lines = 2;
	int max_lines = LINES - 3;
	
	for(int y = 2; y < max_lines; y++) {
		for(int x = 0; x < COLS; x++) {
			mvaddch(y,x,' ');	
		}
	}
	
	ContentNode* contentNode = node->first;
	while(contentNode != NULL) {
		if((lines % max_lines) == 0) {
			lines = 2;
			columns += 20;
		}
		mvprintw(lines, columns, "%.18s ", contentNode->path);
		contentNode = contentNode->next;
		lines++;
	}
}

void free_all() {
	free(path);
	for(int i = 0; i < history_length; i++) {
		free(history[i]);
	}
}
void terminate() {
	free_all();
	endwin();
	exit(0);
}