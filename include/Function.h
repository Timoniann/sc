#ifndef FUNCTION_H
#define FUNCTION_H

#define ORDER_CREATE 1
#define ORDER_CALL 2

#include <string>
#include <vector>

#include <Variable.h>

using namespace std;

class Function
{
    public:
        Function();
        ~Function();
        int Execute(string params = "");
        vector<vector<tuple<unsigned int, string, string>>> cmds;
        map<string, string> vars;
        map<string, Function*> funcs;
    protected:

    private:
};

#endif // FUNCTION_H
