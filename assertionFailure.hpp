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
   struct EqualityAssertionFailure : public AssertionFailure {
      EqualityAssertionFailure(T x, U y, const std::string & msg)
         : AssertionFailure("") 
      {
         if (ostringstreamOutput<T> && ostringstreamOutput<U>) {
            std::ostringstream res { msg };
            res << " (" << x << ((x == y) ? " == " : " != ") << y << ")";
            m_msg = res.str();
         } else {
            m_msg = msg + "(equality assertion)";
         }
      }
   };

   template <typename Error>
   struct ThrowingAssertionFailure : public AssertionFailure {
      ThrowingAssertionFailure()
         : AssertionFailure { "Function didn't throw expected error type. Threw error with type: " + (std::string) typeid(Error).name()} { }
   };
};