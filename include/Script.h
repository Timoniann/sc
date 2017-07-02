#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>
#include <Dictionary.h>
#include <map>
#include <tuple>
#include <iostream>

using namespace std;

class Script;
class Operator;

static map<string, vector<tuple<string, string, Operator*>>> operators;

class Script
{
    public:

        Dictionary<string, Script*> vars;
        Dictionary<string, Script*> funcs;
        Script * parent = nullptr;

        Script();
        Script(vector<string> & strs);
        Script(string type, string value);
        Script(Script * parent, string type, string value, bool standart = true);
        Script(Script * parent);

        virtual Script * Execute(vector<Script*> & parameters);
        virtual Script * Execute(Script & parameter);
        virtual Script * Execute(string param);
        virtual string GetValue();

        void SetConstructor(Script* (* func)(Script * self, Script * params));
        void AddVar(string name, Script * value);
        void SetValue(string val);

        string GetType();
        string StackVariables();

        Script * GetVariable(string val);

        static void copy(Script * s1, Script * s2);

        string type;
    protected:

        Script * (* constructor) (Script * self, Script * params) = nullptr;
    private:
        vector<vector<tuple<unsigned int, string, int>>> cmds;
        //Dictionary<string, Script> operators;
        //make_tuple(COMMAND_ID, "variable", "values");
        vector<string> params;
        string value;
        static void process_op (vector<Script*> & st, string op);
};

class Operator
{
public:
    Operator(Script* (* func)(Script * p1, Script * p2));
    Script * Execute(Script * p1, Script * p2);

protected:
    Script * (* func) (Script * p1, Script * p2);

};

void InitOperators();

void Scripting(Script * global);

#endif // SCRIPT_H
