#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <unordered_map>
#include <ncurses.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define MAKE_MENU_ITEM(A,B) attron(COLOR_PAIR(1)); printw("%2s", A); attroff(COLOR_PAIR(1)); addch(' '); printw("%-10s", B);

int div_up(int n, int d) {
	if (n < 0) {
		return -(-n / d);
  	} else {
    	return (n + d - 1) / d;
 	}
}

/**************************************************
* HISTORY
**************************************************/
class History {
private:
	std::vector<std::string> list;
public:
	void push(std::string path);
	std::string pop();
};
void History::push(std::string path) {
	list.push_back(path);
}
std::string History::pop() {
	std::string s = list.back();
	list.pop_back();
	return s;
}

/**************************************************
* PATH
**************************************************/
class PathList {
private:
	std::vector<std::string> list;
public:
	void navigate(std::string path);
	void navigate_once(std::string path);
	void push(std::string path);
	std::string to_string();
};
void PathList::navigate(std::string path) {
	std::istringstream iss(path);
	std::string token;
	unsigned int i = 0, size = list.size();
	bool broke = false;
	while(std::getline(iss, token, '/')) {
		if(i < size && token == list.at(i)) {
			i++;
			continue;
		} else if(!broke) {
			list.erase(list.begin(), list.begin() + i);
			broke = true;
		}
		navigate_once(token);
	}
}
void PathList::navigate_once(std::string path) {
	if(!path.size() || path == ".") {
		return;
	} else if(path == "..") {
		list.pop_back();
	} else {
		list.push_back(path);
	}
}
void PathList::push(std::string path) {
	list.push_back(path);	
}
std::string PathList::to_string() {
	std::stringstream ss;
	ss << "/";
	std::copy(list.begin() + 1, list.end(), std::ostream_iterator<std::string>(ss, "/"));
	return ss.str();
}

/**************************************************
* DIR CACHE NODE
**************************************************/
class DirCacheNode {
private:
	std::vector<std::string> list;
public:
	std::string dir;
	unsigned int map_height;
	std::vector<std::string> get_list();
	
	time_t last_modified; // st_mtime
	DirCacheNode(char* d, time_t t) : last_modified(t), map_height(0) {
		dir = std::string(d);
	};
	DirCacheNode(std::string d, time_t t) : dir(d), last_modified(t), map_height(0) {};
	
	void push(std::string key);
	std::string at(unsigned int i);
	
	std::vector<std::vector<std::string>> map;
	std::string get_map(unsigned int y, unsigned int x);
	void build_map(unsigned int height);
	
	unsigned int size();
};

std::vector<std::string> DirCacheNode::get_list() {
	return list;
}
void DirCacheNode::push(std::string key) {
	list.push_back(key);
}
std::string DirCacheNode::at(unsigned int i) {
	return list.at(i);	
}

std::string DirCacheNode::get_map(unsigned int y, unsigned int x) {
	return map.at(x).at(y);
}
void DirCacheNode::build_map(unsigned int height) {
	if(map_height == height) return;
	map_height = height;
	unsigned int cols = div_up(list.size(), height);
	map.clear();
	for(int y = 0; y < cols; y++) {
		int from = (y * height) > list.size() ? 0 : (y * height);
		int to = (from + height) > list.size() ? list.size() : (from + height);
		map.push_back(std::vector<std::string>(list.begin() + from, list.begin() + to));
	}
}

unsigned int DirCacheNode::size() {
	return list.size();
}

/**************************************************
* PATH STRING
**************************************************/
std::string Path = "/";
PathList pathList;

/**************************************************
* DIR CACHE
**************************************************/
std::unordered_map<std::string, DirCacheNode> dirCache;

/**************************************************
* HEAD
**************************************************/
int ch;
int cursor_x = 0, cursor_y = 0;
bool cursor_changed = true;
std::string selected;
void interact();
void update();
void list_dir();

/**************************************************
* MAIN
**************************************************/
int main(int argc, char** argv) {
	initscr();
	clear();
	scrollok(stdscr, 1);
	keypad(stdscr, 1);
	nodelay(stdscr, 1);
	noecho();
	cbreak();
	
	start_color();
	init_color(COLOR_WHITE, 1000, 1000, 1000);
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	use_default_colors();
	
	pathList.push("/");
	
	try {
		while(true) {
			ch = getch();
			napms(10);
			interact();
			update();
		}
	} catch(int x) {
		endwin();
		std::cout<<x<<std::endl;
		return 0;
	}
	return 0;
}

/**************************************************
* KEYBOARD INTERACTION
**************************************************/
void interact() {
	if(ch == -1) {
		return;
	}
	
	std::string string = keyname(ch);
	if(string == "KEY_BACKSPACE") {
		Path.pop_back();
	} else if(string == "KEY_UP") {
		cursor_y--;
		if(cursor_y < 2) cursor_y = 0;
		cursor_changed = true;
	} else if(string == "KEY_DOWN") {
		cursor_y++;
		if(cursor_y >= LINES - 5) {
			cursor_y = 0;
			cursor_x++;
		}
		cursor_changed = true;
	} else if(string == "KEY_LEFT") {
		cursor_x--;
		if(cursor_x < 0) cursor_x = 0;
		cursor_changed = true;
	} else if(string == "KEY_RIGHT") {
		cursor_x++;
		cursor_changed = true;
	} else if(string.size() == 2 && string.at(0) == '^') {
		if(string.at(1) == 'N') {
			
		} else if(string.at(1) == 'D') {
			
		} else if(string.at(1) == 'W') {
			
		} else if(string.at(1) == 'C') {
			
		} else if(string.at(1) == 'V') {
			
		} else if(string.at(1) == 'R') {
			
		} else if(string.at(1) == 'X') {
			throw 0;
		} else if(string.at(1) == 'O') {
			
		} else if(string.at(1) == 'L') {
			
		} else if(string.at(1) == 'J') {// && selected != NULL) { // enter key
			pathList.navigate_once(selected);
			Path.assign(pathList.to_string());
			cursor_x = 0;
			cursor_y = 0;
			cursor_changed = true;
		}
	} else if (string == ",") {
		
	} else if (string == ".") {
		
	} else if (string == "?") {
		
	} else if (string.size() == 1) {
		Path += string;
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
	mvprintw(0, 2, "%s", Path.c_str());
	//mvprintw(0, 2, "%s %i %i", selected.c_str(), cursor_y, cursor_x);
	//mvprintw(0, 2, "%s", keyname(ch));
	//mvprintw(0, 2, "%i %i", cursor_y, cursor_x);
	//if(selected != NULL)mvprintw(0, 2, "%s %i %i %i", selected->path, strcmp(selected->path, ".."), cursor_y, cursor_x);
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
	
	move(cursor_y+2, cursor_x*20);
	refresh();
}

void list_dir() {
	DIR* dir = opendir(Path.c_str());
	if(dir == NULL) return;
	
	struct stat dir_stat;
	stat(Path.c_str(), &dir_stat);
		
	DirCacheNode* node = nullptr;
	try {
		node = &dirCache.at(Path);
	} catch(const std::out_of_range&) {
		node = new DirCacheNode(Path, dir_stat.st_mtime);
		struct dirent* dp;
		while((dp = readdir(dir)) != NULL) {
			node->push(std::string(dp->d_name));
		}
		dirCache.insert(std::make_pair(Path, *node));
	}
	
	int max_lines = LINES - 3;
	for(int y = 2; y < max_lines; y++) {
		for(int x = 0; x < COLS; x++) {
			mvaddch(y,x,' ');	
		}
	}
	
	node->build_map(max_lines - 2);
	unsigned int lines = 2, columns = 0;
	unsigned int x = 0, y = 0;
	for(const std::vector<std::string>& vector: node->map) {
		for(const std::string& dir : vector) {
			//mvprintw(lines, columns, "%.3s %i %i %i %i", dir.c_str(), y, x, lines, columns);
			mvprintw(lines, columns, "%.18s", dir.c_str());
			lines++;
			y++;
		}
		lines = 2;
		columns += 20;
		x++;
	}
	
	if(cursor_changed) {
		selected.assign(node->get_map(cursor_y, cursor_x));
		cursor_changed = false;
	}
}