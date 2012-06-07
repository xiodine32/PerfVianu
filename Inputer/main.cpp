//CONSOLE CLIENT
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <stdio.h>


const bool debug=true;
bool recieved=false;
bool running=true;

void d(const char *data){
	if (!debug) return;
	//printf("[DEBUG]%s\n",data);
}

SOCKET client;
char sendText[4096]={0},recvText[4096]={0};
int handle_text(const char *text){
	if (strcmp(text,"help")==0){
		printf("\nAdd an answer: a<TEAM><PROBLEM><ANSWER>\n");
		printf("Start Server: start\n");
		printf("Time Left: timeleft\n");
		printf("Set Team Special Problem: setp<TEAM><#>\n");
		printf("Add/Subtract points: add_<TEAM>|<POINTS> (can be negative)\n");
		printf("\n");
		return 0;
	}
	if (strcmp(text,"START REC")==0){
		strcpy(sendText,"OAIE");
		return 1;
	}
	if (strcmp(text,"STOP REC")==0){
		strcpy(sendText,"PULA");
		return 1;
	}
	if (strcmp(text,"start")==0){
		printf("******************\n*STARTING CONTEST*\n******************\n");
		strcpy(sendText,"start");
		return 1;
	}
	if (strcmp(text,"timeleft")==0){
		strcpy(sendText,"time");
		printf("Requested time left\n");
		return 1;
	}
	if (strstr(text,"setp")){
		sprintf(sendText,"S|%c%c|%c%c",text[4],text[5],text[6],text[7]);
		printf("Set Special Problem for %c%c, problem #%c%c.\n",text[4],text[5],text[6],text[7]);
		return 1;
	}
	if (strstr(text,"add_")){
		text=(text+4);
		sprintf(sendText,"add|%s",text);
		printf("Did that, done that.\n");
		return 1;
	}
	if (text[0]=='a' && (text[1]>='0' && text[1]<='9')){
		char tmp[4096]={0};strcpy(tmp,text);
		printf("Answer for TEAM:%c%c - PROBLEM:%c%c - %c%c%c%c\n",tmp[1],tmp[2],tmp[3],tmp[4],tmp[5],tmp[6],tmp[7],tmp[8],tmp[9]);
		char ans[1024]={0};
		printf("Are you sure? (yes/YES/Y/y) :");scanf("%s",&ans);
		if (strcmp(ans,"yes")==0 || strcmp(ans,"YES")==0 || strcmp(ans,"Y")==0 || strcmp(ans,"y")==0){
			sprintf(sendText,"a|%c%c|%c%c|%c%c%c%c",tmp[1],tmp[2],tmp[3],tmp[4],tmp[5],tmp[6],tmp[7],tmp[8],tmp[9]);
			printf("[SENT]Answer for TEAM:%c%c, PROBLEM:%c%c with %c%c%c%c\n",tmp[1],tmp[2],tmp[3],tmp[4],tmp[5],tmp[6],tmp[7],tmp[8],tmp[9]);
			gets(tmp);
			return 1;

		}
		printf("[NOT SENT]Answer for TEAM:%c%c - PROBLEM:%c%c - %c%c%c%c\n",tmp[1],tmp[2],tmp[3],tmp[4],tmp[5],tmp[6],tmp[7],tmp[8],tmp[9]);
		gets(tmp);
		return 0;
	}
	printf("COMMAND NOT FOUND\n");	
	return 0;
}

DWORD WINAPI listener(LPVOID arg){
	while (running){
		if (recv(client,recvText,sizeof(recvText),0)>0){
			recieved=true;
			d("RECIEVED TEXT");
		}
		Sleep(10);
	}
	return 0;
}
bool newData=false;
char toSend[4096]={0};
DWORD WINAPI sender(LPVOID arg){
	char tmp[4096]={"ok"};
	if (newData){
		newData=false;
	}
	else
		strcpy(toSend,tmp);
	while (running){
		send(client,toSend,sizeof(toSend),0);
	}
	return 0;
}
int main(){
	char text[4096]={"127.0.0.1"};
	printf("ENTER SERVER IP:");scanf("%s",&text);
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
		return 1;
	}
	gets(text);
	CreateThread(NULL,0,listener,&text,0,0);
	memset(text,0,sizeof(text));
	strcpy(text,"IAMCLIENT");
	CreateThread(NULL,0,sender,&text,0,0);
	while (running){
		printf("<SERVER>:");gets(text);
		if (handle_text(text))
			if (!(send(client,sendText,sizeof(sendText),0)>0)){d("ERR");running=false;};
	
	}
	d("end");
	getchar();
	return 0;
}