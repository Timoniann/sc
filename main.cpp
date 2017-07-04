#include <fstream>
#include <cstring>

#include <Script.h>
#include <basic.h>

void ClearSpaces(string & str, unsigned int position = 0)
{
    while(position < str.size())
    {
        if (str[position] == ' '
         || str[position] == '\t'
         || str[position] == '\n')
            { str.erase(position, 1);}
        else break;
    }
}

string GetDigit(string & data, unsigned int & i)
{
    string result = "";
    bool p = false;
    result += data[i];
    i++;
    for (; i < data.size(); i++)
    {
        if (isdigit(data[i])) result += data[i];
        else if(data[i] == '.' && !p) { result += '.'; p = true; }
        else
        {
            return result;
        }
    }
    return result;
}

void ReadCommand(Script * script, string & data, unsigned int & iter)
{
    cout << "Reading...\n";
    ClearSpaces(data, iter);
    string word = GetWord(data, iter);
    if (In(word, statics));

    ClearSpaces(data, iter);
    string name = GetWord(data, iter);
    ClearSpaces(data, iter);
    if (data[iter] != '=') Log((string)"Expected '=', but given '" + data[iter] + "'", 3);
    iter++;
    ClearSpaces(data, iter);
    ///READING VALUE
    string value;
    if (In(data[iter], (char*)"'\"")) value = readString(data, iter);
    else if (data[iter] == '-' || isdigit(data[iter])) value = GetDigit(data, iter);
    else value = GetWord(data, iter);
    //script->AddVar(name, value);
    ClearSpaces(data, iter);

    if (data[iter] != ';') Log((string)"Expected ';', but given '" + data[iter] + "' (" + to_string(iter) + ")", 4);
    iter++;
    if(iter < data.size()) ReadCommand(script, data, iter);
}

Script * ReadScript(char * file_path)
{
    ifstream file(file_path);
    string str;
    vector<string> strs;
    while(getline(file, str))
    {
        strs.push_back(str);
    }
    Script * script = new Script(strs);
    return script;
}

void ZeroMemory(char * str, int size)
{
    for (int i = 0; i < size; i++)
        str[i] = '\0';
}

void Handler(Script &script)
{
    string cmd;
    char str[256];

    vector<Script*> p;
    Log("\nHandle........................................\n");
    Scripting(&script);
    Log("\nClosed........................................\n");
    //cout << "Exit with status: " << script.Execute(p)->GetValue() << "\n";
    cout << "You can write 'exit' or other script's commands ('all')\n";
    while(true)
    {
        ZeroMemory(str, 256);
        cin.getline(str, 256);
        cmd = str;
        if(cmd == "all") { cout << script.StackVariables(); }

        else if (cmd == "exit") break;
    }
}

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

#include <windows.h>

void SetColor(int text, int background)
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}

void SetColor(int text, ConsoleColor background)
{
   HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
   SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}

int main(int argc, char ** argv)
{
    system("COLOR 00");
    cout << "Hello, gigach\n";
    SetColor(White, Black);
    InitOperators();
    /*
    Dictionary<string, unsigned int> dic("Hello", 1);
    dic.add("Hello2", 2);
    dic.foreachReverse([](string key, unsigned int value){cout << key; cout << value;});
    return 0;*/
    Init();
    if (argc == 1) argv[1] = "bin/Debug/main.sc";
        //Log("Nothing to interpret\n");
    //else
    {
        Script * s = ReadScript(argv[1]);
        Handler(*s);
    }
    cout << "Closed\n";
    return 0;
}
