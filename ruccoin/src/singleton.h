//
// Created by flt on 3/18/24.
//

#ifndef SIMPLEBITCOIN_SINGLETON_H
#define SIMPLEBITCOIN_SINGLETON_H

#include <type_traits>


template<typename T>
class PublicSingleton
{
protected:
    PublicSingleton() {}

public:
    static T& getInstance() noexcept(std::is_nothrow_constructible<T>::value)
    {
        static T instance;
        return instance;
    }
    virtual ~PublicSingleton() noexcept {}
    PublicSingleton(const PublicSingleton&) = delete;
    PublicSingleton& operator=(const PublicSingleton&) = delete;
};

#endif //SIMPLEBITCOIN_SINGLETON_H
