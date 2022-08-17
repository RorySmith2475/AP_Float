A exact precision floating point header only library and command line reverse-polish calculator.

Supported operands: +, -, *, /, abs, sqrt
    
    The plus, minus, multiplication, and abs operations are calculated exactly along with conversions to and from float and double types.
    The division and square root operations are approximations, along with parsing string encoded floating point input, all with configurable accuracy.

To build and run the calculator:
    1. Go to the top level directory
    2. cmake -H. -Bbin
    3. cmake --build bin --target arbitrary_precision
    4. ./bin/arbitrary_precision

    The calculator uses reverse-polish notation. An example input line is:
        1.234e-2 2.345e5 *
