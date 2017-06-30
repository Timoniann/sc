#include <Dictionary.h>

template<typename KeyType, typename ValueType>
Dictionary<KeyType, ValueType>::Dictionary()
{

}


template<typename KeyType, typename ValueType>
Dictionary<KeyType, ValueType>::Dictionary(KeyType key, ValueType value)
{
    add(key, value);
}

template<typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::add(KeyType key, ValueType value)
{
    data.push_back(make_pair(key, value));
}

template<typename KeyType, typename ValueType>
ValueType Dictionary<KeyType, ValueType>::operator[](KeyType key)
{
    for(typename vector<pair<ValueType, KeyType>>::iterator it = data.begin(); it != data.end(); it++)
    {
        if(it.first == key) return it.second;
    }
    return 0;
}

template<typename KeyType, typename ValueType>
ValueType Dictionary<KeyType, ValueType>::operator[](int index)
{
    return data[index].second;
}

template<typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::foreach(void func(KeyType key, ValueType value))
{
    for(typename vector<pair<ValueType, KeyType>>::iterator it = data.begin(); it != data.end(); it++)
        func(it.first, it.second);
}

template<typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::foreachReverse(void func(KeyType key, ValueType value))
{
    for(typename vector<pair<ValueType, KeyType>>::iterator it = data.end(); it != data.begin(); it--)
        func(it.first, it.second);
}
