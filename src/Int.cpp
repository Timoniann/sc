#include "Int.h"

class Operator : public Script
{
public:
    Operator() : Script()
    {

    }

    Operator(Script * parent) : Script(parent)
    {

    }
    Script * Execute(Script & parent)
    {
        parent.GetType();
        this->parent->GetType();

        if(parent.GetType() == "int"){
            cout << "Int add types " << this->parent->GetType() << " with " << parent.GetType();
            return new Script(nullptr, "int", to_string(stoi(this->parent->GetValue()) + stoi(parent.GetValue())));
        }
        else cout << "NOT INTEGER!";
        return nullptr;
            //return this.value - parent.value;
    }
};

Int::Int(string val) : Script()
{
    cout << "New int (" << val << ")\n";
    this->type = "int";
    SetValue(val);
    operators["+"] = new Operator(this);
}

Int::~Int()
{
    //dtor
}

Script * Int::Execute(Script & parent)
{
    return this;
}

