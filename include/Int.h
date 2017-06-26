#ifndef INT_H
#define INT_H

#include <Script.h>


class Int : public Script
{
    public:
        Int(string val);
        ~Int();
        Script * Execute(Script & parent);
    protected:
    private:
};

#endif // INT_H
