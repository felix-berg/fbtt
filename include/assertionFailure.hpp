#pragma once

#include <exception>
#include <sstream>
#include <string>

namespace fbtt {
   class AssertionFailure : public std::exception {
   protected:
      std::string m_msg;
   public:
      AssertionFailure(const std::string & msg)
         : m_msg { msg } { };

      virtual const char * what() const noexcept
      {
         return m_msg.c_str();
      };
   };

   struct BooleanAssertionFailure : public AssertionFailure {
      BooleanAssertionFailure(const std::string & msg) 
         : AssertionFailure { msg + " (boolean assertion)" }
      { };
   };

      template <typename T>
   concept ostringstreamOutput = 
      requires (std::ostringstream & os, T t) { os << t; };

   template <typename T, typename U>
      requires ostringstreamOutput<T> 
            && ostringstreamOutput<U>
   void add_equality_string_to_string(std::string & s, const T & x, const U & y)
   {
      std::ostringstream res { };
      res << " (" << x << ((x == y) ? " == " : " != ") << y << ")";
      s += res.str();
   }

   template <typename T, typename U>
      requires (!ostringstreamOutput<T> || !ostringstreamOutput<U>)
   void add_equality_string_to_string(std::string & s, const T &, const U &)
   {
      s += "(equality assertion)";
   }

   template <typename T, typename U>
   struct EqualityAssertionFailure : public AssertionFailure {
      EqualityAssertionFailure(const T & x, const U & y, const std::string & msg)
         : AssertionFailure("") 
      {
         add_equality_string_to_string(m_msg, x, y);
      }
   };

   template <typename Error>
   struct ThrowingAssertionFailure : public AssertionFailure {
      ThrowingAssertionFailure()
         : AssertionFailure { "Function didn't throw expected error type. Threw error with type: " + (std::string) typeid(Error).name()} { }
   };
};