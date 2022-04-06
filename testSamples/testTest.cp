#include <iostream>
#include <sstream>

#include "assert.hpp"
#include "test.hpp"

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
#include "terminalColor.hpp"

int main() {
   
   Test factorialTest { "Factorial", []() { 
      assert_throws_message<FactError>("Cannot compute factorial of negative number.", fact, -1);
      assert_throws_message<FactError>("Integer overflow.", fact, 13);
      
      assert_equals(fact(5), 120);
      assert_equals(fact(10), 3628800);
      assert_equals(fact(1), -1);
   }};
   std::cout << factorialTest << '\n';

   factorialTest.run();
   std::cout << factorialTest << '\n';

   return 0;
}