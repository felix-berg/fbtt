#pragma once

#include "invoke.hpp"
#include <string>
#include <sstream>
#include <exception>

class AssertionError {
public:
   AssertionError() { };
   
   AssertionError(const std::string & str)
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


template <typename T>
   requires std::equality_comparable<T>
class EqualityAssertionError : public AssertionError {
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

/** Assert that the given boolean value is true. */
void assert_true(bool assertion, const std::string & onFail = "") 
{
   if (!assertion)
      if (onFail == "")
         throw AssertionError();
      else 
         throw AssertionError(onFail);
}

/** Assert that the given boolean value is false. */
void assert_false(bool assertion, const std::string & onFail = "")
{
   assert_true(!assertion, onFail);
}

template<typename T>
   requires std::equality_comparable<T>
void assert_equals(T x, T y, const std::string & onFail = "")
{
   if (x != y)
      throw EqualityAssertionError<T>(true, x, y, onFail);
}

template <typename T>
   requires std::equality_comparable<T>
void assert_noteq(T x, T y, const std::string & onFail = "") {
   if (x == y)
      throw EqualityAssertionError<T>(false, x, y, onFail);
}


template <typename ErrorType, typename Function, typename ... Args>
   requires 
      is_error<ErrorType>
         &&
      Callable<Function, Args...>
         &&
      arguments_to<Function, Args...>
         &&
      (!std::same_as<AssertionError, ErrorType>)
void assert_throws(Function function, Args... args)
{
   try {
   
      // try to run function, check for the wanted errortype
      try {
         invoke(function, args...);
         // function did not throw error -> assertion failed
         throw AssertionError();
      } catch (ErrorType & e) {
         // good -> assertion success.
         return;
      }

   } catch (AssertionError & e) {
      // catch the wilfully thrown assertion error,
      // which indicates a successful run.

      throw AssertionError("(Function didn't throw an error)");  

   } catch (std::exception & e) {
      // catch any unexpected error thrown by userFunc

      throw AssertionError(
         (std::string) "Function threw unexpected error: " + std::string(e.what()));
      // we made it out -> no error thrown. Assertion failed.
   }
}

template <typename ErrorType, typename Function, typename ... Args>
   requires 
      is_error<ErrorType>
         &&
      Callable<Function, Args...>
         &&
      arguments_to<Function, Args...>
void assert_throws_message(const std::string & errorMsg, Function function, Args ... args)
{
   try {
      invoke(function, args...);
      // function did not throw error -> assertion failed
      throw AssertionError();
   } catch (ErrorType & e) {
      std::string gottenString = std::string(e.what());
      if (gottenString != errorMsg)
         throw AssertionError("Function didn't throw corrent error string.\n   Expected: \"" + errorMsg + "\",\n   got: \"" + gottenString + "\"");
      else
         return;
   } catch (AssertionError & e) {
      throw AssertionError("(Function didn't throw an error)");
   } catch (std::exception & e) {
      throw AssertionError(
         (std::string) "Function threw unexpected error: " + std::string(e.what()));
      // we made it out -> no error thrown. Assertion failed.
   }
}