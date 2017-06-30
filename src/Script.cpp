#include "Script.h"
#include <basic.h>
#include <algorithm>

#define ORDER_CREATE 1
#define ORDER_CALL 2
#define ORDER_PUSH_VAL 3
#define ORDER_PUSH_OP 4
#define ORDER_PUSH_PARAM 5

Script * _global;
/// SCRIPT -----------------------------------

Script::Script()
{

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

Script::Script(string type, string value)
{
    this->type = type;
    this->value = value;
}

Script * Script::Execute(vector<Script*> & parameters)
{
    if(constructor != nullptr)
        return constructor(this, parameters.size() == 0 ? new Script("null", "") : parameters[0]);
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
                    if(isdigit(val[0])) { cout << val << " - digin\n"; st.push_back(_global->funcs["int"]->Execute(val)); }
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

Script * Script::Execute(string param)
{
    Script * p = new Script("string", param);
    return Execute(*p);
}

void Script::SetConstructor(Script* (* func)(Script * self, Script * params))
{
    constructor = func;
}

void Script::AddVar(string name, Script * value)
{
    //vars.add(value);
    vars[name] = value;
}

Hand(string s){}

string Script::StackVariables()
{
    string result;

    result = result + "Count: " + to_string(vars.size()) + "\n";

    for(pair<string, Script*> p : vars)
        if(p.second != nullptr)
        result += p.first + " = " + p.second->GetValue() + " (" + p.second->GetType() + ")\n";

    return result;
}

void Script::process_op (vector<Script *> & st, string op)
{
    Script * r = st.back();  st.pop_back();
    Script * l = st.back();  st.pop_back();

    auto operators = GetOperator();

    cout << l->GetValue() << "(" + l->GetType() + ")" << ", " << r->GetValue() << "(" + r->GetType() + ")" << "\n";
    if(operators.count(op) > 0)
    {
        typedef vector<tuple<string, string, Operator*>> mass;
        mass & t = operators[op];
        for(mass::iterator it = t.begin(); it != t.end(); it++)
        {
            if(get<0>(*it) == l->GetType() && get<1>(*it) == r->GetType())
                { st.push_back(get<2>(*it)->Execute(l, r)); return; }
        }

        for(mass::iterator it = t.begin(); it != t.end(); it++)
        {
            if(get<0>(*it) == l->GetType() && get<1>(*it) == "null")
                { st.push_back(get<2>(*it)->Execute(l, r)); return; }
        }

        for(mass::iterator it = t.begin(); it != t.end(); it++)
        {
            if(get<0>(*it) == "null" && get<1>(*it) == "null")
                { st.push_back(get<2>(*it)->Execute(l, r)); return; }
        }
    }
    else cout << "Not have operator '" << op << "' (" << operators.size() << " - size)\n";

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

void Script::copy(Script * s1, Script * s2)
{
    s1->SetValue(s2->GetValue());
    s1->type = s2->type;
    s1->vars = s2->vars;
}

/// ----------------------SCRIPT
/// OPERATOR --------------------

Operator::Operator(Script* (* func)(Script * p1, Script * p2))
{
    this->func = func;
}

Script * Operator::Execute(Script * p1, Script * p2)
{
    return func(p1, p2);
}

//Dictionary<string, Script*> Script::operators;

Script * Plus(Script * p1, Script * p2)
{
    return _global->funcs["int"]->Execute(to_string(stoi(p1->GetValue()) + stoi(p2->GetValue())));
}

Script * Assign(Script * p1, Script * p2)
{
    //delete *p1;
    Script::copy(p1, p2);
    return p1;
}

Script * Multi(Script * p1, Script * p2)
{
    return _global->funcs["int"]->Execute(to_string(stoi(p1->GetValue()) * stoi(p2->GetValue())));
}

Script * Comma(Script * p1, Script * p2)
{
    Script * result = _global->funcs["array"]->Execute("Array");
    result->vars.add(p1);
    result->vars.add(p2);
    return result;
}

Script * ArrayPlus(Script * p1, Script * p2)
{
    cout << "ARRAY + \n";
    p1->AddVar("23", p2);
    return p1;
}

Script * IntConstructor(Script * self, Script * params)
{
    if(params->GetValue() == "") {
        if(params->vars.size() != 0)
        {
            cout << params->vars.size() << " - size "; self->SetValue(params->vars[0]->GetValue());
            return self;
        }
        else
        {
            self->SetValue("0");
            return self;
        }
    }
    self->SetValue(params->GetValue());
    cout << "Int constructor\n";
    return self;
}

Script * ArrayConstructor(Script * self, Script * params)
{
    self->SetValue(params->GetValue());
    cout << "Array constructor\n";
    return self;
}

Script * SizeConstructor(Script * self, Script * params)
{
    return new Script("int", to_string(self->parent->vars.size()));
}


string GetValue(Script * self)
{
    string result = "Array(";
    for(int i = 0; i < self->vars.size(); i++)
        result += self->vars[i]->GetValue() + ", ";
    if(result.size() > 0) { result.pop_back(); result.pop_back(); }
    result += ")";
    return result;
}

void InitOperators()
{
    operators["+"].push_back(make_tuple("int", "int", new Operator(Plus)));
    operators["="].push_back(make_tuple("null", "null", new Operator(Assign)));
    operators["*"].push_back(make_tuple("int", "int", new Operator(Multi)));
    operators[","].push_back(make_tuple("null", "null", new Operator(Comma)));
    operators["+"].push_back(make_tuple("array", "null", new Operator(ArrayPlus)));

}

map<string, vector<tuple<string, string, Operator*>>> GetOperator()
{
    return operators;
}

void Scripting(Script * global)
{

    Script * _int = new Script("int", "0");
        _int->SetConstructor(IntConstructor);
    global->funcs.add("int", _int);
        Script * _array = new Script("array", "[]");
        _array->SetConstructor(ArrayConstructor);
            Script * sizeFunc = new Script(_array);
            sizeFunc->SetConstructor(SizeConstructor);
        _array->funcs.add("size", sizeFunc);
    global->funcs.add("array", _array);

    _global = global;

    /// Exec\n
    Script temp(global);
    global->Execute(temp);
}
