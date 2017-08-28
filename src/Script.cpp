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
#define ORDER_PUSH_STATEMENT 18
#define ORDER_PUSH_IF 19
#define ORDER_PUSH_ELSE 20
#define ORDER_CALL_TEMP_FUNC 21
#define ORDER_PUSH_DOT 22

//Script * _global;
Script  * _null,
        * _int,
        * _string,
        * _bool,
        * _array;

/// SCRIPT -----------------------------------

vector<Script*> allScripts;

Script::Script()
{
    allScripts.push_back(this);
}

Script::Script(Script * parent) : Script()
{ this->parent = parent; }

Script::Script(Script * parent, string type, string value) : Script()
{
    this->parent = parent;
    if(parent == nullptr) this->type = type;
    //else this->type = parent->GetType() + '.' + type;
    this->type = type;
    this->value = value;
}

Script::Script(Script* (* func)(Script * self, Script * params)) : Script()
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
#define IF 10
#define IF_OPEN 11
#define ELSE 12
#define RETURN 13
#define TYPE_PUSHED_PARAM 14


void Script::Handler(vector<string> & strs, unsigned int & i, unsigned int & j)
{
    vector<tuple<unsigned int, string, unsigned int>> triple;
    string newScriptName = "";
    vector<string> type_params;
    int last = NOTHING;
    int modify = NOTHING;
    bool doted = false;
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
                    else if(word == "if"){
                        //triple.push_back(make_tuple(ORDER_PUSH_STATEMENT, word, i));
                        last = IF;
                        continue;
                    }
                    else if(word == "else"){
                        //last = ELSE;
                        triple.push_back(make_tuple(ORDER_PUSH_ELSE, word, i));
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
                        last = TYPE_PUSHED_PARAM;
                        continue;
                    }
                    else if (last == TYPE_PUSHED_PARAM){
                        Log("Need symbol ','", MessageEasyError, j);
                        type_params.push_back(word);
                        continue;
                    }
                    else path = word;
                }
                last = LETTER;
            }
            else{
                if(c == '.') {
                    if(last == LETTER || last == NEW_TYPE || last == OPERATOR)
                    {
                        if(path != "") triple.push_back(make_tuple(ORDER_PUSH_VAL, path, i));
                        triple.push_back(make_tuple(ORDER_PUSH_DOT, (string)"" + c, i));
                        doted = true;
                        //path += c;
                        path = "";
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
                    else if (last == IF){
                        //last = IF_OPEN;
                        last = NOTHING;
                        modify = IF;
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
                    if(last == TYPE_PUSH_PARAMS || last == TYPE_PUSHED_PARAM)
                    {
                        last = NEW_TYPE;
                        continue;
                    }
                    if(brackets.size() == 0)
                    {
                        if(modify == IF) {
                                triple.push_back(make_tuple(ORDER_PUSH_IF, "if", i));
                                cmds.push_back(triple);
                                triple.clear();
                                last = NOTHING;
                                //modify = NOTHING;
                        }
                        else Log("Syntax error. Wrong using operator ')'" + to_string(last), MessageEasyError, i);
                        continue;
                    }
                    if(brackets.back()) triple.push_back(make_tuple(ORDER_PUSH_OP, (string)"" + c, i));
                    else
                        if(last == OPEN_FUNC) triple.push_back(make_tuple(ORDER_CALL_FUNC_WITHOUT_PARAMS, (string)"" + c, i));
                        else { triple.push_back(make_tuple(ORDER_CALL_FUNC, (string)"" + c, i)); last = LETTER; brackets.pop_back(); path = ""; continue; }
                    brackets.pop_back();
                    path = "";
                }
                else if(c == ';')// && triple.size() > 0
                {
                    //cout << "BACK: " << get<1>(triple.back());
                    cout << "\n";
                    cmds.push_back(triple);
                    triple.clear();
                    last = NOTHING;
                    modify = NOTHING;
                    doted = false;
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
                        Script * newScript = new Script(this, newScriptName, newScriptName);
                        Script * par = new Script();
                        _array->Clone(par);
                        for(auto it = type_params.begin(); it != type_params.end(); it++){
                            //par->AddVar((*it), new Script(this, "null", "null"));
                        }

                        //newScript->SetParams(par);
                        j++;
                        //triple.push_back(make_tuple(ORDER_PUSH_TYPE, newScriptName, i));
                        newScript->Handler(strs, i, j);
                        str = strs[i];
                        AddFunc(newScriptName, newScript);
                        continue;
                    }
                    else {
                        Script * newScript = new Script(this, "Temporary", "TempFunc");
                        j++;
                        newScript->Handler(strs, i, j);
                        str = strs[i];
                        tempFuncs.push_back(newScript);
                        triple.push_back(make_tuple(ORDER_CALL_TEMP_FUNC, to_string(tempFuncs.size() - 1), i));
                        cmds.push_back(triple);
                        triple.clear();
                        last = NOTHING;
                        //modify = NOTHING;
                        continue;
                    }
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
                else if(c == ',' && last == TYPE_PUSHED_PARAM){
                    last = TYPE_PUSH_PARAMS;
                    continue;
                }
                else { triple.push_back(make_tuple(ORDER_PUSH_OP, readOperator(str, j), i)); }
                last = OPERATOR;

            }
        }
        j = 0;
    }
}

Script * Script::FuncToExecute(string funcName)
{
    if(funcName == "this") return parent;
    if(funcName == "self") return this;
    //if(params.contains(funcName));
    Script * current = this;
    while(current){
        if(current->funcs.count(funcName))
            return current->funcs[funcName];
        if(current->vars.count(funcName))
            return current->vars[funcName];
        current = current->parent;
    }
    return current;
}

Script * Script::Execute(Script * parameters)
{
    if(constructor != nullptr)
        return constructor(this, parameters);
    int iter = 0;
    params = new Script(this);
    _array->Clone(params);
    for(auto it = parameters->vars.begin(); it != parameters->vars.end(); it++){
        if(params->vars.size() > iter) params->vars[iter++] = (*it).second;
        else params->AddVar((*it).second);
    }
    //if(params == nullptr) params = new Script(this, "null", "null");
    vars["params"] = params;
    //if(vars.count("params")) vars["params"] = params;
    //else AddVar("params", params);

    //for(mass::iterator it = t.begin(); it != t.end(); it++)

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
        bool doted = false;
        for(unsigned int j = 0; j < cmd.size(); j++)
        {
            auto t = cmd[j];
            string stt = get<1>(t);
            switch(std::get<0>(t))
            {
            case ORDER_PUSH_NUMBER:
                st.push_back(_int->Execute(get<1>(t))->SetParent(this));
                break;
            case ORDER_PUSH_FUNC:
                if(doted) { funcsToCall.push_back("." + get<1>(t)); doted = false; }
                else funcsToCall.push_back(get<1>(t));
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

                    //Log("Calling function with parameters");
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
                    //st.pop_back();
                    if(prms->GetType() != "array"){
                        Script * arr = _array->Execute("Param to array");
                        arr->AddVar(prms);
                        prms = arr;
                    }

                    Script * toExe = nullptr;
                    if(funcsToCall.back()[0] == '.') {
                        if(st.size() == 0) {
                            Log("Surplus operator '.'", MessageError, get<2>(t));
                            toExe = FuncToExecute(funcsToCall.back());
                        }
                        else {

                            toExe = st.back()->FuncToExecute(funcsToCall.back().substr(1));
                            st.pop_back();
                            st.push_back(toExe->Copy()->Execute(_array->Execute("CFWOP"))->SetParent(this));
                            //st.push_back(toExe->Execute(prms)->SetParent(this));
                            funcsToCall.pop_back();
                            continue;
                        }
                    }
                    else toExe = FuncToExecute(funcsToCall.back());

                    if(toExe == nullptr)
                        if(funcsToCall.back()[0] == '.' && st.size() > 0)
                            Log("Object '" + st.back()->GetType() + "' does not have function '" + funcsToCall.back().substr(1) + "'", MessageEasyError, get<2>(t));
                        else
                            Log("Object '" + GetType() + "' does not have function '" + funcsToCall.back() + "'", MessageEasyError, get<2>(t));
                    else
                        st.push_back(toExe->Copy()->Execute(prms)->SetParent(this));
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
                        //cout << "Calling function without parameters\n";
                        Script * toExe = nullptr;

                        if(funcsToCall.back()[0] == '.') {
                            if(st.size() == 0) {
                                Log("Surplus operator '.'", MessageError, get<2>(t));
                                toExe = FuncToExecute(funcsToCall.back());
                            }
                            else {
                                toExe = st.back()->FuncToExecute(funcsToCall.back().substr(1));
                                st.pop_back();
                            }
                        }
                        else toExe = FuncToExecute(funcsToCall.back());
                        if(toExe != nullptr)
                            st.push_back(toExe->Copy()->Execute(_array->Execute("CFWOP"))->SetParent(this));
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
                if(doted) {
                    Script * last = st.back();
                    st.pop_back();
                    st.push_back(last->GetVariable(get<1>(t)));
                    doted = false;
                }
                else st.push_back(GetVariable(get<1>(t)));
                break;
            case ORDER_PUSH_BOOL:
                st.push_back(_bool->Execute(get<1>(t))->SetParent(this));
                break;
            case ORDER_PUSH_OP:
                {
                    string oper = get<1>(t);
                    //cout << "Op: " << oper << '\n';
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
            case ORDER_PUSH_DOT:
                //Log("TYPE: " + st.back()->GetType(), MessageError, get<2>(t));

                doted = true;
                break;
            case ORDER_PUSH_IF:
                if(st.size() == 0) Log("'If' don't have condition", MessageError, get<2>(t));
                else{
                    while (!op.empty())
                        process_op (st, op.back()),  op.pop_back();
                    if(st.back()->GetType() == "bool"){
                        Log("If statement get bool type :)", MessageWarning, get<2>(t));
                        if(st.back()->GetValue() == "true") {
                            //i++;
                            Log("I got true");
                            break;
                        }
                        else {
                            Log("I got false");
                            i+=2;
                            cmd = cmds[i];
                            j = 0;
                            op.clear();
                            st.clear();
                            break;
                            //continue;
                        }
                    }
                    else Log((string)"HANDLE NOT BOOL " + st.back()->GetType(), MessageWarning, get<2>(t));
                }
                break;
            case ORDER_PUSH_ELSE:
                j = cmd.size();
                break;
            case ORDER_PUSH_POINTER:
                pointers.push_back(get<1>(t));
                break;
            case ORDER_PUSH_PARAM:
                break;
            case ORDER_PUSH_STR:
                st.push_back(_string->Execute(get<1>(t))->SetParent(this));
                break;
            case ORDER_CALL_TEMP_FUNC:
                st.push_back(tempFuncs[atoi(get<1>(t).c_str())]->Copy()->Execute("")->SetParent(this));
                break;
            default:
                Log("Invalid order (" + to_string(get<0>(t)) + ")", MessageError, get<2>(t));
            }
        }
        while (!op.empty())
            if(op.back() == "(") op.pop_back();
            else process_op (st, op.back()),  op.pop_back();
        if(newValue != "" && st.size() > 0)
            AddVar(newValue, st.back());
    }
    return this;
}

Script * Script::SetParent(Script * parent)
{
    this->parent = parent;
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

Script * Script::AddFunc(Script * script)
{
    return funcs.add(script)->SetParent(this);
}

Script * Script::AddVar(Script * script)
{
    return vars.add(script)->SetParent(this);
}

Script * Script::AddFunc(string name, Script * script)
{
    return funcs.add(name, script)->SetParent(this);
}

Script * Script::AddVar(string name, Script * script)
{
    return vars.add(name, script)->SetParent(this);
}

string Script::StackVariables()
{
    string result;
    //cout << "Stack:\n";
    result += "Type: '" + GetType() + "', value: '" + GetValue() + "'\n";
    result += "Count: " + to_string(vars.size()) + "\n";

    for(pair<string, Script*> p : vars)
        if(p.second != nullptr){
            if(p.second->funcs.count("ToString")){
                Script * toS = p.second->funcs["ToString"];
                result += p.first + " = " + toS->Copy()->Execute(p.second)->GetValue() + " (" + p.second->GetType() + ")\n";
            }
            else {
                result += p.first + ": {\n" + p.second->StackVariables() + "\n}";
            }
        }
        else Log("Nullptr variable in StackVariables", MessageWarning);
    result += "Funcs:\n";
    result += "Count: " + to_string(funcs.size()) + "\n";
    for(pair<string, Script*> p : funcs)
        if(p.second != nullptr){
            if(p.second->funcs.count("ToString")){
                Script * toS = p.second->funcs["ToString"];
                result += p.first + " = " + toS->Copy()->Execute(p.second)->GetValue() + " (" + p.second->GetType() + ")\n";
            }
            else {
                result += p.first + ": {\n" + p.second->StackVariables() + "}\n";
            }
        }
        else Log("Nullptr function in StackVariables (" + funcs.keyOfIndex(0) + ")", MessageWarning);

    return result;
}

void Script::process_op (vector<Script *> & st, string op)
{

    /// Handling

    if(st.size() < 2) {
        Log("Compilation problem. Size of stack = " + to_string(st.size()), MessageError);
        if(st.size() == 1)
            cout << st.back()->GetValue() << " - value\n";
        return;
    }

    /// -

    Script * r = st.back();  st.pop_back();
    Script * l = st.back();  st.pop_back();

    if(operators.count(op) > 0)
    {

        //Need operators Copy()
        typedef vector<tuple<string, string, Operator*>> mass;
        mass & t = operators[op];
        for(mass::iterator it = t.begin(); it != t.end(); it++)
        {
            if(get<0>(*it) == l->GetType() && get<1>(*it) == r->GetType())
                { st.push_back(get<2>(*it)->Execute(l, r)->SetParent(this));
                 return; }
        }

        for(mass::iterator it = t.begin(); it != t.end(); it++)
        {
            if(get<0>(*it) == l->GetType() && get<1>(*it) == "null")
                { st.push_back(get<2>(*it)->Execute(l, r)->SetParent(this));
                return; }
        }

        for(mass::iterator it = t.begin(); it != t.end(); it++)
        {
            if(get<0>(*it) == "null" && get<1>(*it) == "null")
                { st.push_back(get<2>(*it)->Execute(l, r)->SetParent(this));
                 return; }
        }
    }
    else cout << "Not have operator '" << op << "'\n";

    Log("Operator '" + op + "' not declared in type '" + l->GetType() + "'", MessageEasyError);
    st.push_back(new Script(nullptr, "null", ""));

}

Script * Script::GetVariable(string val)
{
    if(val == "this") return parent;
    if(val == "self") return this;
    //if(vars.count(val)) return vars[val];
    Script * v = FuncToExecute(val);
    if(v == nullptr){
        Log("Creating... (" + val + ")", MessageWarning);
        AddVar(val, new Script(this, "null", ""))->SetParent(this);
        return vars[val];
    } else return v;
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

Script * Script::Copy()
{
    Script * result = new Script();
    result->type = type;
    result->value = value;
    result->cmds = cmds;
    result->params = params;
    result->tempFuncs = tempFuncs;
    result->constructor = constructor;
    result->parent = parent;

    funcs.foreach([&result](string key, Script * value)
        {
            result->AddFunc(key, value->Copy());
        });

    return result;
}

Script * Script::Clone(Script * to)
{
    if(to == nullptr)
        Log("ERROR, nullptr in cloning", MessageError);
    to->type = type;
    to->value = value;
    to->cmds = cmds;
    to->params = params;
    to->tempFuncs = tempFuncs;
    to->constructor = constructor;
    to->funcs.clear();
    to->vars.clear();

    funcs.foreach([&to](string key, Script * value)
        {
            Script * c = new Script(nullptr, "cloned", "clon");
            value->Clone(c);
            to->AddFunc(key, c);
        });
    vars.foreach([&to](string key, Script * value)
        {
            Script * c = new Script(nullptr, "cloned", "clon");
            value->Clone(c);
            to->AddVar(key, c);
        });
    return to;
}

void Script::SetParams(Script * params)
{
    this->params = params;
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
    return _int->Execute(to_string(stoi(p1->GetValue()) - stoi(p2->GetValue())));
}

Script * Divide(Script * p1, Script * p2)
{
    return _int->Execute(to_string(stoi(p1->GetValue()) / stoi(p2->GetValue())));
}

Script * Plus(Script * p1, Script * p2)
{
    return _int->Execute(to_string(stoi(p1->GetValue()) + stoi(p2->GetValue())));
}

Script * Assign(Script * p1, Script * p2)
{
    p2->Clone(p1);
    return p1;
}

Script * EqualsFull(Script * p1, Script * p2)
{
    return _bool->Execute(to_string(p1 == p2));
}

Script * Equals(Script * p1, Script * p2)
{
    return _bool->Execute(to_string(p1->GetValue() == p2->GetValue()));
}

Script * Multi(Script * p1, Script * p2)
{
    return _int->Execute(to_string(stoi(p1->GetValue()) * stoi(p2->GetValue())));
}

Script * Comma(Script * p1, Script * p2)
{
    int a = allScripts.size();
    Script * result = new Script();
    if(p1->GetType() == _array->GetType()){
        p1->Clone(result);
        result->AddVar(p2);
        return result;
    }
    a = allScripts.size();
    _array->Clone(result);
    a = allScripts.size();
    //Script * result = _global->funcs["array"]->Execute("Array throw comma");
    result->AddVar(p1);
    result->AddVar(p2);
    a = allScripts.size();
    return result;
}

Script * ArrayPlus(Script * p1, Script * p2)
{
    Script * result = new Script();
    *result = *p1;
    result->AddVar(p2);
    return result;
}

Script * StringPlus(Script * p1, Script * p2)
{
    Script * result = new Script();
    _string->Clone(result);
    if(p2->funcs.count("ToString"))
        result->value = p1->value + p2->funcs["ToString"]->Copy()->Execute(p2)->GetValue();
    else
        result->value = p1->value + p2->GetType();
    return result;
}

/// Constructors


Script * IntConstructor(Script * self, Script * params)
{
    try{
        Script * newInt = new Script();

        _int->Clone(newInt);

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
    _string->Clone(newStr);
    //if(params->GetType() == _array->GetType())
    //    if(params->vars.size())
    //        newStr->SetValue(params->vars[0]->GetValue());
    newStr->SetValue(params->GetValue());
    //Log((string)"Param of string: type = '" + params->GetType() + "', value = '" + params->GetValue() + "'", MessageWarning );
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
    for(unsigned int i = 0; i < params->vars.size(); i++)
        if(params->vars[i]->funcs.count("ToString"))
            cout << params->vars[i]->funcs["ToString"]->Copy()->Execute(params->vars[i])->GetValue() << "\n";
        else cout << params->vars[i]->GetType() << "\n";
    return new Script(nullptr, "null", "null");
}

Script * ArrayConstructor(Script * self, Script * params)
{
    Script * newArray = new Script();
    _array->Clone(newArray);

    params->vars.foreach([&newArray](string key, Script * value)
        {
            Script * c = new Script();
            newArray->AddVar(key, value->Clone(c));
        });
    return newArray;
}

Script * BoolConstructor(Script * self, Script * params)
{
    Script * newBool = new Script();
    _bool->Clone(newBool);
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
    operators["="].push_back(make_tuple(_null->GetType(), _null->GetType(), new Operator(Assign)));
    operators[","].push_back(make_tuple(_null->GetType(), _null->GetType(), new Operator(Comma)));

    operators["+"].push_back(make_tuple(_int->GetType(), _int->GetType(), new Operator(Plus)));
    operators["*"].push_back(make_tuple(_int->GetType(), _int->GetType(), new Operator(Multi)));
    operators["-"].push_back(make_tuple(_int->GetType(), _int->GetType(), new Operator(Minus)));
    operators["/"].push_back(make_tuple(_int->GetType(), _int->GetType(), new Operator(Divide)));

    operators["+"].push_back(make_tuple(_array->GetType(), _null->GetType(), new Operator(ArrayPlus)));

    //operators["+"].push_back(make_tuple(_string->GetType(), _null->GetType(), new Operator(StringPlus)));

    operators["==="].push_back(make_tuple(_null->GetType(), _null->GetType(), new Operator(EqualsFull)));

    Operator * eq = new Operator(Equals);
    operators["=="].push_back(make_tuple(_int->GetType(), _int->GetType(), eq));
    operators["=="].push_back(make_tuple(_string->GetType(), _string->GetType(), eq));
    operators["=="].push_back(make_tuple(_bool->GetType(), _bool->GetType(), eq));
    //operators["=="].push_back(make_tuple("", "", eq));

}

Script * IntegerToString(Script * self, Script * params)
{
    return _string->Execute(self->parent)->SetParent(self->parent);
}

Script * ArrayToString(Script * self, Script * params)
{
    Script * str = _string->Execute(self);
    string val = "[";
    function<void(string, Script *)>f = [&val](string key, Script * value)
    {
        if(value->funcs.count("ToString"))
            val += value->funcs["ToString"]->Copy()->Execute(value)->GetValue() + ", ";
    };
    params->vars.foreach(f);
    if (params->vars.size()) val.pop_back(), val.pop_back();
    val += ']';
    str->SetValue(val);
    return str;
}

Script * GetTicks(Script * self, Script * param)
{
    return _int->Execute(to_string(GetTickCount()));
}

Script * ReturnThis(Script * self, Script * param)
{
    return param;
}

void InitGlobal()
{
    _null = new Script(nullptr, "null", "null");

    _int = new Script(nullptr, "int", "0");
    _int->SetConstructor(IntConstructor);
        Script * IntToString = new Script(_int, "function", "IntToString");
        IntToString->SetConstructor(IntegerToString);
    _int->funcs["ToString"] = IntToString;

    _array = new Script(nullptr, "array", "[]");
    _array->SetConstructor(ArrayConstructor);
        Script * sizeFunc = new Script(_array, "function", "size");
        sizeFunc->SetConstructor(SizeConstructor);
    _array->AddFunc("size", sizeFunc);
        Script * arrToString = new Script(_array, "function", "ArrayToString");
        arrToString->SetConstructor(ArrayToString);
    _array->AddFunc("ToString", arrToString);

    _string = new Script(nullptr, "string", "");
    _string->SetConstructor(StringConstructor);
        Script * StringToString = new Script(_string, "function", "ToString");
        StringToString->SetConstructor(ReturnThis);
    _string->AddFunc("ToString", StringToString);

    _bool = new Script(nullptr, "bool", "false");
    _bool->SetConstructor(BoolConstructor);
    _bool->AddFunc("ToString", IntToString);
}

void Scripting(Script * global)
{
        Script * _count = new Script(nullptr, "func", "CountString");
        _count->SetConstructor(CountC);
    global->AddFunc("count", _count);
        Script * _cout = new Script(nullptr, "func", "Cout");
        _cout->SetConstructor(CoutParams);
    global->AddFunc("cout", _cout);

        Script * _ticks = new Script(nullptr, "func", "ticks");
        _ticks->SetConstructor(GetTicks);
    global->AddFunc("ticks", _ticks);

    //_global = global;

    /// Exec\n
    Script * temp(global);
    InitOperators();
    global->Execute(temp);
}

vector<Script*> & getAllScripts()
{
    return allScripts;
}

void DeleteAllScripts(){
    for(int i = 0; i < allScripts.size(); i++){
        delete allScripts[i];
    }
    allScripts.clear();
}
