#ifndef BOOL_H
#define BOOL_H

#include <Script.h>

class Bool : public Script
{
    public:
        Bool(string val);
        ~Bool();
        Script * Execute(Script & parent);
    protected:

    private:
};

#endif // BOOL_H
