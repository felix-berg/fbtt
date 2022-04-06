# FB Testing Tool
Simple testing tool for testing individual functions or classes, as well as interactions between multiple classes.

## Test of a single function: Factorial
To test a single function, an instance of the `fbtt::Test` class must be initialized with a name and a function.
In this example, a `lambda`-expression is used, but any storable function works (e.g. function pointer, std::function, lambda, non-static member functions).
The function must have the signature `void()`.

```C++
Test factorialTest { "Factorial of 5 is equal to 120", []() {
   assert_equals(factorial(5), 120);
}};
```

To run the test, call `Test::run()`.

```C++
factorialTest.run();
```

To display the result of the test, either call `std::string Test::report()` or output the test to an ostream.
```C++
std::string result = factorialTest.report();
// or
std::cout << factorialTest;
```
This will, if the function passed, produce the following output:
![image](https://user-images.githubusercontent.com/93908883/161978697-196a918c-93a7-4f45-927b-db27275fa879.png)


If the function had failed, if `factorial(5)` had been evaluated, to `121`, the test result would have been:
