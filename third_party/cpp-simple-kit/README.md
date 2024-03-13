# C++单头文件库

*C++单头文件的常用代码库。C++ header-only libraries*

## 说明

| 文件名           | 作用                    | 来源                  |
|---------------|-----------------------|---------------------|
| cxxopts.hpp   | 命令行参数解析               | [cxxopts]           |
| make_unique.h | 为C++11增加make_unique模板 | [N3656]             |
| SafeQueue.hpp | 线程安全队列                | [queue-thread-safe] |
| singleton.h   | 单例模板                  | [C++单例模板]           |
| ThreadPool.h  | 线程池                   | [ThreadPool]        |

## 注意

除了历史代码，建议任何新代码都至少以C++11作为最低标准，且跟随最新标准以简化实现，甚至获得性能提升。

**因此本项目最低支持C++11标准。**

## 引用

- [cxxopts]
- [N3656]
- [queue-thread-safe]
- [C++单例模板]
- [ThreadPool]

[cxxopts]: https://github.com/jarro2783/cxxopts "cxxopts"

[N3656]: https://isocpp.org/files/papers/N3656.txt "N3656"

[queue-thread-safe]: https://github.com/alfredopons/queue-thread-safe "queue-thread-safe"

[C++单例模板]: https://333rd.net/zh/posts/read/c++%E5%8D%95%E4%BE%8B%E6%A8%A1%E6%9D%BF/ "C++单例模板"

[ThreadPool]: https://github.com/progschj/ThreadPool "ThreadPool"
