#include "classTest.hpp"

class A {
public:
   bool touched = false;

   class ErrorClass : public std::exception { };

   void throwingMethod() const
   {
      throw ErrorClass();
   }
};

class B {
public:
   bool touched = false;
   void touch(A & a)
   {
      a.touched = true;
   }

   u_int8_t bytes[200];
};

int main()
{
   ClassTest<A, B> test;

   test.add_test("New A and B are not touched", [](A & a, B & b) {
      assert_false(a.touched);
      assert_false(b.touched);
   });  

   test.add_test("After b touches a, a is touched", [](A & a, B & b) {
      b.touch(a);
      assert_true(a.touched);
   });

   test.add_test("A's throwing method throws exception.", [](A & a, B & b) {
      assert_throws<A::ErrorClass>([&]() 
      { a.throwingMethod(); });
   });

   test.run();

   std::cout << test;
}