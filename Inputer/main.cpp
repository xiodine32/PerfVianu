
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <stdio.h>

const bool debug=true;


void d(const char *data){
	if (!debug) return;
	printf("[DEBUG]%s\n",data);
}

SOCKET client;

int main(){
	char text[1024]={"127.0.0.1"};
	//printf("ENTER SERVER IP:");scanf("%s",&text);
	WSADATA dat;
	WSAStartup(MAKEWORD(2,2),&dat);
	d("start");
	d("host");
	struct sockaddr_in sin;

	memset( &sin, 0, sizeof sin );
	
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = ((in_addr *)(gethostbyname(text)->h_addr))->s_addr;
	sin.sin_port = htons( 1030 );

	client=socket(AF_INET,SOCK_STREAM,0);


	if ( connect( client,(sockaddr*)&sin, sizeof sin ) == SOCKET_ERROR )
	{
		d("ERROR CONNECTING");
	}
	bool running=true;
	memset(text,0,sizeof(text));
	strcpy(text,"I AM GOD");
	while (running){
		d("send");
		if (!(send(client,text,sizeof(text),0)>0)){
			d("ERR");
			running=false;
		} else
			Sleep(100);
		
	}
	d("end");
	getchar();
	return 0;
}