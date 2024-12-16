#include "../../source/xcalculator.h"
#include <iostream>

int main()
{
    xcalculator Calc;

    //
    // Example 01 - Evaluate an expression
    //
    double i = Calc.Evaluate("2+3*4");
    double y = Calc.setVar("i", i).Evaluate("-3.5*cos(i*5)+pi*(-2+sqrt(i*pi^2))/3");

    std::cout << "Result 01: " << y << std::endl;


    //
    // Example 02 - Evaluate an expression with a loop
    //
    auto Exp = Calc.Tokenize("-3.5*cos(x*5)+pi*(-2+sqrt(x*pi^2))/3");
    double Sum = 0;
    for (double i = 0; i < 100; i++)
    {
        Calc.setVar("x", std::abs(Sum) );
        Sum += Calc.Evaluate(Exp);
    }

    std::cout << "Result 02: " << Sum << std::endl;

    return 0;
}