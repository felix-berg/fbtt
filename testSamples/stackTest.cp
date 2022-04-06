#include "classTest.hpp"

#include <iostream>

class Stack {
public:
   Stack() {};

   class Underflow : public std::exception { };

   bool isEmpty() const 
   {
      return size == 0;
   }

   void push(int element)
   {
      elements[size++] = element;
   }

   int pop() {
      if (isEmpty()) throw Stack::Underflow();
      return elements[--size];
   }

private:
   int size = 0;
   int elements[64];
};

using namespace fbtt;

int main() {
   ClassTest<Stack> stackTest;

   stackTest.add_test("Nothing", 
      [](Stack & stack) {

      }
   );

   stackTest.add_test("New stack is empty", [](Stack & stack) {
      assert_true(stack.isEmpty(), "Stack is not empty");
   });

   stackTest.add_test("After one push, is not empty", [](Stack & stack) {
      stack.push(0);
      assert_false(stack.isEmpty());

   });

   stackTest.add_test("Popping empty stack throws underflow", [](Stack & stack) {
      assert_throws_m<Stack::Underflow>(stack, &Stack::pop);
   });

   stackTest.add_test("After one push one pop, stack is empty", [](Stack & stack) {
      stack.push(0);
      stack.pop();

      assert_true(stack.isEmpty());
   });

   stackTest.add_test("After two pushes and one pop, stack is not empty", [](Stack & stack) { 
      stack.push(0);
      stack.push(0);
      stack.pop();

      assert_false(stack.isEmpty());
   });

   stackTest.add_test("After pushing x will pop x", [](Stack & stack) {
      stack.push(99);
      assert_equals(99, stack.pop());  
      
      stack.push(88);
      assert_equals(88, stack.pop());
   });

   stackTest.add_test("After pushing x and y, will pop y, then x", [](Stack & stack) {
      stack.push(99);
      stack.push(88);

      assert_equals(88, stack.pop());
      assert_equals(99, stack.pop());
   });

   stackTest.add_test("Failing test", [](Stack & stack) { 
      assert_throws_m<Stack::Underflow>(stack, &Stack::push, 2);
   });



   stackTest.run();
   std::cout << stackTest << '\n';
}