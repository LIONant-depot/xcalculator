#ifndef XCALCULATOR_H
#define XCALCULATOR_H
#pragma once

#include <vector>
#include <unordered_map>
#include <variant>
#include <string>

//-------------------------------------------------------------------------------------
// A Calculator to compute the value of math expressions including functions and variables.
// 
// Example:
// calculator Calc;
// double i = Calc.Evaluate("2+3*4");
// double y = Calc.setVar( "i", i ).Evaluate("-3.5*cos(i*5)+pi*(-2+sqrt(i*pi^2))/3");
//-------------------------------------------------------------------------------------
class xcalculator
{
public:

    struct token_type
    {
        struct op       { char         m_Value; };
        struct sep      { char         m_Value; };
        struct name     { std::string  m_Value; };
        struct number   { double       m_Value; };
    };

    using tokens = std::variant
    < token_type::op
    , token_type::sep
    , token_type::name
    , token_type::number
    >;

    using tokenize = std::vector<tokens>;
    using function = double(*)(double);

public:
                    xcalculator ( void )                                              noexcept;
    double          Evaluate    ( const std::string_view  Expression )        const   noexcept;
    double          Evaluate    ( const tokenize&         Expression )        const   noexcept;
    tokenize        Tokenize    ( const std::string_view  Expression )        const   noexcept;
    xcalculator&    setVar      ( const std::string& VarName, double Value )          noexcept;
    xcalculator&    setFunction ( const std::string& Functionname, function Function) noexcept;

protected:

    void            Init        ( void ) noexcept;

protected:

    std::unordered_map< std::string, double >   m_Variables;
    std::unordered_map< std::string, function > m_Functions;
};

#endif