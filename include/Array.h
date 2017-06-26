#ifndef ARRAY_H
#define ARRAY_H

#include <Script.h>

class Array : public Script
{
    public:
        Array(Script * p1, Script * p2);
        Array();
        ~Array();
        Script * Execute(Script & parameter);
        static string TypeName;

        string GetValue() override;
    protected:
        vector<Script*> arr;
    private:
};

#endif // ARRAY_H
