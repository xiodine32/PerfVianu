
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include <vector>
#include <stack>
using namespace std;
WSADATA dat;

struct client{
	SOCKET s;
	bool destroyed;
}; 
vector<client> clients;
vector<client> tempo;
stack<int> todestroy;


const bool debug=true;


void d(const char *data){
	if (!debug) return;
	printf("[DEBUG]%s\n",data);
}
void s(const char *data){
	printf("[INFO]%s\n",data);
}
SOCKET server;
DWORD WINAPI Threading(LPVOID text){
	while (true){
		while (listen(server, 1000)==INVALID_SOCKET) Sleep(100);
		SOCKET c=accept(server,NULL,NULL);
		if (c!=INVALID_SOCKET){
			d("FOUND A CLIENT!");
			client temp;
			temp.s=c;
			temp.destroyed=false;
			clients.push_back(temp);
		}
		Sleep(100);
	}
	return 0;
}
int main(){
	d("Loading winsock2");
	WSAStartup(MAKEWORD(2,2),&dat);
	d("Loading server");
	
	
	server=socket(AF_INET,SOCK_STREAM,0);
	
	sockaddr_in anews;
	anews.sin_port = htons(1030);
	anews.sin_addr.s_addr = INADDR_ANY;
	anews.sin_family = AF_INET;
	if (bind(server, (sockaddr*)&anews, sizeof(anews)) == SOCKET_ERROR) {
		d("ERROR BINDING");
		goto cleanup;
	}
	HANDLE searchT;
	d("Waiting for clients. DERP.");
	bool running=true;
	int l=sizeof(anews);
	
	d("Starting Thread #1");
	searchT=CreateThread(NULL,0,Threading,&l,0,NULL);
	d("Starting main loop");
	
	while (running){
		while (!todestroy.empty()){
			clients.erase(clients.begin()+todestroy.top());
			d("Deleted a client");
			todestroy.pop();
		}
		tempo=clients;
		int t=-1;
		for (vector<client>::iterator i=tempo.begin();i!=tempo.end();i++){
			++t;
			if (i->destroyed)continue;
			char text[1024];
			int out=recv(i->s,text,sizeof(text),0);
			if (out>0){
				d(text);	
				//HANDLE DATA
			}
			else{
				if (out==0 || out==INVALID_SOCKET){
					todestroy.push(t);
				}
			}
		}
		Sleep(13);
	}
	cleanup:
	d("Finishing");
	
	WSACleanup();
	d("All Done");
	getchar();
	return 0;
}