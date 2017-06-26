#include <basic.h>

void Log(string text, int error_code)
{
    cout << text << "\n";
}

bool delim (char c) {
    return c == ' ' || c == '\t';
}

bool In(char value, char * values)
{
    int size = strlen(values);
    for(int i = 0; i < size; i++)
        if (values[i] == value) return true;
    return false;
}

bool In(string & value, vector<string> & values)
{
    for(int i = 0; i < values.size(); i++)
        if(values[i] == value) return true;
    return false;
}

string GetWord(string & data, int & i)
{
    string word;
    while(i < data.size())
    {
        word += data[i];
        i++;
        if (In(data[i], spaces))
            return word;
    }
    Log("Syntax error", 2);
    return word;
}

vector<string> getStatics(){
    return statics;
}

vector<string> getOperators(){
    return opers;
}

void Init()
{
    statics.push_back("new");
    statics.push_back("push");
    opers.push_back("=");
    opers.push_back("+");
    opers.push_back("-");
    opers.push_back("*");
    opers.push_back("/");
    opers.push_back("%");
    opers.push_back("==");
    opers.push_back("!=");
    opers.push_back(">");
    opers.push_back("<");
}

int priority (string op) {
    return
        op == "==" || op == "<" || op == ">" || op == "!=" ? 1 :
        op == "+" || op == "-" ? 2 :
        op == "*" || op == "/" || op == "%" ? 3 :
        -1;
}
