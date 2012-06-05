#ifndef MAINH
#define MAINH

#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include <vector>
#include <stack>
using namespace std;
struct client{
	SOCKET s;
	bool destroyed;
}; 
DWORD WINAPI Threading(LPVOID text);
void handle_text(const char *text);

#endif