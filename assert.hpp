#pragma once

#include <concepts>
#include "errorConcepts.hpp"
#include "functionConcepts.hpp"
#include "assertionFailure.hpp"

namespace fbtt {
   template <typename Boolable>
      requires std::convertible_to<Boolable, bool>
   void assert_true(Boolable assertion, const std::string & onFail = "")
   {
      if (!assertion)
         throw BooleanAssertionFailure(onFail);
   }

   template <typename Boolable>
      requires std::convertible_to<Boolable, bool>
   void assert_false(Boolable assertion, const std::string & onFail = "")
   {
      if (assertion)
         throw BooleanAssertionFailure(onFail);
   }

   template <typename T, typename U>
      requires std::equality_comparable_with<T, U>
   void assert_equals(T x, U y, const std::string & onFail = "") 
   {
      if (x != y)
         throw EqualityAssertionFailure(x, y, onFail);
   }

   template <typename T, typename U>
      requires std::equality_comparable_with<T, U>
   void assert_noteq(T x, U y, const std::string & onFail = "")
   {
      if (x == y)
         throw EqualityAssertionFailure(x, y, onFail);
   }

   template <ErrorType E, typename Func, typename ... Args>
      requires CallableWith<Func, Args...>
   void assert_throws(Func f, Args ... args)
   {
      try {
         f(args...);
      } catch (E & expected) {
         // good
         return;
      } catch (std::exception & e) {
         throw AssertionFailure("Function didn't throw expected errortype \"" 
            + std::string(typeid(E).name()) + "\". Instead, it threw errortype \"" 
            + std::string(typeid(decltype(e)).name()) + "\"");

         // EXITED
      } catch (...) {
         throw AssertionFailure("Function threw unknown error.");

         // EXITED
      }  

      // Function didn't throw!
      throw AssertionFailure("Function didn't throw error.");
   }
};