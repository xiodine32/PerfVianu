#ifndef RULES
#define RULES

const int PENALITATE_GRESEALA=10;
const int BONUS_MAX=20;
const int PROBLEMA_INITIAL=20;
const int PUNCTAJ_ECHIPA_INITIAL=120;
const bool debug=true;


void d(const char *data);
void s(const char *data);
void readconfig();
void start_contest();
void force_end_contest();
const char *say_time_left();
const char *say_drawables();
void add_team_score(int team,int points);
void add_answer(int team,int problem,int answer);
void set_team_name(int team, char name[8]);
void set_team_bonus(int team,int pb);
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

void tick();
#endif