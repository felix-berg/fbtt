#pragma once

#include "invoke.hpp"
#include <string>
#include <sstream>
#include <exception>

namespace fbtt {
   // error thrown between assert_ functions.
   class AssertionFailure {
   public:
      AssertionFailure() { };
      
      AssertionFailure(const std::string & str)
         : m_errorStr { str }
      { };
   
      virtual const char * what() const noexcept
      {
         return m_errorStr.c_str();
      }

   protected:
      std::string m_errorStr = "";
   };

   // concept of type T can be output to ostream
   template <typename T>
   concept ostreamOutput = requires (std::ostream & os, T x) { os << x; };

   // error-type specific to assert_equals and assert_noteq
   template <typename T>
      requires std::equality_comparable<T>
   class EqualityAssertionError : public AssertionFailure {
   public:

      EqualityAssertionError(bool wasEquality, T x, T y, const std::string & s)
         : m_equality { wasEquality }, 
         n1 { x }, n2 { y }
         {

            if (!n1 || !n2 || !ostreamOutput<T>)
               m_errorStr = s;
            else {
               std::ostringstream ss { "" };
               ss << " (" << n1 << ' ' << (m_equality ? "!=" : "==") << ' ' << n2 << ')';

               m_errorStr = s + ss.str();
            }
         };

      EqualityAssertionError(bool wasEquality, T x, T y) 
         : EqualityAssertionError { wasEquality, x, y, "" } { };  
      
      virtual const char * what() const noexcept
      {
         return m_errorStr.c_str();
      }
   private:
      T n1, n2;

      bool m_equality; 
   };

   /** Assert that the given boolean value is true.
    * @param assertion: The boolean value to check
    * @param onFail: String for AssertionFailure, if the assertion fails. Defaults to ""
    * @throws Throws AssertionFailure if the assertion fails*/
   void assert_true(bool assertion, const std::string & onFail = "") 
   {
      if (!assertion)
         if (onFail == "")
            throw AssertionFailure();
         else 
            throw AssertionFailure(onFail);
   }

   /** Assert that the given boolean value is false.
    * @param assertion: The boolean value to check
    * @param onFail: String for AssertionFailure, if the assertion fails. Defaults to ""
    * @throws Throws AssertionFailure if the assertion fails */
   void assert_false(bool assertion, const std::string & onFail = "")
   {
      assert_true(!assertion, onFail);
   }

   /** Assert that x is equal to y
    * @param onFail: String for AssertionFailure, if the assertion fails. Defaults to ""
    * @throws Throws AssertionFailure if x and y are not equal. */
   template<typename T, typename U>
      requires std::equality_comparable_with<T, U>
   void assert_equals(T x, U y, const std::string & onFail = "")
   {
      if (x != y)
         throw EqualityAssertionError<T>(true, x, U(y), onFail);
   }

   /** Assert that x is not equal to y
    * @param onFail: String for AssertionFailure, if the assertion fails. Defaults to ""
    * @throws Throws AssertionFailure if x and y are equal. */
   template<typename T, typename U>
      requires std::equality_comparable_with<T, U>
   void assert_noteq(T x, U y, const std::string & onFail = "") {
      if (x == y)
         throw EqualityAssertionError<T>(false, x, U(y), onFail);
   }


   /** Assert, that the given function throws instance of ErrorType. 
    * @param ErrorType: The expected error type.
    * @param function: Any callable function
    * @param args... Arguments to call function with
    * @throw Throws AssertionFailure, if the function either doesn't throw an instance of ErrorType, or throws an error instance, that is not of type ErrorType. */
   template <typename ErrorType, typename Function, typename ... Args>
      requires is_error<ErrorType> && Callable<Function, Args...> &&
               arguments_to<Function, Args...> && (!std::same_as<AssertionFailure, ErrorType>)
   void assert_throws(Function function, Args... args)
   {
      try {
         // try to run function, check for the wanted errortype
         try {
            fbtt::invoke(function, args...);
            // function did not throw error -> assertion failed
            throw AssertionFailure();
         } catch (ErrorType & e) {
            // good -> assertion success.
            return;
         }

      } catch (AssertionFailure & e) {
         // catch the wilfully thrown assertion error,
         // which indicates a successful run.

         throw AssertionFailure("(Function didn't throw error type: \"" + (std::string) typeid(ErrorType).name() + "\")");

      } catch (std::exception & e) {
         // catch any unexpected error thrown by userFunc

         throw AssertionFailure(
            (std::string) "Function threw unexpected error: " + std::string(e.what()));
      }
      // we made it out -> no error thrown. Assertion failed.Q
   }

   /** Assert, that a given function throws an error with given message. 
    * @param errorMsg: The error message to expect through exception::what()
    * @param function: The function to expect error from
    * @param args... The arguments to call the function with
    * @throws Throws instance of AssertionFailure, if the function either doesn't throw an error, or throws an error with the incorrect error message. */
   template <typename Function, typename ... Args>
      requires Callable<Function, Args...> &&
               arguments_to<Function, Args...>
   void assert_throws_message(const std::string & errorMsg, Function function, Args ... args)
   {
      // try to run function, check for the wanted error message
      try {
         fbtt::invoke(function, args...);
         // function did not throw error -> assertion failed
         throw AssertionFailure("Function did not throw an error.");
      } catch (std::exception & e) {
         // catch expected error
         // check if gotten string is the same as
         // as the expected string
         
         std::string gottenString = e.what();
         if (errorMsg == gottenString)
            return;
         else
            throw AssertionFailure("Didn't get the correct error message. Expected: \"" + errorMsg + "\", Got: \"" + gottenString + "\"");
      }
   }

   /** Assert, that the given member function throws instance of ErrorType. Intended use: fbtt::assert_method_throws(obj, &Class::method, arg1, arg2, ...);
    * @param ErrorType: Type of error to expect
    * @param obj: The class instance to call the method upon
    * @param method: A pointer to the method to call
    * @param args... Arguments to call the method with
    * @throw Throws AssertionFailure, if the method either doesn't throw an instance of ErrorType, or throws an error instance, that is not of type ErrorType. */
   template <typename ErrorType, typename Class, typename MethodPtr, typename ... Args>
      requires std::is_member_function_pointer<MethodPtr>::value
   void assert_method_throws(Class & obj, MethodPtr method, Args ... args)
   {
      assert_throws<ErrorType>(std::bind(method, obj, args...));
   }
};