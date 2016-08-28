#include "udp_server.h"

//something printf 
static void print_log(std::string _log)
{
	std::cerr<<_log<<std::endl;
}

udp_server::udp_server(unsigned short _port)
    :port(_port)
{
	sock = -1;
}
//记录在线用户
bool udp_server::register_user(const struct sockaddr_in &client, std::string &_msg)
{
	std::string _ip_key = inet_ntoa(client.sin_addr);
	print_log(_ip_key);
	//先将消息反序列化，再根据消息的cmd属性，判断是否需要添加用户或者删除用户
	udp_data _data;
	_data.str_to_val(_msg);
	std::string _cmd;
	_data.get_cmd(_cmd);
	if( _cmd == "MESSAGE" )
	{//先在map中查找该用户，没找到再考虑添加
    		std::map<std::string, struct sockaddr_in>::iterator _iter = online_user.find(_ip_key);
    		if( _iter != online_user.end() )
        	{
    			return false;
    		}
    		online_user.insert(std::pair<std::string, struct sockaddr_in>(_ip_key, client));
    		print_log("register done...");
	}else if(_cmd == "QUIT")
	{
		std::cout<<"erase user "<<_ip_key<<std::endl;
		online_user.erase(_ip_key);
	}
	return true;
}

//初始化服务器套接字描述符，指定监听端口号
//if success ,return 0;
//else exit process
int udp_server::init_server()
{
	sock = socket(AF_INET, SOCK_DGRAM, 0); 
	if(sock < 0){
		print_log(strerror(errno));
		exit(1);
	}
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port   = htons(port);
	local.sin_addr.s_addr   = htonl(INADDR_ANY); //任意IP
	//绑定端口号
	if( bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0 )
	{
		print_log(strerror(errno));
		exit(1);
	}
	return 0;
}
//*********服务器将从客户端接收到的消息推送到消息池中
int udp_server::reliable_recv_msg()
{
	//reliable method
	std::string _out_msg;
	int ret = recv_msg(_out_msg);
	if(ret > 0 ){
		print_log(_out_msg);
		pool.put_msg(_out_msg);
	}
	return ret;
}
//服务器将从客户端接收消息
//if success return 0
//else return 1
int udp_server::recv_msg(std::string &_out_msg)
{
	char buf[G_BLOCK_SIZE];
	memset(buf, '\0', sizeof(buf));
	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	ssize_t _size = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr*)&client, &len);
	if(_size < 0){
		print_log(strerror(errno));
		_out_msg = "";
		return 1;
	}else if( _size >= 0 ){
		_out_msg = buf;
		print_log(_out_msg);
		register_user(client, _out_msg); //记录用户信息
	}
	return _size;
}

//发送数据
//if success return 0
//else return 1
int udp_server::send_msg(const std::string &_in_msg,const struct sockaddr_in& client,socklen_t len)
{
	ssize_t _size = sendto(sock, _in_msg.c_str(), _in_msg.size(), 0, (struct sockaddr*)&client, len);
	if(_size < 0)
	{
		print_log(strerror(errno));
		return 1;
	}
	else
	{
		//Do Nothing...	
	}
	return 0;
}
//************服务器向各个在线客户端广播数据
int udp_server::broadcast_msg()
{
	//首先，从消息池中拿到数据
	std::string msg;
	pool.get_msg(msg);
	std::map<std::string, struct sockaddr_in>::iterator _iter = online_user.begin();
	for(; _iter != online_user.end(); _iter++ )
	{
		print_log("broadcast: ");
		print_log(msg);
		send_msg(msg, _iter->second, sizeof(_iter->second));
	}
}

udp_server::~udp_server()
{
	if(sock != -1){
		close(sock);
	}
}



