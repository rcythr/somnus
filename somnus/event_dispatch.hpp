#pragma once

#include <somnus/rope.hpp>
#include <somnus/is_shared_ptr.hpp>

#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>

namespace somnus
{

typedef uint64_t EventId;
typedef uint64_t ServiceId;
typedef std::shared_ptr<Rope> RopePtr;

typedef std::function<void()> EmptyFunction;
   
template<typename T>
using FunctionType = std::function<void(T)>;

struct EventDispatch
{
    EventDispatch(std::string config_filename);
    
    // Event Registration

    template<typename T>
    void register_event(ServiceId service, EventId event, FunctionType<T> fn, 
        typename std::enable_if<std::is_pointer<T>::value && !is_shared_ptr<T>::value>::type* = 0)
    {
        auto find_itr = serviceToRope.find(service);
        if(find_itr != serviceToRope.end())
        {
            handlers[event] = std::tuple<RopePtr, FunctionType<std::shared_ptr<void>>>(find_itr->second, 
                [=] (std::shared_ptr<void> data) { fn((T)data.get()); });
        }
    }

    template<typename T>
    void register_event(ServiceId service, EventId event, FunctionType<T> fn, 
        typename std::enable_if<!std::is_pointer<T>::value && is_shared_ptr<T>::value>::type* = 0)
    {
        auto find_itr = serviceToRope.find(service);
        if(find_itr != serviceToRope.end())
        {
            handlers[event] = std::tuple<RopePtr, FunctionType<std::shared_ptr<void>>>(find_itr->second, 
                [=] (std::shared_ptr<void> data) { fn(std::static_pointer_cast<typename T::element_type>(data)); });
        }
    }

    template<typename T>
    void register_event(ServiceId service, EventId event, FunctionType<T&> fn,
        typename std::enable_if<!std::is_pointer<T>::value && !is_shared_ptr<T>::value>::type* = 0)
    {
        auto find_itr = serviceToRope.find(service);
        if(find_itr != serviceToRope.end())
        {
            handlers[event] = std::tuple<RopePtr, FunctionType<std::shared_ptr<void>>>(find_itr->second, 
                [=] (std::shared_ptr<void> data) { fn(*std::static_pointer_cast<T>(data).get()); });
        }
    }

    // Event Dispatch

    template<typename T>
    void dispatch(EventId event, T data, 
        typename std::enable_if<std::is_pointer<T>::value && !is_shared_ptr<T>::value>::type* = 0)
    {
        auto find_itr = handlers.find(event);
        if(find_itr != handlers.end())
        {
            auto& tup = find_itr->second;
            auto fn = std::get<1>(tup);
            std::get<0>(tup)->run([=] { fn(std::shared_ptr<void>(data, [] (void* data) {/* Do nothing */})); }); 
        }
    }

    template<typename T>
    void dispatch(EventId event, T data, 
        typename std::enable_if<!std::is_pointer<T>::value && is_shared_ptr<T>::value>::type* = 0)
    {
        auto find_itr = handlers.find(event);
        if(find_itr != handlers.end())
        {
            auto& tup = find_itr->second;
            auto fn = std::get<1>(tup);
            std::get<0>(tup)->run([=] { fn(data); });
        }
    }

    template<typename T>
    void dispatch(EventId event, T data,
        typename std::enable_if<!std::is_pointer<T>::value && !is_shared_ptr<T>::value>::type* = 0)
    {
        auto find_itr = handlers.find(event);
        if(find_itr != handlers.end())
        {
            auto ptr = std::make_shared<T>(data);

            auto& tup = find_itr->second;
            auto fn = std::get<1>(tup);
            std::get<0>(tup)->run([=] { fn(std::static_pointer_cast<void>(ptr)); });
        }
    }

private:
    std::unordered_map<ServiceId, RopePtr> serviceToRope;
    std::unordered_map<EventId, std::tuple<RopePtr, FunctionType<std::shared_ptr<void>>>> handlers;
};

}
