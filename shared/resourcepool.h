#pragma once
#define VK_NO_PROTOTYPES // for volk
#include <vector>
#include <string>
#include <memory>
#include <any>
#include <volk.h>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp> // hash combine for custom structure
#include "di.hpp"
#include "utils.h"
#include "vulkan.h"

namespace SharedUtils
{
    class ILogicalDevice;

    // boost: https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
    // create hash key for custom structure
    template <class T>
    inline void hash_combine(size_t &seed, const T &v)
    {
        std::hash<T> hasher;
        glm::detail::hash_combine(seed, hasher(v));
    }

    // overload createHashKey
    template <typename T>
    inline void createHashKey(size_t &hashkey, const T &value)
    {
        hash_combine(hashkey, value);
    }

    template <typename T, typename... Args>
    inline void createHashKey(size_t &hashkey, const T &arg1, const Args &...restArg)
    {
        createHashKey(hashkey, arg1);
        // recursive
        createHashKey(hashkey, restArg...);
    }

    // not thread-safe
    // rules here:
    // 1st ctor param is device
    // the rest: ...args
    // create hash from ...args.
    template <class T, class... A>
    T &requestResourceFromPool(std::shared_ptr<ILogicalDevice> device, std::unordered_map<std::size_t, T> &resourcePool, A &...args)
    {
        std::size_t hashKey{0U};
        createHashKey(hashKey, args...);
        auto it = resourcePool.find(hashKey);
        if (it != resourcePool.end())
        {
            return it->second;
        }
        // rtti
        const char *resType = typeid(T).name();
        size_t resId = resourcePool.size();
        // create new resource and avoid copy
        T resource(device, args...);
        auto resourceIt = resourcePool.emplace(hashKey, std::move(resource));
        if (!resourceIt.second)
        {
            throw std::runtime_error{"failed to create resource"};
        }
        return res_it->second;
    };
}