#include "Function.h"

Function::Function()
{
    //ctor
}

Function::~Function()
{
    //dtor
}

int Function::Execute(string params)
{
    for(int i = 0; i < cmds.size(); i++)
    {
        vector<tuple<unsigned int, string, string>> cmd = cmds[i];
        for(int j = 0; j < cmd.size(); j++)
        {
            auto t = cmd[i];
            switch(std::get<0>(t))
            {
            case ORDER_CREATE:
                vars[get<1>(t)] = get<2>(t);
                break;
            case ORDER_CALL:
                funcs[get<1>(t)]->Execute(get<2>(t));
                break;
            default:;
                //std::cout << "Some error\n";
            }
        }
    }
}
