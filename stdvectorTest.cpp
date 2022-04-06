#include "fbTestingTool.hpp"
#include <vector>

using namespace fbtt;


int main()
{
   ClassTest<std::vector<int>> vectorTest;

   vectorTest.add_test("New vector has no elements", [](auto & vec) {
      assert_equals(vec.size(), 0);
   });

   vectorTest.add_test("Reserve x elements, have x capacity", [](auto & vec) {
      vec.reserve(80);
      assert_equals(80, vec.capacity());
   });

   vectorTest.add_test("Resize x elements, have x size", [](auto & vec) {
      vec.resize(20);
      assert_equals(20, vec.size());
   });

   vectorTest.add_test("Resize vector and fill with x, every element is x", [](auto & vec) {
      vec.resize(20, 1328);
      for (size_t i = 0; i < vec.size(); i++)
         assert_equals(vec[i], 1328, "Vector element no. " + std::to_string(i) + " has wrong value.");
   });

   vectorTest.add_test("Range check throws error, if out of bounds", [](auto & vec) {
      vec.resize(20);
      // assert_method_throws<std::out_of_range>(vec, std::vector<int>::at, -1);
      // assert_method_throws<std::out_of_range>(vec, std::vector<int>::at, 20);
   });

   vectorTest.run();


   std::cout << vectorTest;

   return 0;
}