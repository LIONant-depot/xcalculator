#include "../../source/xcalculator.h"
#include <iostream>

int main()
{
    xcalculator Calc;

    double i = Calc.Evaluate("2+3*4");
    double y = Calc.setVar("i", i).Evaluate("-3.5*cos(i*5)+pi*(-2+sqrt(i*pi^2))/3");

    std::cout << "Result: " << y << std::endl;

    return 0;
}