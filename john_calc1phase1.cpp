//============================================================================
// File Name   : johncarvajalphase1calc1.cpp
// Author      : John Carvajal
// Date        : 9/6/2018
// Version     : Phase 1
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
// >hypotenuse = a * a + b * b // I'm not sure this works due to l ? r precedence
// >hypotenuse = sqrt hypotenuse
// >print hypotenuse
// >VALUE OF HYPOTENUSE IS 5.0
// >stop
// bash>
//============================================================================

#include <iostream>
#include <math.h>
//#include <bits/stdc++.h>
#include "stdc++.h"
#include <vector>
#include <string>
using namespace std;

enum TOKEN_TYPE{ADD, SUB, MULT, DIV, SQRT, EXP, VAR, PRINT, EQUAL, MEM, LOAD, STOPPROGRAM, NUM};

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
            return 0;
        }
    }
    bool getNoValue()
    {
        return noValue;
    }
    void setToken(TOKEN_TYPE a)
    {
        type = a;
    }
    TOKEN_TYPE getToken()
    {
        return type;
    }
};

// function prototypes
TOKEN_TYPE convertToken(string);
int searchMemory(string , vector <Token_OBJ> &);
double computeValue( double, TOKEN_TYPE, double );
double sqt(double);

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

        // check if there are any comment characters
		for(int i = 0 ; i < lineInput.size() - 1; ++i)
		{
			// remove any character that is not alphanumeric
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
            TOKEN_TYPE s = convertToken(intermediate);
            Token_OBJ temp = Token_OBJ(intermediate,d,s,true);
            if (s == NUM)
            {
                d = stod(intermediate);
                temp.setValue(d);
            }
            tokens.push_back(temp);
        }

        // process the tokens starting at the left
        Token_OBJ currentToken = tokens[0];
        TOKEN_TYPE currentTokenValue = currentToken.getToken();

        switch(currentTokenValue)
        {
            case LOAD :
            {
                // create the variable, prompt the use for a value, and store it
                Token_OBJ nextToken = tokens[1];
                // convert the string to upper case
                std::string upperStr = nextToken.getName();
                std::transform(upperStr.begin(), upperStr.end(),upperStr.begin(), ::toupper);
                std::cout << "ENTER VALUE FOR " << upperStr<< std::endl;

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
                Token_OBJ nextToken = tokens[1];
                string tokenName = nextToken.getName();

                // prints out all the tokens line by line
                for(int i = 2; i < tokens.size(); i++)
                {
                    string tokenString = tokens[i].getName();
                    TOKEN_TYPE tokentype = tokens[i].getToken();
                    if(tokentype == SQRT)
                    {
                        // next position is either a variable or a number
                        if(tokens[i + 1].getToken() == NUM)
                        {
                            // get the number
                            double sVal = tokens[i + 1].getValue();
                            total = total + sqt(sVal);
                        }
                        else
                        {
                            // check of the name for the variable
                            int testMatch = searchMemory( tokens[i + 1].getName(), symbolTable);
                            // token  is found, get the value from it
                            double sVal = symbolTable[testMatch].getValue();
                            total = total + sqt( sVal );
                        }
                        // processed 2 tokens
                        i = i + 1;
                    }
                    else if(tokentype == VAR)
                    {
                        // this means we have a variable and need to process the next 2 tokens at once
                        int testMatch = searchMemory( tokens[i].getName(), symbolTable);
                        // token is found, get the value from the symbol table
                        double firstValue = symbolTable[testMatch].getValue();
                        double secondValue = 0;

                        // get second value (either NUM or VAR
                        if(tokens[i+2].getToken()  == VAR)
                        {
                            int testMatch = searchMemory( tokens[i + 2].getName(), symbolTable);
                            // token is found, get the value from he symbol table
                            secondValue = symbolTable[testMatch].getValue();
                        }
                        else
                        {
                            // must be a NUM
                            secondValue = tokens[i+2].getValue();
                        }

                        // get operator token
                        TOKEN_TYPE operatorType = tokens[i+1].getToken();

                        // call the function with 2 values and an operator
                        total = total + computeValue( firstValue , operatorType, secondValue );

                        // processed 3 tokens so increment the counter
                        i = i + 2;
                    }
                    else if(tokentype == NUM)
                    {
                        // this means we have a number and need to process the next2 tokens at once
                        double firstValue = tokens[i].getValue();
                        double secondValue = 0;

                        // get second value (either NUM or VAR
                        if(tokens[i+2].getToken() == VAR)
                        {
                            int testMatch = searchMemory(tokens[i + 2].getName(), symbolTable);
                            // token  is found, get the value from i
                            secondValue = symbolTable[testMatch].getValue();
                        }
                        else
                        {
                            // must be a NUM
                            secondValue = tokens[i+2].getValue();
                        }

                        // get operator token
                        TOKEN_TYPE operatorType = tokens[i+1].getToken();

                        // call the function with 2 values and an operator
                        total = total + computeValue(firstValue , operatorType, secondValue);
                        // processed 3 tokens so increment the counter
                        i = i + 2;
                    }
                    else if (tokentype == ADD || tokentype == SUB || tokentype == DIV || tokentype == MULT || tokentype == EXP || tokentype == SQRT)
					{
                    	 // this means we have a number and need to process the next2 tokens at once
						double firstValue = total;
						double secondValue = 0;

						// get second value (either NUM or VAR
						if(tokens[i+1].getToken() == VAR)
						{
							int testMatch = searchMemory(tokens[i + 1].getName()  ,  symbolTable);
							// token  is found, get the value from i
							secondValue = symbolTable[testMatch].getValue();
						}
						else
						{
							// must be a NUM
							secondValue = tokens[i+1].getValue();
						}

						// call the function with 2 values and an operator
						total = computeValue(firstValue , tokentype, secondValue);

						// processed 3 tokens so increment the counter
						i = i + 2;
					}
                    // save the result
                    int testMatch = searchMemory(tokens[0].getName(), symbolTable);
                    // token  is found, set the value
                    symbolTable[testMatch].setValue(total);
                }
            } break;
        } //end switch
    } while (stopToken != STOPPROGRAM);
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

TOKEN_TYPE convertToken(string x)
{
    TOKEN_TYPE T;
    if (x == "+")
    {
        T = ADD;
    }
    else if (x == "stop")
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
    else if (x == "sqrt")
    {
        T = SQRT;
    }
    else if (x == "^")
    {
        T = EXP;
    }
    else if (x == "load")
    {
        T = LOAD;
    }
    else if (x == "mem")
    {
        T = MEM;
    }
    else if (x == "=")
    {
        T = EQUAL;
    }
    else if (x == "print")
    {
        T = PRINT;
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
}  // end of convertToken

// Procedure to find name in memory
int searchMemory(string strIn , vector <Token_OBJ> &memoryTokens)
{
    int found = -1;
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
        switch(operatorValue)
        {
            case ADD :
            {
               cal = one + two;
            } break;
            case SUB :
            {
               cal = one - two;
            } break;
            case MULT :
            {
                cal = one * two;
            } break;
            case DIV :
            {
                cal = one / two;
            } break;
            case EXP :
            {
                cal = pow(one,two);
            } break;
            default:
                cout << " ERROR IN computeValue " << endl;
        }
    return cal;
} // end of computeValue

double sqt (double x)
{
	return sqrt(x);
}
