#include <fstream>
#include <cstring>

#include <Script.h>
#include <basic.h>

char * scriptPath;

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

#ifndef ZeroMemory
void ZeroMemory(char * str, int size)
{
    for (int i = 0; i < size; i++)
        str[i] = '\0';
}
#endif // ZeroMemory

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
            InitGlobal();
            script = (ReadScript(scriptPath));;
            Scripting(script);
        }
        else if(cmd == "clear"){
            DeleteAllScripts();
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
    InitGlobal();
    scriptPath = (char*)"bin/Debug/main.sc";
    if (argc > 1) scriptPath = argv[1];

    Handler(ReadScript(scriptPath));

    cout << "Closed\n";
    return 0;
}
