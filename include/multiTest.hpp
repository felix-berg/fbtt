#pragma once

#include "test.hpp"

#include <tuple>
#include <functional>
#include <iostream>

namespace fbtt {
   struct NoConstructor : public std::runtime_error  {
      NoConstructor() 
         : std::runtime_error { "No constructor defined! Define a constructor with MultiTest::add_constructor(). "} { };
   };

   struct UndefinedInstance : public std::runtime_error {
      UndefinedInstance()
         : std::runtime_error { "One of the instances of the multitest is undefined! "} { };
   };

   template <typename ... Classes>
   class MultiTest {
      std::tuple<Classes * ...> m_instanceTuple;
      
      std::vector<std::function<void(Classes * & ...)>> m_constructors;
      std::vector<std::string> m_constructorNames;
      std::vector<AnyTest<Classes & ...> *> m_tests;
      std::vector<TestResult> m_testResults;
      
      std::string m_name;
      bool finished = false;

      template <typename ... Cls>
      friend void add_default_constructor_to_multitest(MultiTest<Cls...> &);

      template <typename ... Cls>
      friend std::ostream & operator << (std::ostream & os, const MultiTest<Cls...> & multiTest);

      std::function<bool(Classes * ... instances)> check_instances = 
         [](Classes * ... instances) -> bool
      {
         return ((... && (instances != nullptr)));
      };

      std::function<void(Classes * ... instances)> destruct = 
         [](Classes * ... instances)
      {
         ((delete instances), ...);
      };

   public:
      MultiTest(const std::string & name)
         : m_name { name }
      {
         bool allDefaultible = true;

         // check for default_initializable in every class
         ((allDefaultible = allDefaultible && std::default_initializable<Classes>
         ), ...);

         if (allDefaultible)
            add_default_constructor_to_multitest(*this);
         
      }

      MultiTest()
         : m_name { "Unnamed" } { };

      ~MultiTest()
      {
         for (auto t : m_tests)
            delete t;
      }

      template <typename Func>
         requires StorableFunction<Func, void, Classes * & ...>
      void add_constructor(const std::string & name, Func && constructor)
      {
         m_constructors.push_back(
            static_cast<std::function<void(Classes * &...)>> (constructor));
         m_constructorNames.push_back(name);
      }

      template <ErrorType E = NoError, typename Func>
         requires StorableFunction<Func, void, Classes & ...>
      void add_test(const std::string & testName, Func && func)
      {  
         AnyTest<Classes &...> * t = new Test<E, Classes & ...>(testName, func);
         m_tests.push_back(t);
      }

      void run()
      {
         if (m_constructors.size() == 0)
            throw NoConstructor();

         for (size_t i = 0; i < m_constructors.size(); i++) {
            for (size_t j = 0; j < m_tests.size(); j++) {
               std::apply(m_constructors[i], m_instanceTuple);
               std::apply(check_instances, m_instanceTuple);

               std::apply([&](Classes * ... instances) {
                  m_tests[i]->run(*instances...);
                  m_testResults.push_back(m_tests[i]->result());
               }, m_instanceTuple);

               std::apply(destruct, m_instanceTuple);
            }
         }
         finished = true;
      }
   };

   template <typename ... Cls>
   concept VariadicDefaultInitializable = ((... && std::default_initializable<Cls>));

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
         os << TerminalColor::RED
            << "### " 
            << TerminalColor::WHITE << TerminalStyle::NONE
            << "Constructor " << consi << ": \""
            << TerminalColor::CYAN  << TerminalStyle::BOLD 
            << multiTest.m_constructorNames[consi] 
            << TerminalColor::WHITE << TerminalStyle::NONE << "\"" 
            << TerminalColor::RED   << TerminalStyle::BOLD
            << " ###\n";

         for (size_t testi = 0; testi < multiTest.m_tests.size(); testi++)
         {
            int resultid = testi + consi * multiTest.m_testResults.size();

            const TestResult & res = multiTest.m_testResults.at(resultid);

            os << TerminalColor::WHITE << TerminalStyle::NONE
               << "   TEST " 
               << TerminalColor::BLUE << TerminalStyle::BOLD
               << "\"" << res.testName << "\" " 
               << (res.test_failed() ? 
                     TerminalColor::RED :
                     TerminalColor::GREEN)
               << res.status();

            if (res.test_failed()) {
               os << TerminalColor::WHITE << TerminalStyle::NONE 
                  << "\n      Reason: "
                  << TerminalColor::YELLOW
                  << res.failString << '\n';
            }
         }
      }
   
      return os;
   }
};