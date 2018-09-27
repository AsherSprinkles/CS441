#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <cctype>

using namespace std;

const char UNSET = -2;
const char CC_EOL = '\n';
const char CC_EOF = -1;
const char CC_PERIOD = '.';
const char CC_LEFTPAR = '(';
const char CC_RIGHTPAR = ')';
const char CC_STAR = '*';
const char CC_COLON = ':';
const char CC_LESSTHAN = '<';
const char CC_GREATTHAN = '>';
const char CC_EQUAL = '=';
const char CC_QUOTE = '\'';
const string CC_SYMBOLS = "+-/[],;^";
const char CC_MIN = ' '; // inclusive
const char CC_MAX = '~'; // exclusive
const int STRWIDTH = 5;

enum class CharCat {eol, eof, whitespc, alpha, digit, period, leftpar, rightpar, star, colon, lessthan,
		greatthan, equals, quote, sym, other, invalid=-1, unknown=-2};

enum class LexCat {identifier, symbol, integer, real, character, error=-1, none=-2};

enum class ScannerState {start, ident, number, decimalpt, floatingpoint, character, singlequote, singlesymbol,
		lsymbol, rsymbol, lpar, incomment, commentstar, error=-1, end};

class Lexeme {
	string body;
	int line;
	int column;
	LexCat type;
	string errorMessage;

	friend class LexAnalyzer;
public:
	Lexeme();
	void print();
}; // Lexeme

class LexAnalyzer {
	int currentLine;
	int currentColumn;
	CharCat currentCategory;
	char currentChar;
	ScannerState state;
	bool end_of_file;
	ifstream file;

	void getNextChar();
	CharCat categorizeChar(char c);
	void handleAlpha(Lexeme &lex);
	void handleNumber(Lexeme &lex);
	void handleString(Lexeme &lex);
	void handleLPar(Lexeme &lex);
	void handleLSymbol(Lexeme &lex);
	void handleSymbol(Lexeme &lex);
public:
	LexAnalyzer(string filename);
	Lexeme getNextLexeme();

	// starts the lexAnalyzer
	void analyze();
}; // LexAnalyzer

bool issymbol(char c);
