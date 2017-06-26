#include "Array.h"

Array::Array(Script * p1, Script * p2) : Array()
{
    arr.push_back(p1);
    arr.push_back(p2);
}

Array::Array() : Script()
{
    this->type = "array";
    this->SetValue("Standart.Array");
    cout << "Creating array\n";
}

Array::~Array()
{
    //dtor
}

Script * Array::Execute(Script & parameter)
{
    return this;
}

string Array::TypeName = "array";

string Array::GetValue()
{
    string result;
    for(int i = 0; i < arr.size(); i++)
        result += arr[i]->GetValue() + ", ";
    if(result.size() > 0) { result.pop_back(); result.pop_back(); }
    return result;
}
