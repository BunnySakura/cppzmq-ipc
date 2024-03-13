#ifndef CZMQ_IPC_H
#define CZMQ_IPC_H

#include "stddef.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

typedef struct {
  const size_t size;
  const uint8_t *byte_arr;
} CZmqIpcByteArr;

typedef struct {
  const size_t size;
  const char *string;
} CZmqIpcString;

typedef void (*CZmqIpcCallbackFunc)(CZmqIpcString, CZmqIpcByteArr);

// 创建对象
DLL_EXPORT int32_t CZmqIpcNew();

// 销毁对象
DLL_EXPORT void CZmqIpcDel(int32_t zid);

// 初始化
DLL_EXPORT void CZmqIpcInit(int32_t zid,
                            CZmqIpcCallbackFunc callback,
                            size_t threads,
                            CZmqIpcString subscriber,
                            CZmqIpcString publisher);
// 订阅消息
DLL_EXPORT void CZmqIpcSubscribe(int32_t zid, CZmqIpcString topic);

// 取消订阅
DLL_EXPORT void CZmqIpcUnsubscribe(int32_t zid, CZmqIpcString topic);

// 发布消息
DLL_EXPORT void CZmqIpcPublish(int32_t zid, CZmqIpcString topic, CZmqIpcByteArr message);

#ifdef __cplusplus
}
#endif

#endif // CZMQ_IPC_H
