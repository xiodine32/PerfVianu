//SERVER
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#include "main.h"
#include "rules.h"
WSADATA dat;


vector<client> clients;
vector<client> tempo;
stack<int> todestroy;

bool listening=true;

SOCKET server;
DWORD WINAPI Threading(LPVOID text){
	while (listening){
		while (listen(server, 1000)==INVALID_SOCKET) Sleep(100);
		SOCKET c=accept(server,NULL,NULL);
		if (c!=INVALID_SOCKET && listening){
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
HANDLE searchT;
void handle_text(client *f,const char *text){
	tick();
	if (strcmp(text,"ok")==0)
		return;
	if (strcmp(text,"start")==0){
		d("Starting contest");
		start_contest();
		return;
	}
	if (text[0]=='s'){
		d("Changing name");
		char tmp[4096]={0};memcpy(tmp,text,sizeof(tmp));
		char *p=strtok(tmp,"|");
		p=strtok(NULL,"|");
		int tta=atoi(p);
		p=strtok(NULL,"|");
		char namec[8];
		strcpy(namec,p);
		set_team_name(tta,namec);
		return;
	}
	if (text[0]=='S'){
		d("Changing Special Problem");
		char tmp[4096]={0};memcpy(tmp,text,sizeof(tmp));
		char *p=strtok(tmp,"|");
		p=strtok(NULL,"|");
		int tta=atoi(p);
		p=strtok(NULL,"|");
		int prbl=atoi(p);
		set_team_bonus(tta,prbl);
		return;
	}
	if (strcmp(text,"time")==0){
		d("Sending timeleft to console");
		char toSend[204]={0};
		strcpy(toSend,say_time_left());
		send(f->s,toSend,sizeof(toSend),0);
		return;
	}
	if (strcmp(text,"data")==0){
		char toSend[4096]={0};
		strcpy(toSend,say_drawables());
		send(f->s,toSend,sizeof(toSend),0);
		return;
	}
	if (strstr(text,"add|")){
		char tmp[4096]={0};memcpy(tmp,text,sizeof(tmp));
		char *p=strtok(tmp,"|");
		p=strtok(NULL,"|");
		int tta=atoi(p),add_amm=0;
		p=strtok(NULL,"|");
		add_amm=atoi(p);
		add_team_score(tta,add_amm);
		return;
	}
	if (strstr(text,"a|")){
		char tmp[4096]={0};memcpy(tmp,text,sizeof(tmp));
		char *Q=strtok(tmp,"|");
		int t,p,a;
		Q=strtok(NULL,"|");t=atoi(Q);
		Q=strtok(NULL,"|");p=atoi(Q);
		Q=strtok(NULL,"|");a=atoi(Q);
		add_answer(t,p,a);
		return;
	}
	if (strcmp(text,"PULA")==0){
		listening=false;
		CloseHandle(searchT);
		d("NO LONGER ACCEPTING THREADS");
		return;
		
	}
	s("command not recognised");
	s(text);
	return;
}


int main(){

	d("Reading configuration");
	FILE *t=fopen("backup.txt","r");
	if (t!=NULL){
		fclose(t);
		s("LOADING BACKUP DATA");
		load_backup();
	} else{
		s("Starting new SERVER");
		readconfig();
	}


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
			char text[4096];
			int out=recv(i->s,text,sizeof(text),0);
			if (out>0){
				handle_text(&(*i),text);
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