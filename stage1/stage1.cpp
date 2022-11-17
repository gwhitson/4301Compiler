//Gavin Whitson & Josh Strickland
//CS 4301
//Stage 1

#define _CRT_SECURE_NO_WARNINGS // visual studio only

#include <stage1.h>
#include <cctype> //needed for lexical functions
#include <iomanip> // needed for emit functions (setw())
#include <time.h> // needed for emit prologue
#include <stack>

//Constructor
Compiler::Compiler(char** argv) // GTG
{
	sourceFile.open(argv[1]);
	listingFile.open(argv[2]);
	objectFile.open(argv[3]);
}

//Destructor
Compiler::~Compiler() // destructor GTG
{
	sourceFile.close();
	listingFile.close();
	objectFile.close();
}

void Compiler::createListingHeader() // GTG
{
	time_t now = time(NULL);
	listingFile << "STAGE0:  " << "Joshua Strickland & Gavin Whitson\t" << ctime(&now) << endl;
	listingFile << "LINE NO." << setw(30) << "SOURCE STATEMENT" << endl;
}

void Compiler::parser() // GTG
{
	lineNo++;
	listingFile << endl << right << setw(5) << lineNo << '|';
	nextChar();

	if (nextToken() != "program")
		processError("keyword \"program\" expected");
	prog();
}

void Compiler::createListingTrailer() // GTG
{
	if (errorCount != 1)
		listingFile << "COMPILATION TERMINATED      " << errorCount << " ERRORS ENCOUNTERED" << endl;
	else
		listingFile << "COMPILATION TERMINATED      " << errorCount << " ERROR ENCOUNTERED" << endl;
	exit(1);
}

//PRODUCTIONS
void Compiler::prog()	// token should be "program" GTG
{
	if (token != "program")
		processError("keyword \"program\" expected");
	progStmt();
	if (token == "const")
	{
		//cout << "call to consts token = " << token << endl;
		consts();
	}
	//cout << "through consts - " << token << endl;
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
	insert(x, PROG_NAME, CONSTANT, x, NO, 0);
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
	//cout << "constStmts token - " << token << endl;
	string x, y, z;
	if (!isNonKeyId(token))
		processError("non-keyword identifier expected");
	x = token;
	if (nextToken() != "=")
		processError("\"=\" expected");
	y = nextToken();
	if (y != "+" && y != "-" && y != "not" && !isNonKeyId(y) && !isBoolean(y) && !isInteger(y))
		processError("token to right of \"=\" illegal");
	if (y == "+" || y == "-")
	{
		if (!isInteger(nextToken()))
			processError("digit expected after sign");
		y = y + token;
	}
	if (y == "not")
	{
		z = nextToken();
		if (!isBoolean(z) && whichType(z) != BOOLEAN)
		{
			processError("boolean expected after “not”");
		}
		//cout << "past check     token z = " << z  << "   token y = " << y<< endl;
		if (token == "true" || whichValue(z) == "true")
			y = "false";
		else
			y = "true";
		//cout << "past replace value" << endl;
	}
	if (nextToken() != ";")
		processError("semicolon expected");
	if (whichType(y) != INTEGER && whichType(y) != BOOLEAN)
	{
		processError("data type of token on the right-hand side must be INTEGER or BOOLEAN");
	}

	insert(x, whichType(y), CONSTANT, whichValue(y), YES, 1);
	x = nextToken();
	if (x != "begin" && x != "var" && !isNonKeyId(x))
		processError("non-keyword identifier, \"begin\", or \"var\" expected");
	if (isNonKeyId(x))
		constStmts();
}

void Compiler::varStmts() // GTG
{
	string x, y, z;
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
	insert(x, whichType(y), VARIABLE, "", YES, 1);
	z = nextToken();
	if (!isNonKeyId(z) && z != "begin")
		processError("non-keyword identifier or \"begin\" expected");
	if (isNonKeyId(token))
		varStmts();
}

string Compiler::ids() //token should be NON_KEY_ID GTG
{
	string temp, tempString;
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
	return tempString;
}

void Compiler::execStmts()      // stage 1, production 2
{
	string x, y, z;																		  //useless rn
	if (token == "read" || token == "write" || isNonKeyId(token))						  //if its read, write, or a non key ID. go into execStmts, otherwise it should do nothing
	{																					  //
		execStmt();																		  //
		//execStmts();			// make this indirectly recursive from execStmt			  //
	}																					  //

}
void Compiler::execStmt()       // stage 1, production 3								  //
{																						  //
	if (token == "read")																  //if its  a read go to readStmt
	{																					  //
		readStmt();																		  //
	}																					  //
	else if (token == "write")															  //if its a write go to writeStmt
	{																					  //
		writeStmt();																	  //
	}																					  //
	else if (isNonKeyId(token))															  //if its a nonKeyID go to assignment
	{																					  //
		assignStmt();																	  //
	}																					  //
	else																				  //otherwise if it got here and its none of the above it should not have got here
		processError("not a valid exec stmt -- exec stmt");
}
void Compiler::assignStmt()     // stage 1, production 4								  //
{																						  //
	if (isNonKeyId(token))																  //redundant but never hurts, opens possibility to throw special error too
	{																					  //
		nextToken();																	  //only if its still good, get next token
		if (token == ":=")																  //check if its the assignment operator
		{																				  //
			nextToken();																  //get next token to send to express
			express();																	  //goes to express? according to motls book on stage1
			nextToken();																  //get next token again, this time we want a semicolon
			if (token != ";")															  //
				processError("Semicolon expected -- assign stmt");						  //if theres no semicolon it throws an error
		}																				  //
	}
	else
		processError("how did you get here? -- assignStmt");
}
void Compiler::readStmt()       // stage 1, production 5		 // ask motl, do we add the readList?? not in header and seemed to cause issues addin
{																 // ask motl, do we add the readList?? not in header and seemed to cause issues addin
	if (token == "read")										 // ask motl, do we add the readList?? not in header and seemed to cause issues addin
	{															 // ask motl, do we add the readList?? not in header and seemed to cause issues addin
																 // ask motl, do we add the readList?? not in header and seemed to cause issues addin
	}															 // ask motl, do we add the readList?? not in header and seemed to cause issues addin
}																 // ask motl, do we add the readList?? not in header and seemed to cause issues addin
//void Compiler::readList()      // stage 1, production 6		 // ask motl, do we add the readList?? not in header and seemed to cause issues addin
//{																 // ask motl, do we add the readList?? not in header and seemed to cause issues addin
//																 // ask motl, do we add the readList?? not in header and seemed to cause issues addin
//}																 // ask motl, do we add the readList?? not in header and seemed to cause issues addin
void Compiler::writeStmt()      // stage 1, production 7
{

}
//void Compiler::writeList()      // stage 1, production 8
//{
//
//}
void Compiler::express()        // stage 1, production 9
{
	term();			   // does not feel right...
					   // does not feel right...
	nextToken();	   // does not feel right...
	expresses();		   // does not feel right...
}
void Compiler::expresses()      // stage 1, production 10
{

}
void Compiler::term()           // stage 1, production 11
{

}
void Compiler::terms()          // stage 1, production 12
{

}
void Compiler::factor()         // stage 1, production 13
{

}
void Compiler::factors()        // stage 1, production 14
{

}
void Compiler::part()           // stage 1, production 15
{

}
//void Compiler::relOp()           // stage 1, production 16
//{
//
//}
//void Compiler::addLevelOp()           // stage 1, production 17
//{
//
//}
//void Compiler::multLevelOp()           // stage 1, production 18
//{
//
//}

//HELPER FUNCTIONS
bool Compiler::isKeyword(string s) const // GTG
{
	if (s == "program" || s == "begin" || s == "end" || s == "var" || s == "const" || s == "integer" || s == "boolean" || s == "true" || s == "false" || s == "not" || s == "mod" || s == "div" || s == "and" || s == "or" || s == "read" || s == "write")
		return true;
	return false;
}

bool Compiler::isSpecialSymbol(char c) const// GTG
{
	if (c == '=' || c == ':' || c == ',' || c == ';' || c == '.' || c == '+' || c == '-') //|| c == '-')
		return true;
	return false;
}

bool Compiler::isNonKeyId(string s) const // determines if s is a non_key_id
{
	for (unsigned int i = 0; i < s.length(); i++)
	{
		if (islower(s[i]) || isdigit(s[i]) || s[i] == '_')
		{
			if (isKeyword(s))
				return false;
			if (s[i] == '_' && s[i - 1] == '_')
				return false;
		}
		else
			return false;
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
			return false;
	}
	return true;
}

bool Compiler::isBoolean(string s) const // GTG
{
	if (s == "true" || s == "false")
		return true;
	return false;
}

bool Compiler::isLiteral(string s) const // GTG
{
	if (isInteger(s) || isBoolean(s) || (s.substr(0, 3) == "not" && isBoolean(s.substr(3, s.length() - 1))) || (s[0] == '+' && isInteger(s.substr(1, s.length() - 1))) || (s[0] == '-' && isInteger(s.substr(1, s.length() - 1))))
	{

	}
	else
		return false;
	return true;
}

//ACTION ROUTINES
void Compiler::insert(string externalName, storeTypes inType, modes inMode, string inValue, allocation inAlloc, int inUnits)
{
	while (externalName.length() > 0)                                           	// this is the result left in test from our current else                                                  // break ID from ExtNnames
	{                                                                     			//                                                                                                        // break ID from ExtNnames
		uint index = externalName.find(',');                              			// finds a comma, it can either be a proper index, some big ass number, or 0. Get rid of the two outliers // break ID from ExtNnames
		string name;
		//                                               ^                                                        // break ID from ExtNnames
		//                                               |                                                        // break ID from ExtNnames
		if (index != 0 && index <= externalName.length())                 			// this gets rid of those two outliers mentioned |                                                        // break ID from ExtNnames
		{                                                                 			//                                                                                                        // break ID from ExtNnames
			name = externalName.substr(0, index);                      	  			// breaks the first identifier from the list                                                              // break ID from ExtNnames
			externalName = externalName.substr(index + 1, externalName.length());   // returns the rest of the list, missing the first element and its following comma                        // break ID from ExtNnames
																					//                                                                                                        // break ID from ExtNnames                          		    //                                                                                                        // break ID from ExtNnames
		}                                                              			    //                                                                                                        // break ID from ExtNnames
		else                                                            		    // this is when there is no remaining comma in the list of names, essentially one left and a colon        // break ID from ExtNnames
		{                                                               		    //                                                                                                        // break ID from ExtNnames
			index = externalName.length();                                          // find the colon, (last character so really could use length but this is a little safer ig)              // break ID from ExtNnames
			name = externalName.substr(0, index);                         		    // breaks the last name from the string                                                                   // break ID from ExtNnames
			uint colon = name.find(':');
			if (colon > 0 && colon < name.length())
			{
				name = name.substr(0, name.length() - 1);
			}
			externalName = externalName.substr(index, externalName.length());       // makes test ":" so that it breaks the while loop                                                        // break ID from ExtNnames
		}

		if (symbolTable.count(name) != 0)
		{
			processError("symbol " + name + " is multiply defined");
		}
		else if (isKeyword(name))
		{
			processError("illegal use of keyword");
		}

		//name contains the name we just broke from the list, here we need to check if its uppercase or lowercase. uppercase represents the internal name and we can create the map entry with the name as is
		// if the name is lowercase, that means it comes from pascal source code (external name) and needs to have the internal name generated
		if (symbolTable.size() < 256)
		{
			if (name[0] < 'Z')
			{
				symbolTable.insert(pair<string, SymbolTableEntry>(name, SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits)));
			}
			else
			{
				symbolTable.insert(pair<string, SymbolTableEntry>(name, SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)));
			}
		}
		else
			processError("symbolTable overflow");
	}
}

storeTypes Compiler::whichType(string name) //tells which data type a name has 
{
	map<string, SymbolTableEntry>::iterator itr;
	itr = symbolTable.find(name);
	storeTypes DT;

	if (name == "integer")
	{
		DT = INTEGER;
		return DT;
	}

	if (name == "boolean")
	{
		DT = BOOLEAN;
		return DT;
	}
	if (isLiteral(name))
		if (isBoolean(name))
		{
			DT = BOOLEAN;
			return DT;
		}
		else
		{
			DT = INTEGER;
			return DT;
		}
	else
	{
		if (itr != symbolTable.end())
			DT = itr->second.getDataType(); // maybe
		else
		{
			processError("reference to undefined constant");
		}
	}

	return DT;
}

string Compiler::whichValue(string name) //tells which value a name has 
{
	map<string, SymbolTableEntry>::iterator itr;
	itr = symbolTable.find(name);
	string value;
	if (isLiteral(name))
		value = name;
	else
		if (itr != symbolTable.end() && itr->second.getValue() != "")         //getInternalName(symbolTable[name]) != NULL && getValue(symbolTable[name]) != NULL)	//name is an identifier and hopefully a constant
			value = itr->second.getValue();
		else
		{
			processError("reference to undefined constant");
		}
	return value;
}

void Compiler::code(string op, string operand1, string operand2)
{
	if (op == "program")
		emitPrologue(operand1);
	else if (op == "end")
		emitEpilogue();
	else if (op == "read")
		emitReadCode(op, operand2);
	else if (op == "write")
		emitWriteCode(op, operand2);
	else if (op == ":=")
		emitAssignCode(operand1, operand2);
	else if (op == "+")
		emitAdditionCode(operand1, operand2);
	else if (op == "-")
		emitSubtractionCode(operand1, operand2);
	else if (op == "*")
		emitMultiplicationCode(operand1, operand2);
	else if (op == "div")
		emitDivisionCode(operand1, operand2);
	else if (op == "mod")
		emitModuloCode(operand1, operand2);
	else if (op == "neg")
		emitNegationCode(operand1);
	else if (op == "not")
		emitNotCode(operand1);
	else if (op == "and")
		emitAndCode(operand1, operand2);
	else if (op == "or")
		emitOrCode(operand1, operand2);
	else if (op == "=")
		emitEqualityCode(operand1, operand2);
	else if (op == "<>")
		emitInequalityCode(operand1, operand2);
	else if (op == "<")
		emitLessThanCode(operand1, operand2);
	else if (op == "<=")
		emitLessThanOrEqualToCode(operand1, operand2);
	else if (op == ">")
		emitGreaterThanCode(operand1, operand2);
	else if (op == ">=")
		emitGreaterThanOrEqualToCode(operand1, operand2);
	else
		processError("compiler error since function code should not be called with illegal arguments");
}

void Compiler::pushOperator(string op)
{
	operatorStk.push(op);
}

string Compiler::popOperator()
{

	if (!operatorStk.empty())
		operatorStk.pop();
	else
	{
		cout << "compiler error; operator stack underflow\nLine: " << lineNo << "\nToken: " << token << endl;
		processError("compiler error; operator stack underflow");
	}
}

void Compiler::pushOperand(string operand)
{
	
	map<string, SymbolTableEntry>::iterator itr;
	string name = operand; // is this supposed to be operand? -gw
	itr = symbolTable.find(name);
	if (isLiteral(name) && itr != symbolTable.end())
	{
		//insert();
	}
}

string Compiler::popOperand()
{
	if (!operandStk.empty())
		operandStk.pop();
	else
	{
		cout << "compiler error; operand stack underflow\nLine: " << lineNo << "\nToken: " << token << endl;
		processError("compiler error; operand stack underflow");
	}
}

//EMIT ROUTINES
void Compiler::emit(string label, string instruction, string operands, string comment) //GTG
{
	objectFile << left << setw(8) << label << setw(8) << instruction << setw(24) << operands << comment << endl;
}

void Compiler::emitPrologue(string progName, string) // GTG
{
	time_t t = time(NULL);

	objectFile << ";Gavin Whitson - Joshua Stickland\t\t" << asctime(localtime(&t));
	objectFile << "%INCLUDE \"Along32.inc\"" << endl;
	objectFile << "%INCLUDE \"Macros_Along.inc\"" << endl;
	string fProgName = "; program " + progName;
	emit("\nSECTION", " .text", "", "");
	emit("global", "_start", "", fProgName);
	emit("\n_start:");
}

void Compiler::emitEpilogue(string operand1, string operand2) // GTG
{
	emit("", "Exit", "{0}");
	emitStorage();
}

void Compiler::emitStorage()
{

	emit("\nSECTION", " .data", "", "");
	string itemid;
	string comment;
	string val;
	map<string, SymbolTableEntry>::iterator itr;

	for (itr = symbolTable.begin(); itr != symbolTable.end(); itr++)
	{
		itemid = itr->first;
		comment = "; " + itemid;
		if (symbolTable.at(itemid).getMode() == CONSTANT && symbolTable.at(itemid).getDataType() != PROG_NAME)
		{
			val = symbolTable.at(itemid).getValue();
			if (val == "false")
				val = "0";
			else if (val == "true")
				val = "-1";
			emit(symbolTable.at(itemid).getInternalName(), "dd", val, comment);
		}
	}
	objectFile << endl;
	emit("SECTION", ".bss", "", "");
	for (itr = symbolTable.begin(); itr != symbolTable.end(); itr++)
	{
		itemid = itr->first;
		comment = "; " + itemid;
		if (symbolTable.at(itemid).getMode() == VARIABLE && symbolTable.at(itemid).getDataType() != PROG_NAME)
		{
			emit(symbolTable.at(itemid).getInternalName(), "resd", "1", comment);
		}
	}
}

void Compiler::emitReadCode(string operand, string operand2)
{

}
void Compiler::emitWriteCode(string operand, string operand2)
{

}
void Compiler::emitAssignCode(string operand1, string operand2)         // op2 = op1
{

}
void Compiler::emitAdditionCode(string operand1, string operand2)       // op2 +  op1
{

}
void Compiler::emitSubtractionCode(string operand1, string operand2)    // op2 -  op1
{

}
void Compiler::emitMultiplicationCode(string operand1, string operand2) // op2 *  op1
{

}
void Compiler::emitDivisionCode(string operand1, string operand2)       // op2 /  op1
{

}
void Compiler::emitModuloCode(string operand1, string operand2)         // op2 %  op1
{

}
void Compiler::emitNegationCode(string operand1, string operand2)           // -op1
{

}
void Compiler::emitNotCode(string operand1, string operand2)                // !op1
{

}
void Compiler::emitAndCode(string operand1, string operand2)            // op2 && op1
{

}
void Compiler::emitOrCode(string operand1, string operand2)             // op2 || op1
{

}
void Compiler::emitEqualityCode(string operand1, string operand2)       // op2 == op1
{

}
void Compiler::emitInequalityCode(string operand1, string operand2)     // op2 != op1
{

}
void Compiler::emitLessThanCode(string operand1, string operand2)       // op2 <  op1
{

}
void Compiler::emitLessThanOrEqualToCode(string operand1, string operand2) // op2 <= op1
{

}
void Compiler::emitGreaterThanCode(string operand1, string operand2)    // op2 >  op1
{

}
void Compiler::emitGreaterThanOrEqualToCode(string operand1, string operand2) // op2 >= op1
{

}

//LEXICAL ROUTINES
char Compiler::nextChar()  //GTG
{
	ch = sourceFile.get();
	if (sourceFile.eof())
	{
		ch = END_OF_FILE;
		return ch;
	}
	while (ch == '\n')
	{
		ch = sourceFile.get();
		if (sourceFile.eof())
		{
			ch = END_OF_FILE;
			listingFile << endl << endl;
			return ch;
		}
		lineNo++;
		listingFile << endl << right << setw(5) << lineNo << "|";
	}

	listingFile << ch;
	return ch;
}

string Compiler::nextToken() // GTG
{
	token = "";
	string com;
	while (token == "")
	{

		if (ch == '{')							// case '{'
		{
			com = ch;
			char x = nextChar();
			while (x != END_OF_FILE && x != '}')
			{
				if (x == '}')
				{
					com += ch;
					x = nextChar();
					break;
				}
				com += ch;
				x = nextChar();
			}
			if (ch == END_OF_FILE)
				processError("unexpected end of file");
			else
				nextChar();
		}

		else if (ch == '}')						// case '}'
			processError("'}' cannot begin token");

		else if (ch == ' ' || ch == '\t')						// case ' '
			nextChar();

		else if (isSpecialSymbol(ch))	// case isSpecialSymbol
		{
			token = ch;
			nextChar();
		}
		else if (islower(ch)) 	// case islower
		{
			token = ch;
			char x = nextChar();
			while (((isdigit(x)) || (islower(x)) || x == '_') && x != END_OF_FILE)
			{
				//cout << "token in progress -- " << token  << "    x = " << x<< endl;
				if (isKeyword(token) && isalpha(x))
					return token;
				token += x;
				x = nextChar();
			}
			if (ch == END_OF_FILE)
			{
				processError("unexpected end of file");
			}
		}

		else if (isdigit(ch))	// case isdigit
		{
			token = ch;
			char x = nextChar();
			while ((isdigit(x)) && ch != END_OF_FILE)
			{
				token += x;
				x = nextChar();
			}
			if (ch == END_OF_FILE)
				processError("unexpected end of file");
		}

		else if (ch == END_OF_FILE)
			token = ch;
		else
		{
			processError("illegal symbol");
		}
	}
	token = token.substr(0, 15);
	//cout << "token = " << token << endl;
	return token;
}
//OTHER ROUTINES                          
string Compiler::genInternalName(storeTypes stype) const //GTG
{
	static int count_bool = 0;
	static int count_ints = 0;
	static int count_prog = 0;

	string name = "";

	//my playground

	if (stype == BOOLEAN)
	{
		name = "B" + to_string(count_bool);
		count_bool++;
	}
	else if (stype == INTEGER)
	{
		name = "I" + to_string(count_ints);
		count_ints++;
	}
	else if (stype == PROG_NAME)
	{
		name = "P" + to_string(count_prog);
		count_prog++;
	}
	return name;
}

void Compiler::processError(string err) // GTG
{
	listingFile << endl << "Error: Line " << lineNo << ": " << err << endl << endl;
	errorCount++;
	cout << endl << "Error: Line " << lineNo << ": " << err << endl << endl;	// debug
	createListingTrailer();
}

void Compiler::freeTemp()
{
	currentTempNo--;
 	if (currentTempNo < -1)
 		processError("compiler error, currentTempNo should be ≥ –1:");
}

string Compiler::getTemp()
{
	string temp;
	currentTempNo++;
	temp = "T" + currentTempNo;
	if (currentTempNo > maxTempNo)
		insert(temp, UNKNOWN, VARIABLE, "", NO, 1);
	maxTempNo++;
	return temp;
}

bool Compiler::isTemporary(string s) const // determines if s represents a temporary
{

}
