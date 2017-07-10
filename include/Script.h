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

        Script(){};
        Script(Script * parent){ this->parent = parent; };
        Script(Script * parent, string type, string value);
        Script(Script* (* func)(Script * self, Script * params));

        void Handler(vector<string> & strs, unsigned int & i, unsigned int & j);

        Script * Execute(Script * parameters);
        Script * Execute(string param);
        string GetValue();

        void SetConstructor(Script* (* func)(Script * self, Script * params));
        void AddVar(string name, Script * value);
        void SetValue(string val);

        string GetType();
        string StackVariables();

        Script * GetVariable(string val);

        static void copy(Script * s1, Script * s2);

        string type;
        string value;
    protected:

        Script * (* constructor) (Script * self, Script * params) = nullptr;
    private:
        vector<vector<tuple<unsigned int, string, unsigned int>>> cmds;
        //Dictionary<string, Script> operators;
        //make_tuple(COMMAND_ID, "variable", "values");
        vector<string> params;
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
