#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "rules.h"

void d(const char *data){
	if (!debug) return;
	printf("[DEBUG]%s\n",data);
}
void s(const char *data){
	printf("[INFO]%s\n",data);
}

team echipa[101];
problema probl[101];
int echipe,probleme;
int contest_time=120;
int time_start;
bool started;

void readconfig(){
	FILE *f=fopen("config.txt","r");
	char text[4096];
	fgets(text,4096,f);
	fscanf(f,"\n%d\n",&echipe);
	fgets(text,4096,f);
	fscanf(f,"\n%d\n",&contest_time);
	fgets(text,4096,f);
	fscanf(f,"\n%d\n",&probleme);
	fgets(text,4096,f);fscanf(f,"\n");
	for (int i=1;i<=probleme;i++){
		fscanf(f,"%d ",&probl[i].raspuns);
		probl[i].puncte=PROBLEMA_INITIAL;
		probl[i].bonus=BONUS_MAX;
		for (int j=1;j<=echipe;j++)
			probl[i].answered[j]=false;
	}
	fgets(text,4096,f);
	for (int i=1;i<=echipe;i++){
		fgets(echipa[i].nume,40,f);
		while (echipa[i].nume[strlen(echipa[i].nume)-1]=='\n')
			echipa[i].nume[strlen(echipa[i].nume)-1]=0;

	}

	//generate content
	for (int i=1;i<=echipe;i++){
		echipa[i].corect_pb=false;
		echipa[i].punctaj=PUNCTAJ_ECHIPA_INITIAL;
		echipa[i].corecte=0;
		echipa[i].raspunsuri[0]=0;
		echipa[i].problema_bonus=-1;
		sprintf(text,"created team %d",i);
		d(text);
	}
	
	fclose(f);
}
void nostart(){
	d("CONTEST NOT STARTED YET");
}

void start_contest(){
	if (!started){
		time_start=clock();
		started=true;
	}
	else
		d("CONTEST ALREADY STARTED");
}

void force_end_contest(){
	if (!started) {nostart();return;}
}

const char *say_time_left(){
	
	char towrite[4096]={"ERROR"};
	if (!started) {nostart();return towrite;}
	sprintf(towrite,"%d",contest_time*60-(clock()-time_start)/CLK_TCK);
	return towrite;
}
const char *say_drawables(){
	char towrite[4096]={"ERROR"};memset(towrite,0,sizeof(towrite));strcpy(towrite,"ERROR");
	if (!started) {nostart();return towrite;}
	sprintf(towrite,"%s|%d|%d|",say_time_left(),echipe,probleme);
	for (int i=1;i<=echipe;i++){
		sprintf(towrite,"%s%s|%d|%d|%d|",towrite,echipa[i].nume,echipa[i].punctaj,echipa[i].corecte,echipa[i].problema_bonus);
		for (int j=1;j<=probleme;j++)
			sprintf(towrite,"%s%d|",towrite,echipa[i].raspunsuri[j]);
	}
	for (int i=1;i<=probleme;i++){
		sprintf(towrite,"%s%d|%d|%d|",towrite,probl[i].puncte,probl[i].raspuns,probl[i].bonus);
		for (int j=1;j<=echipe;j++)
			sprintf(towrite,"%s%d|",towrite,probl[i].answered[j]?1:0);
	}
	return towrite;
}
void add_team_score(int team,int points){
	echipa[team].punctaj+=points;
}
void add_finish_bonus(int tema ){
	static int conquered=0;
	printf("GZ TEAM %d\n",tema);
	if (++conquered==1) echipa[tema].punctaj+=100;
	if (++conquered==2) echipa[tema].punctaj+=60;
	if (++conquered==3) echipa[tema].punctaj+=40;
	if (++conquered==4) echipa[tema].punctaj+=30;
	if (++conquered==5) echipa[tema].punctaj+=20;
	if (++conquered==6) echipa[tema].punctaj+=10;
}
void add_answer(int team,int problem,int answer){
	if (probl[problem].answered[team]) {s("Duplicate data truncated");return;}
	echipa[team].raspunsuri[problem]++;
	if (probl[problem].raspuns==answer){
		probl[problem].answered[team]=true;
		echipa[team].corecte++;
		if (echipa[team].corecte==probleme){
			add_finish_bonus(team);
		}
		echipa[team].punctaj+=probl[problem].puncte;
		echipa[team].punctaj+=probl[problem].bonus;
		if (echipa[team].problema_bonus==problem){
			//double power
			if (!echipa[team].corect_pb){
				echipa[team].punctaj+=probl[problem].puncte;
				echipa[team].punctaj+=probl[problem].bonus;
				echipa[team].corect_pb=true;
			}
		}
		switch (probl[problem].bonus){
		case BONUS_MAX:probl[problem].bonus=15;break;
		case 15:probl[problem].bonus=10;break;
		case 10:probl[problem].bonus=8;break;
		case 8:probl[problem].bonus=6;break;
		case 6:probl[problem].bonus=4;break;
		case 4:probl[problem].bonus=3;break;
		case 3:probl[problem].bonus=2;break;
		case 2:probl[problem].bonus=1;break;
		case 1:probl[problem].bonus=0;break;
		}
	}
	else{
		
		if (echipa[team].problema_bonus==problem)
			echipa[team].punctaj-=PENALITATE_GRESEALA;
		echipa[team].punctaj-=PENALITATE_GRESEALA;
		if (probl[problem].bonus==BONUS_MAX)
			probl[problem].puncte+=2;
	}
}

void tick(){
	if (!started) return;
	static int draw=-1;
	if (++draw==0){
		char tw[4096]={0};strcpy(tw,say_drawables());
		FILE *a=fopen("backup.txt","w");
		fprintf(a,"%s",tw);
		fclose(a);
		draw=-5*60;
	}

	static int lleft=-1;
	int left=contest_time*60-(clock()-time_start)/CLK_TCK;
	if ((clock()-time_start)/CLK_TCK>20*60){
		for (int i=1;i<=echipe;i++)
			if (echipa[i].problema_bonus==-1)
				echipa[i].problema_bonus=1;
	}
	if (lleft==-1) lleft=left;
	if (lleft==left) return;
	int nMin=(left/60)*60;
	if (left==nMin && left>20*60){
		for (int i=1;i<=probleme;i++)
			if (probl[i].bonus==BONUS_MAX)
				probl[i].puncte++;
	}
	lleft=left;
}
void set_team_name(int team, char name[8]){
	strcpy(echipa[team].nume,name);
}
void set_team_bonus(int team,int pb){
	if (!started) {nostart();return;}

	int ela=(clock()-time_start)/CLK_TCK;
	if (ela>600) {s("Cannot modify team bonus, time expired.");return;}
	if (echipa[team].problema_bonus!=-1){
		s("Team bonus already modified");
		return;
	}
	echipa[team].problema_bonus=pb;

}

void load_backup(){
	started=true;
	FILE *f=fopen("backup.txt","r");
	char pd[4096];
	fgets(pd,4096,f);
	
	if (strcmp(pd,"ERROR")==0)return;
	d("processing data");
	char *mover;
	mover=strtok(pd,"|");  time_start=clock()-120*60*CLK_TCK;
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
}
 