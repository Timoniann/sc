#include "Script.h"

#include <basic.h>
#include <algorithm>

#define ORDER_CREATE 1
#define ORDER_CALL 2
#define ORDER_PUSH_VAL 3
#define ORDER_PUSH_OP 4
#define ORDER_PUSH_PARAM 5
#define ORDER_PUSH_STR 6
#define ORDER_PUSH_POINTER 8
#define ORDER_PUSH_NUMBER 9
#define ORDER_PUSH_OPEN_BRACKETS 10
#define ORDER_PUSH_START_FUNC_PARAMS 11
#define ORDER_PUSH_CLOSE_BRACKETS 12
#define ORDER_PUSH_FUNC 13
#define ORDER_CALL_FUNC 14

Script * _global;
/// SCRIPT -----------------------------------

Script::Script()
{

}

Script::Script(Script * parent) : Script()
{
    this->parent = parent;
}

Script::Script(Script * parent, string type, string value, bool standart)
{
    this->parent = parent;
    this->type = type;
    this->value = value;
    if (standart) Script();
}

Script::Script(vector<string> & strs, int a) : Script()
{
    bool currentIsOp = false;
    vector<Script *> st;
    vector<tuple<unsigned int, string, unsigned int>> triple;
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
                j--;
                vector<string> stats = getStatics();
                if(In(operand, stats))
                    triple.push_back(make_tuple(ORDER_CREATE, operand, i));
                else if(isdigit(operand[0]))
                    triple.push_back(make_tuple(ORDER_PUSH_VAL, operand, i));
                else {
                    while(In(strs[i][j++], spaces));
                    if(strs[i][j] == '('){
                       triple.push_back(make_tuple(ORDER_PUSH_FUNC, operand, i));
                       j++;
                    }
                    else{
                        j--;
                        triple.push_back(make_tuple(ORDER_PUSH_VAL, operand, i));
                    }
                }

                if(strs[i][j] == ';' && triple.size() > 0){
                    cmds.push_back(triple);
                    triple.clear();
                    continue;
                }
            }
            else {

                if(c == '\'' || c == '"')
                {
                    triple.push_back(make_tuple(ORDER_PUSH_STR, GetString(strs[i], j), i));
                    j--;
                    continue;
                }
                //else if (c == '(' || c == ')')
                    ;//triple.push_back(make_tuple(ORDER_PUSH_, "", i));
                while(j < strs[i].size() && !isalnum(strs[i][j]) && !delim(strs[i][j]) && strs[i][j] != ';')
                    operand += strs[i][j++];
                j--;
                vector<string> opes = getOperators();
                if(In(operand, opes)){
                    triple.push_back(make_tuple(ORDER_PUSH_OP, operand, i));
                }
                else Log("Operator '" + operand + "' not found.\n", 4);
            }
        }
    }
}

Script::Script(string type, string value) : Script()
{
    this->type = type;
    this->value = value;
}


#define NOTHING 0
#define NUMBER 1
#define LETTER 2
#define OPERATOR 3

Script::Script(vector<string> & strs) : Script()
{
    vector<tuple<unsigned int, string, unsigned int>> triple;

    int last = NOTHING;
    string path = "";
    vector<bool> brackets;

    for(unsigned int i = 0; i < strs.size(); i++)
    {
        string & str = strs[i];
        for(unsigned int j = 0; j < str.size(); j++)
        {
            char c = str[j];
            if(delim(c)) continue;
            cout << "'" << c << "' ";
            if(isdigit(c)) triple.push_back(make_tuple(ORDER_PUSH_NUMBER, readNumber(str, j), i)), last = NUMBER;
            else if(isalpha(c)) {
                if(last == LETTER) path += readWord(str, j);
                else path = readWord(str, j);
                last = LETTER;
            }
            else{
                if(c == '.') {
                    if(last == LETTER)
                    {
                        path += c;
                        continue;
                    }
                    else Log((string)"Syntax error wrong using '" + c + "'");
                }
                else if(c == '(')
                {
                    if(last == NUMBER) Log("Syntax error. Operator '(' after number can not be");
                    else if(last == OPERATOR || last == NOTHING)
                    {
                        brackets.push_back(true);
                        triple.push_back(make_tuple(ORDER_PUSH_OP, (string)"" + c, i));
                    }
                    else if(last == LETTER)
                    {
                        cout << "LTR\n";
                        brackets.push_back(false);
                        triple.push_back(make_tuple(ORDER_PUSH_FUNC, path, i));
                        path = "";
                        last = OPERATOR;
                        continue;
                    }
                }
                else if(last == LETTER)
                {
                    triple.push_back(make_tuple(ORDER_PUSH_VAL, path, i));
                }
                if(c == ')')
                {
                    if(brackets.size() == 0)
                    {
                        Log("Syntax error. Wrong using operator ')'");
                        continue;
                    }
                    if(brackets.back()) triple.push_back(make_tuple(ORDER_PUSH_OP, "" + c, i));
                    else triple.push_back(make_tuple(ORDER_CALL_FUNC, (string)"" + ')', i));
                    brackets.pop_back();
                }
                else if(c == ';' && triple.size() > 0)
                {
                    cout << "\n";
                    cmds.push_back(triple);
                    triple.clear();
                    last = NOTHING;
                    continue;
                }
                else if(c == '/' && strs[i][j + 1] == '/')
                {
                    if(triple.size() > 0) Log("Unexpected '//' in line " + to_string(i)), last = NOTHING;
                    cout << "Comments\n";
                    break;
                }
                else if(c == '{') Log((string)"Unhandled operator '" + c + "'");
                else if(c == '}') Log((string)"Unhandled operator '" + c + "'");
                else if(c == '[') Log((string)"Unhandled operator '" + c + "'");
                else if(c == ']') Log((string)"Unhandled operator '" + c + "'");
                else { triple.push_back(make_tuple(ORDER_PUSH_OP, (string)"" + c, i)); }
                last = OPERATOR;

            }

        }
    }
}

Script * Script::Execute(vector<Script*> & parameters)
{
    if(constructor != nullptr)
        return constructor(this, parameters.size() == 0 ? new Script("null", "") : parameters[0]);

    cout << "Really\n";
    cout << cmds.size() << " - size\n";
    for(unsigned int i = 0; i < cmds.size(); i++){
        auto cmd = cmds[i];
        for(unsigned int j = 0; j < cmd.size(); j++)
            cout << get<0>(cmd[j]) <<  ".\"" << get<1>(cmd[j]) << "\", ";
        cout << "\n";
    }
    cout << "\nClose view\n";

    for(unsigned int i = 0; i < cmds.size(); i++)
    {
        vector<tuple<unsigned int, string, unsigned int>> cmd = cmds[i];
        vector<Script *> st;
        vector<string> op;
        vector<string> pointers;
        string newValue = "";
        vector<string> funcsToCall;
        for(unsigned int j = 0; j < cmd.size(); j++)
        {
            auto t = cmd[j];
            switch(std::get<0>(t))
            {
            case ORDER_PUSH_NUMBER:
                st.push_back(_global->funcs["int"]->Execute(get<1>(t)));
                break;
            case ORDER_PUSH_FUNC:
                funcsToCall.push_back(get<1>(t));
                break;
            case ORDER_CALL_FUNC:
                if(funcsToCall.size() == 0)
                {
                    Log("Can't call function " + get<1>(t) + " in line " + to_string(get<2>(t)));
                    break;
                }
                st.push_back(funcs[funcsToCall.back()]->Execute(get<1>(t)));
                funcsToCall.pop_back();
                break;
            case ORDER_CREATE:
                newValue = get<1>(t);
                //vars[get<1>(t)] = get<2>(t);
                break;
            case ORDER_CALL:
                //funcs[get<1>(t)]->Execute(get<2>(t));
                break;
            case ORDER_PUSH_VAL:
            {
                st.push_back(GetVariable(get<1>(t)));
                break;
            }
            case ORDER_PUSH_OP:
                {
                    string oper = get<1>(t);
                    cout << "Op: " << oper << '\n';
                    if(oper == "(") { op.push_back (oper); }
                    else if (oper == ")") {
                        while (op.back() != "("){
                            process_op (st, op.back());
                            op.pop_back();
                        }
                        op.pop_back();
                    }
                    else {
                        while (!op.empty() && priority(op.back()) >= priority(oper)){
                            cout << op.back();
                            process_op(st, op.back()), op.pop_back();
                        }
                        op.push_back (oper);
                    }
                }
                break;
            case ORDER_PUSH_POINTER:
                pointers.push_back(get<1>(t));
                break;
            case ORDER_PUSH_PARAM:
                break;
            case ORDER_PUSH_STR:
                st.push_back(_global->funcs["string"]->Execute(get<1>(t)));
                break;
            default:
                cout << "Invalid order(" << get<0>(t) << ")\n";
            }
        }
        while (!op.empty())
            process_op (st, op.back()),  op.pop_back();
        if(newValue != "" && st.size() > 0)
            AddVar(newValue, st.back());
    }
    return this;
}

Script * Script::Execute(vector<Script*> & parameters, int a)
{
    if(constructor != nullptr)
        return constructor(this, parameters.size() == 0 ? new Script("null", "") : parameters[0]);

    for(unsigned int i = 0; i < cmds.size(); i++)
    {
        vector<tuple<unsigned int, string, unsigned int>> cmd = cmds[i];
        vector<Script *> st;
        vector<string> op;
        vector<string> pointers;
        string newValue = "";
        for(unsigned int j = 0; j < cmd.size(); j++)
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
                if(isdigit(val[0])) st.push_back(_global->funcs["int"]->Execute(val));
                else st.push_back(GetVariable(val));
            }
            break;
            case ORDER_PUSH_OP:
                {
                    string oper = get<1>(t);
                    if(oper == "(") { op.push_back (oper); }
                    else if (oper == ")") {
                        while (op.back() != "("){
                            process_op (st, op.back());
                            op.pop_back();
                        }
                        op.pop_back();
                    }
                    else {
                        while (!op.empty() && priority(op.back()) >= priority(oper)){
                            cout << op.back();
                            process_op(st, op.back()), op.pop_back();
                        }
                        op.push_back (oper);
                    }
                }
                break;
            case ORDER_PUSH_POINTER:
                pointers.push_back(get<1>(t));
                break;
            case ORDER_PUSH_PARAM:
                break;
            case ORDER_PUSH_STR:
                st.push_back(_global->funcs["string"]->Execute(get<1>(t)));
                break;
            case ORDER_PUSH_FUNC:
                if(funcs.count(get<1>(t)))
                    funcs[get<1>(t)]->Execute("");
                else Log("This object not have function '" + get<1>(t) + "'");
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

string Script::StackVariables()
{
    string result;

    result = result + "Count: " + to_string(vars.size()) + "\n";

    for(pair<string, Script*> p : vars)
        if(p.second != nullptr){
                Script * toS = p.second->funcs["ToString"];
            if(toS != nullptr)
                result += p.first + " = " + toS->Execute(*p.second)->GetValue() + " (" + p.second->GetType() + ")\n";
                else Log("ToString() is not defined in type '" + p.second->GetType() + "'");
        }

    return result;
}

void Script::process_op (vector<Script *> & st, string op)
{

    Script * r = st.back();  st.pop_back();
    Script * l = st.back();  st.pop_back();

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
    else cout << "Not have operator '" << op << "'\n";

    Log("Operator '" + op + "' not declared in type '" + l->GetType() + "'", 5);
    st.push_back(new Script(nullptr, "null", ""));

}

Script * Script::GetVariable(string val)
{
    if(vars.count(val)) return vars[val];
    AddVar(val, new Script(this, "null", ""));
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
    s1->funcs = s2->funcs;
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

Script * Minus(Script * p1, Script * p2)
{
    return _global->funcs["int"]->Execute(to_string(stoi(p1->GetValue()) - stoi(p2->GetValue())));
}

Script * Divide(Script * p1, Script * p2)
{
    return _global->funcs["int"]->Execute(to_string(stoi(p1->GetValue()) / stoi(p2->GetValue())));
}

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
    Script * result = _global->funcs["array"]->Execute("Array throw comma");
    result->vars.add((Script *)p1);
    result->vars.add((Script *)p2);
    return result;
}

Script * ArrayPlus(Script * p1, Script * p2)
{
    Script * result = new Script();
    *result = *p1;
    result->vars.add(p2);
    return result;
}


/// Constructors


Script * IntConstructor(Script * self, Script * params)
{
    try{
        Script * newInt = new Script();
        Script::copy(newInt, self);
        newInt->SetValue(to_string(stoi(params->GetValue())));
        return newInt;
    } catch(exception e)
    {
        Log("Can't parsing to int(" + params->GetValue() + ")");
        return new Script("int", "0");
    }
}

Script * StringConstructor(Script * self, Script * params)
{
    Script * newStr = new Script();
    Script::copy(newStr, self);
    newStr->SetValue(params->GetValue());
    return newStr;
}

Script * CountC(Script * self, Script * params)
{
    cout << "Calling function of count\n";
    return new Script("null", "null");
}

Script * ArrayConstructor(Script * self, Script * params)
{
    Script * newArray = new Script();
    Script::copy(newArray, self);
    newArray->vars = params->vars;
    return newArray;
}

Script * SizeConstructor(Script * self, Script * params)
{
    return new Script("int", to_string(self->parent->vars.size()));
}

/// ~Constructors

string GetValue(Script * self)
{
    string result = "Array(";
    for(unsigned int i = 0; i < self->vars.size(); i++)
        result += self->vars[i]->GetValue() + ", ";
    if(result.size() > 0) { result.pop_back(); result.pop_back(); }
    result += ")";
    return result;
}

void InitOperators()
{
    operators["="].push_back(make_tuple("null", "null", new Operator(Assign)));
    operators[","].push_back(make_tuple("null", "null", new Operator(Comma)));

    operators["+"].push_back(make_tuple("int", "int", new Operator(Plus)));
    operators["*"].push_back(make_tuple("int", "int", new Operator(Multi)));
    operators["-"].push_back(make_tuple("int", "int", new Operator(Minus)));
    operators["/"].push_back(make_tuple("int", "int", new Operator(Divide)));

    operators["+"].push_back(make_tuple("array", "null", new Operator(ArrayPlus)));

}

Script * IntegerToString(Script * self, Script * params)
{
    return _global->funcs["string"]->Execute(*params);
}

Script * ArrayToString(Script * self, Script * params)
{
    Script * str = _global->funcs["string"]->Execute(*self);
    string val = "[";
    function<void(string, Script *)>f = [&val](string key, Script * value)
    {
        val += value->funcs["ToString"]->Execute(*value)->GetValue() + ", ";
    };
    params->vars.foreach(f);
    if (params->vars.size()) val.pop_back(), val.pop_back();
    val += ']';
    str->SetValue(val);
    return str;
}

void Scripting(Script * global)
{

    Script * _int = new Script("int", "0");
        _int->SetConstructor(IntConstructor);
            Script * IntToString = new Script(_int);
            IntToString->SetConstructor(IntegerToString);
        _int->funcs["ToString"] = IntToString;
    global->funcs.add("int", _int);

    Script * _array = new Script("array", "[]");
        _array->SetConstructor(ArrayConstructor);
            Script * sizeFunc = new Script(_array);
            sizeFunc->SetConstructor(SizeConstructor);
        _array->funcs.add("size", sizeFunc);
            Script * arrToString = new Script(_array);
            arrToString->SetConstructor(ArrayToString);
        _array->funcs.add("ToString", arrToString);
    global->funcs.add("array", _array);

    Script * _string = new Script("string", "");
        _string->SetConstructor(StringConstructor);
        _string->funcs.add("ToString", IntToString);
    global->funcs.add("string", _string);

        Script * _count = new Script("count", "CountString");
        _count->SetConstructor(CountC);
    global->funcs.add("count", _count);


    _global = global;

    /// Exec\n
    Script temp(global);
    global->Execute(temp);
}
