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
    for(unsigned int i = 0; i < values.size(); i++)
        if(values[i] == value) return true;
    return false;
}

string GetWord(string & data, unsigned int & i)
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
    opers.push_back(",");
}

int priority (string op) {
    return
        op == "," ? 0 :
        op == "==" || op == "<" || op == ">" || op == "!=" ? 1 :
        op == "+" || op == "-" ? 2 :
        op == "*" || op == "/" || op == "%" ? 3 :
        -1;
}

string GetString(string & data, unsigned int & i)
{
    if(!In(data[i], "'\"")) { Log((string)"GET STRING NOT STRING!!!", 1); return ""; }
    char parser = data[i];
    string str;
    i++;
    while(i < data.size())
    {
        str += data[i];
        i++;
        if (data[i] == '\\') { i+=2; continue; }
        if (data[i] == parser){ i++; return str; }
    }
    Log((string)"Syntax GetString error. Expected '" + parser + "', but given '\n'", 2);
    return str;
}
