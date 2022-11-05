//Gavin Whitson & Josh Strickland
//CS 4301
//Stage 0

#include <stage0.h>
#include <cctype> //needed for lexical functions
#include <iomanip> // needed for emit functions (setw())
#include <time.h> // needed for emit prologue

//Constructor
Compiler::Compiler(char **argv)
{
	sourceFile.open(argv[1]);
	listingFile.open(argv[2]);
	objectFile.open(argv[3]);
}

//Destructor
Compiler::~Compiler() // destructor
{
	sourceFile.close();
	listingFile.close();
	objectFile.close();
}

void Compiler::createListingHeader()
{
	time_t now = time (NULL);
	listingFile << "STAGE0:  " << "Joshua Strickland\t" << ctime(&now) << endl;
	listingFile << "LINE NO." << setw(30) << "SOURCE STATEMENT" << endl;
}

void Compiler::parser()
{
	lineNo++;
	listingFile << endl << right << setw(5) << lineNo << '|';
	//nextChar();
	if (nextToken() != "program")
		processError("keyword \"program\" expected");
	prog();
}

void Compiler::createListingTrailer()
{
	listingFile << "COMPILATION TERMINATED\t" << errorCount << " ERRORS ENCOUNTERED";
}

void Compiler::processError(string err) // GTG
{
	listingFile << endl << "Error: Line " << lineNo << ": " << err;
	cout << endl << "Error: Line " << lineNo << ": " << err << endl << endl;	// debug
	exit(0);
}

//PRODUCTIONS
void Compiler::prog()	// token should be "program"
{
	if (token != "program")
		processError("keyword \"program\" expected");
	progStmt();
	if (token == "const")
		consts();
	if (token == "var")
		vars();
	if (token != "begin")
		processError("keyword \"begin\" expected");
	beginEndStmt();
	if (token != "$")
		processError("no text may follow \"end\"");
}

void Compiler::progStmt() //token should be "program" GTG
{
	string x;
	if (token != "program")
		processError("keyword \"program\" expected");
	x = nextToken();
	if (!isNonKeyId(token))
		processError("program name expected");
	if (nextToken() != ";")
		processError("semicolon expected");
	nextToken();
	code("program", x);
	insert(x,PROG_NAME,CONSTANT,x,NO,0);
}

void Compiler::consts() //token should be "const" GTG
{
	if (token != "const")
		processError("keyword \"const\" expected");
	if (!isNonKeyId(nextToken()))
		processError("non-keyword identifier must follow \"const\"");
	constStmts();
}

void Compiler::vars() //token should be "var" GTG
{
	if (token != "var")
		processError("keyword \"var\" expected");
	if (!isNonKeyId(nextToken()))
		processError("non-keyword identifier must follow \"var\"");
	varStmts();
}

void Compiler::beginEndStmt() //token should be "begin" GTG
{
	if (token != "begin")
		processError("keyword \"begin\" expected");
	if (nextToken() != "end")
		processError("keyword \"end\" expected");
	if (nextToken() != ".")
		processError("period expected");
	nextToken();
	code("end", ".");
}

void Compiler::constStmts() //token should be NON_KEY_ID GTG
{
	string x,y;
	if (!isNonKeyId(token))
		processError("non-keyword identifier expected");
	x = token;
	if (nextToken() != "=")
	{
		processError("\"=\" expected");
	}
	y = nextToken();
	if (y != "+" && y != "-" && y != "not" && !isNonKeyId(y) && !isBoolean(y) && !isInteger(y))
		processError("token to right of \"=\" illegal");
	if (y == "+" || y == "-") 
	{
		if (!isInteger(nextToken()))
			processError("integer expected after sign");
		y = y + token;
	}
	if (y == "not")
	{
		if (!isBoolean(nextToken())) 
			processError("boolean expected after “not”");
		if (token == "true")
			y = "false";
		else
			y = "true";
	}
	if (nextToken() != ";")
		processError("semicolon expected");
	if (whichType(y) != INTEGER && whichType(y) != BOOLEAN)			
		processError("data type of token on the right-hand side must be INTEGER or BOOLEAN");
	insert(x,whichType(y),CONSTANT,whichValue(y),YES,1);
	x = nextToken();
	if (x != "begin" && x != "var" && !isNonKeyId(x)) 
		processError("non-keyword identifier, \"begin\", or \"var\" expected");
	if(token == "var")
		vars();
	if (isNonKeyId(x))
		constStmts();
}

void Compiler::varStmts()
{
	string x,y,z;
	if (!isNonKeyId(token))
		processError("non-keyword identifier expected");
	x = ids();
	if (token != ":")
		processError("\":\" expected");
	z = nextToken();
	if (z != "integer" && z != "boolean")
		processError("illegal type follows \":\"");
	y = token;
	if (nextToken() != ";")
		processError("semicolon expected");
	insert(x,whichType(y),VARIABLE,"",YES,1);
	z = nextToken();
	if (!isNonKeyId(z) && z != "begin")
		processError("non-keyword identifier or \"begin\" expected");
	if (isNonKeyId(token))
		varStmts();
}

string Compiler::ids() //token should be NON_KEY_ID GTG
{
	string temp,tempString;
	if (!isNonKeyId(token))
		processError("non-keyword identifier expected");
	tempString = token;
	temp = token;
	if (nextToken() == ",")
	{
		if (!isNonKeyId(nextToken()))
			processError("non-keyword identifier expected");
		tempString = temp + "," + ids();
	}
	cout << "ids: " << tempString << endl;	//debug
	return tempString;
}


//HELPER FUNCTIONS
bool Compiler::isKeyword(string s) const // GTG
{
	if (s == "program" || s == "begin" || s == "end" || s == "var" || s == "const" || s == "integer" || s == "boolean" || s == "true" || s == "false" || s == "not")
		return true;
	return false;
}

bool Compiler::isSpecialSymbol(char c) const// GTG
{
	if (c == '=' || c == ':' || c == ',' || c == ';' || c == '.' || c == '+' || c == '-')
		return true;
	return false;
}

bool Compiler::isNonKeyId(string s) const // determines if s is a non_key_id
{
	for (unsigned int i = 0; i < s.length(); i++)
	{
		if (islower(s[i]) || isdigit(s[i]) || s[i] == '_')
		{

		}
		else
		{
       		return false;
		}
	}
	if (s[s.length() - 1] == '_')
		return false;
	return true;
}

bool Compiler::isInteger(string s) const // GTG
{
	for (unsigned int i = 0; i < s.length(); i++)
	{
		if (isdigit(s[i]))
        {
			
        } 
	  else
		{
       	    return false;
       	}
	}
	return true;
}

bool Compiler::isBoolean(string s) const // GTG
{
	if (s == "true" || s == "false")
		return true;
	return false;
}

bool Compiler::isLiteral(string s) const //GTG
{
	//cout << "isLiteral name check on string: " << s << endl;
		if (isInteger(s) || isBoolean(s) || (s.substr(0,3) == "not" && isBoolean(s.substr(3,s.length() - 1))) || (s[0] == '+' && isInteger(s.substr(1,s.length() - 1))) || (s[0] == '-' && isInteger(s.substr(1,s.length() - 1))))
		{
		    
		}
		else
		    return false;
	return true;
}

//ACTION ROUTINES
void Compiler::insert(string externalName, storeTypes inType, modes inMode, string inValue, allocation inAlloc, int inUnits)
{
	while (externalName != ":")                                           			// this is the result left in test from our current else                                                  // break ID from ExtNnames
	{                                                                     			//                                                                                                        // break ID from ExtNnames
		uint index = externalName.find(',');                              			// finds a comma, it can either be a proper index, some big ass number, or 0. Get rid of the two outliers // break ID from ExtNnames
		string name;			                                                                                                                                                                               
		//cout << index << endl; //debug                                  			//                                               ^                                                        // break ID from ExtNnames
																					//                                               |                                                        // break ID from ExtNnames
		if (index != 0 && index <= externalName.length())                 			// this gets rid of those two outliers mentioned |                                                        // break ID from ExtNnames
		{                                                                 			//                                                                                                        // break ID from ExtNnames
			name = externalName.substr(0, index);                      	  			// breaks the first identifier from the list                                                              // break ID from ExtNnames
			externalName = externalName.substr(index + 1, externalName.length());   // returns the rest of the list, missing the first element and its following comma                        // break ID from ExtNnames
																					//                                                                                                        // break ID from ExtNnames
			///*debug*/cout << name1 << endl;                            		    //                                                                                                        // break ID from ExtNnames
		}                                                              			    //                                                                                                        // break ID from ExtNnames
		else                                                            		    // this is when there is no remaining comma in the list of names, essentially one left and a colon        // break ID from ExtNnames
		{                                                               		    //                                                                                                        // break ID from ExtNnames
			index = externalName.find(':');                                         // find the colon, (last character so really could use length but this is a little safer ig)              // break ID from ExtNnames
			name = externalName.substr(0, index);                         		    // breaks the last name from the string                                                                   // break ID from ExtNnames
			externalName = externalName.substr(index, externalName.length());       // makes test ":" so that it breaks the while loop                                                        // break ID from ExtNnames
		}                                                                 
		/*debug*/cout << name << endl; 
		//name contains the name we just broke from the list, here we need to check if its uppercase or lowercase. uppercase represents the internal name and we can create the map entry with the name as is
		// if the name is lowercase, that means it comes from pascal source code (external name) and needs to have the internal name generated
		if (name[0] < 'Z')
		{
			symbolTable.insert(pair<string, SymbolTableEntry>(name, SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits)));
		}
		else
		{
			symbolTable.insert(pair<string, SymbolTableEntry>(name, SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)));
		}
		
	}
}

storeTypes Compiler::whichType(string name) // tells which data type a name has
{
	//storeTypes type/* = BOOLEAN*/;
	return symbolTable.at(name).getDataType();
	
	//return type;
}

string Compiler::whichValue(string name) // tells which value a name has
{
	return symbolTable.at(name).getValue();
}

void Compiler::code(string op, string operand1, string operand2)
{
}

//EMIT ROUTINES
void Compiler::emit(string label, string instruction, string operands, string comment)
{
	objectFile << left  << setw(8) << label << setw(8) << instruction << setw(24) << operands << comment << endl;
}

void Compiler::emitPrologue(string progName, string)
{
	time_t t = time(NULL);
	
	objectFile << ";Gavin Whitson - Joshua Stickland\t\t" << asctime(localtime(&t));
	objectFile << "%INCLUDE \"Along32.inc\"" << endl;
	objectFile << "%INCLUDE \"Macros_Along.inc\"" << endl;
	string fProgName = "; program" + progName; 
	emit("\nSECTION", ".text", "", "");
	emit("global", "_start", "", fProgName);
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
	listingFile << ch; // may need to change some for new lines?
	
	//cout << ch << endl;
	return ch;
}


string Compiler::nextToken()
{
	while (token == "")
	{
		char ch = nextChar();
		cout << ch << endl;
		
		if (ch == '{')
		{
			while ((ch != END_OF_FILE) && (ch != '}'))
			{
				ch = nextChar();
				if (ch == END_OF_FILE)
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
		}
		else if (ch == '}')
		{
			processError("\'}\' cannot begin token");
			break;
		}
		else if(isSpecialSymbol(ch))
		{
			token = ch;
			ch = nextChar();
		}
		else if(islower(ch))
		{
			while (((isalnum(ch)) || (ch == '_')) && ch != END_OF_FILE)
				{
					token += ch;
					ch = nextChar();
				}
				if (ch == END_OF_FILE)
				{
					processError("unexpected end of file");
					break;
				}
			break;
		}
		else if(isdigit(ch))
		{
			while ((isdigit(ch)) && ch != END_OF_FILE)
				{
					token += ch;
					ch = nextChar();
				}
				if (ch == END_OF_FILE)
				{
					processError("unexpected end of file");
					break;
				}
			break;
		}
		else if (ch == END_OF_FILE)
		{
			token = ch;
		}
		else
		{
			processError("illegal symbol");
			break;
		}
	}
	cout << token << endl;
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

//void Compiler::processError(string err)
//{
//	listingFile << err << endl;
//	exit(0);
//}
//