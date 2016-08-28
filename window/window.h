#pragma once

#include <iostream>
#include <ncurses.h>
#include <string>
#include <string.h>
#include <pthread.h>

class chat_window{
	public:
		//初始化，将终端屏幕初始化为curses 模式
		chat_window();
		void init();
		
		//创建窗口
		WINDOW* create_win(const int &hei, const int &wth, const int &y, const int &x);
		//围绕窗口大小框架绘制边框并刷新窗口
		void win_refresh(WINDOW *_win);
		
		//向窗口写入字符
		void put_str_to_win(WINDOW *_win, int y, int x, const std::string &_str);
		//抓取数据
		void get_str_from_win(WINDOW *_win, std::string &_out);
		//清除字符
		void clear_win_line(WINDOW *_win, int begin, int num);
		
		~chat_window();
		
		//构建各窗口框架
		void draw_header();
		void draw_output();
		void draw_friends_list();
		void draw_input();

		WINDOW *get_header_window()
		{
			return this->header; 
		}

		WINDOW *get_output_window()
		{ 
			return this->output; 
		}

		WINDOW *get_friends_list_window()
		{ 
			return this->friends_list;
		}

		WINDOW *get_input_window()
		{
			return this->input;
		}
	private:
		pthread_mutex_t lock;
		WINDOW *header;
		WINDOW *output;
		WINDOW *friends_list;
		WINDOW *input;
};
