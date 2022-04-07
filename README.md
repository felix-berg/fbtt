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


If the function had failed, if `factorial(5)` had been evaluated to `121`, the test result would have been:

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

## Test of Class: std::vector
To show an example of how a class could be tested with the `fbtt::MultiTest<>` class, the following example will test the standard library `std::vector`.

A MultiTest is declared by giving the test a name, and passing template parameters of the different classes, you wish to test.

Here, we are testing the empty `std::vector<int>` class.
```C++
using namespace fbtt;
//...

MultiTest<std::vector<int>> emptyVectorTest;
```
A simple test can be added. For instance, a vectors `::resize()` and `::size()` methods can be tested.

```C++
emptyVectorTest.add_test(
   "When resized to x, has size x", [](std::vector<int> & vector)
   {
      vector.resize(20);
      assert_equals(vector.size(), 20);
   }
)
```
As you can see, the lambda function is passed an instance of `std::vector<int>` by reference. 

Another function may be created. This function could be a test of an error, thrown by `std::vector<int>`. If you want to test for an error, you can add an expected error to the test as a template parameter:
```C++
emptyVectorTest.add_test<std::out_of_range>(
   "When referencing element -1 with ::at(), throws out of range", 
   [](auto & vec) {
      vec.at(-1);
   }
);
```

Because `std::vector<int>` has a default constructor, an instance is constructed by default before every test. If this is not possible, (if you have a class, which has no default constructer) a constructor can be added with `MultiTest::add_constructor()`
```C++
emptyVectorTest.add_constructor(
   "initializing with 0", [](std::vector<int> * & vec)
   {
      vec = new std::vector<int>(0);
   }
)
```
If this is defined, the tests will begin with this constructor, instead of the predefined constructor. Any number of constructors can be added to the `MultiTest`.

Constructors added with `add_constructor` must allocate to free store with `new`.

### Flow of testing
When `MultiClass::run()` is called, the following happens (assume 2 constructors, 3 tests):

- The **1st constructor** is called
   - The **1st** test is executed
- The destructor is called (`delete` on object)
- The **1st constructor** is called
   - The **2nd** test is executed
- The destructor is called
- The **1st constructor** is called
   - The **3rd** test is executed
- The destructor is called
- 
- The **2nd constructor** is called
   - The **1st** test is executed
- The destructor is called (`delete` on object)
- The **2nd constructor** is called
   - The **2nd** test is executed
- The destructor is called
- The **2nd constructor** is called
   - The **3rd** test is executed
- The destructor is called

When the `MultiTest` is output to an output stream, the following output is produced:
```C++
std::cout << multiTest;
```
![image](https://user-images.githubusercontent.com/93908883/162222684-288c4df2-ef3b-491a-8871-68b98278e034.png)
This multitest can be found in `"examples/vectorTest.cpp"`.


#### Comment about `add_constructor`
The `MultiTest::add_constructor`-method takes a function pointer as its second argument. This function pointer has the signature `void(Classes * & ...)`.
This would, for a test with `std::vector<int>` mean, that the signature is `void(std::vector<int> * &)`. This has been done, so simple construction functions would remain simple, e.g.:

```C++
test.add_constructor(
   "Simple constructor for T", [](T * & t) {
      t = new T { x, y, z };
   }
);
```