#ifndef ARRAY_H
#define ARRAY_H

#include <Script.h>

class Array : public Script
{
    public:
        Array(string val);
        ~Array();
        Script * Execute(Script & parameter);
    protected:

    private:
};

#endif // ARRAY_H
