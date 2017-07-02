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
    Log("Syntax GetString error", 2);
    return str;
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
    if (In(data[iter], (char*)"'\"")) value = GetString(data, iter);
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
    Log("\nHandle........................................\n");
    string cmd;
    char str[256];

    vector<Script*> p;
    Scripting(&script);
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

int main(int argc, char ** argv)
{
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
