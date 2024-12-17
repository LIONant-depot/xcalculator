<img src="https://i.imgur.com/0fxo7vo.jpeg" align="right" width="180px" />


# xcalculator
A simple C++ 20 class used to compute an expression, code base on [asl-calculator](https://github.com/aslze/asl-calculator)

**"A math expression parser and evaluator"**
<br><img src="https://i.imgur.com/9a5d2ee.png" align="right" width="150px" />
This class allows evaluating math expressions expressed as strings, including named variables and
functions. It implements the [Shunting-yard_algorithm](https://en.wikipedia.org/wiki/Shunting-yard_algorithm).

One of the goals of the class is to be forgiving allowing badly formatted expreesion to be evaluated.
So that users can make some mistakes and you may still get something useful out of it...

```cpp
xcalculator Calc;
double y = Calc.setVar("i", 25.5).Evaluate("-3.5*cos(i*5)+pi*(-2+sqrt(i*pi^2))/3");
```
<br>
You can define you own variables and single parameter functions. 

### default functions:

`abs`, `sqrt`, `sin`, `cos`, `tan`, `acos`, `asin`, `atan`, `exp`, `floor`, `ln`, `log`, `deg2rad`, `rad2deg`

### default constants:
`e`, `pi`

## Optimizations

You can tokenize an expression and evaluate it multiple times. This is useful when the values of variables
change but the expression remains the same.
```cpp
xcalculator Calc;
auto Exp = Calc.Tokenize("-3.5*cos(x*5)+pi*(-2+sqrt(x*pi^2))/3");
double Sum = 0;
for (double i = 0; i < 10000; i++) 
{
    Calc.setVar("x", Sum);
    Sum += Calc.Evaluate(Exp);
}
```

# Features
* MIT license
* Small size
* C++ 20
* Strong typed tokens
* Minimize memory usage
* No dependencies
* No exceptions
* Minimalistic API
* Custom functions
* Custom variables
* Use #include "xcalculator.cpp" as single header lib
* Basic example included


---
