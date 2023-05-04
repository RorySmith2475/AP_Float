# AP_Float

An arbitrary precision header only floating point library

## Features
* ### Assignment
The AP::Float constructors and assignment operators can accept any floating point or integral value exactly. String encoded floating point values can be used to allow for inputs outside the normal range of floating point values. However, parsing these values is done in non constant time may not be done exactly in some cases.
```c++
AP::Float f1(1.23f);
AP::Float f2 = 1.23;
AP::Float f3(1U);
AP::Float f4 = "1.23e2000";
AP::Float f5(std::numeric_limits<double>::infinity());
```

* ### Exact precision functions `+` `-` `*` `abs()`
```c++
AP::Float f = AP::Float(1.23f) + AP::Float("4.56");
f += 7.89;
f -= AP::Float("1.23e-123");
f *= 4UL;
f = AP::abs(f);
```

* ### Arbitrary precision functions `/` `sqrt()`
```c++
AP::Float f = AP::Float(1.23) / AP:Float("4.56");
f = AP::sqrt(f);
```

* ### Casting / Conversions 
Explicit casting to built in types is done in constant time with precision reduced to fit inside the data type. Converting to string is done in non constant time and to arbitrary precision.

Note: The to_string() function is a WIP, results are not guaranteed
```c++
float f = static_cast<float>(AP::Float("1.23"));
double d = static_cast<double>(AP::Float("1.23"));
std::string s = AP::to_string(AP::Float("1.23"));
std::cout << AP::Float(1.23) std::endl;
```

## Building Tests
    1. Go to the top level directory
    2. cmake -H. -Bbin
    3. cmake --build bin --target tests
    4. ./bin/tests
