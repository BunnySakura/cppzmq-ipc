#ifndef MAKE_UNIQUE_H
#define MAKE_UNIQUE_H

/**
 * @brief 实现了C++14及以上版本的std::make_unique功能，以便在旧版本的C++编译器中使用。
 *
 * 这段代码提供了一个与C++14标准库中的std::make_unique功能相同的接口，使得在不支持C++14的编译器环境中也能使用unique_ptr的智能指针。
 *
 * 使用方法如下：
 * - 对于单个对象的构造，使用`details::make_unique<类型>(参数)`。
 * - 对于动态数组的构造，使用`make_unique<T[]>(n)`，其中`T`是数组元素的类型，`n`是数组的大小。
 * - 对于静态数组的构造，使用`make_unique<T[N]>(args...)`，其中`T`是数组的类型，`N`是数组的大小，`args...`是构造数组元素所需的参数。
 * - 注意：静态数组的构造被显式删除，因为unique_ptr不支持管理静态数组。
 *
 * 示例：
 * @code
 * auto ptr = details::make_unique<std::vector<int>>(10); // 创建一个包含10个整数的unique_ptr管理的vector
 * auto arrayPtr = make_unique<int[]>(10); // 创建一个包含10个整数的unique_ptr管理的动态数组
 * // make_unique<int[10]>(...); // 这将导致编译错误，因为不支持静态数组
 * @endcode
 */

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace details {
#if __cplusplus >= 201402L  // C++14 and beyond
using std::make_unique;
#else
template<class T>
struct _Unique_if {
  typedef unique_ptr<T> _Single_object;
};

template<class T>
struct _Unique_if<T[]> {
  typedef unique_ptr<T[]> _Unknown_bound;
};

template<class T, size_t N>
struct _Unique_if<T[N]> {
  typedef void _Known_bound;
};

template<class T, class... Args>
typename _Unique_if<T>::_Single_object
make_unique(Args &&... args) {
  return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<class T>
typename _Unique_if<T>::_Unknown_bound
make_unique(size_t n) {
  typedef typename remove_extent<T>::type U;
  return unique_ptr<T>(new U[n]());
}

template<class T, class... Args>
typename _Unique_if<T>::_Known_bound
make_unique(Args &&...) = delete;
#endif // __cplusplus >= 201402L
} // namespace details

#endif  // MAKE_UNIQUE_H
