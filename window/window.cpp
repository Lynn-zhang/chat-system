#include "window.h"

#define G_BLOCK_SIZE 1024

chat_window::chat_window()
{
	header = NULL;
	output = NULL;
	friends_list = NULL;
	input = NULL;
}

void chat_window::init()
{
	pthread_mutex_init(&lock, NULL);  //初始化互斥锁
	initscr();   //将终端屏幕初始化为curses 模式
}

WINDOW* chat_window::create_win(const int &hei, const int &wth,	const int &y,const int &x)
{
	////创建从(y, x)开始的hei行，wth列的窗口
	WINDOW *_win = newwin(hei, wth, y, x);
	return _win;
}

void chat_window::win_refresh(WINDOW *_win)
{
	box(_win, 0, 0);  //围绕窗口绘制边框
	pthread_mutex_lock(&lock);  //加锁
	wrefresh(_win);	 //刷新窗口
	pthread_mutex_unlock(&lock);
}
//清除数据
void chat_window::clear_win_line(WINDOW *_win, int begin, int num)
{
	while( num-- > 0 )
	{
		wmove(_win, begin++, 0);  //移动光标行
		wclrtoeol(_win);  //clrtoeol是从光标位置清除到光标所在行的结尾
	}
}

void chat_window::put_str_to_win(WINDOW *_win, int y, int x, const std::string &_str)
{
	mvwaddstr(_win, y, x, _str.c_str());  //用于在窗口中移动光标增加字符
}

void chat_window::get_str_from_win(WINDOW *_win, std::string &out) //out为输出型参数
{
	char buf[G_BLOCK_SIZE];
	memset(buf, '\0', sizeof(buf));
	wgetnstr(_win, buf, sizeof(buf));  //将窗口中字符写入buf

	out = buf;
}
 //绘制窗口...
void chat_window::draw_header()
{
	int hei = LINES/5;
	int wth = COLS;
	int y = 0;
	int x = 0;

	this->header = create_win(hei, wth, y, x);
}


void chat_window::draw_output()
{
	int hei = LINES*3/5;
	int wth = COLS*3/4;
	int y   = LINES/5;
	int x   = 0;
	this->output = create_win(hei, wth, y, x);
}

void chat_window::draw_friends_list()
{
	int hei = LINES*3/5;
	int wth = COLS/4;
	int y = LINES/5;
	int x = COLS*3/4;
	this->friends_list = create_win(hei, wth, y, x);
}

void chat_window::draw_input()
{
	int hei = LINES/5;
	int wth = COLS;
	int y = LINES*4/5;
	int x = 0;
	this->input = create_win(hei, wth, y, x);
}

//析构，删除窗口，退出curses模式
chat_window::~chat_window()
{
	pthread_mutex_destroy(&lock);
	delwin(this->header);
	delwin(this->output);
	delwin(this->friends_list);
	delwin(this->input);
	endwin(); //endwin()函数退出curses 模式,释放了curses 子系统和相关数据结构占用的内存
	pthread_mutex_unlock(&lock);
}
#if 0
int main()
{
    chat_window win;
     win.draw_header();
    return 0;
}
#endif
