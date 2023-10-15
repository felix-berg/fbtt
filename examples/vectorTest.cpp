#include "../include/fbtt.hpp"

using namespace fbtt;

int main()
{
   MultiTest<std::vector<int>> emptyVectorTest { "Test of empty vector" };

    emptyVectorTest.addConstructor(
        "Default constructor", [](auto*& vec) {
            vec = new std::vector<int>;
        }
    );

    emptyVectorTest.addConstructor(
        "Construct with size 0", [](auto*& vec) {
            vec = new std::vector<int>(0);
        }
    );

    emptyVectorTest.addTest(
        "new vector is empty", [](auto& vec) {
            assertEquals(vec.size(), 0, "Vector size is not 0");
            assertTrue(vec.empty(), "Vector is not empty");
            assertEquals(vec.capacity(), 0, "Vector capacity is not 0");
        }
    );

    emptyVectorTest.addTest<std::length_error>(
        "resize to -1 throws length-error", [](auto& vec) {
            vec.resize(-1);
        }
    );

    emptyVectorTest.addTest<std::out_of_range>(
        "accessing out of range throws out of range", [](auto& vec) {
            vec.resize(20);
            vec.at(-1);
            vec.at(21);
        }
    );

    emptyVectorTest.addTest(
        "reserving x leads to capacity x", [](auto& vec) {
            vec.reserve(20);
            assertEquals(20, vec.capacity());
        }
    );

    emptyVectorTest.addTest(
        "first in, last out", [](auto& vec) {
            vec.push_back(32);
            vec.push_back(23);
            assertEquals(vec.back(), 23);
            vec.pop_back();
            assertEquals(vec.back(), 32);
            vec.pop_back();
        }
    );

    emptyVectorTest.addTest(
        "setting element i to x, leads to element i having the value of x",
        [](auto& vec) {
            vec.resize(10);
            vec[9] = 3;
            assertEquals(vec[9], 3);
        }
    );

    emptyVectorTest.addTest(
        "resizing vector with x leads to filled with x", [](auto& vec) {
            vec.resize(130, -1325);

            for (int& i: vec)
                assertEquals(i, -1325);
        }
    );

    emptyVectorTest.addTest(
        "failing test", [](auto& vec) {
            assertFalse(true);
        }
    );

   std::cout << "Running tests...\n";
   emptyVectorTest.run();

   std::cout << emptyVectorTest;

   return getErrorCode(emptyVectorTest.getResults());
}