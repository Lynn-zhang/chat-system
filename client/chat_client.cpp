#include <iostream>
#include <string>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <vector>
#include <signal.h>
#include <strstream>
#include "udp_client.h"
#include "window.h"
using namespace std;

chat_window win;
static udp_client *_sig_client = NULL;
string nick_name;
string school;
std::vector<std::string> list;

//标题窗口线程：在header框中将信息在左端和右端之间来回滑动
void *client_header(void *arg)
{
	win.draw_header();
	std::string message = "Everybody together to chat !";
	int step = 6;
	while(1){
		win.put_str_to_win(win.get_header_window(), 2, step++, message);
		win.win_refresh(win.get_header_window());
		step %= (COLS-6);
		if(step == 0){
			step = 6;
		}
		sleep(1);
		win.clear_win_line(win.get_header_window(), 2, 1);
	}
}

void *client_output(void *arg)
{
	udp_client *client = (udp_client*)arg;
	udp_data _data;
	std::string _msg;
	std::string info;
	std::string cmd;
	int total = 1;
	win.draw_output();
	win.win_refresh(win.get_output_window());
	while(1)
	{
		client->reliable_recv_msg(_msg);
		_data.str_to_val(_msg);
		
		_data.get_nick_name(nick_name);
		_data.get_school(school);
		_data.get_msg(info);
		_data.get_cmd(cmd);

		if(cmd == "MESSAGE")
		{
			std::string show_msg ='['+ _data.nick_name;
			show_msg += "-";
			show_msg += _data.school;
            		show_msg += ']';

			list.push_back(show_msg);

			show_msg += ": ";
			show_msg += info;
			win.put_str_to_win(win.get_output_window(), total++, 2, show_msg);
			win.win_refresh(win.get_output_window());

			total %= (LINES*3/5-1);
			if(total == 0)
			{
     				total = 1;
				win.clear_win_line(win.get_output_window(), 1, LINES*3/5-1);
				win.win_refresh(win.get_output_window());
			}
			 std::string friend_key=nick_name + "/" +school;
        		 client->add_friend(friend_key);

		}
	        else if(cmd=="QUIT")
	        {
			std::string friend_key=nick_name + "/" +school;
	        	client->del_friend(friend_key);
	        	exit(0);
	        }
		usleep(100000);
	}
}
void *client_list(void *arg)
{
    //好友列表窗口线程:	画线->获取好友列表->计算好友列表的页数->向list窗口写入每页的好友信息
	udp_client *cli = (udp_client*)arg;
	win.draw_friends_list();	//画线
	WINDOW *l = win.get_friends_list_window();
	int x = 2;
	while(1){
		int max_y,max_x;
		win.win_refresh(l);
		getmaxyx(l, max_y, max_x);
		//获取好友列表
		std::vector<std::string> _vec=cli->get_friends_list();
		int friends_num = _vec.size();
		int page = max_y-3;
		//计算好友列表的页数
		int page_num = friends_num/page;
		int page_mod = friends_num%page;
		if( page_mod > 0 ){
			page_num++;
		}
		//向frined_list窗口写入页码及每页的好友信息
		std::strstream ss;
		std::string str_page_num;
		ss<<page_num;
		ss>>str_page_num;
		std::string _out_page;
		win.win_refresh(l);
		for(int i=0; i<page_num; i++)
		{
			std::string str_i;
			std::strstream ssi;
			ssi<<i+1;
			ssi>>str_i;		
			_out_page= str_i+"/"+str_page_num;
			win.clear_win_line(l, max_y-2, 1);
			win.put_str_to_win(l, max_y-2, max_x/2, _out_page);//写入页码

			int y = 0;
			for(int j=0; j<page; j++)
        		{
  				y++;
				y=y%(max_y-3);
				int index = i*page+j;
				if( index < friends_num)
        			{
   					std::string _fri = _vec[index];
					if(y == 0)
                			{
    		        		        win.win_refresh(l);
		        		 	win.clear_win_line(l, 1, max_y-4);
			  		   	continue;
					}
		        		win.put_str_to_win(l, y, x, _fri);
				}
        			else
        			{
					break;
				}
			}
		}
	}
}
void *client_input(void *arg)
{
	udp_client *client = (udp_client*)arg;
	
	std::string message = "Please Enter: ";

	std::string client_msg;
	std::string cmd = "MESSAGE";

	std::string send_string;
	udp_data _data;

	win.draw_input();
	while(1){
		win.win_refresh(win.get_input_window());
		win.put_str_to_win(win.get_input_window(), 1, 2, message);
		win.get_str_from_win(win.get_input_window(), client_msg);
        if(client_msg==string("quit"))
        {
            	cmd="QUIT";
		std::string friend_key=nick_name + "/" +school;
	        client->del_friend(friend_key);
            	exit(0);
        }

		_data.set_nick_name(nick_name);
		_data.set_school(school);
		_data.set_msg(client_msg);
		_data.set_cmd(cmd);


		_data.val_to_str(send_string);
		client->reliable_send_msg(send_string);
		usleep(100000);
		win.clear_win_line(win.get_input_window(), 1, 1);
	}
}

static void usage(char* proc)
{
    cout<<"Please Enter:# "<<proc<<"  [IP]  [PORT]"<<endl;
}
int main(int argc,char* argv[])
{
    if(argc!=3)
    {
        usage(argv[0]);
        exit(0);
    }
    cout<<"Please Enter Your Nick Name: ";
    getline(cin,nick_name);
    std::cout<<"Please Enter Your School: ";
    getline(cin,school);
    std::cout<<std::endl;

    udp_client client;
    client.init_client();
    win.init();
    //
    //////////////////////////////////////
    //
	pthread_t header, output, list, input;
	pthread_create(&header, NULL, client_header, NULL);
	pthread_create(&output, NULL, client_output, (void*)&client);
	pthread_create(&list, NULL, client_list, (void*)&client);
	pthread_create(&input, NULL, client_input, (void*)&client);

	pthread_join(header, NULL);
	pthread_join(output, NULL);
	pthread_join(list, NULL);
	pthread_join(input, NULL);
	return 0;
}
