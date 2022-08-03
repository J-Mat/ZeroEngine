#pragma once

#include <type_traits>

namespace Zero
{
    template<typename T>
    class IPublicSingleton
    {
    protected:
        IPublicSingleton() = default;

    public:
        [[nodiscard]] static T& GetInstance() noexcept(std::is_nothrow_constructible<T>::value)
        {
            static T instance;
            return instance;
        }
        virtual ~IPublicSingleton() noexcept = default;
        IPublicSingleton(const IPublicSingleton&) = delete;
        IPublicSingleton& operator=(const IPublicSingleton&) = delete;
    };
}