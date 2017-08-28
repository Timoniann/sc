#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <vector>
#include <iterator>
#include <functional>

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

        ValueType & add(KeyType key, ValueType value)
        {
            data.push_back(make_pair(key, value));
            return data[data.size() - 1].second;
            return data.back().second;
        }

        ValueType & add(KeyType key)
        {
            data.push_back(make_pair(key, nullptr));
            return data.back().second;
        }

        ValueType & add(ValueType value)
        {
            data.push_back(make_pair("", value));
            return data[data.size() - 1].second;
            return data.back().second;
        }

        ValueType & push_back(ValueType value)
        {
            data.push_back(make_pair("", value));
            return data.back().second;
        }


        void foreach(function<void(KeyType key, ValueType value)>func)
        {
            for(typename vector<pair<KeyType, ValueType>>::iterator it = data.begin(); it != data.end(); it++)
                func((*it).first, (*it).second);
        }

        void foreachReverse(function<void(KeyType key, ValueType value)>func)
        {
            for(typename vector<pair<KeyType, ValueType>>::reverse_iterator it = data.rbegin(); it != data.rend(); it++)
                func((*it).first, (*it).second);
        }

        ValueType & operator[](KeyType key)
        {
            int a = data.size();
            for(typename vector<pair<KeyType, ValueType>>::iterator it = data.begin(); it != data.end(); it++)
                if((*it).first == key) return (*it).second;
            return add(key);
        }

        ValueType & operator[](int index)
        {
            return data[index].second;
        }

        size_t size(){ return data.size(); }

        int count(KeyType key)
        {
            int count = 0;
            for(typename vector<pair<KeyType, ValueType>>::iterator it = data.begin(); it != data.end(); it++)
                if((*it).first == key) count++;
            return count;
        }

        void clear()
        {
            data.clear();
        }

        KeyType keyOfIndex(int index)
        {
            return data[index].first;
        }

        typedef pair<KeyType, ValueType>* iterator;
        typedef const pair<KeyType, ValueType>* const_iterator;

        iterator begin() { return &data[0]; }
        const_iterator begin() const { return &data[0]; }
        iterator end() { return &data[size()];}
        const_iterator end() const { return &data[size()]; }


        typedef typename vector<pair<KeyType, ValueType>>::iterator* vec_iterator;
        vec_iterator find(KeyType key)
        {
            int count = 0;
            for(typename vector<pair<KeyType, ValueType>>::iterator it = data.begin(); it != data.end(); it++)
                if((*it).first == key) return it;
            return data.back();
        }
    protected:

    private:
};

#endif // DICTIONARY_H
