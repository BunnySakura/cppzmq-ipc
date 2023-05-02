#ifndef MAKE_UNIQUE_H
#define MAKE_UNIQUE_H

/*
由于std::make_unique在C++14标准及以上才可使用，
复制标准库实现如下。

使用方法：details::make_unique<类型>(参数)
*/

// make_unique<T>(args...)
// make_unique<T[]>(n)
// make_unique<T[N]>(args...) = delete
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace details
{
    // make_unique support for pre c++14

#if __cplusplus >= 201402L // C++14 and beyond
    using std::make_unique;
#else
#if 0 // 简单实现，不支持数组
    template <typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args &&...args)
    {
        static_assert(!std::is_array<T>::value, "arrays not supported");
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
#else

    template <class T>
    struct _Unique_if
    {
        typedef std::unique_ptr<T> _Single_object;
    };

    template <class T>
    struct _Unique_if<T[]>
    {
        typedef std::unique_ptr<T[]> _Unknown_bound;
    };

    template <class T, size_t N>
    struct _Unique_if<T[N]>
    {
        typedef void _Known_bound;
    };

    template <class T, class... Args>
    typename _Unique_if<T>::_Single_object
    make_unique(Args &&...args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template <class T>
    typename _Unique_if<T>::_Unknown_bound
    make_unique(size_t n)
    {
        typedef typename std::remove_extent<T>::type U;
        return std::unique_ptr<T>(new U[n]());
    }

    template <class T, class... Args>
    typename _Unique_if<T>::_Known_bound
    make_unique(Args &&...) = delete;
#endif // 0
#endif // __cplusplus >= 201402L
} // namespace details

#endif // MAKE_UNIQUE_H