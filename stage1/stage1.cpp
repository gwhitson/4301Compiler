//Gavin Whitson & Josh Strickland
//CS 4301
//Stage 1

#define _CRT_SECURE_NO_WARNINGS // visual studio only

#include <stage1.h>
#include <cctype> //needed for lexical functions
#include <iomanip> // needed for emit functions (setw())
#include <time.h> // needed for emit prologue
#include <stack>

bool insertFalse = false;
bool insertTrue = false;

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
		processError("keyword \"program\" expected -- prog");
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
		processError("keyword \"begin\" expected -- prog");
	beginEndStmt();
	if (token != "$")
		processError("no text may follow \"end\" -- prog");
}

void Compiler::progStmt() //token should be "program" GTG
{
	string x;
	if (token != "program")
		processError("keyword \"program\" expected -- progStmt");
	x = nextToken();
	if (!isNonKeyId(token))
		processError("program name expected -- progStmt");
	if (nextToken() != ";")
		processError("semicolon expected -- progStmt");
	nextToken();
	code("program", x);
	insert(x, PROG_NAME, CONSTANT, x, NO, 0);
}

void Compiler::consts() //token should be "const" GTG
{
	if (token != "const")
		processError("keyword \"const\" expected -- consts");
	if (!isNonKeyId(nextToken()))
		processError("non-keyword identifier must follow \"const\" -- consts");
	constStmts();
}

void Compiler::vars() //token should be "var" GTG
{
	if (token != "var")
		processError("keyword \"var\" expected -- var");
	if (!isNonKeyId(nextToken()))
		processError("non-keyword identifier must follow \"var\" -- var");
	varStmts();
}

void Compiler::beginEndStmt() //token should be "begin" GTG
{
	if (token != "begin")
		processError("keyword \"begin\" expected -- beginEndStmt");
	nextToken();
	while (token != "end" /*|| token.at(0) != END_OF_FILE || token != "."*/)
	{
		execStmts();
		nextToken();
		if (token[0] == END_OF_FILE)
		{
			processError("\"end\" expected -- beginEndStmt");
		}
	}

	if (nextToken() != ".")
		processError("period expected -- beginEndStmt");
	nextToken();
	code("end", ".");

	//execStmts();
	//if (nextToken() != "end")
	//	processError("keyword \"end\" expected");
	//if (nextToken() != ".")
	//	processError("period expected -- beginEndStmt");
	//nextToken();
	//code("end", ".");
}

void Compiler::constStmts() //token should be NON_KEY_ID GTG
{
	//cout << "constStmts token - " << token << endl;
	string x, y, z;
	if (!isNonKeyId(token))
		processError("non-keyword identifier expected -- constStmts");
	x = token;
	if (nextToken() != "=")
		processError("\"=\" expected");
	y = nextToken();
	if (y != "+" && y != "-" && y != "not" && !isNonKeyId(y) && !isBoolean(y) && !isInteger(y))
		processError("token to right of \"=\" illegal -- constStmts");
	if (y == "+" || y == "-")
	{
		if (!isInteger(nextToken()))
			processError("digit expected after sign -- constStmts");
		y = y + token;
	}
	if (y == "not")
	{
		z = nextToken();
		if (!isBoolean(z) && whichType(z) != BOOLEAN)
		{
			processError("boolean expected after “not” -- constStmts");
		}
		//cout << "past check     token z = " << z  << "   token y = " << y<< endl;
		if (token == "true" || whichValue(z) == "true")
			y = "false";
		else
			y = "true";
		//cout << "past replace value" << endl;
	}
	if (nextToken() != ";")
		processError("semicolon expected -- constStmts");
	if (whichType(y) != INTEGER && whichType(y) != BOOLEAN)
	{
		processError("data type of token on the right-hand side must be INTEGER or BOOLEAN -- constStmts");
	}

	insert(x, whichType(y), CONSTANT, whichValue(y), YES, 1);
	x = nextToken();
	if (x != "begin" && x != "var" && !isNonKeyId(x))
		processError("non-keyword identifier, \"begin\", or \"var\" expected -- constStmts");
	if (isNonKeyId(x))
		constStmts();
}

void Compiler::varStmts() // GTG
{
	string x, y, z;
	if (!isNonKeyId(token))
		processError("non-keyword identifier expected -- varStmts");
	x = ids();
	if (token != ":")
		processError("\":\" expected");
	z = nextToken();
	if (z != "integer" && z != "boolean")
		processError("illegal type follows \":\" -- varStmts");
	y = token;
	if (nextToken() != ";")
		processError("semicolon expected -- varStmts");
	insert(x, whichType(y), VARIABLE, "", YES, 1);
	z = nextToken();
	if (!isNonKeyId(z) && z != "begin")
		processError("non-keyword identifier or \"begin\" expected -- varStmts");
	if (isNonKeyId(token))
		varStmts();
}

string Compiler::ids() //token should be NON_KEY_ID GTG
{
	string temp, tempString;
	if (!isNonKeyId(token))
		processError("non-keyword identifier expected -- ids");
	tempString = token;
	temp = token;
	if (nextToken() == ",")
	{
		if (!isNonKeyId(nextToken()))
			processError("non-keyword identifier expected -- ids");
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
	cout << "execStmt" << endl;
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
{																						  //                                                                               // i feel like i need to rewrite this now
	cout << "assignStmt" << endl;
	if (isNonKeyId(token))																  //redundant but never hurts, opens possibility to throw special error too        // i feel like i need to rewrite this now
	{																					  //                                                                               // i feel like i need to rewrite this now
		pushOperand(token);
		nextToken();																	  //only if its still good, get next token                                         // i feel like i need to rewrite this now
		if (token == ":=")																  //check if its the assignment operator                                           // i feel like i need to rewrite this now
		{																				  //                                                                               // i feel like i need to rewrite this now
			pushOperator(token);
			nextToken();																  //get next token to send to express                                              // i feel like i need to rewrite this now
			express();																	  //goes to express? according to motls book on stage1                             // i feel like i need to rewrite this now
			//nextToken();																  //get next token again, this time we want a semicolon                            // i feel like i need to rewrite this now
			if (token != ";")															  //                                                                               // i feel like i need to rewrite this now
				processError("Semicolon expected -- assign stmt");						  //if theres no semicolon it throws an error                                      // i feel like i need to rewrite this now
		}																				  //                                                                               // i feel like i need to rewrite this now
		cout << "assignStmt pops" << endl;
		string opand1, opand2;
		opand1 = popOperand();
		opand2 = popOperand();
		code(popOperator(), opand2, opand1);
		
		//pushOperand("temp");
		pushOperand(contentsOfAReg);
		
	}                                                                                                                                                                      // i feel like i need to rewrite this now
	else                                                                                                                                                                   // i feel like i need to rewrite this now
		processError("how did you get here? -- assignStmt");                                                                                                               // i feel like i need to rewrite this now
		
	cout << "assignStmt close" << endl;
}
void Compiler::readStmt()       // stage 1, production 5, production 6 (readList) is included in the code for this one
{
	cout << "readStmt" << endl;
	if (token != "read")                                                                  //
		processError("read expected -- readStmt");                                        //hella redundant
	nextToken();                                                                          //
	if (token != "(")                                                                     //should be the start of the ids
		processError("\'(\' expected -- readStmt");                                       //
	nextToken();
	string tempIds = ids();                                                               //uhhhh were going to need to do something with this but im getting the stucture in now
																						  //ids seems to stop when nextToken finds something that is not a , after the word; idea is it should be the ) at this point
	if (token != ")")                                                                     //should be end of the ids part
		processError("\')\' expected -- readStmt");                                       //
	code("read", tempIds);
	nextToken();                                                                          //
	if (token != ";")                                                                     //semicolon at the end of this part
		processError("\';\' expected -- readStmt");                                       //
	cout << "readStmt close" << endl;
}
void Compiler::writeStmt()      // stage 1, production 7, 8's code is included here
{
	cout << "writeStmt" << endl;
	if (token != "write")
		processError("write expected -- writeStmt");                                      // these seem the exact same...
	nextToken();                                                                          // these seem the exact same...
	if (token != "(")                                                                     // these seem the exact same...
		processError("\'(\' expected -- writeStmt");                                      // these seem the exact same...
	nextToken();
	string tempIds = ids();                                                               // these seem the exact same...
																						  // these seem the exact same...
	if (token != ")")                                                                     // these seem the exact same...
		processError("\')\' expected -- writeStmt");                                      // these seem the exact same...
	code("write", tempIds);
	nextToken();                                                                          // these seem the exact same...
	if (token != ";")                                                                     // these seem the exact same...
		processError("\';\' expected -- writeStmt");                                      // these seem the exact same...
	cout << "writeStmt close with" << token << endl;
}
void Compiler::express()        // stage 1, production 9
{                                                                                                                         //these will probably be heavy on the stack shit
	cout << "express" << endl;
	//takes in some token -- prodcutions calling this should call nextToken before this production                        //these will probably be heavy on the stack shit
	term();                                                                                                               //these will probably be heavy on the stack shit
	expresses();
	cout << "express close with " << token << endl;
}
void Compiler::expresses()      // stage 1, production 10
{
	cout << "expresses" << endl;
	if (token == "=" || token == "<>" || token == "<=" || token == ">=" || token == "<" || token == ">")
	{
		string opand1, opand2, oper;
		pushOperator(token);
		nextToken();
		cout << " this next token call was in expresses" << endl; 
		term();
		cout << "expresses pops" << endl;
		opand1 = popOperand();
		opand2 = popOperand();
		oper = popOperator();
		code(oper, opand2, opand1);
		
		//pushOperand("temp");
		pushOperand(contentsOfAReg);
		
		expresses();
		
		//test
		//if (oper == "*")
		//{
		//	
		//}
	}
	cout << "expresses close" << endl;
}
void Compiler::term()           // stage 1, production 11
{
	cout << "term" << endl;
	//assuming this also takes in some token grabbed by previous production
	factor();
	//nextToken();
	cout << "nextToken in term" << endl;
	terms();
	
	cout << "term close with " << token << endl;

}
void Compiler::terms()          // stage 1, production 12
{
	cout << "terms" << endl;
	if (token == "+" || token == "-" || token == "or")
	{
		string opand1, opand2;
		pushOperator(token);
		factor();

		cout << "terms pops" << endl;
		opand1 = popOperand();					 // gen format code call
		opand2 = popOperand();					 // gen format code call
		code(popOperator(), opand2, opand1);	 // gen format code call
		
		//pushOperand("temp");
		pushOperand(contentsOfAReg);

		terms();
	}
	
	cout << "terms close with " << token << endl;
}
void Compiler::factor()         // stage 1, production 13
{
	cout << "factor" << endl;
	//assuming this ALSO takes in some token from a prior production
	part();
	factors();
	cout << "factor closed with"  << token << endl;
}
void Compiler::factors()        // stage 1, production 14
{
	cout << "factors" << endl;
	string temp = token;
	if (token == "*" || token == "div" || token == "mod" || token == "and")
	{
		string opand1, opand2;
		pushOperator(token);
		nextToken();
		part();
		
		
		cout << "factors pops" << endl;
		opand1 = popOperand();					 // gen format code call
		opand2 = popOperand();					 // gen format code call
		code(popOperator(), opand2, opand1);	 // gen format code call
		
		//test
		pushOperand(contentsOfAReg);
		
		//pushOperand("temp");
		

		factors();
	}
	cout << "factors close with " << token << endl;
}
void Compiler::part()           // stage 1, production 15
{
	cout << "part" << endl;
	if (token == "not")                                                                                                                            //   NOT SECTION
	{                                                                                                                                              //
		nextToken();                                                                                                                               //
		if (token == "(")                                                                                                                          //
		{                                                                                                                                          //
			nextToken();                                                                                                                           //
			express();                                                                                                                             //
			nextToken();                                                                                                                           //
			if (token != ")")                                                                                                                      //
				processError("\')\' expected -- part ugh .. this is the not section");                                                             //
			code("not", popOperand());                                                                                                             //
		}                                                                                                                                          //
		else if (isBoolean(token))                                                                                                                 //
		{                                                                                                                                          //
			if (token == "true")                                                                                                                   //
				pushOperand("false");                                                                                                              //
			else                                                                                                                                   //
				pushOperand("true");                                                                                                               //
		}                                                                                                                                          //
		else if (isNonKeyId(token))                                                                                                                //
		{                                                                                                                                          //
			if (whichType(token) != BOOLEAN)                                                                                                       //
			{                                                                                                                                      //
				processError("symbol of type BOOLEAN expected -- part (not, isNonKeyID)");                                                         //
			}                                                                                                                                      //
			code("not", token);                                                                                                                    //
		}                                                                                                                                          //
		else                                                                                                                                       //
			processError("We either needed a boolean, boolean value, or an expresssion equating to a boolean -- part (not)");                      //
	}                                                                                                                                              //
	else if (token == "+")                                                                                                                         //   + SECTION
	{                                                                                                                                              //
		nextToken();                                                                                                                               //
		if (token == "(")                                                                                                                          //
		{                                                                                                                                          //
			nextToken();                                                                                                                           //
			express();                                                                                                                             //
			nextToken();                                                                                                                           //
			if (token != ")")                                                                                                                      //
				processError("\')\' expected -- part ugh .. this is the + section");                                                               //
		}                                                                                                                                          //
		else if (token == "-")
		{
			nextToken();
			if (isInteger(token))
			{
				pushOperand("-" + token);
				nextToken();
			}
		}
		else if (isInteger(token))                                                                                                                 //
		{                                                                                                                                          //
			pushOperand(token);
			nextToken();
		}                                                                                                                                          //
		else if (isNonKeyId(token))                                                                                                                //
		{                                                                                                                                          //
			pushOperand(token);
			nextToken();
		}                                                                                                                                          //
		else                                                                                                                                       //
			processError("We either needed a integer, integer value, or an expresssion equating to a integer -- part (+)");                        //
	}                                                                                                                                              //
	else if (token == "-")                                                                                                                         //   - SECTION
	{                                                                                                                                              //
		nextToken();                                                                                                                               //
		if (token == "(")                                                                                                                          //
		{                                                                                                                                          //
			nextToken();                                                                                                                           //
			express();                                                                                                                             //
			nextToken();                                                                                                                           //
			if (token != ")")                                                                                                                      //
				processError("\')\' expected -- part ugh .. this is the - section");                                                               //
		}                                                                                                                                          //
		else if (isInteger(token))                                                                                                                 //
		{                                                                                                                                          //
			pushOperand("-" + token);
			nextToken();
		}                                                                                                                                          //
		else if (isNonKeyId(token))                                                                                                                //
		{                                                                                                                                          //
			pushOperand("-" + whichValue(token));
			nextToken();
		}                                                                                                                                          //
		else                                                                                                                                       //
			processError("We either needed a integer, integer value, or an expresssion equating to a integer -- part (-)");                        //
	}                                                                                                                                              //
	else if (token == "(") // another expression                                                                                                   //   SECOND EXPRESSION SECTION
	{                                                                                                                                              //
		nextToken();                                                                                                                               //
		express();                                                                                                                                 //
		//nextToken();                                                                                                                               //
		if (token != ")")                                                                                                                          //
			processError("\')\' expected -- part(expression)");                                                                                    //
		nextToken();
	}                                                                                                                                              //
	else if (isInteger(token))                                                                                                                     //  PURE INT SECTION
	{                                                                                                                                              //
			pushOperand(token);                                                                                                                    //
			nextToken();
	}                                                                                                                                              //
	else if (isBoolean(token))                                                                                                                     //  PURE BOOL SECTION
	{                                                                                                                                              //
			pushOperand(token);                                                                                                                    //
			nextToken();
	}                                                                                                                                              //
	else if (isNonKeyId(token))                                                                                                                    //   NON_KEY_ID SECTION
	{                                                                                                                                              //
			pushOperand(token);                                                                                                                    //
			nextToken();
	}                                                                                                                                              //
	else                                                                                                                                           //
		processError("fail in part");                                                                                                              //
		
	cout << "part found: " << token << " -- part closed"<< endl;
}


//HELPER FUNCTIONS
bool Compiler::isKeyword(string s) const // GTG
{
	if (s == "program" || s == "begin" || s == "end" || s == "var" || s == "const" || s == "integer" || s == "boolean" || s == "true" || s == "false" || s == "not" || s == "mod" || s == "div" || s == "and" || s == "or" || s == "read" || s == "write" || s == ":=" || s == "<=" || s == ">=" || s == "<>")
		return true;
	return false;
}

bool Compiler::isSpecialSymbol(char c) const// GTG
{
	if (c == '=' || c == ':' || c == ',' || c == ';' || c == '.' || c == '+' || c == '-' || ch == '(' || ch == ')' || ch == '*' || ch == '<' || ch == '>') //|| c == '-')
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
			processError("reference to undefined symbol -- whichType");
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
		if (itr != symbolTable.end() /*&& itr->second.getValue() != ""*/)         //getInternalName(symbolTable[name]) != NULL && getValue(symbolTable[name]) != NULL)	//name is an identifier and hopefully a constant
			value = itr->second.getValue();
		else
		{
			cout << "Token: " << token << endl;
			processError("reference to undefined symbol -- whichValue");
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
		emitReadCode(operand1);
	else if (op == "write")
		emitWriteCode(operand1);
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
	cout << "pushOperator: " << op << endl;
	operatorStk.push(op);
}

string Compiler::popOperator()
{
	string stackStr;
	if (!operatorStk.empty())
	{
		stackStr = operatorStk.top();
		operatorStk.pop();
	}
	else
	{
		cout << "compiler error; operator stack underflow\nLine: " << lineNo << "\nToken: " << token << endl;
		processError("compiler error; operator stack underflow");
	}
	return stackStr;
}

void Compiler::pushOperand(string operand)
{
	map<string, SymbolTableEntry>::iterator itr;
	itr = symbolTable.find(operand.substr(0,15));
	if (isLiteral(operand) && itr != symbolTable.end())
	{
		if (operand == "true")
		{
			if (!insertTrue)
			{
				if (symbolTable.size() == 255)
					processError("symbol table overflow");
				insert("TRUE",BOOLEAN,CONSTANT,"-1",YES,1);
				insertTrue = true;
			}
		}
		else if (operand == "false")
		{
			if (!insertFalse)
			{
				if (symbolTable.size() == 255)
					processError("symbol table overflow");
				insert("FALSE",BOOLEAN,CONSTANT,"0",YES,1);
				insertFalse = true;
			}
		}
	}
	else if (itr == symbolTable.end())                                                                            // unsure
		insert(operand, whichType(operand), CONSTANT, whichValue(operand), YES, 1);           // unsure
	
	
	cout << "pushOperand: " << operand << endl;
	operandStk.push(operand);
}

string Compiler::popOperand()
{
	string stackStr;
	if (!operandStk.empty())
	{
		stackStr = operandStk.top();
		operandStk.pop();
	}
	else
	{
		cout << "compiler error; operand stack underflow\nLine: " << lineNo << "\nToken: " << token << endl;
		processError("compiler error; operand stack underflow");
	}
	return stackStr;
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
	//static bool definedSorage = false;
	map<string, SymbolTableEntry>::iterator itr;
	while (operand.length() > 0)
	{
		uint index = operand.find(',');                              
		string name;

		if (index != 0 && index <= operand.length())                 
		{                                                            
			name = operand.substr(0, index);                      	 
			operand = operand.substr(index + 1, operand.length());   
		}                                                            
		else                                                         
		{                                                            
			index = operand.length();                                
			name = operand.substr(0, index);                         
			uint colon = name.find(':');
			if (colon > 0 && colon < name.length())
				name = name.substr(0, name.length() - 1);
			operand = operand.substr(index, operand.length());       
		}
		itr = symbolTable.find(name);
		if (itr != symbolTable.end())
		{
			
		}
		else
		{
			cout << "Name: " << name << endl;
			processError("reference to undefined symbol");
		}
		if (itr->second.getDataType() != INTEGER)
			processError("can't read variables of this type");
		if (itr->second.getMode() != VARIABLE)
			processError("attempting to read to a read-only location");
		emit(" ", "call", "ReadInt", "; read int; value placed in eax");
		emit(" ", "mov", "[" + itr->second.getInternalName() + "], eax", "; store eax at " + itr->first);
		contentsOfAReg = name;
	}
}

void Compiler::emitWriteCode(string operand, string operand2)
{
	//static bool definedSorage = false;
	map<string, SymbolTableEntry>::iterator itr;
	while (operand.length() > 0)
	{
		uint index = operand.find(',');                              
		string name;

		if (index != 0 && index <= operand.length())                 
		{                                                            
			name = operand.substr(0, index);                      	 
			operand = operand.substr(index + 1, operand.length());   
		}                                                            
		else                                                         
		{                                                            
			index = operand.length();                                
			name = operand.substr(0, index);                         
			uint colon = name.find(':');
			if (colon > 0 && colon < name.length())
				name = name.substr(0, name.length() - 1);
			operand = operand.substr(index, operand.length());       
		}
		itr = symbolTable.find(name);
		if (itr != symbolTable.end())
		{
			
		}
		else
		{
			cout << "Name: " << name << endl;
			processError("reference to undefined symbol");
		}
		if (itr->first != contentsOfAReg)
		{
			emit(" ", "mov", "eax, [" + itr->second.getInternalName() + "]", "; load " + name + " in eax");
			contentsOfAReg = name;
		}
		if (itr->second.getDataType() != INTEGER || itr->second.getDataType() != BOOLEAN)
			emit(" ", "call", "WriteInt", "; output contents of eax");
		emit(" ", "call", "Crlf", "; output newline");
	}
}

void Compiler::emitAssignCode(string operand1, string operand2)         // op2 = op1
{
	map<string, SymbolTableEntry>::iterator itr;
	itr = symbolTable.find(operand2);
	if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
		processError("incompatible types");
	if (symbolTable.at(operand2).getMode() != VARIABLE)
		processError("symbol on left-hand side of assignment must have a storage mode of VARIABLE");
	if (operand1 == operand2)
		return;
	if (operand1 != contentsOfAReg)
		emit(" ", "mov", "eax, [" + symbolTable.at(operand1).getInternalName() + "]", "; load " + symbolTable.at(operand1).getInternalName() + " in eax");	// mov	eax, [operand1's inName]
	emit(" ", "mov", "[" + symbolTable.at(operand2).getInternalName() + "], eax", "; store eax in " + symbolTable.at(operand2).getInternalName());
	if (isTemporary(operand1))
		freeTemp();
	contentsOfAReg = "";
}

void Compiler::emitAdditionCode(string operand1, string operand2)       // op2 +  op1
{
	cout << "operand1: " << operand1 << "    operand2: " << operand2 << endl;
	if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
		processError("incompatible types");
	if (isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 && contentsOfAReg != operand2))
	{
		emit(" ", "mov", "[" + contentsOfAReg + "], eax", "; deassign eax");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 && contentsOfAReg != operand2))
       contentsOfAReg = "";
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		emit(" ", "mov", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; eax = " + symbolTable.at(operand2).getValue());
		contentsOfAReg = operand2;
	}
	if (contentsOfAReg == operand1)
		emit(" ", "add", "eax, [" + symbolTable.at(operand1).getInternalName() + "]", "; eax = " + symbolTable.at(operand2).getValue() + " + " + symbolTable.at(operand1).getValue());
	else
		emit(" ", "add", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; eax = " + symbolTable.at(operand1).getValue() + " + " + symbolTable.at(operand2).getValue());
	
	if (isTemporary(contentsOfAReg))
	{
		freeTemp();
		contentsOfAReg = "";
	}
	if (isTemporary(operand1) && contentsOfAReg != operand1)
		freeTemp();
	if (isTemporary(operand2) && contentsOfAReg != operand2)
		freeTemp();
    contentsOfAReg = getTemp();
    symbolTable.at(contentsOfAReg).setDataType(INTEGER);
    pushOperand(contentsOfAReg);
}

void Compiler::emitSubtractionCode(string operand1, string operand2)    // op2 -  op1
{
	if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
		processError("incompatible types");
	if (isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 && contentsOfAReg != operand2))
	{
		emit(" ", "mov", "[" + contentsOfAReg + "], eax", "; deassign eax");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 && contentsOfAReg != operand2))
       contentsOfAReg = "";
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		emit(" ", "mov", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; eax = " + symbolTable.at(operand2).getValue());
		contentsOfAReg = operand2;
	}
	if (contentsOfAReg == operand1)
		emit(" ", "sub", "eax, [" + symbolTable.at(operand1).getInternalName() + "]", "; eax = " + symbolTable.at(operand2).getValue() + " - " + symbolTable.at(operand1).getValue());
	else
		emit(" ", "sub", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; eax = " + symbolTable.at(operand1).getValue() + " - " + symbolTable.at(operand2).getValue());

	if (isTemporary(contentsOfAReg))
	{
		freeTemp();
		contentsOfAReg = "";
	}
	if (isTemporary(operand1) && contentsOfAReg != operand1)
		freeTemp();
	if (isTemporary(operand2) && contentsOfAReg != operand2)
		freeTemp();
   contentsOfAReg = getTemp();
   symbolTable.at(contentsOfAReg).setDataType(INTEGER);
   pushOperand(contentsOfAReg);
}

void Compiler::emitMultiplicationCode(string operand1, string operand2) // op2 *  op1
{
	if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
		processError("incompatible types");
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand2)
	{
		emit(" ", "mov", "[" + contentsOfAReg + "], eax", "; deassign eax");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand2)
		contentsOfAReg = "";
	if (contentsOfAReg != operand2)
	{
		emit(" ", "mov", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; mov " + symbolTable.at(operand2).getInternalName() + " into eax");
		contentsOfAReg = operand2;
	}
	if (contentsOfAReg == operand2)
		emit(" ", "imul", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; eax = " + operand2 + " times " + operand1);
	else
		emit(" ", "imul", "dword [" + symbolTable.at(operand2).getInternalName() + "]", "; eax = " + operand1 + " times " + operand2); 
	if (isTemporary(contentsOfAReg))
	{
		freeTemp();
		contentsOfAReg = "";
	}
	if (isTemporary(operand1) && contentsOfAReg != operand1)
		freeTemp();
	if (isTemporary(operand2) && contentsOfAReg != operand2)
		freeTemp();
   contentsOfAReg = getTemp();
   symbolTable.at(contentsOfAReg).setDataType(INTEGER);
   pushOperand(contentsOfAReg);
}

void Compiler::emitDivisionCode(string operand1, string operand2)       // op2 /  op1
{
	if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
		processError("incompatible types");
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand2)
	{
		emit(" ", "mov", "[" + contentsOfAReg + "], eax", "; deassign eax");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand2)
		contentsOfAReg = "";
	if (contentsOfAReg != operand2)
	{
		emit(" ", "mov", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; mov " + symbolTable.at(operand2).getInternalName() + " into eax");
		contentsOfAReg = operand2;
	}
	emit(" ", "cdq", " ", "; extend sign of dividend from the A register to edx:eax");
	
	if (contentsOfAReg == operand2)
		emit(" ", "idiv", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; eax = " + operand2 + " divided by " + operand1);
	else
		emit(" ", "idiv", "dword [" + symbolTable.at(operand2).getInternalName() + "]", "; eax = " + operand1 + " divided by " + operand2); 
	if (isTemporary(contentsOfAReg))
	{
		freeTemp();
		contentsOfAReg = "";
	}
	if (isTemporary(operand1) && contentsOfAReg != operand1)
		freeTemp();
	if (isTemporary(operand2) && contentsOfAReg != operand2)
		freeTemp();
   contentsOfAReg = getTemp();
   symbolTable.at(contentsOfAReg).setDataType(INTEGER);
   pushOperand(contentsOfAReg);
}

void Compiler::emitModuloCode(string operand1, string operand2)         // op2 %  op1
{
	if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
		processError("incompatible types");
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand2)
	{
		emit(" ", "mov", "[" + contentsOfAReg + "], eax", "; deassign eax");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand2)
		contentsOfAReg = "";
	if (contentsOfAReg != operand2)
	{
		emit(" ", "mov", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; mov " + symbolTable.at(operand2).getInternalName() + " into eax");
		contentsOfAReg = operand2;
	}
	emit(" ", "cdq", " ", "; extend sign of dividend from the A register to edx:eax");
	if (contentsOfAReg == operand2)
		emit(" ", "idiv", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; eax = " + operand2 + " divided by " + operand1);
	else
		emit(" ", "idiv", "dword [" + symbolTable.at(operand2).getInternalName() + "]", "; eax = " + operand1 + " divided by " + operand2); 
	emit("", "xchg", "eax,edx", "remainder");
	if (isTemporary(contentsOfAReg))
	{
		freeTemp();
		contentsOfAReg = "";
	}
	if (isTemporary(operand1) && contentsOfAReg != operand1)
		freeTemp();
	if (isTemporary(operand2) && contentsOfAReg != operand2)
		freeTemp();
   contentsOfAReg = getTemp();
   symbolTable.at(contentsOfAReg).setDataType(INTEGER);
   pushOperand(contentsOfAReg);
}

void Compiler::emitNegationCode(string operand1, string operand2)           // -op1
{
	if (symbolTable.at(operand1).getDataType() != INTEGER)
		processError("incompatible types");
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1)
	{
		emit(" ", "mov", "[" + contentsOfAReg + "], eax", "; deassign eax");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1)
		contentsOfAReg = "";
	if (contentsOfAReg != operand1)
		emit(" ", "mov", "eax, [" + symbolTable.at(operand1).getInternalName() + "]", "; eax = " + operand1);
	emit(" ", "neg", "eax", "; negate eax");

	if (isTemporary(contentsOfAReg))
	{
		freeTemp();
		contentsOfAReg = "";
	}
	if (isTemporary(operand1) && contentsOfAReg != operand1)
		freeTemp();
   contentsOfAReg = getTemp();
   symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
   pushOperand(contentsOfAReg);
}

void Compiler::emitNotCode(string operand1, string operand2)                // !op1
{
    if (symbolTable.at(operand1).getDataType() != BOOLEAN || symbolTable.at(operand2).getDataType() != BOOLEAN)
		processError("illegal type");
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand2)
	{
		emit(" ", "mov", "[" + contentsOfAReg + "], eax", "; deassign eax");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1)
		contentsOfAReg = "";
	if (contentsOfAReg != operand1)
		emit(" ", "mov", "eax, [" + symbolTable.at(operand1).getInternalName() + "]", " ; eax = " + operand1);
	if (isTemporary(contentsOfAReg))
	{
		freeTemp();
		contentsOfAReg = "";
	}
	if (isTemporary(operand1) && contentsOfAReg != operand1)
		freeTemp();
	if (isTemporary(operand2) && contentsOfAReg != operand2)
		freeTemp();
   contentsOfAReg = getTemp();
   symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
   pushOperand(contentsOfAReg);
}

void Compiler::emitAndCode(string operand1, string operand2)            // op2 && op1
{
    if (symbolTable.at(operand1).getDataType() != BOOLEAN || symbolTable.at(operand2).getDataType() != BOOLEAN)
		processError("illegal type");
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand2)
	{
		emit(" ", "mov", "[" + contentsOfAReg + "], eax", "; deassign eax");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && ((contentsOfAReg != operand2) || contentsOfAReg != operand2))
		contentsOfAReg = "";
	if (contentsOfAReg != operand1 || contentsOfAReg != operand2)
		emit(" ", "mov", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; eax = " + symbolTable.at(operand2).getValue());
	if (contentsOfAReg == operand2)
		emit(" ", "and", "eax, [" + symbolTable.at(operand1).getInternalName() + "]", "; eax = " + operand2 + " and " + operand1);
	else if (contentsOfAReg == operand1)
		emit(" ", "and", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; eax = " + operand1 + " and " + operand2);
	if (isTemporary(contentsOfAReg))
	{
		freeTemp();
		contentsOfAReg = "";
	}
	if (isTemporary(operand1) && contentsOfAReg != operand1)
		freeTemp();
	if (isTemporary(operand2) && contentsOfAReg != operand2)
		freeTemp();
   contentsOfAReg = getTemp();
   symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
   pushOperand(contentsOfAReg);
}

void Compiler::emitOrCode(string operand1, string operand2)             // op2 || op1
{
    if (symbolTable.at(operand1).getDataType() != BOOLEAN || symbolTable.at(operand2).getDataType() != BOOLEAN)
		processError("illegal type");
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand2)
	{
		emit(" ", "mov", "[" + contentsOfAReg + "], eax", "; deassign eax");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && ((contentsOfAReg != operand2) || contentsOfAReg != operand2))
		contentsOfAReg = "";
	if (contentsOfAReg != operand1 || contentsOfAReg != operand2)
		emit(" ", "mov", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; eax = " + symbolTable.at(operand2).getValue());
	if (contentsOfAReg == operand2)
		emit(" ", "or", "eax, [" + symbolTable.at(operand1).getInternalName() + "]", "; eax = " + operand2 + " or " + operand1);
	else if (contentsOfAReg == operand1)
		emit(" ", "or", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; eax = " + operand1 + " or " + operand2);
	if (isTemporary(contentsOfAReg))
	{
		freeTemp();
		contentsOfAReg = "";
	}
	if (isTemporary(operand1) && contentsOfAReg != operand1)
		freeTemp();
	if (isTemporary(operand2) && contentsOfAReg != operand2)
		freeTemp();
   contentsOfAReg = getTemp();
   symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
   pushOperand(contentsOfAReg);
}

void Compiler::emitEqualityCode(string operand1, string operand2)       // op2 == op1
{
	cout << "operand1: " << operand1 << "    operand2: " << operand2 << endl;
	if (isLiteral(operand1) && isLiteral(operand2))
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
		processError("incompatible types");
	if (!isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 && contentsOfAReg != operand2))
		contentsOfAReg = "";
	if (contentsOfAReg != operand1 || contentsOfAReg != operand2)
	{
		emit(" ", "mov", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; eax = " + symbolTable.at(operand2).getValue());	// mov	eax, [operand2's inName]
        contentsOfAReg = operand2;
    }
    string label1 = getLabel();
    string label2 = getLabel();
    if (contentsOfAReg == operand2)
       emit(" ", "cmp", "eax, [" + symbolTable.at(operand1).getInternalName() + "]", "compare " + operand2 + " and " + operand1);
    else
       emit(" ", "cmp", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "compare " + operand1 + " and " + operand2);
    emit(" ", "je", label1, "; if " + operand2 + " = " + operand1 + " jump and mov TRUE into eax");
    emit(" ", "mov", "eax, [FALSE]", "; else mov FALSE into eax");
    
	if (!insertTrue)
    {
		if (symbolTable.size() == 255)
			processError("symbol table overflow");
		insert("TRUE",BOOLEAN,CONSTANT,"-1",YES,1);
		insertTrue = true;
    }
    if (!insertFalse)
    {
		if (symbolTable.size() == 255)
			processError("symbol table overflow");
		insert("FALSE",BOOLEAN,CONSTANT,"0",YES,1);
		insertFalse = true;
    }
    emit(" ", "jmp", label2, "; unconditional jump to the next label");
    emit(label1 + ":", " ", "; label");
    emit(" ", "mov", "eax, [TRUE]", "; mov TRUE into eax");
    emit(label2 + ":");

	if (isTemporary(contentsOfAReg))
	{
		freeTemp();
		contentsOfAReg = "";
	}
	if (isTemporary(operand1) && contentsOfAReg != operand1)
		freeTemp();
	if (isTemporary(operand2) && contentsOfAReg != operand2)
		freeTemp();
   contentsOfAReg = getTemp();
   symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
   pushOperand(contentsOfAReg);
}

void Compiler::emitInequalityCode(string operand1, string operand2)     // op2 != op1
{
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
		processError("incompatible types");
	if (!isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 && contentsOfAReg != operand2))
		contentsOfAReg = "";
	if (contentsOfAReg != operand1 || contentsOfAReg != operand2)
	{
		emit(" ", "mov", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; eax = " + symbolTable.at(operand2).getValue());	// mov	eax, [operand2's inName]
        contentsOfAReg = operand2;
    }
    string label1 = getLabel();
    string label2 = getLabel();
    if (contentsOfAReg == operand2)
       emit(" ", "cmp", "eax, [" + symbolTable.at(operand1).getInternalName() + "]", "compare " + operand2 + " and " + operand1);
    else
       emit(" ", "cmp", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "compare " + operand1 + " and " + operand2);
    emit(" ", "jne", label1, "; if " + operand2 + " <> " + operand1 + " jump and mov TRUE into eax");
    emit(" ", "mov", "eax, [FALSE]", "; else mov FALSE into eax");
    
	if (!insertTrue)
    {
		if (symbolTable.size() == 255)
			processError("symbol table overflow");
		insert("TRUE",BOOLEAN,CONSTANT,"-1",YES,1);
		insertTrue = true;
    }
    if (!insertFalse)
    {
		if (symbolTable.size() == 255)
			processError("symbol table overflow");
		insert("FALSE",BOOLEAN,CONSTANT,"0",YES,1);
		insertFalse = true;
    }
    emit(" ", "jmp", label2, "; unconditional jump to the next label");
    emit(label1 + ":", " ", "; label");
    emit(" ", "mov", "eax, [TRUE]", "; mov TRUE into eax");
    emit(label2 + ":");

	if (isTemporary(contentsOfAReg))
	{
		freeTemp();
		contentsOfAReg = "";
	}
	if (isTemporary(operand1) && contentsOfAReg != operand1)
		freeTemp();
	if (isTemporary(operand2) && contentsOfAReg != operand2)
		freeTemp();
   contentsOfAReg = getTemp();
   symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
   pushOperand(contentsOfAReg);
}

void Compiler::emitLessThanCode(string operand1, string operand2)       // op2 <  op1
{
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
		processError("incompatible types");
	if (!isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 && contentsOfAReg != operand2))
		contentsOfAReg = "";
	if (contentsOfAReg != operand1 || contentsOfAReg != operand2)
	{
		emit(" ", "mov", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; eax = " + symbolTable.at(operand2).getValue());	// mov	eax, [operand2's inName]
        contentsOfAReg = operand2;
    }
    string label1 = getLabel();
    string label2 = getLabel();
    if (contentsOfAReg == operand2)
	{
       emit(" ", "cmp", "eax, [" + symbolTable.at(operand1).getInternalName() + "]", "compare " + operand2 + " and " + operand1);
	   emit(" ", "jl", label1, "; if " + operand2 + " < " + operand1 + " jump and mov TRUE into eax");
	}
    else
	{
       emit(" ", "cmp", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "compare " + operand1 + " and " + operand2);
	   emit(" ", "jl", label1, "; if " + operand1 + " < " + operand2 + " jump and mov TRUE into eax");
	}
    emit(" ", "mov", "eax, [FALSE]", "; else mov FALSE into eax");
    
	if (!insertTrue)
    {
		if (symbolTable.size() == 255)
			processError("symbol table overflow");
		insert("TRUE",BOOLEAN,CONSTANT,"-1",YES,1);
		insertTrue = true;
    }
    if (!insertFalse)
    {
		if (symbolTable.size() == 255)
			processError("symbol table overflow");
		insert("FALSE",BOOLEAN,CONSTANT,"0",YES,1);
		insertFalse = true;
    }
    emit(" ", "jmp", label2, "; unconditional jump to the next label");
    emit(label1 + ":", " ", "; label");
    emit(" ", "mov", "eax, [TRUE]", "; mov TRUE into eax");
    emit(label2 + ":");

	if (isTemporary(contentsOfAReg))
	{
		freeTemp();
		contentsOfAReg = "";
	}
	if (isTemporary(operand1) && contentsOfAReg != operand1)
		freeTemp();
	if (isTemporary(operand2) && contentsOfAReg != operand2)
		freeTemp();
   contentsOfAReg = getTemp();
   symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
   pushOperand(contentsOfAReg);
}

void Compiler::emitLessThanOrEqualToCode(string operand1, string operand2) // op2 <= op1
{
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
		processError("incompatible types");
	if (!isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 && contentsOfAReg != operand2))
		contentsOfAReg = "";
	if (contentsOfAReg != operand1 || contentsOfAReg != operand2)
	{
		emit(" ", "mov", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; eax = " + symbolTable.at(operand2).getValue());	// mov	eax, [operand2's inName]
        contentsOfAReg = operand2;
    }
    string label1 = getLabel();
    string label2 = getLabel();
    if (contentsOfAReg == operand2)
	{
       emit(" ", "cmp", "eax, [" + symbolTable.at(operand1).getInternalName() + "]", "compare " + operand2 + " and " + operand1);
	   emit(" ", "jle", label1, "; if " + operand2 + " <= " + operand1 + " jump and mov TRUE into eax");
	}
    else
	{
       emit(" ", "cmp", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "compare " + operand1 + " and " + operand2);
	   emit(" ", "jle", label1, "; if " + operand1 + " <= " + operand2 + " jump and mov TRUE into eax");
	}
    emit(" ", "mov", "eax, [FALSE]", "; else mov FALSE into eax");
    
	if (!insertTrue)
    {
		if (symbolTable.size() == 255)
			processError("symbol table overflow");
		insert("TRUE",BOOLEAN,CONSTANT,"-1",YES,1);
		insertTrue = true;
    }
    if (!insertFalse)
    {
		if (symbolTable.size() == 255)
			processError("symbol table overflow");
		insert("FALSE",BOOLEAN,CONSTANT,"0",YES,1);
		insertFalse = true;
    }
    emit(" ", "jmp", label2, "; unconditional jump to the next label");
    emit(label1 + ":", " ", "; label");
    emit(" ", "mov", "eax, [TRUE]", "; mov TRUE into eax");
    emit(label2 + ":");

	if (isTemporary(contentsOfAReg))
	{
		freeTemp();
		contentsOfAReg = "";
	}
	if (isTemporary(operand1) && contentsOfAReg != operand1)
		freeTemp();
	if (isTemporary(operand2) && contentsOfAReg != operand2)
		freeTemp();
   contentsOfAReg = getTemp();
   symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
   pushOperand(contentsOfAReg);
}
void Compiler::emitGreaterThanCode(string operand1, string operand2)    // op2 >  op1
{
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
		processError("incompatible types");
	if (!isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 && contentsOfAReg != operand2))
		contentsOfAReg = "";
	if (contentsOfAReg != operand1 || contentsOfAReg != operand2)
	{
		emit(" ", "mov", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; eax = " + symbolTable.at(operand2).getValue());	// mov	eax, [operand2's inName]
        contentsOfAReg = operand2;
    }
    string label1 = getLabel();
    string label2 = getLabel();
    if (contentsOfAReg == operand2)
	{
       emit(" ", "cmp", "eax, [" + symbolTable.at(operand1).getInternalName() + "]", "compare " + operand2 + " and " + operand1);
	   emit(" ", "jg", label1, "; if " + operand2 + " > " + operand1 + " jump and mov TRUE into eax");
	}
    else
	{
       emit(" ", "cmp", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "compare " + operand1 + " and " + operand2);
	   emit(" ", "jg", label1, "; if " + operand1 + " > " + operand2 + " jump and mov TRUE into eax");
	}
    emit(" ", "mov", "eax, [FALSE]", "; else mov FALSE into eax");
    
	if (!insertTrue)
    {
		if (symbolTable.size() == 255)
			processError("symbol table overflow");
		insert("TRUE",BOOLEAN,CONSTANT,"-1",YES,1);
		insertTrue = true;
    }
    if (!insertFalse)
    {
		if (symbolTable.size() == 255)
			processError("symbol table overflow");
		insert("FALSE",BOOLEAN,CONSTANT,"0",YES,1);
		insertFalse = true;
    }
    emit(" ", "jmp", label2, "; unconditional jump to the next label");
    emit(label1 + ":", " ", "; label");
    emit(" ", "mov", "eax, [TRUE]", "; mov TRUE into eax");
    emit(label2 + ":");

	if (isTemporary(contentsOfAReg))
	{
		freeTemp();
		contentsOfAReg = "";
	}
	if (isTemporary(operand1) && contentsOfAReg != operand1)
		freeTemp();
	if (isTemporary(operand2) && contentsOfAReg != operand2)
		freeTemp();
   contentsOfAReg = getTemp();
   symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
   pushOperand(contentsOfAReg);
}

void Compiler::emitGreaterThanOrEqualToCode(string operand1, string operand2) // op2 >= op1
{
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
		processError("incompatible types");
	if (!isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 && contentsOfAReg != operand2))
		contentsOfAReg = "";
	if (contentsOfAReg != operand1 || contentsOfAReg != operand2)
	{
		emit(" ", "mov", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; eax = " + symbolTable.at(operand2).getValue());	// mov	eax, [operand2's inName]
        contentsOfAReg = operand2;
    }
    string label1 = getLabel();
    string label2 = getLabel();
    if (contentsOfAReg == operand2)
	{
       emit(" ", "cmp", "eax, [" + symbolTable.at(operand1).getInternalName() + "]", "compare " + operand2 + " and " + operand1);
	   emit(" ", "jge", label1, "; if " + operand2 + " >= " + operand1 + " jump and mov TRUE into eax");
	}
    else
	{
       emit(" ", "cmp", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "compare " + operand1 + " and " + operand2);
	   emit(" ", "jge", label1, "; if " + operand1 + " >= " + operand2 + " jump and mov TRUE into eax");
	}
    emit(" ", "mov", "eax, [FALSE]", "; else mov FALSE into eax");
    
	if (!insertTrue)
    {
		if (symbolTable.size() == 255)
			processError("symbol table overflow");
		insert("TRUE",BOOLEAN,CONSTANT,"-1",YES,1);
		insertTrue = true;
    }
    if (!insertFalse)
    {
		if (symbolTable.size() == 255)
			processError("symbol table overflow");
		insert("FALSE",BOOLEAN,CONSTANT,"0",YES,1);
		insertFalse = true;
    }
    emit(" ", "jmp", label2, "; unconditional jump to the next label");
    emit(label1 + ":", " ", "; label");
    emit(" ", "mov", "eax, [TRUE]", "; mov TRUE into eax");
    emit(label2 + ":");

	if (isTemporary(contentsOfAReg))
	{
		freeTemp();
		contentsOfAReg = "";
	}
	if (isTemporary(operand1) && contentsOfAReg != operand1)
		freeTemp();
	if (isTemporary(operand2) && contentsOfAReg != operand2)
		freeTemp();
   contentsOfAReg = getTemp();
   symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
   pushOperand(contentsOfAReg);
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
				processError("unexpected end of file -- nextToken");
			else
				nextChar();
		}

		else if (ch == '}')						// case '}'
			processError("'}' cannot begin token -- nextToken");

		else if (ch == ' ' || ch == '\t')						// case ' '
			nextChar();

		else if (isSpecialSymbol(ch))	// case isSpecialSymbol
		{
			token = ch;
			nextChar();
			if (ch == '=')
			{
				token += ch;
				nextChar();
			}
			
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
				processError("unexpected end of file -- nextToken");
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
				processError("unexpected end of file -- nextToken");
		}

		else if (ch == END_OF_FILE)
			token = ch;
		else
		{
			cout << int(ch) << endl;
			processError("illegal symbol -- nextToken");
		}
	}
	token = token.substr(0, 15);
	cout << "token = " << token << "    -- nextToken" << endl;
	return token;
}
//OTHER ROUTINES                          
string Compiler::genInternalName(storeTypes stype) const //GTG
{
	static int count_bool = 0;
	static int count_ints = 0;
	static int count_prog = 0;

	string name = "";

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
	temp = "T" + to_string(currentTempNo);
	if (currentTempNo > maxTempNo)
		insert(temp, UNKNOWN, VARIABLE, "", NO, 1);
	maxTempNo++;
	return temp;
}

string Compiler::getLabel()
{
	static int count_label = 0;
	string str = ".L" + to_string(count_label);
	count_label++;
	return str;
}

bool Compiler::isTemporary(string s) const // determines if s represents a temporary
{
	if (s.substr(0,1) == "T")
		return true;
	return false;
}
