#pragma once

#include "assert.hpp"
#include "errorConcepts.hpp"
#include "functionConcepts.hpp"
#include "terminalColor.hpp"

#include <string>

namespace fbtt {
   template <OptionalError ExpectedError = NoError, typename ... TestArgs>
   class Test {
      const std::function<void(TestArgs...)> m_function;
      const std::string m_name;
      std::string m_failureString = "";

      enum Status {
         NOT_RUN,
         PASSED, 
         ASSERTION_FAILURE,
         UNEXPECTED_ERROR,
         DIDNT_THROW_EXPECTED,
         UNKNOWN_FAILURE
      };

      Status m_statusCode;

   public:
      template <typename Func>
         requires StorableFunction<Func, void, TestArgs...>
      Test(const std::string & testName, Func function) 
         : m_function { static_cast<std::function<void(TestArgs...)>> ( function ) },
           m_name { testName } { };

      /** Run test. Arguments to test function are supplied through args.*/
      void run(TestArgs ... args) noexcept
      {
         try {
            // try running function
            m_function(args...);
            
            // --- function didn't throw error ---
            // if ExpectedError is void, test passed
            if (std::same_as<ExpectedError, NoError>) {
               m_statusCode = PASSED;
            } else {
               // if ExpectedError is not void -> we didn't recieve the error, we were expecting
               m_statusCode = DIDNT_THROW_EXPECTED;
               m_failureString = "didn't throw error of type: " + std::string(typeid(ExpectedError).name());
            }
         
         } catch (AssertionFailure & e) {
            // test threw assertion failure 
            m_statusCode = ASSERTION_FAILURE;
            m_failureString = std::string(e.what());
         } catch (ExpectedError & expected) {
            // function threw expected error -> pass!
            m_statusCode = PASSED;
         } catch (std::exception & e) {
            // function threw unexpected error -> fail
            m_statusCode = UNEXPECTED_ERROR;
            m_failureString = "threw unexpected error of type: " + std::string(typeid(decltype(e)).name());

         } catch (...) {
            // caught error, that is not derived from std::exception -> unkown failure
            m_statusCode = UNKNOWN_FAILURE;
            m_failureString = "caught error, that is not derived from std::exception";
         }
      
      }

      /** @returns Name of test */
      const std::string & name() const { return m_name; };

      /** @returns true, if test failed, false otherwise.*/
      bool failed() const { 
         return !(m_statusCode == PASSED || m_statusCode == NOT_RUN);
      };

      /** @returns Status of test. */
      std::string status() const {
         switch (m_statusCode) {
            case PASSED:
               return "passed";
            case NOT_RUN:
               return "hasn't been executed";
            case ASSERTION_FAILURE:
               return "failed in assertion";
            case UNEXPECTED_ERROR:
               return "threw unexpected error";
            case DIDNT_THROW_EXPECTED:
               return "didn't throw expected error";
            default:
               return "unknown failure";
         };
      };

      /** @returns Reason for failure or "", if test didn't fail. */
      const std::string & reason() const {
         return m_failureString;
      }

      /** @returns Formatted report of test run. */
      std::string report() const {
         return "TEST \"" + name() + "\" " + (failed() ? "✕" : "✓") + " " + status() + ". " + (failed() ? "Reason: " + reason() : "");
      }
   };


   template <typename E, typename ... A>
   std::ostream & operator << (std::ostream & os, const Test<E, A...> & test)
   {
      os << TerminalStyle::NONE << "TEST "
         << TerminalColor::BRIGHT_BLUE << TerminalStyle::BOLD << test.name() << ' '
         << (test.failed() ? TerminalColor::RED : TerminalColor::GREEN)
         << test.status();

      if (test.failed()) {
         os << TerminalStyle::NONE << TerminalColor::RESET
            << "\n   Reason: "
            << TerminalColor::YELLOW
            << test.reason();
      }

      return os << '\n' << TerminalColor::RESET << TerminalStyle::NONE;
   }
};