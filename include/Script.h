#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>
#include <Dictionary.h>
#include <vector>
#include <tuple>
#include <iostream>

using namespace std;

class Array;

class Script
{
    friend class Array;
    public:
        Script();
        Script(vector<string> & strs);
        Script(string type, string value);
        Script(Script * parent, string type, string value);
        Script(Script * parent);



        virtual Script * Execute(vector<Script*> & parameters);
        virtual Script * Execute(Script & parameter);
        virtual Script * Execute(Array & params){};
        virtual string GetValue();

        void AddVar(string name, Script * value);
        void SetValue(string val);

        string GetType();
        string StackVariables();

        Script * GetVariable(string val);


    protected:
        Script * parent = nullptr;
        Dictionary<string, Script*> operators;
        Dictionary<string, Script*> vars;
        string type;
    private:
        vector<vector<tuple<unsigned int, string, int>>> cmds;
        //Dictionary<string, Script> operators;
        //make_tuple(COMMAND_ID, "variable", "values");
        Dictionary<string, Script*> funcs;
        vector<string> params;
        string value;
        static void process_op (vector<Script*> & st, string op);
};

class Int : public Script
{
    public:
        Int(string val);
        ~Int();
        Script * Execute(Script & parent);
    protected:
    private:
};

class Bool : public Script
{
    public:
        Bool(string val);
        ~Bool();
        Script * Execute(Script & parent);
    protected:

    private:
};

class Array : public Script
{
    public:
        Array(Script * p1, Script * p2);
        Array();
        ~Array();
        Script * Execute(Script & parameter);
        static string TypeName;

        string GetValue() override;
    protected:
    private:
};

#endif // SCRIPT_H
