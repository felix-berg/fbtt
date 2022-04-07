#include "fbTestingTool.hpp"


using namespace fbtt;

struct A { 
public:
   int a;
   A(int b) : a { b } { };
};

int main()
{
   MultiTest<std::vector<int>> vectorTest { "Test of vector" };

   MultiTest<A> aTest { "A Class" };

   aTest.add_constructor("2 constructor", [](auto * & a) {
      a = new A { 2 };
   });

   aTest.add_test("Hello, A", [](A & a) {
      assert_equals(a.a, 2);
   });


   aTest.run();
   std::cout << aTest << '\n';

   vectorTest.run();   

}