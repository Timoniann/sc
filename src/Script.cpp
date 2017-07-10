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
#define ORDER_PUSH_BOOL 16
#define ORDER_PUSH_TYPE 17

Script * _global;
/// SCRIPT -----------------------------------

Script::Script(Script * parent, string type, string value)
{
    this->parent = parent;
    if(parent == nullptr) this->type = type;
    else this->type = parent->GetType() + '.' + type;
    this->value = value;
}

Script::Script(Script* (* func)(Script * self, Script * params))
{
    constructor = func;
}

#define NOTHING 0
#define NUMBER 1
#define LETTER 2
#define OPERATOR 3
#define STRING 4
#define OPEN_FUNC 5
#define DOT 6 // . - dot
#define MODIFICATOR 7
#define NEW_TYPE 8
#define TYPE_PUSH_PARAMS 9

void Script::Handler(vector<string> & strs, unsigned int & i, unsigned int & j)
{
    vector<tuple<unsigned int, string, unsigned int>> triple;
    string newScriptName = "";
    vector<string> type_params;
    int last = NOTHING;
    string path = "";
    vector<bool> brackets;

    for(; i < strs.size(); i++)
    {
        string & str = strs[i];
        for(; j < str.size(); j++)
        {
            char c = str[j];
            if(delim(c)) continue;
            cout << "'" << c << "' ";
            if(isdigit(c)) {
                if(last == OPERATOR || last == NOTHING || last == OPEN_FUNC) triple.push_back(make_tuple(ORDER_PUSH_NUMBER, readNumber(str, j), i)), last = NUMBER;
                else if(last == NUMBER || last == LETTER || last == STRING) Log((string)"Unexpected '" + c + "'", MessageError, i);
                else Log("Unhandled exception", MessageUnhandled);
            }
            else if(isalpha(c)) {
                if(last == NUMBER || last == STRING || last == LETTER) Log((string)"Unexpected typed '" + readWord(str, j) + "'", MessageError, i);

                else if(last == DOT) path += readWord(str, j);
                else {
                    string word = readWord(str, j);
                    if(word == "type"){
                        if(last == NOTHING){
                            //triple.push_back(make_tuple(ORDER_PUSH_TYPE, "type", i));
                            last = MODIFICATOR;
                            continue;
                        }
                        else Log("Unexpected '" + word + "'", MessageError, i+1);
                    }
                    else if(word == "true" || word == "false") {
                            triple.push_back(make_tuple(ORDER_PUSH_BOOL, word, i));
                            last = NUMBER;
                            continue;
                        }
                    else if(last == MODIFICATOR) {
                            //triple.push_back(make_tuple(ORDER_PUSH_TYPE, word, i));
                            newScriptName = word;
                            last = NEW_TYPE;
                            continue;
                        }
                    else if(last == TYPE_PUSH_PARAMS){
                        type_params.push_back(word);
                        continue;
                    }
                    else path = word;
                }
                last = LETTER;
            }
            else{
                if(c == '.') {
                    if(last == LETTER)
                    {
                        path += c;
                        last = DOT;
                        continue;
                    }
                    else Log((string)"Syntax error wrong using '" + c + "'", MessageError, i);
                }
                else if(c == '(')
                {
                    if(last == NUMBER) Log("Syntax error. Operator '(' after number can not be", MessageError, i);
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
                    else if(last == NEW_TYPE){
                        last = TYPE_PUSH_PARAMS;
                        continue;
                    }
                    else Log("Syntax error. Operator '(' after string can't be used", MessageEasyError, i);
                }
                else if(last == LETTER && path != "")
                {
                    triple.push_back(make_tuple(ORDER_PUSH_VAL, path, i));
                }
                if(c == ')')
                {
                    if(last == TYPE_PUSH_PARAMS)
                    {
                        last = NEW_TYPE;
                        continue;
                    }
                    if(brackets.size() == 0)
                    {
                        Log("Syntax error. Wrong using operator ')'", MessageEasyError, i);
                        continue;
                    }
                    if(brackets.back()) triple.push_back(make_tuple(ORDER_PUSH_OP, (string)"" + c, i));
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
                    if(triple.size() > 0) Log("Unexpected '//' in line " + to_string(i), MessageWarning, i); //last = NOTHING;
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
                else if(c == '{') {
                    /// Creating new object(type);
                    if(last == NEW_TYPE){
                        Log("Creating new object", MessageWarning, i);
                        if(newScriptName == "") {
                            Log("Name of new script is empty", MessageEasyError, i);
                        }
                        Script * newScript = new Script(this, newScriptName, "Compiled new script");
                        j++;
                        //triple.push_back(make_tuple(ORDER_PUSH_TYPE, newScriptName, i));
                        newScript->Handler(strs, i, j);
                        str = strs[i];
                        Log("HANDLED!", MessageWarning, i);
                        funcs.add(newScriptName, newScript);
                        continue;
                    }
                    else Log("Unhandled operator '{'");
                }
                else if(c == '}') {
                    if(parent != nullptr){
                        j++;
                        Log("Close function", MessageWarning, i);
                        return;
                    }
                    else Log("Unexpected '}'. Not have parent", MessageError, i);
                }
                else if(c == '[') Log((string)"Unhandled operator '" + c + "'", MessageWarning);
                else if(c == ']') Log((string)"Unhandled operator '" + c + "'", MessageWarning);
                else { triple.push_back(make_tuple(ORDER_PUSH_OP, readOperator(str, j), i)); }
                last = OPERATOR;

            }
        }
        j = 0;
    }
}

Script * Script::Execute(Script * parameters)
{
    if(constructor != nullptr)
        return constructor(this, parameters);

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
                        Log("Can't call function " + get<1>(t), MessageError, get<2>(t));
                        break;
                    }
                    Script * prms = nullptr;
                    if(op.back() == "//") { // 1 param
                        cout << "Calling function with 1 parameter\n";
                        while (op.back() != "//"){
                            if(op.size() == 0) {
                                Log("Syntax error. Cannot call function with parameters", MessageError, get<2>(t));
                                break;
                            }
                            process_op (st, op.back());
                            op.pop_back();
                        }
                        op.pop_back();
                        prms = st.back();
                        if(prms->GetType() != "array"){
                            Script * arr = _global->funcs["array"]->Execute("Param to array");
                            arr->vars.add(prms);
                            prms = arr;
                        }
                    }
                    else { // more params
                        while (op.back() != "//"){
                            if(op.size() == 0) {
                                Log("Syntax error. Cannot call function with parameters", MessageError, get<2>(t));
                                break;
                            }
                            if(op.back() == "(") { op.pop_back(); continue; }
                            process_op (st, op.back());
                            op.pop_back();
                        }
                        op.pop_back();
                        prms = st.back();
                        if(prms->GetType() != "array"){
                            Script * arr = _global->funcs["array"]->Execute("Param to array");
                            arr->vars.add(prms);
                            prms = arr;
                        }
                    }
                    if(funcs.count(funcsToCall.back()))
                        st.push_back(funcs[funcsToCall.back()]->Execute(prms));
                    else if(vars.count(funcsToCall.back())){
                            cout << "Executing variable\n";
                            st.push_back(vars[funcsToCall.back()]->Execute(prms));
                        }
                        else Log("Object '" + GetType() + "' does not have function '" + funcsToCall.back() + "'", MessageEasyError, get<2>(t));
                    funcsToCall.pop_back();
                }
                break;
            case ORDER_CALL_FUNC_WITHOUT_PARAMS:
                    if(funcsToCall.size() == 0)
                    {
                        Log("Can't call function " + get<1>(t), MessageError, get<2>(t));
                        break;
                    }
                    if(op.back() == "//") {
                        cout << "Calling function without parameters\n";
                        if(funcs.count(funcsToCall.back()))
                            st.push_back(funcs[funcsToCall.back()]->Execute(_global->funcs["array"]->Execute("CFWOP")));
                        else Log("Object '" + GetType() + "' does not have function '" + funcsToCall.back() + "'", MessageEasyError, get<2>(t));
                        op.pop_back();
                        funcsToCall.pop_back();
                    }
                    else Log("Wrong calling function. Not find open bracket", MessageError, get<2>(t));
                break;
            case ORDER_CREATE:
                newValue = get<1>(t);
                //vars[get<1>(t)] = get<2>(t);
                break;
            case ORDER_CALL:
                //funcs[get<1>(t)]->Execute(get<2>(t));
                break;
            case ORDER_PUSH_VAL:
                st.push_back(GetVariable(get<1>(t)));
                break;
            case ORDER_PUSH_BOOL:
                st.push_back(_global->funcs["bool"]->Execute(get<1>(t)));
                break;
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
                        while (!op.empty() && op.back() != "//" && op.back() != "(" && priority(op.back()) >= priority(oper)){
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
                Log("Invalid order (" + to_string(get<0>(t)) + ")", MessageError, get<2>(t));
            }
        }
        cout << "Other opers\n";
        while (!op.empty())
            if(op.back() == "(") op.pop_back();
            else process_op (st, op.back()),  op.pop_back();
        if(newValue != "" && st.size() > 0)
            AddVar(newValue, st.back());
    }
    return this;
}

Script * Script::Execute(string param)
{
    Script * p = new Script(nullptr, "string", param);
    return Execute(p);
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
    //cout << "Stack:\n";
    result = result + "Count: " + to_string(vars.size()) + "\n";

    for(pair<string, Script*> p : vars)
        if(p.second != nullptr){
            Script * toS = p.second->funcs["ToString"];
            if(toS != nullptr)
                result += p.first + " = " + toS->Execute(p.second)->GetValue() + " (" + p.second->GetType() + ")\n";
            else {
                result += p.first + ": {\n" + p.second->StackVariables() + "\n}";
            }
            /*else{
                Script * sc = _global->funcs["array"]->Execute(p.second);
                result += sc->StackVariables();
                delete sc;
            }*/
            //else Log("ToString() is not defined in type '" + p.second->GetType() + "' of variable '" + p.first + "'", MessageEasyError);
        }
        else Log("Nullptr in StackVariables", MessageWarning);

    return result;
}

void Script::process_op (vector<Script *> & st, string op)
{

    /// Handling

    if(st.size() < 2) {
        Log("Compilation problem. Size of stack = " + to_string(st.size()), MessageError);
        return;
    }

    /// -

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

Script * EqualsFull(Script * p1, Script * p2)
{
    return _global->funcs["bool"]->Execute(to_string(p1 == p2));
}

Script * Equals(Script * p1, Script * p2)
{
    return _global->funcs["bool"]->Execute(to_string(p1->GetValue() == p2->GetValue()));
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
    result->value += p2->funcs["ToString"]->Execute(p2)->GetValue();
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
        return new Script(nullptr, "int", "0");
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
    return new Script(nullptr, "null", "null");
}

Script * CoutParams(Script * self, Script * params)
{
    //cout << "Size: " << params->vars.size() << "\n";
    for(int i = 0; i < params->vars.size(); i++)
        cout << params->vars[i]->funcs["ToString"]->Execute(params->vars[i])->GetValue() << "\n";
    return new Script(nullptr, "null", "null");
}

Script * ArrayConstructor(Script * self, Script * params)
{
    Script * newArray = new Script();
    Script::copy(newArray, self);
    newArray->vars = params->vars;
    return newArray;
}

Script * BoolConstructor(Script * self, Script * params)
{
    Script * newBool = new Script();
    Script::copy(newBool, self);
    if(params == nullptr)
        newBool->SetValue("false");
    else if(params->GetType() == "null")
        newBool->SetValue("false");
    else if(params->GetType() == "int")
        if(atoi(params->GetValue().c_str()) == 0) newBool->SetValue("false");
        else newBool->SetValue("true");
    else if(params->GetType() == "bool")
        newBool->SetValue(params->GetValue());
    else if(params->GetType() == "string")
        if(params->GetValue() == "" || params->GetValue() == "0")
        newBool->SetValue("false");
    else newBool->SetValue("true");
    return newBool;
}

Script * SizeConstructor(Script * self, Script * params)
{
    return new Script(nullptr, "int", to_string(self->parent->vars.size()));
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

    operators["==="].push_back(make_tuple("null", "null", new Operator(EqualsFull)));

    Operator * eq = new Operator(Equals);
    operators["=="].push_back(make_tuple("int", "int", eq));
    operators["=="].push_back(make_tuple("string", "string", eq));
    operators["=="].push_back(make_tuple("bool", "bool", eq));
    //operators["=="].push_back(make_tuple("", "", eq));

}

Script * IntegerToString(Script * self, Script * params)
{
    return _global->funcs["string"]->Execute(params);
}

Script * ArrayToString(Script * self, Script * params)
{
    Script * str = _global->funcs["string"]->Execute(self);
    string val = "[";
    function<void(string, Script *)>f = [&val](string key, Script * value)
    {
        if(value->funcs.count("ToString"))
            val += value->funcs["ToString"]->Execute(value)->GetValue() + ", ";
    };
    params->vars.foreach(f);
    if (params->vars.size()) val.pop_back(), val.pop_back();
    val += ']';
    str->SetValue(val);
    return str;
}

Script * GetTicks(Script * self, Script * param)
{
    return _global->funcs["int"]->Execute(to_string(GetTickCount()));
}

void Scripting(Script * global)
{

    Script * _int = new Script(nullptr, "int", "0");
        _int->SetConstructor(IntConstructor);
            Script * IntToString = new Script(_int);
            IntToString->SetConstructor(IntegerToString);
        _int->funcs["ToString"] = IntToString;
    global->funcs.add("int", _int);

    Script * _array = new Script(nullptr, "array", "[]");
        _array->SetConstructor(ArrayConstructor);
            Script * sizeFunc = new Script(_array);
            sizeFunc->SetConstructor(SizeConstructor);
        _array->funcs.add("size", sizeFunc);
            Script * arrToString = new Script(_array);
            arrToString->SetConstructor(ArrayToString);
        _array->funcs.add("ToString", arrToString);
    global->funcs.add("array", _array);

    Script * _string = new Script(nullptr, "string", "");
        _string->SetConstructor(StringConstructor);
        _string->funcs.add("ToString", IntToString);
    global->funcs.add("string", _string);

    Script * _bool = new Script(nullptr, "bool", "false");
        _bool->SetConstructor(BoolConstructor);
        _bool->funcs.add("ToString", IntToString);
    global->funcs.add("bool", _bool);


        Script * _count = new Script(nullptr, "count", "CountString");
        _count->SetConstructor(CountC);
    global->funcs.add("count", _count);
        Script * _cout = new Script(nullptr, "cout", "Cout");
        _cout->SetConstructor(CoutParams);
    global->funcs.add("cout", _cout);

        Script * _ticks = new Script(nullptr, "func", "0");
        _ticks->SetConstructor(GetTicks);
    global->funcs.add("ticks", _ticks);

    _global = global;

    /// Exec\n
    Script temp(global);
    global->Execute(&temp);
}
