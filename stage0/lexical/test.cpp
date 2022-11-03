//test for lexical member functions of stage0

#include <fstream>
#include <iostream>
#include <cctype>

using namespace std;

ifstream file("test.txt");

char nextChar()
{

	char c;
	c = file.get();
	if (c > 0)
	{
		return c;
	}
	else
		return (char)-1;

}

int main() //nextToken, but its in a testable state so we can see what we get as the token, which states it calls, test.txt in this same directory is used to test this function
{
	string token;
	while (token == "")
	{
		char ch = nextChar();
		char state;
		
		if (islower(ch))
			state = 'l';
		else if(isdigit(ch))
			state = 'd';
		/*else if (isSpecialSymbol(ch))
			state = 's';*/
		else if(ch == (char)-1)
			state = 'e';
		else
			state = ch;
		cout << state << "----" << ch << endl;
		
		switch(state)
		{
			case '{':                                                                            // process comments
				while ((ch != (char)-1/*END_OF_FILE*/) && (ch != '}'))
				{
					ch = nextChar();
					//cout << ch;
					if (ch == (char)-1/*END_OF_FILE*/)
					{
						cout << "fail" << endl;
					}
					else if (ch == '}')
					{
						cout << "coomment good" << endl;
						break;
					}
					
				}
				break;
				
			//case '}': -- still need to write processError
			
			case ' ':
				break;
			
			//case isSpecialSymbol(): --josh is supposed to write the isSpecialSymbol function as of right now (11/2/22 @ 8:17pm)
			
			case 'l'/*isLower*/:
				while (((isalnum(ch)) || (ch == '_')) && ch != (char)-1/*END_OF_FILE*/)
				{
					token += ch;
					ch = nextChar();
				}
				/*if (ch is END_OF_FILE)
				{
					processError(unexpected end of file)
				}*/
				break;
			
			case 'd'/*isDigit*/:
				while ((isdigit(ch)) && ch != (char)-1/*END_OF_FILE*/)
				{
					token += ch;
					ch = nextChar();
				}
				/*if (ch is END_OF_FILE)
				{
					processError(unexpected end of file)
				}*/
				break;
			
			case 'e'/*END_OF_FILE*/:
				token = ch;
				break;
			default:
				break;
		}
	}
	cout << token << endl;
}
