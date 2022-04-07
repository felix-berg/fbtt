#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <functional>

#include "test.hpp"

namespace fbtt {
   template <typename ... Classes>
   class ClassTest {
   public:
      ClassTest()
      { }

      /** Function, that can be defined to change the behavior of this ClassTest instance, 
       * when the Class instances need to be constructed. The arguments to the function are references to the pointers
       * to the class instances. (Class * & inst). 
       * By default, the class instances are constructed on the heap with default values. 
       * Instance must be created with new. */
      std::function<void(Classes * &...)> construction = 
         [](Classes * & ... instances) 
         { 
            ((instances = new Classes { }), ...);
         };

      /** Function, that can be defined to change the behavior of this ClassTest instance, 
       * when the Class instances need to be destructed. The arguments to the function are pointers
       * to the class instances. (Class * & inst) */
      std::function<void(Classes *...)> destruction = 
         [](Classes * ... instances) 
         {
            ((delete instances), ...);
         };


      /** Add test 
       * @param name: Wanted name for test
       * @param func: The function to be called by the test. Requires that the arguments are references to the test classes. */
      template <typename Function>
         requires storable_function<Function, void, Classes & ...>
      void add_test(const std::string & name, Function func)
      {
         m_tests.push_back(Test<Classes &...> {name, func});
      }

      /** Run every test added to this instance. 
       * Result of test can be gotten with ClassTest::report() or through using std::ostream::operator<<. */
      void run()
      {
         for (size_t i = 0; i < m_tests.size(); i++) {
            std::apply(construction, m_instanceTuple);

            std::apply([&](Classes * ... instances) {
               m_tests[i].run(*instances...);
            }, m_instanceTuple);

            std::apply(destruction, m_instanceTuple);
         }

      }

      /** @returns Report of this class test. */
      std::string report() 
      {
         std::ostringstream result = "";
         result << "Summary of class test: ";
         
         ((result << typeid(Classes).name()) << " ", ...); 
         result << '\n';

         for (size_t i = 0; i < m_tests.size(); i++) {
            result << "   " << m_tests[i].report() << '\n';
         }

         return result.str();
      }


   private:
      std::tuple<Classes * ...> m_instanceTuple;
      std::vector<Test<Classes & ...>> m_tests; 

      template <typename ... S>
      friend std::ostream & operator << (std::ostream &, const fbtt::ClassTest<S...> &);

   };


   /** Output formatted test report to given ostream. */
   template <typename ... Classes>
   std::ostream & operator << (std::ostream & os, const ClassTest<Classes...> & classTest)
   {
      os << fbtt::TerminalStyle::BOLD
         << "Summary of class test: "
         << fbtt::TerminalColor::BRIGHT_YELLOW;
      ((os << typeid(Classes).name()) << " ", ...);
      os << fbtt::TerminalColor::RESET << fbtt::TerminalStyle::NONE
         << '\n';
      for (size_t i = 0; i < classTest.m_tests.size(); i++) {
         os << "   " << classTest.m_tests[i] << '\n';
      }
      return os;
   }

};

