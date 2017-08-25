#include <fstream>
#include <cstring>

#include <Script.h>
#include <basic.h>

char * scriptPath;

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
    if (data[iter] != '=') Log((string)"Expected '=', but given '" + data[iter] + "'", MessageError);
    iter++;
    ClearSpaces(data, iter);
    ///READING VALUE
    string value;
    if (In(data[iter], (char*)"'\"")) value = readString(data, iter);
    else if (data[iter] == '-' || isdigit(data[iter])) value = GetDigit(data, iter);
    else value = GetWord(data, iter);
    //script->AddVar(name, value);
    ClearSpaces(data, iter);

    if (data[iter] != ';') Log((string)"Expected ';', but given '" + data[iter] + "' (" + to_string(iter) + ")", MessageError);
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
    unsigned int i = 0;
    unsigned int j = 0;
    Script * script = new Script(nullptr, "Global", "STARTING");
    script->Handler(strs, i, j);
    return script;
}

/*
void ZeroMemory(char * str, int size)
{
    for (int i = 0; i < size; i++)
        str[i] = '\0';
}*/

void Handler(Script * script)
{
    string cmd;
    char str[256];

    vector<Script*> p;
    Log("\nHandle........................................\n");
    Scripting(script);
    Log("\nClosed........................................\n");
    //cout << "Exit with status: " << script.Execute(p)->GetValue() << "\n";
    //cout << "You can write 'exit' or other script's commands ('all')\n";
    while(true)
    {
        cout << "You can write 'exit' or other script's commands ('all')\n";
        ZeroMemory(str, 256);
        cin.getline(str, 256);
        cmd = str;
        if(cmd == "all") { cout << script->StackVariables(); }
        else if(cmd == "get all scripts"){
            vector<Script*> scripts = getAllScripts();
            for(int i = 0; i < scripts.size(); i++){
                if(scripts[i] == nullptr) cout << "Nullptr\n";
                else
                cout << "Type: '"
                << scripts[i]->GetType()
                << "'; Value: '"
                << scripts[i]->GetValue()
                << "'\n";
            }
        }
        else if(cmd == "reload"){
            DeleteAllScripts();
            script = (ReadScript(scriptPath));;
            Scripting(script);
        }
        else if(cmd == "exit") break;
    }
}

int main(int argc, char ** argv)
{
    system("COLOR F0");
    cout << "Hello, gigach\n";
    SetColor(Black, White);

    /*
    Dictionary<string, unsigned int> dic("Hello", 1);
    dic.add("Hello2", 2);
    dic.foreachReverse([](string key, unsigned int value){cout << key; cout << value;});
    return 0;*/
    Init();
    scriptPath = (char*)"bin/Debug/main.sc";
    if (argc > 1) scriptPath = argv[1];

    Handler(ReadScript(scriptPath));

    cout << "Closed\n";
    return 0;
}
