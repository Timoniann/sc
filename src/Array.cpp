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
    cout << "Hello, i am array\n";
    //ctor
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
