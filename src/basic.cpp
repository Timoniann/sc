#include <basic.h>

void SetColor(int text, ConsoleColor background)
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}

void Log(string text, MESSAGE_TYPE msg_type, int error_code)
{
    SetColor(msg_type, White);
    cout << text << "\n";
    SetColor(MessagePlain, White);
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
    Log("Syntax error in GetWord function", MessageError);
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
    opers.push_back("(");
    opers.push_back(")");
}

int priority (string op) {
    return
        //op == "(" || ")" ? 5 :
        op == "," ? -2 :
        op == "==" || op == "<" || op == ">" || op == "!=" ? 2 :
        op == "+" || op == "-" ? 3 :
        op == "*" || op == "/" || op == "%" ? 4 :
        -1;
}


string readNumber(string & str, unsigned int & iter)
{
    string result = "";
    while(iter < str.size())
        if(isdigit(str[iter])) result += str[iter++];
        else { iter--; return result; }
    iter--;
    return result;
}

string readWord(string & str, unsigned int & iter)
{
    string result = "";
    while(isalnum(str[iter]) && iter < str.size()){
        result += str[iter];
        iter++;
    }
    iter--;
    return result;
}

string readString(string & data, unsigned int & iter)
{
    if(!In(data[iter], "'\"")) { Log((string)"GET STRING NOT STRING!!!", MessageError); return ""; }
    char parser = data[iter];
    string str;
    iter++;
    while(iter < data.size())
    {
        str += data[iter++];
        if (data[iter] == '\\') { str += data[iter++]; str += data[iter++]; continue; }
        if (data[iter] == parser){ return str; }
    }
    Log((string)"Syntax GetString error. Expected '" + parser + "', but given '\\n'", MessageError);
    cout << "Str: " << str << "\n";
    return str;
}
