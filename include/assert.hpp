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
   void assertTrue(Boolable assertion, const std::string & onFail = "")
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
   void assertFalse(Boolable assertion, const std::string & onFail = "")
   {
      if (assertion)
         throw BooleanAssertionFailure(onFail);
   }

   /** Assert that x is equal to y
    * @param onFail: String for AssertionFailure, if the assertion fails. Defaults to ""
    * @throws Throws AssertionFailure if x and y are not equal. */
   template <typename T, typename U>
      requires (std::equality_comparable<T> && std::convertible_to<T, U>)
   void assertEquals(const T & x, const U & y, const std::string & onFail = "")
   {
      if (x != T(y))
         throw EqualityAssertionFailure(x, T(y), onFail);
   }

   /** Asserts that x is approximately equal to y -> within given margin (defaults to 0.0001f).
    * @param onFail: String for AssertionFailure, if the assertion fails. Defaults to ""
    * @throws Throws AssertionFailure if x is not within the margin of y. */
   template <typename T, typename U>
      requires (std::three_way_comparable<T> && std::convertible_to<T, U>)
   void assertApprox(const T & x, const U & y, const std::string & onFail = "", T margin = 0.0001f)
   {
      if (x < T(y) - margin || T(y) + margin < x) 
         throw EqualityAssertionFailure(x, T(y), onFail);
   }

   /** Assert that x is not equal to y
    * @param onFail: String for AssertionFailure, if the assertion fails. Defaults to ""
    * @throws Throws AssertionFailure if x and y are equal. */
   template <typename T, typename U>
      requires (std::equality_comparable<T> && std::convertible_to<T, U>)
   void assertNeq(const T & x, const U & y, const std::string & onFail = "")
   {
      if (x == T(y))
         throw EqualityAssertionFailure(x, T(y), onFail);
   }

   /** Assert, that the given function throws instance of ErrorType. 
    * @param ErrorType: The expected error type.
    * @param function: Any callable function
    * @param args... Arguments to call function with
    * @throw Throws AssertionFailure, if the function either doesn't throw an instance of ErrorType, or throws an error instance, that is not of type ErrorType. */
   template <ErrorType E, typename Func, typename ... Args>
      requires CallableWith<Func, Args...>
   void assertThrows(Func f, Args ... args)
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
