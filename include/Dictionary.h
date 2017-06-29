#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <vector>

using namespace std;

template<typename KeyType, typename ValueType>
class Dictionary
{
    public:
        vector<pair<KeyType, ValueType>> data;
        Dictionary(){}
        Dictionary(KeyType key, ValueType value)
        {
            add(key, value);
        }

        void add(KeyType key, ValueType value)
        {
            data.push_back(make_pair(key, value));
        }

        void foreach(void func(KeyType key, ValueType value))
        {
            for(typename vector<pair<KeyType, ValueType>>::iterator it = data.begin(); it != data.end(); it++)
                func((*it).first, (*it).second);
        }
        void foreachReverse(void func(KeyType key, ValueType value))
        {
            for(typename vector<pair<KeyType, ValueType>>::reverse_iterator it = data.rbegin(); it != data.rend(); it++)
                func((*it).first, (*it).second);
        }

        ValueType operator[](KeyType key)
        {
            for(typename vector<pair<ValueType, KeyType>>::iterator it = data.begin(); it != data.end(); it++)
                if(it.first == key) return it.second;
            return 0;
        }

        ValueType operator[](int index)
        {
            return data[index].second;
        }
    protected:

    private:
};

#endif // DICTIONARY_H
