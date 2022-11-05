//Gavin Whitson & Josh Strickland
//CS 4301
//Stage 0

#include <stage0.h>
#include <cctype> //needed for lexical functions

//Constructor
Compiler::Compiler(char **argv)
{
}

//Destructor
Compiler::~Compiler()
{
}

void Compiler::createListingHeader()
{
}
void Compiler::parser()
{
}
void Compiler::createListingTrailer()
{
}


//PRODUCTIONS
void Compiler::prog()           // stage 0, production 1
{
}
void Compiler::progStmt()       // stage 0, production 2
{
}
void Compiler::consts()         // stage 0, production 3
{
}
void Compiler::vars()           // stage 0, production 4
{
}
void Compiler::beginEndStmt()   // stage 0, production 5
{
}
void Compiler::constStmts()     // stage 0, production 6
{
}
void Compiler::varStmts()       // stage 0, production 7
{
}
string Compiler::ids()          // stage 0, production 8
{
	return "temp";
}


//HELPER FUNCTIONS
bool Compiler::isKeyword(string s) const  // determines if s is a keyword
{
	return false;
}
bool Compiler::isSpecialSymbol(char c) const // determines if c is a special symbol
{
	return false;
}
bool Compiler::isNonKeyId(string s) const // determines if s is a non_key_id
{
	return false;
}
bool Compiler::isInteger(string s) const  // determines if s is an integer
{
	return false;
}
bool Compiler::isBoolean(string s) const  // determines if s is a boolean
{
	return false;
}
bool Compiler::isLiteral(string s) const  // determines if s is a literal
{
	return false;
}

//ACTION ROUTINES
void Compiler::insert(string externalName, storeTypes inType, modes inMode, string inValue, allocation inAlloc, int inUnits)
{
	while (externalName != ":")                                                   // this is the result left in test from our current else                                                      // break ID from list of names
	{                                                                     //                                                                                                            // break ID from list of names
		uint index = externalName.find(',');                                      // finds a comma, it can either be a proper index, some big ass number, or 0. Get rid of the two outliers     // break ID from list of names
		string name;
		//cout << index << endl; //debug                                  //                                               ^                                                            // break ID from list of names
																		  //                                               |                                                            // break ID from list of names
		if (index != 0 && index <= externalName.length())                         // this gets rid of those two outliers mentioned |                                                            // break ID from list of names
		{                                                                 //                                                                                                            // break ID from list of names
			name = externalName.substr(0, index);                      		  // breaks the first identifier from the list                                                                  // break ID from list of names
			externalName = externalName.substr(index + 1, externalName.length());                 // returns the rest of the list, missing the first element and its following comma                            // break ID from list of names
																		  //                                                                                                            // break ID from list of names
			///*debug*/cout << name1 << endl;                               //                                                                                                            // break ID from list of names
		}                                                                 //                                                                                                            // break ID from list of names
		else                                                              // this is when there is no remaining comma in the list of names, essentially one left and a colon            // break ID from list of names
		{                                                                 //                                                                                                            // break ID from list of names
			index = externalName.find(':');                                       // find the colon, (last character so really could use length but this is a little safer ig)                  // break ID from list of names
			name = externalName.substr(0, index);                         		  // breaks the last name from the string                                                                       // break ID from list of names
			externalName = externalName.substr(index, externalName.length());                     // makes test ":" so that it breaks the while loop                                                            // break ID from list of names
			///*debug*/cout << name1 << endl;                               //                                                                                                            // break ID from list of names
		}                                                                 //                                                                                                            // break ID from list of names
		/*debug*/cout << name << endl; 
		//name contains the name we just broke from the list, here we need to check if its uppercase or lowercase. uppercase represents the internal name and we can create the map entry with the name as is
		// if the name is lowercase, that means it comes from pascal source code (external name) and needs to have the internal name generated
		if (name[0] < 'Z')
		{
			symbolTable.insert(pair<string, SymbolTableEntry>(name, SymbolTableEntry(name, inType, inMode, "0", inAlloc, inUnits)));
		}
		else
		{
			symbolTable.insert(pair<string, SymbolTableEntry>(name, SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)));
		}
		
	}
}
storeTypes Compiler::whichType(string name) // tells which data type a name has
{
	return BOOLEAN;
}
string Compiler::whichValue(string name) // tells which value a name has
{
	return "temp";
}
void Compiler::code(string op, string operand1, string operand2)
{
}

//EMIT ROUTINES
void Compiler::emit(string label, string instruction, string operands, string comment)
{
}
void Compiler::emitPrologue(string progName, string)
{
}
void Compiler::emitEpilogue(string, string)
{
}
void Compiler::emitStorage()
{
}

//LEXICAL ROUTINES
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

//OTHER ROUTINES                          
string Compiler::genInternalName(storeTypes stype) const
{
	int count_bool = 0;
	int count_ints = 0;
	int count_prog = 0;
	
	string name = "";
	
	//my playground
	
	if (stype == BOOLEAN)
	{
		name = "B" + count_bool;
		count_bool++;
	}
	else if (stype == INTEGER)
	{
		name = "I" + count_ints;
		count_ints++;
	}
	else if (stype == PROG_NAME)
	{
		name = "P" + count_prog;
		count_prog++;
	}
	return name;
}

void Compiler::processError(string err)
{
	listingFile << err << endl;
	exit(0);
}