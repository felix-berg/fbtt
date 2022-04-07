#pragma once

#include <concepts>
#include <functional>

template <typename Func, typename ... Args>
concept CallableWith =
   requires (Func func, Args ... args) { func(args...); };
   
template <typename Func, typename ReturnT, typename ... Args>
concept FunctionReturns = 
   CallableWith<Func, Args...>
      &&
   requires (Func func, Args ... args) {
      requires std::same_as<decltype(func(args...)), ReturnT>;
   };

template <typename Func, typename ReturnT, typename ... Args>
concept FunctionWithSignature =
   CallableWith<Func, Args...>
      &&
   FunctionReturns<Func, ReturnT, Args...>;

template <typename Func, typename ReturnT, typename ... Args>
concept StorableFunction = 
   FunctionWithSignature<Func, ReturnT, Args...>
      &&
   std::convertible_to<Func, std::function<ReturnT(Args...)>>;
