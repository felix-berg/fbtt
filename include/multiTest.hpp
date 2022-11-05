#pragma once

#include "test.hpp"

#include <tuple>
#include <functional>
#include <vector>
#include <iostream>
#include <iomanip>

namespace fbtt {
   /** Error thrown by MultiTest, when tests are executed, and no constructor is defined. */
   struct NoConstructor : public std::runtime_error  {
      NoConstructor() 
         : std::runtime_error { "No constructor defined! Define a constructor with MultiTest::add_constructor(). "} { };
   };

   /** Error thrown by MultiTest, when an instance in the MultiClass is undefined, after a constructor has been called. */
   struct UndefinedInstance : public std::runtime_error {
      UndefinedInstance()
         : std::runtime_error { "One of the instances of the multitest is undefined! "} { };
      UndefinedInstance(const std::string & s)
         : std::runtime_error { "An instance is nullptr after constructor: " + s } { };
   };

   /** MultiTest class. Class for testing 0 or more classes. Constructs class with either default or user-defined (by add_constructor) constructor, and runs every test with the constructed instance[s]. 
    * @param add_test(): Add test with a name and storable function, that takes references to instances of "Classes..."
    * @param add_constructor(): Add constructor to be run before every test. Default constructor is automatically added, if every type in "Classes..." is default constructible.
    * @param run(): Run tests.
   */
   template <typename ... Classes>
   class MultiTest {
      std::tuple<Classes * ...> m_instanceTuple;
      
      std::vector<std::function<void(Classes * & ...)>> m_constructors;
      std::vector<std::string> m_constructorNames;
      std::vector<AbstractTest<Classes & ...> *> m_tests;
      std::vector<TestResult> m_testResults;
      
      std::string m_name;
      bool finished = false;

      template <typename ... Cls>
      friend void add_default_constructor_to_multitest(MultiTest<Cls...> &);

      template <typename ... Cls>
      friend std::ostream & operator << (std::ostream & os, const MultiTest<Cls...> & multiTest);

      std::function<bool(Classes * ... instances)> instances_are_nullptr = 
         [](Classes * ... instances) -> bool
      {
         return ((... || (instances == nullptr)));
      };

      std::function<void(Classes * ... instances)> destruct = 
         [](Classes * ... instances)
      {
         ((delete instances), ...);
      };

      std::function<void(Classes * & ... instances)> set_instances_to_null =
         [](Classes * & ... instances) 
      {
         ((instances = nullptr), ...);
      };

   public:
      MultiTest(const std::string & name)
         : m_name { name }
      { }

      MultiTest()
         : m_name { "Unnamed" } { };

      ~MultiTest()
      {
         for (auto t : m_tests)
            delete t;
      }

      /** Add constructor to test. A default constructor is added if possible, but is removed, if a constructor is added by user. 
       * @param name: Name of constructor
       * @param constructor: Pointer to storable function with signature void(Classes * & ...) (std::function, function pointer, lambda, non-static member-function...) */
      void add_constructor(const std::string & name, std::function<void(Classes * & ...)> && constructor)
      {
         m_constructors.push_back(
            static_cast<std::function<void(Classes * &...)>> (constructor));
         m_constructorNames.push_back(name);
      }

      /** Add test to multitest.
       * @param E: Type of error to expect from the test
       * @param func: Pointer to storable function with signature void(Classes &...) (std::function, function pointer, lambda, non-static member-functio...) */
      template <ErrorType E = NoError>
      void add_test(const std::string & testName, std::function<void(Classes &...)> func)
      {  
         AbstractTest<Classes &...> * t = new Test<E, Classes & ...>(testName, func);
         m_tests.push_back(t);
      }

      /** Run and evaluate all tests. */
      void run()
      {
         if (m_constructors.size() == 0) {
            if (VariadicDefaultInitializable<Classes...>)
               add_default_constructor_to_multitest(*this);
            else
               throw NoConstructor();
         }

         for (size_t i = 0; i < m_constructors.size(); i++) {
            for (size_t j = 0; j < m_tests.size(); j++) {
               std::apply(set_instances_to_null, m_instanceTuple);

               std::apply(m_constructors[i], m_instanceTuple);

               std::apply([&](Classes * ... instances) {
                  if (instances_are_nullptr(instances...))
                     throw UndefinedInstance(m_constructorNames[i]);
               }, m_instanceTuple);

               std::apply([&](Classes * ... instances) {
                  m_tests[j]->run(*instances...);
               }, m_instanceTuple);

               // push back result of test
               m_testResults.push_back(m_tests[j]->result());

               std::apply(destruct, m_instanceTuple);
            }
         }
         finished = true;
      }
   };

   // for multi tests, where every class is default initializable
   template <typename ... Cls>
      requires VariadicDefaultInitializable<Cls...>
   void add_default_constructor_to_multitest(MultiTest<Cls...> & mt)
   {
      mt.add_constructor("Default constructor", [](Cls * & ... instances) {
         ((instances = new Cls { }), ...);
      });
   }

   // for multi tests, where not every class is default initializable
   template <typename ... Cls>
      requires (!VariadicDefaultInitializable<Cls...>)
   void add_default_constructor_to_multitest(MultiTest<Cls...> & mt)
   { }

   template <typename ... Classes>
   std::ostream & operator << (std::ostream & os, const MultiTest<Classes...> & multiTest)
   {
      if (!multiTest.finished) {
         return os << "Test \"" << multiTest.m_name << "\" is not finished.";
      }

      os << TerminalColor::WHITE <<  TerminalStyle::BOLD  
         << "Summary of multi test: \"" + multiTest.m_name << "\"\n";
      
      for (size_t consi = 0; consi < multiTest.m_constructors.size(); consi++) { 
         os << TerminalColor::WHITE << TerminalStyle::NONE
            << "With constructor: \""
            << TerminalColor::CYAN  << TerminalStyle::BOLD 
            << multiTest.m_constructorNames[consi] 
            << TerminalColor::WHITE << TerminalStyle::NONE << "\":\n";

         for (size_t testi = 0; testi < multiTest.m_tests.size(); testi++)
         {
            int resultid = testi + consi * multiTest.m_tests.size();

            const TestResult & res = multiTest.m_testResults.at(resultid);

            os << TerminalColor::WHITE << TerminalStyle::NONE
               << "   TEST " 
               << std::setw(2) << testi << " "
               << (res.test_failed() ? 
                     TerminalColor::RED :
                     TerminalColor::GREEN)
               << res.status()
               << TerminalColor::GRAY
               << " - "
               << TerminalColor::BLUE << TerminalStyle::BOLD
               << "\"" << res.testName << "\"";

            if (res.test_failed()) {
               os << TerminalColor::WHITE << TerminalStyle::NONE 
                  << "\n      Reason: "
                  << TerminalColor::YELLOW << TerminalStyle::BOLD
                  << res.failString;
            }
            os << '\n' << TerminalColor::WHITE << TerminalStyle::NONE;
         }
      }
   
      return os;
   }
};