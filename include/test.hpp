#pragma once

#include "assertionFailure.hpp"
#include "errorConcepts.hpp"
#include "functionConcepts.hpp"
#include "terminalColor.hpp"

#include <string>

namespace fbtt {
   struct TestResult {
      enum Status {
         NOT_RUN,
         PASSED, 
         ASSERTION_FAILURE,
         UNEXPECTED_ERROR,
         DIDNT_THROW_EXPECTED,
         UNKNOWN_FAILURE
      };

      const std::string testName;
      const Status statusCode;
      const std::string failString = "";

      bool test_failed() const;
      std::string status() const;
      std::string report() const;
   };

   // abstract base class for any test with any error type
   // used, so multiclasstest can have multiple different
   // types of tests in a single vector of <AnyTest *>
   template <typename ... TestArgs>
   class AnyTest { 
   public:
      AnyTest() { };
      virtual void run(TestArgs...) = 0;
      virtual const std::string & name() const = 0;
      virtual TestResult result() const = 0;
   };

   template <OptionalError ExpectedError = NoError, typename ... TestArgs>
   class Test : public AnyTest<TestArgs...> {
      const std::function<void(TestArgs...)> m_function;
      const std::string m_name;


      std::string m_failureString = "";
      TestResult::Status m_statusCode = TestResult::Status::NOT_RUN;

   public:
      /** Construct a new test around a new name and any storable function.
       * @param name: Name for constructed test.
       * @param func: Any storable function (e.g. function, lambda, std::function, non-static member function...) */
      template <typename Func>
         requires StorableFunction<Func, void, TestArgs...>
      Test(const std::string & testName, Func && function) 
         : m_function { static_cast<std::function<void(TestArgs...)>> ( function ) },
           m_name { testName } { };

      /** Run test 
       * @param args... Arguments to run test with. Will most likely be (). */
      void run(TestArgs ... args) noexcept
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
            m_failureString = "threw unexpected error of type: " + std::string(typeid(decltype(e)).name());

         } catch (...) {
            // caught error, that is not derived from std::exception -> unkown failure
            m_statusCode = TestResult::Status::UNKNOWN_FAILURE;
            m_failureString = "caught error, that is not derived from std::exception";
         }
      
      }

      /** @returns Name of test */
      const std::string & name() const { return m_name; };

      TestResult result() const 
      {
         return { name(), m_statusCode, m_failureString };
      }
   };

   std::ostream & operator << (std::ostream & os, const TestResult & res);

   template <typename ... A>
   std::ostream & operator << (std::ostream & os, const AnyTest<A...> & test)
   {
      return os << test.result();
   }
};