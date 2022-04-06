#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <functional>

#include "test.hpp"

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
   friend std::ostream & operator << (std::ostream &, const ClassTest<S...> &);
};

template <typename ... Classes>
std::ostream & operator << (std::ostream & os, const ClassTest<Classes...> & classTest)
{
   os << TerminalStyle::BOLD
      << "Summary of interaction test: "
      << TerminalColor::BRIGHT_YELLOW;
   ((os << typeid(Classes).name()) << " ", ...);
   os << TerminalColor::RESET << TerminalStyle::NONE
      << '\n';
   for (size_t i = 0; i < classTest.m_tests.size(); i++) {
      os << "   " << classTest.m_tests[i] << '\n';
   }
   return os;
}