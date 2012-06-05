//DRAWER

#include <winsock2.h>
#include <windows.h>
#include <vadefs.h>
#include <stdio.h>
#include <SDL.h>
#include <string.h>
#include <SDL_opengl.h>

struct team {
	char nume[8];
	int punctaj;
	int corecte;
	int problema_bonus;
	int raspunsuri[101];
} ;
struct problema{
	int raspuns;
	int puncte;
	int bonus;
	bool answered[101];

};
unsigned int siglaID;
team echipa[101],echipa_last[101];
problema probl[101],probl_last[101];
int echipe,probleme;

void d(const char *x){
	printf("[DEBUG]%s\n",x);
}
SOCKET client;
void drawText(int x,int y,double size,double r,double g,double b,double a,const char *X,...);
char latestData[4096];
int SCREEN_WIDTH,SCREEN_HEIGHT;
bool running=true,key[512],keyl[512];
int screen=0;
int time_left=0;


void draw2D(int x,int y,int w,int h,double r,double g,double b,double a,bool hollow=false){
	glDisable(GL_TEXTURE_2D);
	glColor4f(r,g,b,a);
	if (!hollow)
		glBegin(GL_QUADS);
	else
		glBegin(GL_LINE_LOOP);
		glVertex2i(x,y);
		glVertex2i(x,y+h);
		glVertex2i(x+w,y+h);
		glVertex2i(x+w,y);
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void teamscore_u(){

}
void teamscore_d(){
	int targetwidth=SCREEN_WIDTH-200;
	int max=-1,maxi;
	for (int i=1;i<=echipe;i++){
		if (echipa[i].punctaj>max){
			max=echipa[i].punctaj;
			maxi=i;
		}
	}
	for (int i=1;i<=echipe;i++){
		int drawedwidth=int(echipa[i].punctaj/double(max)*(SCREEN_WIDTH-200));
		drawText(0,130+32*(i-1),16,1,1,1,1,echipa[i].nume,i);
		if (max==echipa[i].punctaj){
			draw2D(100,130+32*(i-1),drawedwidth,16,0,1,0,1);
		}
		else
			draw2D(100,130+32*(i-1),drawedwidth,16,1,0,0,1);
		drawText(SCREEN_WIDTH-100,130+32*(i-1),16,1,1,1,1,"%.3d",echipa[i].punctaj);
	}
}

void problemscore_u(){
}
void problemscore_d(){
	int max=-1;
	int targetwidth=SCREEN_WIDTH-200;
		for (int i=1;i<=probleme;i++){
			if (probl[i].puncte>max){
			max=probl[i].puncte;
		}
	}
	for (int i=1;i<=probleme;i++){
		int drawedwidth=int(probl[i].puncte/double(max)*(SCREEN_WIDTH-200));
		drawText(0,130+32*(i-1),16,1,1,1,1,"prob %.2d",i);
		if (probl[i].bonus==20)
			draw2D(100,130+32*(i-1),drawedwidth,16,0,1,0,1);
		else
			draw2D(100,130+32*(i-1),drawedwidth,16,1,0,0,1);
		drawText(SCREEN_WIDTH-100,130+32*(i-1),16,1,1,1,1,"%.2d",probl[i].puncte);
	}
}

void tablescreen_u(){
}
void tablescreen_d(){
	for (int j=1;j<=probleme;j++){
		for (int i=1;i<=echipe;i++){
			drawText(0,150+(i-1)*32+8,16,1,1,1,1,echipa[i].nume);
			if (echipa[i].raspunsuri[j]==0)
				draw2D(100+(j-1)*32,150+(i-1)*32,32,32,0.25,0.25,0.25,1);
			else
				if (probl[j].answered[i])
					draw2D(100+(j-1)*32,150+(i-1)*32,32,32,0.25,1,0.25,1);
				else
					draw2D(100+(j-1)*32,150+(i-1)*32,32,32,1,0.25,0.25,1);


			drawText(100+(j-1)*32+2,150+(i-1)*32+8+2,16,0,0,0,1,"%d",echipa[i].raspunsuri[j]);
			drawText(100+(j-1)*32,150+(i-1)*32+8,16,1,1,1,1,"%d",echipa[i].raspunsuri[j]);
			
		}
		drawText(100+(j-1)*32,134,16,1,1,1,1,"%.2d",j);
	}
	for (int j=1;j<=probleme;j++)
		for (int i=1;i<=echipe;i++)
			if (echipa[i].problema_bonus==j)
				draw2D(100+(j-1)*32,150+(i-1)*32,32,32,1,1,1,1,true);
}

void waitscreen_u(){

}
void waitscreen_d(){
	drawText(SCREEN_WIDTH/2-strlen("Pregatiti-va!")*32+64,SCREEN_HEIGHT/2-32,64,1,1,1,1,"Pregatiti-va!");
}

void process_data(){
	char pd[4096];
	memcpy(pd,latestData,sizeof(latestData));
	
	if (strcmp(pd,"ERROR")==0)return;
	d("processing data");
	char *mover;
	mover=strtok(pd,"|");  time_left=atoi(mover);
	mover=strtok(NULL,"|");echipe=atoi(mover);
	mover=strtok(NULL,"|");probleme=atoi(mover);
	for (int i=1;i<=echipe;i++){
		mover=strtok(NULL,"|");strcpy(echipa[i].nume,mover);
		mover=strtok(NULL,"|");echipa[i].punctaj=atoi(mover);
		mover=strtok(NULL,"|");echipa[i].corecte=atoi(mover);
		mover=strtok(NULL,"|");echipa[i].problema_bonus=atoi(mover);
		for (int j=1;j<=probleme;j++){
			mover=strtok(NULL,"|");echipa[i].raspunsuri[j]=atoi(mover);
		}

	}
	for (int i=1;i<=probleme;i++){
		mover=strtok(NULL,"|");probl[i].puncte=atoi(mover);
		mover=strtok(NULL,"|");probl[i].raspuns=atoi(mover);
		mover=strtok(NULL,"|");probl[i].bonus=atoi(mover);
		for (int j=1;j<=echipe;j++){
			mover=strtok(NULL,"|");probl[i].answered[j]=atoi(mover);
		}
	}
	memcpy(echipa_last,echipa,sizeof(echipa));
	memcpy(probl_last,probl,sizeof(probl));
}

#pragma region Data writing

struct {
	double sx,sy;
} chars[512];
void generate_font(){
	char text[]={"abcdefgh\nijklmnop\nqrstuvwx\nyz012345\n6789!@#$\n%^&*()[]\n{}-=_+/\\\n,.<>`~| "};
	double X,Y;
	X=Y=0;
	for (int i=0;text[i]!=0;i++){
		if (text[i]=='\n'){
			X=0;
			Y+=0.125;
			continue;
		}
		chars[text[i]].sx=X;
		chars[text[i]].sy=Y;
		X+=0.125;
	}
}
unsigned int textureID=-1;
void load(){
	SDL_Surface *surface=SDL_LoadBMP("font.bmp");
	unsigned char *s;
	UCHAR *r=new UCHAR[512*512*4];
	long c=-1;
	s=(unsigned char *)surface->pixels;
	for (int i=0;i<512*512*3;i+=3){
			unsigned char R=s[i];
			unsigned char G=s[i+1];
			unsigned char B=s[i+2];
			unsigned char A=255;
			if (R==255 && B==255 && G==0){
				R=G=B=0;
				A=0;
			}
			r[++c]=R;
			r[++c]=G;
			r[++c]=B;
			r[++c]=A;
		}
		glGenTextures(1,&textureID);
		glBindTexture(GL_TEXTURE_2D,textureID);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexImage2D( GL_TEXTURE_2D, 0, 4, 512, 512, 0,GL_RGBA, GL_UNSIGNED_BYTE,r);
		delete r;
		SDL_FreeSurface(surface);
		SDL_Surface *fas=SDL_LoadBMP("sigla.bmp");
		glGenTextures(1,&siglaID);
		glBindTexture(GL_TEXTURE_2D,siglaID);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexImage2D( GL_TEXTURE_2D, 0, 3, 512, 512, 0,GL_BGR, GL_UNSIGNED_BYTE,fas->pixels);
}
void drawText(int x,int y,double size,double r,double g,double b,double a,const char *X,...){
	va_list arg;
	va_start(arg,X);
	char text[4096]={0};
	vsprintf(text,X,arg);
	for (int i=0;text[i]!=0;i++)
		if (text[i]>='A' && text[i]<='Z')
			text[i]=text[i]-'A'+'a';
	glBindTexture(GL_TEXTURE_2D,textureID);
	int DX=x,DY=y;
	glColor4f(r,g,b,a);
	for (int i=0;text[i]!=0;i++){
		if (text[i]==' ') {DX+=size;continue;}
		glBegin(GL_QUADS);
		glTexCoord2d(chars[text[i]].sx,chars[text[i]].sy);
		glVertex2i(DX,DY);
		glTexCoord2d(chars[text[i]].sx,chars[text[i]].sy+0.125);
		glVertex2i(DX,DY+size);
		glTexCoord2d(chars[text[i]].sx+0.125,chars[text[i]].sy+0.125);
		glVertex2i(DX+size,DY+size);
		glTexCoord2d(chars[text[i]].sx+0.125,chars[text[i]].sy);
		glVertex2i(DX+size,DY);
		glEnd();
		DX+=(5*size)/6;
	}


}
#pragma endregion


int s_moved=-300; 
void tick_sigla(){
	s_moved++;
	if (s_moved>-1)
		s_moved=-300;
}

void draw_timer(){
	drawText(SCREEN_WIDTH/2-5*64,0,128,1,1,1,1,"%.2dm%.2ds",time_left/60,time_left%60);
}
void draw_sigla(){
	for (int i=0;i<=1580;i+=300){
	glColor4f(1,1,1,1);
	glBindTexture(GL_TEXTURE_2D,siglaID);
	glBegin(GL_QUADS);
	glTexCoord2d(0,0);
	glVertex2i(s_moved+i,1024-200);
	glTexCoord2d(0,1);
	glVertex2i(s_moved+i,1024-200+300);
	glTexCoord2d(1,1);
	glVertex2i(s_moved+i+300,1024-200+300);
	glTexCoord2d(1,0);
	glVertex2i(s_moved+i+300,1024-200);
	glEnd();
	}
}


void e(){
	SDL_Event tmp;
	while (SDL_PollEvent(&tmp)){
		switch (tmp.type){
		case SDL_QUIT:running=false;return;
		case SDL_KEYDOWN:key[tmp.key.keysym.sym]=true;return;
		case SDL_KEYUP:key[tmp.key.keysym.sym]=false;return;

		}
	}
}
void up(){
	if (strcmp(latestData,"ERROR")==0)
		screen=0;
	else
		if (screen==0)
			screen=1;
	if (!key[SDLK_1] && keyl[SDLK_1])
		screen=1;
	if (!key[SDLK_2] && keyl[SDLK_2])
		screen=2;
	if (!key[SDLK_3] && keyl[SDLK_3])
		screen=3;
	if (key[SDLK_ESCAPE])
		running=false;
	tick_sigla();
	switch(screen){
	case 0:waitscreen_u();break;
	case 1:teamscore_u();break;
	case 2:problemscore_u();break;
	case 3:tablescreen_u();break;
	}

	process_data();
	memcpy(keyl,key,sizeof(key));
}
void dr(){
	glClear(GL_COLOR_BUFFER_BIT);
	draw_sigla();
	draw_timer();
	switch(screen){
	case 0:waitscreen_d();break;
	case 1:teamscore_d();break;
	case 2:problemscore_d();break;
	case 3:tablescreen_d();break;
	}
	
	SDL_GL_SwapBuffers();
}

DWORD WINAPI thread(LPVOID arg){
	generate_font();
	SDL_Init(SDL_INIT_VIDEO);
	int w=int(GetSystemMetrics(SM_CXFULLSCREEN)),h=int(GetSystemMetrics(SM_CYFULLSCREEN));
	SCREEN_WIDTH=1280;SCREEN_HEIGHT=1024;
	SDL_SetVideoMode(w,h,0,SDL_OPENGL|SDL_FULLSCREEN);

	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0,0,w,h);
	glOrtho(0,SCREEN_WIDTH,SCREEN_HEIGHT,0,-1,1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	

	load();

	int lTick=SDL_GetTicks();
	while (running){
		e();
		while (SDL_GetTicks()-lTick>1000/60){
			up();
			lTick+=1000/60;
		}
		dr();
	}
	return 0;
}

int main(int a,char **argv){
	char data[4096];
	char strIPADDR[128]={0};printf("IP ADDRESS:");scanf("%s",&strIPADDR);
	int tmp=0;
	CreateThread(NULL,0,thread,&tmp,0,0);

	WSADATA dat;
	WSAStartup(MAKEWORD(2,2),&dat);
	d("host");
	struct sockaddr_in sin;

	memset( &sin, 0, sizeof sin );
	
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = ((in_addr *)(gethostbyname(strIPADDR)->h_addr))->s_addr;
	sin.sin_port = htons( 1030 );

	client=socket(AF_INET,SOCK_STREAM,0);

	

	if ( connect( client,(sockaddr*)&sin, sizeof sin ) == SOCKET_ERROR )
	{
		d("ERROR CONNECTING");
		return 1;
	}
	memset(strIPADDR,0,sizeof(strIPADDR));
	strcpy(strIPADDR,"data");
	
	while (running){
		if (!(send(client,strIPADDR,sizeof(strIPADDR),0)>0)){d("ERR");running=false;};
		recv(client,data,sizeof(data),0);
	//	d(data);
		memcpy(latestData,data,sizeof(data));
	}
	return 0;
}