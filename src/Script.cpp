#include "Script.h"
#include <basic.h>
#include <algorithm>
#include <Int.h>
#include <Bool.h>
#include <Array.h>

#define ORDER_CREATE 1
#define ORDER_CALL 2
#define ORDER_PUSH_VAL 3
#define ORDER_PUSH_OP 4
#define ORDER_PUSH_PARAM 5

class OpEqual : public Script // a = 2
{
public:
    OpEqual(Script * parent) : Script(parent) {

    }
    Script * Execute(Script & parameter)
    {
        if(parameter.GetType() == Array::TypeName){
            cout << parameter.GetValue() << " - EQUAL WITH ARRAY\n";
        }
        //delete parent;

        *parent = parameter;
        //parent = &parameter;
        if(parameter.GetType() == Array::TypeName){
            cout << "Here\n";
            cout << parent->GetValue();
        }
        return parent;
    }
};

class OpComma : public Script // a , 2
{
public:
    OpComma (Script * parent) : Script(parent) {

    }
    Script * Execute(Script & parameter)
    {
        cout << "Operator comma " << parent->GetType() << " with " << parameter.GetType() << "\n";

        if(parent->GetType() == Array::TypeName) parent->AddVar("Temp", &parameter);
        else
        {
            Script * result = new Array(parent, &parameter);
            cout << result->GetValue() << " ------------------------ value\n";
            return result;
        }
        return parent;
    }
};



Script::Script()
{
    operators["="] = new OpEqual(this);
    operators[","] = new OpComma(this);
}

Script::Script(Script * parent)
{
    this->parent = parent;
}

Script::Script(Script * parent, string type, string value) : Script()
{
    this->parent = parent;
    this->type = type;
    this->value = value;
}

Script::Script(vector<string> & strs)
{
    vector<Script *> st;
    vector<tuple<unsigned int, string, int>> triple;
    for(unsigned int i = 0; i < strs.size(); i++)
    {
        for(unsigned int j = 0; j < strs[i].size(); j++)
        {
            char c = strs[i][j];
            if(delim(c)) continue;
            if(c == ';') if(triple.size() > 0){
                cmds.push_back(triple);
                triple.clear();
                continue;
            }
            string operand;
            if(isalnum(c)){
                while(j < strs[i].size() && isalnum(strs[i][j]))
                    operand += strs[i][j++];
                vector<string> stats = getStatics();
                if(In(operand, stats))
                    triple.push_back(make_tuple(ORDER_CREATE, operand, i));
                else if(isdigit(operand[0]))
                    triple.push_back(make_tuple(ORDER_PUSH_VAL, operand, i));
                else if(strs[i][j] == '.' || strs[i][j] == '('){
                        vector<Script*> ps;
                        while(strs[i][j] != ')')
                        {
                            j++;
                        }
                    }
                else
                    triple.push_back(make_tuple(ORDER_PUSH_VAL, operand, i));
                if(strs[i][j] == ';') if(triple.size() > 0){
                    cmds.push_back(triple);
                    triple.clear();
                    continue;
                }
            }
            else {
                while(j < strs[i].size() && !isalnum(strs[i][j]) && !delim(strs[i][j]))
                    operand += strs[i][j++];
                vector<string> opes = getOperators();
                if(In(operand, opes)){
                    triple.push_back(make_tuple(ORDER_PUSH_OP, operand, i));
                }
                else Log("Operator " + operand + " not found.\n", 4);
            }
        }
    }
}


Script * Script::Execute(vector<Script*> & parameters)
{
    for(int i = 0; i < cmds.size(); i++)
    {
        vector<tuple<unsigned int, string, int>> cmd = cmds[i];
        vector<Script *> st;
        vector<string> op;
        string newValue = "";
        for(int j = 0; j < cmd.size(); j++)
        {
            auto t = cmd[j];
            switch(std::get<0>(t))
            {
            case ORDER_CREATE:
                newValue = get<1>(t);
                //vars[get<1>(t)] = get<2>(t);
                break;
            case ORDER_CALL:
                //funcs[get<1>(t)]->Execute(get<2>(t));
                break;
            case ORDER_PUSH_VAL:
                {
                    string val = get<1>(t);
                    if(isdigit(val[0])) st.push_back(new Int(val));
                    else st.push_back(GetVariable(val));
                }
            break;
            case ORDER_PUSH_OP:
                {
                    string oper = get<1>(t);
                    while (!op.empty() && priority(op.back()) >= priority(oper))
                        process_op(st,
                                   op.back()),
                                   op.pop_back();
                    op.push_back (oper);
                }
                break;
            case ORDER_PUSH_PARAM:
                break;
            default:
                cout << "Invalid order\n";
            }
        }
        while (!op.empty())
            process_op (st, op.back()),  op.pop_back();
        if(newValue != "" && st.size() > 0)
            AddVar(newValue, st.back());
    }
    return this;
}

Script * Script::Execute(Script & parameter)
{
    vector<Script*> p;
    p.push_back(&parameter);
    return Execute(p);
}


void Script::AddVar(string name, Script * value)
{
    vars[name] = value;
}

Hand(string s){}

string Script::StackVariables()
{
    string result;

    result = result + "Count: " + to_string(vars.size()) + "\n";

    for(pair<string, Script*> p : vars)
        if(p.second != nullptr)
        result += p.first + " = " + p.second->value + " (" + p.second->GetType() + ")\n";

    return result;
}

void Script::process_op (vector<Script *> & st, string op)
{
    Script * r = st.back();  st.pop_back();
    Script * l = st.back();  st.pop_back();

    if(l->operators.count(op) > 0)
    {
        Script * result = (l->operators[op]->Execute(*r));
        if(op == ",") cout << result->GetValue() << " - value of result(must be 'Array(...)')!\n";
        st.push_back(result);
        return;
    }
    Log("Operator '" + op + "' not declared in type '" + l->GetType() + "'", 5);
    st.push_back(new Script(nullptr, "null", ""));

}

Script * Script::GetVariable(string val)
{
    if(vars.count(val)) return vars[val];
    AddVar(val, new Script(this, "null", ""));
    //cout << "New variable\n";
    return vars[val];
}


string Script::GetValue()
{
    return value;
}

void Script::SetValue(string val)
{
    this->value = val;
}

string Script::GetType()
{
    return type;
}

//Dictionary<string, Script*> Script::operators;
