#include <iostream>
#include <sstream>

#include "fbTestingTool.hpp"

typedef std::runtime_error FactError;

int fact(int n)
{
   if (n < 0) 
      throw FactError("Cannot compute factorial of negative number.");
   if (n > 11)
      throw FactError("Integer overflow.");

   if (n <= 2)
      return n;
   else
      return n * fact(n - 1);
}

using namespace fbtt;

int main() {
   
   Test factorialTest { "Factorial of 5 is equal to 120", []() { 
      assert_throws<FactError>(fact, -1);
      assert_throws_message("Integer overflow.", fact, 13);
      
      assert_equals(fact(5), 120);
      assert_equals(fact(10), 3628800);
      // assert_equals(fact(1), -1);
   }};

   std::cout << factorialTest << '\n';

   factorialTest.run();
   std::cout << factorialTest << '\n';

   return 0;
}