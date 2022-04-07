#include "fbTestingTool.hpp"

using namespace fbtt;

int main()
{
   MultiTest<std::vector<int>> emptyVectorTest { "Test of empty vector" };

   emptyVectorTest.add_constructor(
      "Default constructor", [](auto * & vec) {
         vec = new std::vector<int>;
      }
   );

   emptyVectorTest.add_constructor(
      "Construct with size 0", [](auto * & vec) {
         vec = new std::vector<int> (0);
      }
   );
   

   emptyVectorTest.add_test(
      "new vector is empty", [](auto & vec) {
         assert_equals(vec.size(), 0, "Vector size is not 0");
         assert_true(vec.empty(), "Vector is not empty");
         assert_equals(vec.capacity(), 0, "Vector capacity is not 0");
      }
   );

   emptyVectorTest.add_test<std::length_error>(
      "resize to -1 throws length-error", [](auto & vec) {
         vec.resize(-1);
      }
   );

   emptyVectorTest.add_test<std::out_of_range>(
      "accessing out of range throws out of range", [](auto & vec) {
         vec.resize(20);
         vec.at(-1);
         vec.at(21);
      }
   );

   emptyVectorTest.add_test(
      "reserving x leads to capacity x", [](auto & vec) {
         vec.reserve(20);
         assert_equals(20, vec.capacity());
      }
   );

   emptyVectorTest.add_test(
      "first in, last out", [](auto & vec) {
         vec.push_back(32);
         vec.push_back(23);
         assert_equals(vec.back(), 23);
         vec.pop_back();
         assert_equals(vec.back(), 32);
         vec.pop_back();
      }
   );

   emptyVectorTest.add_test(
      "setting element i to x, leads to element i having the value of x", [](auto & vec) {
         vec.resize(10);
         vec[9] = 3;
         assert_equals(vec[9], 3);
      }
   );

   emptyVectorTest.add_test(
      "resizing vector with x leads to filled with x", [](auto & vec) {
         vec.resize(130, -1325);

         for (int & i : vec)
            assert_equals(i, -1325);
      }
   );

   std::cout << "Running tests...\n";
   emptyVectorTest.run();

   std::cout << emptyVectorTest;
}