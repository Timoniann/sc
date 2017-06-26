#ifndef BASIC_H_INCLUDED
#define BASIC_H_INCLUDED

#include <string>
#include <cstring>
#include <vector>
#include <iostream>

using namespace std;

static char * spaces = " \t\n";
static vector<string> statics;
static vector<string> opers;


void Log(string text, int error_code = -1);

bool delim (char c);

bool In(char value, char * values);

bool In(string & value, vector<string> & values);

string GetWord(string & data, int & i);

void Init();

vector<string> getStatics();

vector<string> getOperators();

int priority (string op);

#endif // BASIC_H_INCLUDED
