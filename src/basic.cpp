#include <basic.h>

void SetColor(int text, ConsoleColor background)
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}

void Log(string text, MESSAGE_TYPE msg_type, unsigned int line, int error_code)
{
    if(msg_type == MessagePlain)
        SetColor(msg_type, White);
    else SetColor(msg_type, LightBlue);
    cout << text;
    if(line > 0) cout << " in line " << line + 1;
    cout << "\n";
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
        op == "," ? 1 :
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
    while(iter < data.size())
    {
        iter++;
        if (data[iter] == parser){ return str; }
        if (data[iter] == '\\')
        {
            char c = data[++iter];
            switch(c)
            {
            case '\'': str += 0x27; break;
            case '"': str += 0x22; break;
            case '?': str += 0x3f; break;
            case '\\': str += 0x5c; break;
            case '0': str += (char)0x00; break;
            case 'a': str += 0x07; break;
            case 'b': str += 0x08; break;
            case 'f': str += 0x0c; break;
            case 'n': str += 0x0a; break;
            case 'r': str += 0x0d; break;
            case 't': str += 0x09; break;
            case 'v': str += 0x0b; break;
            default: str += c; break;
            }
            //iter++;
            //continue;
        }
        else str += data[iter];
            //{ str += data[iter++]; str += data[iter++]; continue; }
    }
    Log((string)"Syntax GetString error. Expected '" + parser + "', but given '\\n'", MessageError);
    cout << "Str: " << str << "\n";
    return str;
}

char * impOps = "()\"\\";

string readOperator(string & data, unsigned int & iter)
{
    string result = "";
    result += data[iter++];
    while(iter < data.size() && !delim(data[iter]) && !isalnum(data[iter]))
    {
        char c = data[iter];
        if(In(c, impOps)) break;
        result += c;
        iter++;
    }
    iter--;
    return result;
}
