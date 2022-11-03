#ifndef STAGE0_H
#define STAGE0_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

const char END_OF_FILE = '$';      // arbitrary choice

enum storeTypes {INTEGER, BOOLEAN, PROG_NAME};
enum modes {VARIABLE, CONSTANT};
enum allocation {YES, NO};

class SymbolTableEntry
{
public:
  SymbolTableEntry(string in, storeTypes st, modes m,
                   string v, allocation a, int u)
  {
    setInternalName(in);
    setDataType(st);
    setMode(m);
    setValue(v);
    setAlloc(a);
    setUnits(u);
  }    

  string getInternalName() const
  {
    return internalName;
  }

  storeTypes getDataType() const
  {
    return dataType;
  }

  modes getMode() const
  {
    return mode;
  }

  string getValue() const
  {
    return value;
  }

  allocation getAlloc() const
  {
    return alloc;
  }

  int getUnits() const
  {
    return units;
  }

  void setInternalName(string s)
  {
    internalName = s;
  }

  void setDataType(storeTypes st)
  {
    dataType = st;
  }

  void setMode(modes m)
  {
    mode = m;
  }

  void setValue(string s)
  {
    value = s;
  }

  void setAlloc(allocation a)
  {
    alloc = a;
  }

  void setUnits(int i)
  {
    units = i;
  }

private:
  string internalName;
  storeTypes dataType;
  modes mode;
  string value;
  allocation alloc;
  int units;
};

class Compiler
{
public:
  Compiler(char **argv); // constructor
  ~Compiler();           // destructor

  void createListingHeader();
  void parser();
  void createListingTrailer();

  // Methods implementing the grammar productions
  void prog();           // stage 0, production 1
  void progStmt();       // stage 0, production 2
  void consts();         // stage 0, production 3
  void vars();           // stage 0, production 4
  void beginEndStmt();   // stage 0, production 5
  void constStmts();     // stage 0, production 6
  void varStmts();       // stage 0, production 7
  string ids();          // stage 0, production 8

  // Helper functions for the Pascallite lexicon
  bool isKeyword(string s) const;  // determines if s is a keyword
  bool isSpecialSymbol(char c) const; // determines if c is a special symbol
  bool isNonKeyId(string s) const; // determines if s is a non_key_id
  bool isInteger(string s) const;  // determines if s is an integer
  bool isBoolean(string s) const;  // determines if s is a boolean
  bool isLiteral(string s) const;  // determines if s is a literal

  // Action routines                                                                    //mine to do
  void insert(string externalName, storeTypes inType, modes inMode,                     //mine to do
              string inValue, allocation inAlloc, int inUnits);                         //mine to do
  storeTypes whichType(string name); // tells which data type a name has                //mine to do
  string whichValue(string name); // tells which value a name has                       //mine to do
  void code(string op, string operand1 = "", string operand2 = "");                     //mine to do
																						//mine to do
  // Emit Functions                                                                     //mine to do
  void emit(string label = "", string instruction = "", string operands = "",           //mine to do
            string comment = "");                                                       //mine to do
  void emitPrologue(string progName, string = "");                                      //mine to do
  void emitEpilogue(string = "", string = "");                                          //mine to do
  void emitStorage();                                                                   //mine to do
																						//mine to do
  // Lexical routines                                                                   //mine to do
  char nextChar(); // returns the next character or END_OF_FILE marker                  //mine to do
  string nextToken(); // returns the next token or END_OF_FILE marker                   //mine to do
																						//mine to do
  // Other routines                                                                     //mine to do
  string genInternalName(storeTypes stype) const;                                       //mine to do
  void processError(string err);                                                        //mine to do
																						//mine to do
private:                                                                                //mine to do
  map<string, SymbolTableEntry> symbolTable;                                            //mine to do
  ifstream sourceFile;                                                                  //mine to do
  ofstream listingFile;                                                                 //mine to do
  ofstream objectFile;                                                                  //mine to do
  string token;          // the next token                                              //mine to do
  char ch;               // the next character of the source file                       //mine to do
  uint errorCount = 0;   // total number of errors encountered                          //mine to do
  uint lineNo = 0;       // line numbers for the listing                                //mine to do
};                                                                                      //mine to do
																						//mine to do
#endif
