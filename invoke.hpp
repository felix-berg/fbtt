#pragma once

#include <concepts>
#include <exception>
#include <functional>

template <typename Function, typename ... Args>
concept Callable = requires (Function f, Args ... args) { f(args...); };

template <typename Function, typename ... Args>
concept arguments_to = requires (Function f, Args ... args) { f(args...); };

template <typename Function, typename ReturnType, typename ... Args>
concept function_returns = requires(Function f, Args ... args) {
   requires std::same_as<decltype(f(args...)), ReturnType>;
};

template <typename Function, typename ReturnType, typename ... Args>
concept storable_function = 
   Callable<Function, Args...>
      &&
   arguments_to<Function, Args...>
      &&
   function_returns<Function, ReturnType, Args...>
      &&
   std::convertible_to<Function, std::function<ReturnType(Args...)>>;

template <typename Function, typename ReturnType = void, typename ... Args>
concept function_with_signature = 
   Callable<Function, Args...>
      &&
   arguments_to<Function, Args...>
      &&
   function_returns<Function, ReturnType, Args...>;

template <typename ErrorType>
concept is_error = std::derived_from<ErrorType, std::exception>;

template <typename Function, typename ... Args>
   requires arguments_to<Function, Args...>
               &&
            Callable<Function, Args...>
void invoke(Function f, Args ... args)
{
   f(args...);
}

