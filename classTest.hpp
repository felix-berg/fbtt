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

      // this function is run once, where instances is a
      // parameter pack of the instances in the m_instanceTuple.
      std::function<void(Classes * &...)> construct = 
         [](Classes * & ... instances) 
         { 
            ((instances = new Classes { }), ...);
         };

      std::function<void(Classes *...)> destruct = 
         [](Classes * ... instances) 
         {
            ((delete instances), ...);
         };

      template <typename Function>
         requires storable_function<Function, void, Classes & ...>
      void add_test(const std::string & name, Function func)
      {
         m_tests.push_back(Test<Classes &...> {name, func});
      }

      void run()
      {

         for (size_t i = 0; i < m_tests.size(); i++) {
            std::apply(construct, m_instanceTuple);

            std::apply([&](Classes * ... instances) {
               m_tests[i].run(*instances...);
            }, m_instanceTuple);

            std::apply(destruct, m_instanceTuple);
         }

      }


   private:
      std::tuple<Classes * ...> m_instanceTuple;
      std::vector<Test<Classes & ...>> m_tests; 

      template <typename ... S>
      friend std::ostream & operator << (std::ostream &, const fbtt::ClassTest<S...> &);
   };
   
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

