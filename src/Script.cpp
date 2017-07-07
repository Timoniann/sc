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
#define ORDER_CALL_FUNC_WITHOUT_PARAMS 15

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

Script::Script(Script* (* func)(Script * self, Script * params))
{
    constructor = func;
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
#define STRING 4
#define OPEN_FUNC 5

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
                    else Log((string)"Syntax error wrong using '" + c + "'", MessageError);
                }
                else if(c == '(')
                {
                    if(last == NUMBER) Log("Syntax error. Operator '(' after number can not be", MessageError);
                    else if(last == OPERATOR || last == NOTHING)
                    {
                        brackets.push_back(true);
                        triple.push_back(make_tuple(ORDER_PUSH_OP, (string)"" + c, i));
                    }
                    else if(last == LETTER && path != "")
                    {
                        cout << " Func ";
                        brackets.push_back(false);
                        triple.push_back(make_tuple(ORDER_PUSH_FUNC, path, i));
                        path = "";
                        last = OPEN_FUNC;
                        continue;
                    }
                    else Log("Syntax error. Operator '(' after string can't be used", MessageEasyError);
                }
                else if(last == LETTER && path != "")
                {
                    triple.push_back(make_tuple(ORDER_PUSH_VAL, path, i));
                }
                if(c == ')')
                {
                    if(brackets.size() == 0)
                    {
                        Log("Syntax error. Wrong using operator ')'", MessageEasyError);
                        continue;
                    }
                    if(brackets.back()) triple.push_back(make_tuple(ORDER_PUSH_OP, "" + c, i));
                    else
                        if(last == OPEN_FUNC) triple.push_back(make_tuple(ORDER_CALL_FUNC_WITHOUT_PARAMS, (string)"" + c, i));
                        else { triple.push_back(make_tuple(ORDER_CALL_FUNC, (string)"" + c, i)); last = LETTER; brackets.pop_back(); continue; }
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
                    if(triple.size() > 0) Log("Unexpected '//' in line " + to_string(i), MessageWarning); //last = NOTHING;
                    cout << "Comments\n";
                    break;
                }
                else if(c == '"') {
                    string s = readString(str, j);
                    triple.push_back(make_tuple(ORDER_PUSH_STR, s, i));
                    last = STRING;
                    cout << "<String: '" << s << "'> ";
                    continue;
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

    cout << "Start view\n";
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
                op.push_back("//");
                break;
            case ORDER_CALL_FUNC:
                {
                    if(funcsToCall.size() == 0)
                    {
                        Log("Can't call function " + get<1>(t) + " in line " + to_string(get<2>(t)));
                        break;
                    }
                    Script * prms = nullptr;
                    if(op.back() == "//") { // 1 param
                        cout << "Calling function with parameters\n";
                        while (op.back() != "//"){
                            if(op.size() == 0) {
                                Log("Syntax error. Cannot call function with parameters in line '" + to_string(i) + "'", MessageError);
                                break;
                            }
                            process_op (st, op.back());
                            op.pop_back();
                        }
                        op.pop_back();
                        prms = st.back();
                        cout << "_+_+_+_+_+_+_" << prms->GetValue() << "_+_+_+_+_+_+_\n";
                        if(prms->GetType() != "array"){
                            Script * arr = _global->funcs["array"]->Execute("Param to array");
                            arr->vars.add(prms);
                            prms = arr;
                        }
                    }
                    else { // more params
                        while (op.back() != "//"){
                        if(op.size() == 0) {
                            Log("Syntax error. Cannot call function with parameters in line '" + to_string(i) + "'", MessageError);
                            break;
                        }
                        process_op (st, op.back());
                        op.pop_back();
                        }
                        op.pop_back();
                        prms = st.back();
                        cout << "(_+_+_+_+_+_+_" << prms->GetValue() << "_+_+_+_+_+_+_)\n";
                        if(prms->GetType() != "array"){
                            Script * arr = _global->funcs["array"]->Execute("Param to array");
                            arr->vars.add(prms);
                            prms = arr;
                        }
                    }
                    if(funcs.count(funcsToCall.back()))
                        st.push_back(funcs[funcsToCall.back()]->Execute(*prms));
                    else Log("Object '" + GetType() + "' does not have function '" + funcsToCall.back() + "'", MessageEasyError);
                    funcsToCall.pop_back();
                }
                break;
            case ORDER_CALL_FUNC_WITHOUT_PARAMS:
                    if(funcsToCall.size() == 0)
                    {
                        Log("Can't call function " + get<1>(t) + " in line " + to_string(get<2>(t)), MessageError);
                        break;
                    }
                    if(op.back() == "//") {
                        cout << "Calling function without parameters\n";
                        if(funcs.count(funcsToCall.back()))
                            st.push_back(funcs[funcsToCall.back()]->Execute(*_global->funcs["array"]->Execute("CFWOP")));
                        else Log("Object '" + GetType() + "' does not have function '" + funcsToCall.back() + "'", MessageEasyError);
                        op.pop_back();
                        funcsToCall.pop_back();
                    }
                    else Log("Wrong calling function. Not find open bracket", MessageError);
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
                        while (op.back() != "(" && op.back() != "//"){
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
        cout << "Other opers\n";
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
    cout << "Stack:\n";
    result = result + "Count: " + to_string(vars.size()) + "\n";

    for(pair<string, Script*> p : vars)
        if(p.second != nullptr){
                Script * toS = p.second->funcs["ToString"];
            if(toS != nullptr)
                result += p.first + " = " + toS->Execute(*p.second)->GetValue() + " (" + p.second->GetType() + ")\n";
            else Log("ToString() is not defined in type '" + p.second->GetType() + "'", MessageEasyError);
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

    Log("Operator '" + op + "' not declared in type '" + l->GetType() + "'", MessageEasyError);
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

Script * StringPlus(Script * p1, Script * p2)
{
    Script * result = new Script();
    *result = *p1;
    result->value += p2->funcs["ToString"]->Execute(*p2)->GetValue();
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
        Log("Can't parsing to int(" + params->GetValue() + ")", MessageEasyError);
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

Script * CoutParams(Script * self, Script * params)
{
    cout << "\n____________________Log____________________\n";
    cout << "Size: " << params->vars.size() << "\n";
    for(int i = 0; i < params->vars.size(); i++)
        cout << "'" << params->vars[i]->funcs["ToString"]->Execute(*params->vars[i])->GetValue() << "' ";
    cout << "\n\\___________________Log___________________/\n";
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

    operators["+"].push_back(make_tuple("string", "null", new Operator(StringPlus)));

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
        if(value->funcs.count("ToString"))
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
        Script * _cout = new Script("cout", "Cout");
        _cout->SetConstructor(CoutParams);
    global->funcs.add("cout", _cout);



    _global = global;

    /// Exec\n
    Script temp(global);
    global->Execute(temp);
}
