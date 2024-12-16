// Base on: https://github.com/aslze/asl-calculator
// License: MIT, Project link: https://github.com/LIONant-depot/xcalculator
#include "xcalculator.h"
#include <cassert>
#include <stack>


//------------------------------------------------------------------------------
// Helper functions
//------------------------------------------------------------------------------
namespace details
{
    template<typename T_VARIANT, typename T, std::size_t T_INDEX_V>
    consteval std::size_t variant_index(void)
    {
        static_assert(std::variant_size_v<T_VARIANT> > T_INDEX_V, "Type not found in variant");
        if constexpr (T_INDEX_V == std::variant_size_v<T_VARIANT>) return T_INDEX_V;
        else if constexpr (std::is_same_v< std::variant_alternative_t<T_INDEX_V, T_VARIANT>, T> ) return T_INDEX_V;
        else return variant_index<T_VARIANT, T, T_INDEX_V + 1>();
    }
}
template< typename T_VARIANT, typename T >
static constexpr auto ivariant_v = details::variant_index<T_VARIANT, T, 0>();

//------------------------------------------------------------------------------

static
constexpr int Precedence(const char o) noexcept
{
    if (o == '+' || o == '-')
        return 1;
    else if (o == '*' || o == '/')
        return 2;
        
    return 3;
}

//------------------------------------------------------------------------------
// member functions
//------------------------------------------------------------------------------

xcalculator::xcalculator( void ) noexcept
{
    Init();
}

//------------------------------------------------------------------------------

xcalculator& xcalculator::setVar(const std::string& VarName, double Value) noexcept
{
    m_Variables[VarName] = Value;
    return *this;
}

//------------------------------------------------------------------------------

xcalculator& xcalculator::setFunction(const std::string& Functionname, function Function) noexcept
{
    m_Functions[Functionname] = Function;
    return *this;
}

//------------------------------------------------------------------------------

void xcalculator::Init( void ) noexcept
{
    m_Variables.insert({"e", 2.718281828459045});
    m_Variables.insert({"pi", 3.141592653589793});

    m_Functions.insert({"abs",  std::fabs   });
    m_Functions.insert({"acos", std::acos   });
    m_Functions.insert({"asin", std::asin   });
    m_Functions.insert({"atan", std::atan   });
    m_Functions.insert({"cos",  std::cos    });
    m_Functions.insert({"exp",  std::exp    });
    m_Functions.insert({"floor",std::floor  });
    m_Functions.insert({"ln",   std::log    });
    m_Functions.insert({"log",  std::log10  });
    m_Functions.insert({"sin",  std::sin    });
    m_Functions.insert({"sqrt", std::sqrt   });
    m_Functions.insert({"tan",  std::tan    });

    m_Functions.insert({"deg2rad", [](double d){return (3.141592653589793/180) * d;} });
    m_Functions.insert({"rad2deg", [](double r){return (180/3.141592653589793) * r;} });
}

//------------------------------------------------------------------------------

double xcalculator::Evaluate(const std::string_view Expression) const noexcept
{
    return Evaluate(Tokenize(Expression));
}

//------------------------------------------------------------------------------

xcalculator::tokenize xcalculator::Tokenize( const std::string_view Expression) const noexcept
{
    // If we don't have an expression, return an empty list
    if( Expression.empty() ) return {};

    // Let's tokenize the input string
    tokenize    Input;

    //
    // Read the input string and produce a list of input tokens
    //
    {
        constexpr std::size_t   token_type_none_v   = 0xffff;
        std::size_t             iTokenType          = token_type_none_v;
        const std::string       ExpStorage          = std::string(Expression) + ' ';
        const char*             pExpression         = ExpStorage.c_str();
        std::string             TokenString;

        while( const char c = *pExpression++ ) switch(iTokenType)
        {
        case token_type_none_v:
            switch (c)
            {
            case '+': 
            case '-':
            case '*':
            case '/':
            case '^':
                Input.emplace_back(token_type::op{ c });
                break;
            case '(': 
            case ')':
                Input.emplace_back(token_type::sep{ c });
                break;
            default:
                if ((c >= '0' && c <= '9') || c == '.' || c == '-' ) 
                {
                    iTokenType = ivariant_v<tokens, token_type::number>;
                    TokenString = c;
                }
                else if (isalpha(c)) 
                {
                    iTokenType = ivariant_v<tokens, token_type::name>;
                    TokenString = c;
                }
                break;
            }
        break;
        case ivariant_v<tokens, token_type::number>:
            if (!((c >= '0' && c <= '9') || c == '.' || c == 'e' || (c == '-' && *(pExpression - 2) == 'e')))
            {
                Input.emplace_back(token_type::number{ std::stod(TokenString) });
                iTokenType = token_type_none_v;
                pExpression--;
                break;
            }
            else
            {
                TokenString += c;
            }
        break;
        case ivariant_v<tokens, token_type::name>:
            if (isalnum(c))
            {
                TokenString += c;
            }
            else
            {
                Input.emplace_back(token_type::name{ std::move(TokenString) });
                pExpression--;
                iTokenType = token_type_none_v;
            }
        }
    }

    //
    // Parse the input token list and organize them correctly in the output list
    //
    using operator_token = std::variant
    < token_type::op
    , token_type::sep
    , token_type::name
    >;

    std::stack<operator_token>  Operators;
    tokenize                    Output;
    {
        Output.reserve(Input.size());

        int i = 0;
        for (auto& E : Input) std::visit([&]<typename T>(T& Token) constexpr noexcept
        {
            if constexpr ( std::is_same_v<T, token_type::number> )
            {
                Output.emplace_back(Token);
            }
            else if constexpr (std::is_same_v<T, token_type::name>)
            {
                const auto iplus1 = i+1u;
                if( iplus1 < Input.size() )
                {
                    const auto& X = Input[iplus1];
                    if( std::holds_alternative<token_type::sep>(X) && std::get<token_type::sep>(X).m_Value == '(' )
                    {
                        Operators.push(Token);
                    }
                    else
                    {
                        Output.emplace_back(std::move(Token));
                    }
                }
                else
                {
                    // Most likely a variable
                    Output.emplace_back(std::move(Token));
                }
            }
            else if constexpr (std::is_same_v<T, token_type::sep>)
            {
                switch( Token.m_Value )
                {
                case '(': Operators.push( Token );
                    break;
                case ')': while (   Operators.size() 
                                && ( false == std::holds_alternative<token_type::sep>(Operators.top())
                                        ||  std::get<token_type::sep>(Operators.top() ).m_Value != '(' ) )
                    {
                        std::visit([&](auto& V) constexpr noexcept
                        {
                            Output.emplace_back(std::move(V));
                        }, Operators.top());
                            
                        Operators.pop();
                    }

                    if(Operators.size()) Operators.pop();
                    break;
                }
            }
            else if constexpr (std::is_same_v<T, token_type::op>)
            {
                if(  (Token.m_Value == '-' || Token.m_Value == '+') 
                    && (  i == 0 
                        || std::holds_alternative<token_type::op>(Input[i - 1]) 
                        || ( std::holds_alternative<token_type::sep>(Input[i - 1]) 
                        && std::get<token_type::sep>(Input[i - 1]).m_Value == '(' )
                        ))
                {
                    if( Token.m_Value == '-' ) Operators.push( token_type::op{ '_' } );
                }
                else
                {
                    //
                    // Make sure we have the right precedence...
                    //
                    while(Operators.size() > 0
                            && [&]() constexpr noexcept
                            {
                                auto& Top = Operators.top();

                                if( std::holds_alternative<token_type::op>(Top) )
                                {
                                    return ((Token.m_Value != '^' && Precedence(Token.m_Value) <= Precedence(std::get<token_type::op>(Top).m_Value))
                                         || (Token.m_Value == '^' && Precedence(Token.m_Value) <  Precedence(std::get<token_type::op>(Top).m_Value)));
                                }
                                else if ((Token.m_Value != '^' && Precedence(Token.m_Value) <= Precedence('a')) 
                                      || (Token.m_Value == '^' && Precedence(Token.m_Value) <  Precedence('a')))
                                {
                                    if (std::holds_alternative<token_type::sep>(Top)) return std::get<token_type::sep>(Top).m_Value != '(';
                                    return true;
                                }
                                return false;
                            }())
                    {
                        std::visit([&](auto& V) constexpr noexcept
                        {
                            Output.emplace_back(std::move(V));
                        }, Operators.top());

                        Operators.pop();
                    }
                    Operators.push(Token);
                }
            }
            else
            {
                assert(false);
            }

            // Increment the index
            ++i;

        }, E );
    }
    
    while (Operators.size() > 0)
    {
        // if we have '(' or ')' in the stack, there is a mismatch
        if( std::holds_alternative<token_type::sep>(Operators.top() ))
        {
            printf("Mismatch\n");
            return {};
        }
        else
        {
            std::visit( [&]( auto& V ) constexpr noexcept
            {
                Output.emplace_back( std::move(V));
            }, Operators.top() );

            Operators.pop();
        }
    }

    return Output;
}

//------------------------------------------------------------------------------

double xcalculator::Evaluate( const tokenize& Output ) const noexcept
{
    // The empty list is the literal meaning of zero...
    if (Output.empty()) return 0;

    // let's evaluate the expression
    std::stack<double> Operands;

    for( auto& E : Output ) std::visit([&]<typename T>( const T& Token ) constexpr noexcept
    {
        if constexpr( std::is_same_v<T, token_type::number> )
        {
            Operands.push(Token.m_Value);
        }
        else if constexpr( std::is_same_v<T, token_type::op> )
        {
            double x1, x2;
            switch( Token.m_Value )
            {
            case '_':
                Operands.top() = -Operands.top();
                break;
            case '+':
                x2 = Operands.top();
                Operands.pop();
                x1 = Operands.top();
                Operands.top() = x1 + x2;
                break;
            case '-':
                x2 = Operands.top();
                Operands.pop();
                x1 = Operands.top();
                Operands.top() = x1 - x2;
                break;
            case '*':
                x2 = Operands.top();
                Operands.pop();
                x1 = Operands.top();
                Operands.top() = x1 * x2;
                break;
            case '/':
                x2 = Operands.top();
                Operands.pop();
                x1 = Operands.top();
                Operands.top() = x1 / x2;
                break;
            case '^':
                x2 = Operands.top();
                Operands.pop();
                x1 = Operands.top();
                Operands.top() = x1 + x2;
                if (x2 == 2)
                    Operands.top() = x1 * x1;
                else
                    Operands.top() = pow(x1, x2);
                break;
            }
        }
        else if constexpr (std::is_same_v<T, token_type::name>)
        {
            if (auto X = m_Functions.find(Token.m_Value); X != m_Functions.end())
            {
                const auto pFunction = X->second;
                const auto x1        = Operands.top();
                Operands.top() = (*pFunction)(x1);
            }
            //else if (_functions.has(token.val))
            // {
            //    x1 = _operands.popget();
                //   _operands.push(_functions(token.val, x1));
            //}
            else
            {
                if( auto E = m_Variables.find(Token.m_Value);  E == m_Variables.end() )
                {
                    // If it is not a function or a variable... we assume zero.
                    // We could throw an exception here, but we are not doing it for simplicity.
                    Operands.push( 0 );
                }
                else
                {
                    Operands.push( E->second );
                }
            }
        }
        else
        {
            assert(false);
        }

    }, E );

    return Operands.top();
}