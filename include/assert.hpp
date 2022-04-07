#pragma once

#include <concepts>
#include "errorConcepts.hpp"
#include "functionConcepts.hpp"
#include "assertionFailure.hpp"

namespace fbtt {

   /** Assert that the given boolean value is true.
    * @param assertion: The boolean value to check
    * @param onFail: String for AssertionFailure, if the assertion fails. Defaults to ""
    * @throws Throws AssertionFailure if the assertion fails*/
   template <typename Boolable>
      requires std::convertible_to<Boolable, bool>
   void assert_true(Boolable assertion, const std::string & onFail = "")
   {
      if (!assertion)
         throw BooleanAssertionFailure(onFail);
   }

   /** Assert that the given boolean value is false.
    * @param assertion: The boolean value to check
    * @param onFail: String for AssertionFailure, if the assertion fails. Defaults to ""
    * @throws Throws AssertionFailure if the assertion fails */
   template <typename Boolable>
      requires std::convertible_to<Boolable, bool>
   void assert_false(Boolable assertion, const std::string & onFail = "")
   {
      if (assertion)
         throw BooleanAssertionFailure(onFail);
   }

   /** Assert that x is equal to y
    * @param onFail: String for AssertionFailure, if the assertion fails. Defaults to ""
    * @throws Throws AssertionFailure if x and y are not equal. */
   template <typename T, typename U>
      requires std::equality_comparable_with<T, U>
   void assert_equals(T x, U y, const std::string & onFail = "") 
   {
      if (x != y)
         throw EqualityAssertionFailure(x, y, onFail);
   }

   /** Assert that x is not equal to y
    * @param onFail: String for AssertionFailure, if the assertion fails. Defaults to ""
    * @throws Throws AssertionFailure if x and y are equal. */
   template <typename T, typename U>
      requires std::equality_comparable_with<T, U>
   void assert_noteq(T x, U y, const std::string & onFail = "")
   {
      if (x == y)
         throw EqualityAssertionFailure(x, y, onFail);
   }

   /** Assert, that the given function throws instance of ErrorType. 
    * @param ErrorType: The expected error type.
    * @param function: Any callable function
    * @param args... Arguments to call function with
    * @throw Throws AssertionFailure, if the function either doesn't throw an instance of ErrorType, or throws an error instance, that is not of type ErrorType. */
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
            + std::string(typeid(E).name()) + "\". Instead, it threw error with message: \"" 
            + std::string(e.what()) + "\"");
      } catch (...) {
         throw AssertionFailure("Function threw unknown error.");
      }  

      // Function didn't throw!
      throw AssertionFailure("Function didn't throw error.");
   }
};