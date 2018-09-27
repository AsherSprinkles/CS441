#include "Proj1.h"

//----------------------------------------------------------------------
// 							Lexeme Constructor
//----------------------------------------------------------------------
Lexeme::Lexeme() {
	body = "";
	line = 0;
	column = 0;
	type = LexCat::none;
	errorMessage = "";
}

//----------------------------------------------------------------------
// 							Lexeme::print
//----------------------------------------------------------------------
// Outputs formatted lexeme information to stdout (or stderr for errors)
//----------------------------------------------------------------------
void Lexeme::print() {
	if (type != LexCat::error) {
		cout << setw(STRWIDTH) << line << setw(STRWIDTH) << column << setw(STRWIDTH)
		 	<< (int)type << "\t" << body << endl;
	}
	else {
		cerr << setw(STRWIDTH) << line << setw(STRWIDTH) << column << "\t"
			<< "Error: " << errorMessage << endl;
	}
}

//----------------------------------------------------------------------
// 							LexAnalyzer Constructor
//----------------------------------------------------------------------
LexAnalyzer::LexAnalyzer(string filename) {
	currentLine = 1;
	currentColumn = 0;
	currentChar = UNSET;
	currentCategory = CharCat::unknown;
	state = ScannerState::end;
	end_of_file = false;
	file.open(filename.c_str());
	if (!file.is_open()) {
		cerr << "File read error\n";
		exit(1);
	}
}

//----------------------------------------------------------------------
// 							LexAnalyzer::analyze
//----------------------------------------------------------------------
// Calls getNextLexeme repeatedly, printing each successive lexeme.
//----------------------------------------------------------------------
void LexAnalyzer::analyze() {
	Lexeme lex;
	do {
		lex = getNextLexeme();
		if (lex.type != LexCat::none) {
			lex.print();
		}
	} while(!end_of_file && state != ScannerState::error);
}

//----------------------------------------------------------------------
// 							LexAnalyzer::getNextLexeme
//----------------------------------------------------------------------
// Starting function for the FSM. Returns the next lexeme in the file
// along with all relevant information (type, line/col #)
//----------------------------------------------------------------------
Lexeme LexAnalyzer::getNextLexeme() {
	Lexeme lex;
	if (state == ScannerState::error || end_of_file)
		return lex;
	else
		state = ScannerState::start;
	if (currentChar == UNSET) {
		getNextChar();
	}
	char first = currentChar;
	CharCat firstCategory = categorizeChar(first);
	// ignore whitespace at start of lexeme
	if (firstCategory == CharCat::eol || firstCategory == CharCat::whitespc) {
		do {
			getNextChar();
			first = currentChar;
			firstCategory = categorizeChar(currentChar);
		} while(firstCategory == CharCat::eol || firstCategory == CharCat::whitespc);
	}
	if (end_of_file) {
		return lex;
	}
	if (firstCategory == CharCat::invalid || firstCategory == CharCat::other) {
		lex.line = currentLine;
		lex.column = currentColumn;
		return lex;
	}
	else { // valid start
		lex.body += first;
		lex.line = currentLine;
		lex.column = currentColumn;
		if (firstCategory == CharCat::alpha) {
			state = ScannerState::ident;
			handleAlpha(lex);
		}
		else if (firstCategory == CharCat::digit) {
			state = ScannerState::number;
			handleNumber(lex);
		}
		else if (firstCategory == CharCat::quote) {
			state = ScannerState::character;
			handleString(lex);
		}
		else if (firstCategory == CharCat::leftpar) {
			state = ScannerState::lpar;
			handleLPar(lex);
		}
		else if (firstCategory == CharCat::lessthan || firstCategory == CharCat::greatthan
			|| firstCategory == CharCat::colon || firstCategory == CharCat::period)
		{
			state = ScannerState::lsymbol;
			handleLSymbol(lex);
		}
		else {
			state = ScannerState::singlesymbol;
			handleSymbol(lex);
		}
	}
	return lex;
}

//----------------------------------------------------------------------
// 							LexAnalyzer::handleAlpha
//----------------------------------------------------------------------
// Continues the FSM from the start state for identifiers
//----------------------------------------------------------------------
void LexAnalyzer::handleAlpha(Lexeme &lex) {
	lex.type = LexCat::identifier;
	getNextChar();
	while(currentCategory == CharCat::digit || currentCategory == CharCat::alpha) {
		state = ScannerState::ident;
		lex.body += currentChar;
		getNextChar();
	}
	state = ScannerState::end;
	return;
}

//----------------------------------------------------------------------
// 							LexAnalyzer::handleNumber
//----------------------------------------------------------------------
// Continues the FSM from the start state for numbers (floats and ints)
//----------------------------------------------------------------------
void LexAnalyzer::handleNumber(Lexeme &lex) {
	getNextChar();
	while(currentCategory == CharCat::digit) {
		lex.body += currentChar;
		getNextChar();
	}
	if (currentCategory == CharCat::period) { // 123.
		state = ScannerState::decimalpt;
		char next = file.peek(); // for the case of 123..
		currentCategory = categorizeChar(next);
		if (currentCategory == CharCat::period) {
			lex.type = LexCat::integer;
			state = ScannerState::end; // leave period for next lexeme
			return;
		}
		else { // 123.x
			lex.body += currentChar;
			getNextChar();
			if (currentCategory == CharCat::digit) {
				state = ScannerState::floatingpoint;
				while (currentCategory == CharCat::digit) {
					lex.body += currentChar;
					getNextChar();
				}
			}
			else {
				lex.type = LexCat::error;
				state = ScannerState::error;
				lex.errorMessage = "Following Digit Expected";
				return;
			}
		}
	}
	if (state == ScannerState::number) {
		lex.type = LexCat::integer;
	}
	else if (state == ScannerState::floatingpoint) {
		lex.type = LexCat::real;
	}
}

//----------------------------------------------------------------------
// 							LexAnalyzer::handleString
//----------------------------------------------------------------------
// Continues the FSM from the start state for character literals.
//----------------------------------------------------------------------
void LexAnalyzer::handleString(Lexeme &lex) {
	lex.type = LexCat::character;
	while(currentCategory != CharCat::eol && currentCategory != CharCat::eof) {
		getNextChar();
		if (currentCategory != CharCat::eol && currentCategory != CharCat::eof
			&& currentCategory != CharCat::quote)
		{
			lex.body += currentChar;
		}
		else if (currentCategory == CharCat::eof || currentCategory == CharCat::eol) {
			lex.type = LexCat::error;
			lex.errorMessage = "End of character literal expected";
			state = ScannerState::error;
			lex.line = currentLine;
			lex.column = currentColumn;
			return;
		}
		else if (currentCategory == CharCat::quote) {
			lex.body += currentChar;
			getNextChar();
			if (currentCategory == CharCat::quote) {
				continue;
			}
			else {
				state = ScannerState::end;
				return;
			}
		}
	}
}

//----------------------------------------------------------------------
// 							LexAnalyzer::handleLPar
//----------------------------------------------------------------------
// Continues the FSM from the start state for left parentheses
//----------------------------------------------------------------------
void LexAnalyzer::handleLPar(Lexeme &lex) {
	getNextChar();
	if (currentCategory == CharCat::star) {
		state = ScannerState::incomment;
		lex.type = LexCat::none;
		getNextChar();
		while(currentCategory != CharCat::eof) {
			if (currentCategory == CharCat::star) {
				state = ScannerState::commentstar;
				getNextChar();
				if (currentCategory == CharCat::rightpar) {
					state == ScannerState::end;
					getNextChar();
					return;
				}
				else continue;
			}
			getNextChar();
		}
		if (currentCategory == CharCat::eof) {
			lex.type = LexCat::error;
			lex.errorMessage = "End of Comment Expected";
			state = ScannerState::error;
			lex.line = currentLine;
			lex.column = currentColumn;
			return;
		}
	}
	else {
		lex.type = LexCat::symbol;
		state = ScannerState::end;
	}
}

//----------------------------------------------------------------------
// 							LexAnalyzer::handleLSymbol
//----------------------------------------------------------------------
// Continues the FSM from the start state for symbols that may be two-char
//----------------------------------------------------------------------
void LexAnalyzer::handleLSymbol(Lexeme &lex) {
	lex.type = LexCat::symbol;
	if (currentCategory == CharCat::lessthan) {
		getNextChar();
		if (currentCategory == CharCat::greatthan) {
			lex.body += currentChar;
			state = ScannerState::end;
			getNextChar();
			return;
		}
		else if (currentCategory == CharCat::equals) {
			lex.body += currentChar;
			state = ScannerState::end;
			getNextChar();
			return;
		}
		else {
			state = ScannerState::end;
			return;
		}
	}
	else if (currentCategory == CharCat::greatthan) {
		getNextChar();
		if (currentCategory == CharCat::equals) {
			lex.body += currentChar;
			state = ScannerState::end;
			getNextChar();
			return;
		}
		else {
			state = ScannerState::end;
			return;
		}
	}
	else if (currentCategory == CharCat::colon) {
		getNextChar();
		if (currentCategory == CharCat::equals) {
			lex.body += currentChar;
			state = ScannerState::end;
			getNextChar();
			return;
		}
		else {
			state = ScannerState::end;
			return;
		}
	}
	else if (currentCategory == CharCat::period) {
		getNextChar();
		if (currentCategory == CharCat::period) {
			lex.body += currentChar;
			state = ScannerState::end;
			getNextChar();
			return;
		}
		else {
			state = ScannerState::end;
			return;
		}
	}
}

//----------------------------------------------------------------------
// 							LexAnalyzer::handleSymbol
//----------------------------------------------------------------------
// Continues the FSM from the start state for single symbols.
//----------------------------------------------------------------------
void LexAnalyzer::handleSymbol(Lexeme &lex) {
	state == ScannerState::end;
	lex.type = LexCat::symbol;
	getNextChar();
}
//----------------------------------------------------------------------
// 							LexAnalyzer::getNextChar
//----------------------------------------------------------------------
// updates line #, column #, currentChar, and currentCategory.
//----------------------------------------------------------------------
void LexAnalyzer::getNextChar() {
	currentChar = file.get();
	currentCategory = categorizeChar(currentChar);
	if(currentChar == CC_EOL) {
		currentLine += 1;
		currentColumn = 0;
	}
	else if (currentChar == CC_EOF){
		cout << "END OF FILE" << endl;
		end_of_file = true;
	}
	else {
		currentColumn += 1;
	}

	if (currentCategory == CharCat::invalid) {
		cerr << setw(5) << currentLine << setw(5) << currentColumn <<
			 "\t" << "Error: Unrecognized Character" << endl;
		state = ScannerState::error;
		exit(1);
	}
}

//----------------------------------------------------------------------
// 							LexAnalyzer::categorizeChar
//----------------------------------------------------------------------
// given a character, returns the CharCat category for that character
//----------------------------------------------------------------------
CharCat LexAnalyzer::categorizeChar(char c) {
	CharCat category = CharCat::unknown;
	switch (c) { // special symbols and cases
		case CC_EOL: category = CharCat::eol;
			break;
		case CC_EOF: category = CharCat::eof;
			break;
		case CC_PERIOD: category = CharCat::period;
			break;
		case CC_LEFTPAR: category = CharCat::leftpar;
			break;
		case CC_RIGHTPAR: category = CharCat::rightpar;
			break;
		case CC_STAR: category = CharCat::star;
			break;
		case CC_COLON: category = CharCat::colon;
			break;
		case CC_LESSTHAN: category = CharCat::lessthan;
			break;
		case CC_GREATTHAN: category = CharCat::greatthan;
			break;
		case CC_EQUAL: category = CharCat::equals;
			break;
		case CC_QUOTE: category = CharCat::quote;
	}
	if (category == CharCat::unknown) {
		if (isalpha(c)) category = CharCat::alpha;
		else if (isdigit(c)) category = CharCat::digit;
		else if (issymbol(c)) category = CharCat::sym;
		else if (isspace(c)) category = CharCat::whitespc;
		else if (c >= CC_MIN && c < CC_MAX) category = CharCat::other;
		else category = CharCat::invalid;
	}
	return category;
}

//----------------------------------------------------------------------
// 							  issymbol
//----------------------------------------------------------------------
// returns true if the given character is a valid symbol, false otherwise.
//----------------------------------------------------------------------
bool issymbol(char c) {
	for (size_t i = 0; i < CC_SYMBOLS.size(); i++) {
		if (c == CC_SYMBOLS[i]) return true;
	}
	return false;
}

//----------------------------------------------------------------------
// 									main
//----------------------------------------------------------------------
int main(int argc, char **argv) {
	string filename = "";
	if (argc > 1) {
		filename = argv[1];
	}
	else {
		cout << "Enter a filename: ";
		cin >> filename;
	}
	LexAnalyzer lex(filename);
	lex.analyze();
	return 0;
}
