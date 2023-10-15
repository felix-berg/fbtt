#pragma once

#include "assertionFailure.hpp"
#include "errorConcepts.hpp"
#include "functionConcepts.hpp"
#include "terminalColor.hpp"

#include <string>

namespace fbtt {
   /** Container for result of a test. Gotten with Test::Result
    * @param testName: Name of test
    * @param failString: Error message for test
    * @param testFailed(): True, if test failed, false otherwise
    * @param status(): String indicating the status of the test
    * @param report(): Combined information about the testresult
   */
   struct TestResult {
      enum Status {
         NOT_RUN,
         PASSED, 
         ASSERTION_FAILURE,
         UNEXPECTED_ERROR,
         DIDNT_THROW_EXPECTED,
         UNKNOWN_FAILURE
      };

      const std::string testName; // name of the test, this is the result for
      const Status statusCode; // status code for test 
      const std::string failString = ""; // reason for potential test failure

      bool testFailed() const;
      std::string status() const;
      std::string report() const;
   };

   // abstract base class for any test with any error type
   // used, so multiclasstest can have multiple different
   // types of tests in a single vector of <AbstractTest *>
   template <typename ... TestArgs>
   class AbstractTest { 
   public:
      AbstractTest() { };
      virtual void run(TestArgs...) = 0;
      virtual const std::string & name() const = 0;
      virtual TestResult result() const = 0;
      virtual ~AbstractTest() { };
   };

   /** Basic test class.
    * @param Test(): Constructor, where a name and function is given.
    * @param run(): Run and evaluate result of test
    * @param result(): Return result of test : TestResult 
    * @param name(): Returns name of test */
   template <OptionalError ExpectedError = NoError, typename ... TestArgs>
   class Test : public AbstractTest<TestArgs...> {
      const std::function<void(TestArgs...)> m_function;
      const std::string m_name;

      std::string m_failureString = "";
      TestResult::Status m_statusCode = TestResult::Status::NOT_RUN;

   public:
      /** Construct a new test around a new name and any storable function.
       * @param name: Name for constructed test. */
      Test(const std::string & testName, std::function<void(TestArgs...)> function) 
         : m_function { function },
           m_name { testName } { };
      
      /** Construct a new test around a new name and any storable function.
       * @param name: Name for constructed test.
       * @param func: Any storable function (e.g. function, lambda, std::function, non-static member function...) */
      template <typename Function>
         requires StorableFunction<Function, void, TestArgs...>
      Test(const std::string & testName,
      Function && func)
         : m_function { static_cast<std::function<void(TestArgs...)>> (func) },
           m_name { testName } { };

      Test (const Test<ExpectedError, TestArgs...> &) = delete;
      Test & operator = (const Test<ExpectedError, TestArgs...> &) = delete;
      Test (Test<ExpectedError, TestArgs...> &&) = default;
      Test & operator = (Test<ExpectedError, TestArgs...> &&) = default;

      /** Run test 
       * @param args... Arguments to run test with. Will most likely be (void). */
      virtual void run(TestArgs ... args) noexcept
      {
         try {
            // try running function
            m_function(args...);
            
            // --- function didn't throw error ---
            // if ExpectedError is void, test passed
            if (std::same_as<ExpectedError, NoError>) {
               m_statusCode = TestResult::Status::PASSED;
            } else {
               // if ExpectedError is not void -> we didn't recieve the error, we were expecting
               m_statusCode = TestResult::Status::DIDNT_THROW_EXPECTED;
               m_failureString = "didn't throw error of type: " + std::string(typeid(ExpectedError).name());
            }
         
         } catch (AssertionFailure & e) {
            // test threw assertion failure 
            m_statusCode = TestResult::Status::ASSERTION_FAILURE;
            m_failureString = std::string(e.what());
         } catch (ExpectedError & expected) {
            // function threw expected error -> pass!
            m_statusCode = TestResult::Status::PASSED;
         } catch (std::exception & e) {
            // function threw unexpected error -> fail
            m_statusCode = TestResult::Status::UNEXPECTED_ERROR;
            if (std::same_as<ExpectedError, NoError>) {
               m_failureString = "test threw error with message: " + std::string(e.what());
            } else {
               m_failureString = "Type of error is not " + std::string(typeid(ExpectedError).name()) + ". Error message: " + std::string(e.what());
            }
         }
      }

      /** @returns Name of test */
      virtual const std::string & name() const { return m_name; };

      /** @returns Result of test */
      virtual TestResult result() const 
      {
         return { name(), m_statusCode, m_failureString };
      }

      virtual ~Test() { };
   };

   std::ostream & operator << (std::ostream & os, const TestResult & res);

   template <typename ... A>
   std::ostream & operator << (std::ostream & os, const AbstractTest<A...> & test)
   {
      return os << test.result();
   }

   /** @returns True, if test failed, false otherwise */
bool TestResult::testFailed() const
{
   return !(statusCode == PASSED ||
            statusCode == NOT_RUN);
}

/** @returns test status. */
std::string TestResult::status() const
{
   switch (statusCode) {
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

}

   /** @returns Formatted report of test run. */
   std::string TestResult::report() const
   {
      return "TEST \"" + testName + "\" " + (testFailed() ? "✕" : "✓") + " " + status() + ". " + (testFailed() ? "Reason: " + failString : "");
   }

   std::ostream & operator << (std::ostream & os, const TestResult & res)
   {
      os <<  "TEST " << TerminalStyle::BOLD
         << TerminalColor::BRIGHT_BLUE << res.testName << ' '
         << (res.testFailed() ? TerminalColor::RED : TerminalColor::GREEN)
         << res.status();

      if (res.testFailed()) {
         os << TerminalStyle::NONE << TerminalColor::WHITE
            << "\n      Reason: "
            << TerminalColor::YELLOW
            << res.failString;
      }

      return os << '\n'<< TerminalStyle::NONE  << TerminalColor::WHITE;
   }
};