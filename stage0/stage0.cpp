//Gavin Whitson & Josh Strickland
//CS 4301
//Stage 0


#include <cctype> //needed for lexical functions

//Constructor
Compiler::Compiler()
{
}

//Destructor
Compiler::~Compiler()
{
}

//
char Compiler::nextChar()
{
	//ifstream file(sourceFile); // is this necessary? i think the constructor opens all needed files
	char ch = sourceFile.get(); // in final compiler, will probably have to remove the char as ch is declared as a char in private data
	if (ch == -1) // file.get picks up chars that dont really exist, return them as essentially (char)-1 --- may need to be less than 0?? 
	{
		ch = END_OF_FILE;
	}
	listingFile << ch << endl; // may need to change some for new lines?
	
	return ch;
}

//
string Compiler::nextToken()
{
	string token; // declared in private data, can likely remove this full line
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
		
		switch(state)
		{
			case '{': // process comments
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
				
			//case '}': -- still need to write processError("'}' cannot begin token);
			
			case ' ': // isSpace()
				break;
			
			//case isSpecialSymbol(): --josh is supposed to write the isSpecialSymbol function as of right now (11/2/22 @ 8:17pm)
			
			case 'l': //isLower
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
			
			case 'd': //isDigit
				while ((isdigit(ch)) && ch != (char)-1/*END_OF_FILE*/)
				{
					token += ch;
					ch = nextChar();
				}
				/*if (ch is END_OF_FILE)
				{
					processError(unexpected end of file);
				}*/
				break;
			
			case 'e': //END_OF_FILE
				token = ch;
				break;
			default:
				break; //processError(illegal symbol);
		}
	}
	return token;
}

void Compiler::insert(string externalName, storeTypes inType, modes inMode, string inValue, allocation inAlloc, int inUnits)
{
}	
storeTypes Compiler::whichType(string name) // tells which data type a name has    
{
}	
string Compiler::whichValue(string name) // tells which value a name has     
{
}	
void Compiler::code(string op, string operand1 = "", string operand2 = "")
{
}	
																						
  // Emit Functions                                                                   
void Compiler::emit(string label = "", string instruction = "", string operands = "", string comment = "");                                                     
void Compiler::emitPrologue(string progName, string = "");                                    
void Compiler::emitEpilogue(string = "", string = "");                                        
void Compiler::emitStorage(); 