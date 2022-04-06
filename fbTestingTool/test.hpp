#pragma once

#include "invoke.hpp"
#include "assert.hpp"
#include "terminalColor.hpp"

namespace fbtt {

   enum TestResult {
      NORUN, // test hasn't been executed yet
      PASSED, 
      ASSERTION_FAILURE, // test failed in assertion
      UNWANTED_ERROR
   };

   template<typename ... Args>
   class Test {
   public:
      // for errors in this error detection
      typedef std::runtime_error Error;

      template <typename Function>
         requires storable_function<Function, void, Args...>
      Test(const std::string & testName, Function func)
         : m_testName { testName }, 
         m_userFunc { std::function<void(Args...)> ( func ) }
      {

      };

      void run(Args ... args) {
         if (m_userFunc == nullptr)
            throw Test::Error("Test function undefined.");
         
         try {
            m_userFunc(args...);

            // function didn't throw -> pass
            m_result = TestResult::PASSED;
         
         } catch (AssertionError & e) {
            m_result = TestResult::ASSERTION_FAILURE;

            std::string what = e.what();
            m_failReason = what == "" ? "Not specified" : what; 
            
         } catch (std::exception & e) {
         
            m_result = TestResult::UNWANTED_ERROR;
            m_failReason = "Typeid: " + (std::string) typeid(e).name() + ", message: " + (std::string) e.what();
         }
      }

      std::string getReport() {
         std::string result = "Test \"" + m_testName + "\" ";
         switch(m_result) {
            case TestResult::NORUN:
               result += "has not been run yet.";
               return result;
            case TestResult::PASSED:
               result += "passed.";
               return result;
            case TestResult::ASSERTION_FAILURE:
               result += "failed in assertion:\n   Reason: " + m_failReason;
               return result;
            case TestResult::UNWANTED_ERROR:
               result += "failed because of unwanted error: " + m_failReason; 
               return result;
            default:
               throw Test::Error("Unknown test result.");
         };
      }

   private:
      template <typename ... Classes>
      friend std::ostream & operator << (std::ostream &, const Test<Classes...> &);

      std::function<void(Args...)> m_userFunc = nullptr;

      std::string m_testName;
      TestResult m_result = NORUN;
      std::string m_failReason = "";
   };


template <typename ... Classes>
std::ostream & operator << (std::ostream & os, const Test<Classes...> & test)
{
   os << "TEST " 
      << TerminalStyle::BOLD << TerminalColor::BLUE
      << test.m_testName 
      << TerminalColor::RESET << TerminalStyle::NONE << ' ';

   switch(test.m_result) {
      case TestResult::NORUN:
         os << TerminalColor::YELLOW
            << "has not been run yet."
            << TerminalColor::RESET;
         break;
      case TestResult::PASSED:
         os << TerminalColor::GREEN << TerminalStyle::BOLD
            << "passed."
            << TerminalColor::RESET << TerminalStyle::NONE;
         break;
      case TestResult::ASSERTION_FAILURE:
         os << TerminalColor::RED << TerminalStyle::BOLD
            << "failed in assertion.\n   Reason: "
            << TerminalColor::RESET << TerminalStyle::NONE
            << test.m_failReason;
         break;
      case TestResult::UNWANTED_ERROR:
         os << TerminalColor::RED << TerminalStyle::BOLD
            << "failed because of unexpected error: " 
            << TerminalColor::RESET << TerminalStyle::NONE
            << test.m_failReason;
         break;
         break;
   };
   return os;
}
};
