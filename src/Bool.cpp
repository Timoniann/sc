#include "Bool.h"

class OpEquals : public Script // a == 2
{
public:
    OpEquals(Script * parent) : Script(parent)
    {

    }
    Script * Execute(Script & parameter)
    {
        cout << "Equals(==) " << parent->GetType() << " with " << parameter.GetType() << "\n";
        if(parent->GetType() != parameter.GetType() || parent->GetValue() != parameter.GetValue()) return new Script(nullptr, "null", "");
        return new Bool("true");
    }
};

Bool::Bool(string val) : Script()
{
    cout << "New bool (" << val << ")\n";
    this->type = "bool";
    if(val == "" || val == "false" || val == "0")
        SetValue("false");
    else SetValue("true");

    operators["=="] = new OpEquals(this);
}

Bool::~Bool()
{
    //dtor
}

Script * Bool::Execute(Script & parent)
{
    return this;
}
