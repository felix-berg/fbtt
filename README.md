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

To display the result of the test, either call `Test::report()` or output the test to an ostream.
```C++
std::string result = factorialTest.report();
// or
std::cout << factorialTest;
```
This will, if the function passed, produce the following output:

![image](https://user-images.githubusercontent.com/93908883/161978697-196a918c-93a7-4f45-927b-db27275fa879.png)


If the function had failed, if `factorial(5)` had been evaluated, to `121`, the test result would have been:

![image](https://user-images.githubusercontent.com/93908883/161979104-1f129abd-7a83-4988-8b23-af4fe59c22cb.png)

## Assertions
The different types of available assertions are:
#### Assert true
```C++ 
assert_true(bool assertion, const std::string & onFail = "");
```
- Assert that `assertion` is true
   - `onFail`: String to error, if assertion fails. Defaults to "".

##### Example usage
```C++
assert_true(ball.isBouncy, "Ball is not bouncy");
```

#### Assert false
```C++ 
assert_false(bool assertion, const std::string & onFail = "");
```
- Assert that `assertion` is false
   - `onFail`: String to error, if assertion fails. Defaults to "".

##### Example usage
```C++
assert_false(dog.isBarking(), "Dog is barking");
```

#### Assert equals
```C++ 
assert_equals(T x, T y, const std::string & onFail = "");
```
- Assert that value `x` and `y` of type `T` are equal
   - `onFail`: String to error, if assertion fails. Defaults to "".

##### Example usage
```C++
// assert factorial(5) == 120
assert_equals(factorial(5), 120, "Factorial 5 is not equal to 120");
```
#### Assert equals
```C++ 
assert_noteq(T x, T y, const std::string & onFail = "");
```
- Assert that value `x` and `y` of type `T` are not equal
   - `onFail`: String to error, if assertion fails. Defaults to "".

##### Example usage
```C++
// assert 7.0 != 3.0
assert_noteq(7.0, 3.0, "Maths is broken: 7.0 is equal to 3.0");
```
#### Assert throws
```C++ 
assert_throws<ErrorType>(Function f, Args ... args);
```
- Assert, that `f` throws an instance of `ErrorType`, when invoked with `(args...)`

##### Example usage
```C++
// assert, that factorial throws instance of FactorialError, when factorial(-1) is called.
assert_throws<FactorialError>(factorial, -1);
```

#### Assert throws specific message
```C++ 
assert_throws_message(const std::string & errorMsg, Function f, Args ... args);
```
- Assert, that `f` throws an error with message `errorMsg`, when invoked with `(args...)`

##### Example usage
```C++
// Assert, that factorial(100) throws error with message "Integer overflow".
assert_throws_message("Integer overflow", factorial, 100);
```

#### Assert throws (method call)
```C++ 
assert_method_throws<ErrorType>(Class obj, MethodPtr m, Args ... args);
```
- For non-static member functions: Assert that member function `m` (e.g. `&Class::method`) 
  throws and exception of type `ErrorType` when invoked with `(args...)`.

##### Example usage
```C++
std::vector<int> empty;
// assert that empty.resize(-1) throws an std::length_error
assert_method_throws<std::length_error>(empty, &std::vector<int>::resize, -1);
```