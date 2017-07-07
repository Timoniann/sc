#ifndef BASIC_H_INCLUDED
#define BASIC_H_INCLUDED

#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <windows.h>

using namespace std;

static char * spaces = " \t\n";
static vector<string> statics;
static vector<string> opers;

enum ConsoleColor
{
    Black         = 0,
    Blue          = 1,
    Green         = 2,
    Cyan          = 3,
    Red           = 4,
    Magenta       = 5,
    Brown         = 6,
    LightGray     = 7,
    DarkGray      = 8,
    LightBlue     = 9,
    LightGreen    = 10,
    LightCyan     = 11,
    LightRed      = 12,
    LightMagenta  = 13,
    Yellow        = 14,
    White         = 15
};

enum MESSAGE_TYPE
{
    MessageError = Red,
    MessageWarning = Yellow,
    MessagePlain = Black,
    MessageEasyError = Cyan
};

void SetColor(int text, ConsoleColor background);

void Log(string text, MESSAGE_TYPE msg_type = MessagePlain, int error_code = -1);

bool delim (char c);

bool In(char value, char * values);

bool In(string & value, vector<string> & values);

string GetWord(string & data, unsigned int & i);

void Init();

vector<string> getStatics();

vector<string> getOperators();

int priority (string op);

string readNumber(string & str, unsigned int & iter);
string readWord(string & str, unsigned int & iter);
string readString(string & data, unsigned int & iter);
string readOperator(string & data, unsigned int & iter);

#endif // BASIC_H_INCLUDED
