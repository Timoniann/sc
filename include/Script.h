#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <iostream>

using namespace std;

class Script
{
    public:
        /** Default constructor */
        Script();
        Script(vector<string> & strs);
        Script(string type, string value);
        Script(Script * parent, string type, string value);
        Script(Script * parent);

        void AddVar(string name, Script * value);
        string StackVariables();
        virtual Script * Execute(vector<Script*> & parameters);
        virtual Script * Execute(Script & parameter);
        static void process_op (vector<Script*> & st, string op);
        virtual string GetValue();
        void SetValue(string val);
        string GetType();
        Script * GetVariable(string val);
    protected:
        Script * parent = nullptr;
        map<string, Script*> operators;
        string type;
    private:
        vector<vector<tuple<unsigned int, string, int>>> cmds;
        //map<string, Script> operators;
        //make_tuple(COMMAND_ID, "variable", "values");
        map<string, Script*> vars;
        vector<string> params;
        map<string, Script*> funcs;
        string value;
};

#endif // SCRIPT_H
