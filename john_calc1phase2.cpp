//============================================================================
// File Name   : johncarvajalphase1calc1.cpp
// Author      : John Carvajal
// Class       : Organization of Programming Languages
// Date        : 10/11/2018
// Version     : Phase 2
//============================================================================
//
// Purpose     : Build an interpreter
//
// Description :
// There exists a simple language suitable for simple algebraic
// expressions. The language consists of a few basic statements. CALC1 uses lower
// case input, but prints out in upper case.
//
// This language is interpreted - input will be typed one line at a time, and then executed.
//
// load a //create variable a, prompt the use for a value, and store it
// mem c // create variable c, no value
// c = a + 5
// c = sqrt c
// c = c ^ 5
// print c
// stop
//
// All variables can be thought of as floating point. A "//" starts a comment,
// which can occur anywhere on the line, or on a line by itself. All math is left
// to right, no order of precedence. The legal operations are + , - , / and *.
// Also, sqrt and ^ exist. There are no unary operators.
//
// Here is a sample program showing the interactions:
//
// bash> ./calc1
// >READY FOR INPUT
// >load first
// >ENTER VALUE FOR FIRST 4.0
// >load second
// >ENTER VALUE FOR SECOND 3.0
// >mem hypotenuse
// >hypotenuse = first * first + second * second // I'm not sure this works due to l ? r precedence
// >hypotenuse = sqrt hypotenuse
// >print hypotenuse
// >VALUE OF HYPOTENUSE IS 4.69042
// >stop
// bash>
//============================================================================

#include <iostream>
#include <math.h>
#include <bits/stdc++.h>
//#include "stdc++.h"
#include <vector>
#include <string>

// use this for debugging  (make 0 for no extra printing)
#define DEBUG_PRINT 0

using namespace std;

enum TOKEN_TYPE{ADD, SUB, MULT, DIV, SQRT, EXP, VAR, PRINT, EQUAL, MEM, LOAD, STOPPROGRAM, NUM, LEFT_PARA, RIGHT_PARA};

// create a token object
class Token_OBJ
{
    private:
    string name;
    double value;
    TOKEN_TYPE type;
    bool noValue;

    public:
    // default constructors
    Token_OBJ()
    {
        name = "null";
        value = 0;
        type = ADD;
        noValue = true;
    }

    Token_OBJ(string nameIn,double valueIn,TOKEN_TYPE typeIn,bool noValueIn)
    {
        if (noValueIn == true)
        {
            noValue = true;
        }
        else
        {
            noValue = false;
            value = valueIn;
        }
        name = nameIn;
        type = typeIn;
    }
    void setName(string a)
    {
        name = a;
    }
    string getName()
    {
        return name;
    }
    void setValue(double a)
    {
        noValue = false;
        value = a;
    }
    double getValue()
    {
        if( noValue == false)
        {
            return value;
        }
        else
        {
            std::cout << "ERROR in value " <<  name << std::endl;
            return value;
        }
    }
    bool getNoValue()
    {
        return noValue;
    }
    void setType(TOKEN_TYPE a)
    {
        type = a;
    }
    TOKEN_TYPE getType()
    {
        return type;
    }
};

// function prototypes
TOKEN_TYPE convertToToken(string);
int searchMemory(string , vector <Token_OBJ> &);
double computeValue( double, TOKEN_TYPE, double );
double sqt(double);
int findPriority( TOKEN_TYPE );
bool isOperator( TOKEN_TYPE );
void convertToPostFix( vector<Token_OBJ> & , vector <Token_OBJ>& );
double getTheValueResolved(  Token_OBJ , vector <Token_OBJ> &);

int main()
{
    // used for stopping program
    TOKEN_TYPE stopToken;
    // vector of tokens that saves the memory names
    vector <Token_OBJ> symbolTable;

    std::cout << "READY FOR INPUT" << std::endl;
    do
    {

        // my vector of strings that saves the tokens
        vector <Token_OBJ> tokens;
        string lineInput;
        string intermediate;
        // start of main loop, read each line one at a time
        getline(cin, lineInput);

        // check if there are an extra end of line character
        char c = '\n';
        if( DEBUG_PRINT == 1 )   std::cout << "lineinpue size of string " << lineInput.size()  << std::endl;
        if (lineInput[lineInput.size() -1] == '\n')
        {
            lineInput.erase(lineInput.size()-1);
            if( DEBUG_PRINT == 1 )   std::cout << "Extra EOL, size of string " << lineInput.size()  << std::endl;
        }

        // check if there are any comment characters
        for(int i = 0 ; i < lineInput.size() - 1; ++i)
        {
            // remove any character that is after comment
            if((lineInput[i] == '/') && (lineInput[i+1] == '/'))
            {
                // erase the character
                lineInput.erase(i,lineInput.size()-i) ;
            }
        }

        // read string as a stream
        stringstream check1(lineInput);

        // tokenize the string with spaces as delimiter
        while(getline(check1, intermediate, ' '))
        {
            double d = 0.0;
            TOKEN_TYPE s = convertToToken(intermediate);
            Token_OBJ temp = Token_OBJ(intermediate,d,s,true);
            if (s == NUM)
            {
                d = stod(intermediate);
                temp.setValue(d);
            }
            tokens.push_back(temp);
            if( DEBUG_PRINT == 1 ) std::cout << " tokenize  name and type " <<  temp .getName() <<  " " <<  temp .getType()  << std::endl;

        }

        // check for input error assignment error with not equal sign
        if( tokens[0].getType()  == VAR && tokens[1].getType() != EQUAL )
        {
            std::cout << "ERROR unknown input " << tokens[0].getName()  << std::endl;
        }
        else
        {
            // known input
            vector<Token_OBJ> postFixTokens;
            // if an assignment, build the post fix version
            if( tokens[1].getType() == EQUAL )
            {
                if( DEBUG_PRINT == 1 ) std::cout << "infix  expression " << lineInput << std::endl;

                // convert to post infix
                convertToPostFix(tokens , postFixTokens);
                if( DEBUG_PRINT == 1 ) std::cout << " post fix convert tokenString size is " << postFixTokens.size() << std::endl;
                if( DEBUG_PRINT == 1 ) std::cout << " post fix convert output expression " ;
                if( DEBUG_PRINT == 1 )
                {
                    for( int i = 0; i < postFixTokens.size(); i++)
                    {
                        std::cout << postFixTokens[i].getName()  << " ";
                    }
                }
                if( DEBUG_PRINT == 1 ) std::cout << std::endl;
            }

            // process the tokens starting at the left
            Token_OBJ currentToken = tokens[0];
            TOKEN_TYPE currentTokenValue = currentToken.getType();
            if( DEBUG_PRINT == 1 ) std::cout << " first token to process is "  << currentToken.getName() << std::endl;

            switch(currentTokenValue)
            {
                case LOAD :
                {
                    // create the variable, prompt the use for a value, and store it
                    Token_OBJ nextToken = tokens[1];
                    // convert the string to upper case
                    std::string upperStr = nextToken.getName();
                    std::transform(upperStr.begin(), upperStr.end(),upperStr.begin(), ::toupper);
                    std::cout << "ENTER VALUE FOR " << upperStr << std::endl;

                    // read in the double value
                    double inputVal;
                    string tempInput = "";
                    getline(std::cin, tempInput);
                    // convert the string to double to double
                    inputVal = std::stod(tempInput);

                    // save the input value in the object token
                    nextToken.setValue(inputVal);

                    // check of the name for the load already exist
                    int testMatch = searchMemory(nextToken.getName(), symbolTable);

                    // if i found a match, set the value
                    if (testMatch >= 0)
                    {
                        // token  is found, set the value
                        symbolTable[testMatch].setValue(inputVal);
                    }
                    else
                    {
                        //add the token to the memory vector
                        symbolTable.push_back(nextToken);
                    }
                } break;
                case MEM :
                {
                    Token_OBJ nextToken = tokens[1];
                    // create variable with no value (default to zero)
                    // add the token to the memory vector
                    symbolTable.push_back(nextToken);
                } break;
                case PRINT :
                {
                    Token_OBJ   nextToken = tokens[1];
                    string tokenName = nextToken.getName();
                    double value = 0;

                    for(int i = 0; i < symbolTable.size();i++)
                    {
                        string stringToTest = symbolTable[i].getName();
                        if (tokenName == stringToTest)
                        {
                            value = symbolTable[i].getValue();
                            std::cout << "VALUE OF " << stringToTest << " IS " << value << std::endl;
                        }
                    }
                } break;
                case STOPPROGRAM :
                {
                    stopToken = STOPPROGRAM;
                } break;
                default:
                {
                    // this means the second token is an equal sign
                    double total = 0;

                    // create an empty stack
                    stack<Token_OBJ> theStack;
                    double firstValue  = 0;
                    double secondValue  = 0;

                    // process eash token fron left to right
                    for(int i = 0; i < postFixTokens.size(); i++)
                    {
                        string tokenString = postFixTokens[i].getName();
                        TOKEN_TYPE tokentype = postFixTokens[i].getType();
                        if( DEBUG_PRINT == 1 ) std::cout << " for loop tokenString " << tokenString << " IS " << tokentype << std::endl;

                        // check if SQRT
                        if(tokentype == SQRT)
                        {
                            if( DEBUG_PRINT == 1 ) std::cout << "token type is SQRT" << std::endl;

                            // Pop the stack and get the first token
                            firstValue = getTheValueResolved( theStack.top() , symbolTable);

                            // pop the stack
                            theStack.pop();

                            // evaluate the function with 1 numerical values and the sqrt
                            double sqrtValue = total + sqt( firstValue );
                            if( DEBUG_PRINT == 1 ) std::cout << "total  sqrt = " << sqrtValue << std::endl;

                            // Push the resulting value onto the stack
                            Token_OBJ tempToken = Token_OBJ("result", sqrtValue, NUM, false);
                            theStack.push( tempToken );
                        }

                        // check if a number
                        else if(tokentype == NUM || tokentype == VAR)
                        {
                            if( DEBUG_PRINT == 1 ) std::cout << "token type is NUM or VAR" << std::endl;
                            // push it onto the stack
                            theStack.push( postFixTokens[i] );
                        }
                        //  check if an operator is found
                        else if(isOperator(tokentype) == true)
                        {
                            if( DEBUG_PRINT == 1 ) std::cout << "token type is OPERATOR" << std::endl;
                            // Pop the stack and get the first token
                            firstValue = getTheValueResolved( theStack.top() , symbolTable);
                            theStack.pop();
                            // Pop the stack and get the next token
                            secondValue = getTheValueResolved( theStack.top() , symbolTable);
                            theStack.pop();

                            // evaluate the function with 2 numerical values and the operator
                            total = computeValue( firstValue, tokentype, secondValue );

                            // Push the resulting value onto the stack
                            Token_OBJ tempToken = Token_OBJ("operator", total, NUM, false);
                            theStack.push( tempToken );
                        }
                        else
                        {
                            std::cout << "token type is ERROR (not found)" << std::endl;
                        }

                    } // end for

                    if( DEBUG_PRINT == 1 )  std::cout << "token name and type " <<  theStack.top() .getName() <<  " " <<  theStack.top() .getType()  << std::endl;

                    // Pop the stack (this is the final value)
                    total = getTheValueResolved( theStack.top() , symbolTable);
                    if( DEBUG_PRINT == 1 ) std::cout << "total = " << total << std::endl;
                    theStack.pop();

                    // save the result
                    int testMatch = searchMemory(tokens[0].getName(), symbolTable);
                    // token  is found, set the value
                    symbolTable[testMatch].setValue(total);
                } break;
            } //end switch

        } // end if

    } while (stopToken != STOPPROGRAM); // end do while

    std::cout << "PROGRAM HAS FINISHED." << std::endl;
    return 0;

} // end main

bool is_number(const std::string& s)
{
    try
    {
        std::stod(s);
    }
    catch(...)
    {
        return false;
    }
    return true;
}

TOKEN_TYPE convertToToken(string x)
{
    TOKEN_TYPE T;

    // find the token
    if (x == "+")
    {
        T = ADD;
    }
    else if (x == "stop" || x == "STOP")
    {
        T = STOPPROGRAM;
    }
    else if (x == "-")
    {
        T = SUB;
    }
    else if (x == "*")
    {
        T = MULT;
    }
    else if (x == "/")
    {
        T = DIV;
    }
    else if (x == "sqrt" || x == "SQRT")
    {
        T = SQRT;
    }
    else if (x == "^")
    {
        T = EXP;
    }
    else if (x == "load" || x == "LOAD")
    {
        T = LOAD;
    }
    else if (x == "mem"|| x == "MEM")
    {
        T = MEM;
    }
    else if (x == "=")
    {
        T = EQUAL;
    }
    else if (x == "print"|| x == "PRINT")
    {
        T = PRINT;
    }
    else if (x == "(")
    {
        T = LEFT_PARA;
    }
    else if (x == ")")
    {
        T = RIGHT_PARA;
    }
    else
    {
        if (is_number(x))
        {
            T = NUM;
        }
        else
        {
            T = VAR;
        }
    }
    return T;
}  // end of convertToToken

// Procedure to find name in memory
int searchMemory(string strIn , vector <Token_OBJ> &memoryTokens)
{
    int found = -1; // -1
    // prints out all the tokens line by line
    for(int i = 0; i < memoryTokens.size(); i++)
    {
        string tempStr = memoryTokens[i].getName();
        if( strIn == tempStr)
        {
            found = i;
            break;
        }
    }
    return found;
}; //  end of searchMemory

double computeValue( double one, TOKEN_TYPE operatorValue, double two)
{
    double cal = 0;

    if( DEBUG_PRINT == 1 ) std::cout << "one is " << one << " two is " << two << std::endl;
    switch(operatorValue)
    {
        case ADD :
        {
           cal = one + two;
        } break;
        case SUB :
        {
           cal = two - one;
        } break;
        case MULT :
        {
            cal = one * two;
        } break;
        case DIV :
        {
            cal = two / one;
        } break;
        case EXP :
        {
            cal = pow(two, one);
        } break;
        default:
            cout << " ERROR IN computeValue " << endl;
    }

    if( DEBUG_PRINT == 1 ) std::cout << "computeValue returns " << cal << std::endl;
    return cal;
} // end of computeValue

double sqt (double x)
{
    return sqrt(x);
}

// start of convertToPostFix
void convertToPostFix( vector<Token_OBJ> &tokens , vector<Token_OBJ> &postFix )
{

    stack<Token_OBJ> theStack;

    // start at the third token over (skip before = sign)
    for(unsigned int i = 2; i < tokens.size(); i++)
    {
        // check if value
        if ( tokens[i].getType() == VAR || tokens[i].getType() == NUM )
        {
            postFix.push_back( tokens[i] );
        }
        // check if ( symbol
        else if(tokens[i].getType() == LEFT_PARA)
        {
            theStack.push( tokens[i] );
        }
        // check if ) symbol
        else if(tokens[i].getType() == RIGHT_PARA)
        {
            while( !( theStack.empty() ) && ( theStack.top().getType() != LEFT_PARA ) )
            {
                postFix.push_back(theStack.top());
                theStack.pop();
            }
            theStack.pop();
        }
        // check if opertor
        else if(isOperator(tokens[i].getType()) == true)
        {
            while( !( theStack.empty() ) &&
                        ( findPriority( theStack.top().getType() ) >= findPriority( tokens[i].getType() ) ) )
            {
                postFix.push_back(theStack.top());
                theStack.pop();
            }
            theStack.push( tokens[i] );
        }
    }

    //pop any remaining operators from the stack and insert to postFix
    while(!theStack.empty())
    {
        postFix.push_back(theStack.top());
        theStack.pop();
    }

} // end of convertToPostFix


int findPriority(TOKEN_TYPE type)
{
    // priority from hightest to lowest
    if( type == EXP || type == SQRT )
    {
        return 3;
    }
    else if( type == MULT || type == DIV )
    {
        return 2;
    }
    else if( type == ADD || type == SUB )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}  // end of findPriority

bool isOperator(TOKEN_TYPE type)
{
    return (type == ADD ||
                 type == SUB ||
                 type == MULT ||
                 type == DIV ||
                 type == SQRT ||
                 type == EXP);
}  // end of isOperator

double getTheValueResolved( Token_OBJ theToken ,  vector <Token_OBJ> &symbolTable )
{
    double rtnValue = 0;

    if( theToken.getType()  == NUM  )
    {
        // get the number directly
        rtnValue = theToken.getValue();
    }
    else
    {
        // check of the name for the variable
        int testMatch = searchMemory( theToken.getName(), symbolTable);
        // token  is found, get the value from it
        rtnValue = symbolTable[testMatch].getValue();//double equals
    }

    if( DEBUG_PRINT == 1 ) std::cout << "getTheValueResolved returned " << rtnValue << std::endl;
    return rtnValue;
}  // end of getTheValueResolved
