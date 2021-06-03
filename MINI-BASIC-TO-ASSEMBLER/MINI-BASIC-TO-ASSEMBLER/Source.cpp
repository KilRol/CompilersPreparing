#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <list>
#include <stack>
#include <vector>
#include <map>
using namespace std;

enum class State {
	A1, A2, A3, B1, C1, C2, D1, D2, D3, D4, D5, D6, E1, E2, F1, F2, F3, G1, H1, Exit
};
enum class SymbolicTokenClass {
	Letter, Digit, Arithmet, Relation, OpenBracket, ClosedBracket, Point, Space, LF, Error, EndOfFile
};
enum class LexemeTokenClass {
	LINE, OPERAND, RELATION, NEXT, LET, FOR, GOTO, GOSUB, OPENBRACKET, CLOSEDBRACKET, IF, RETURN, END, TO, STEP, COMMENT, ERROR, ENDMARKER, ARITHMET
};

enum class AtomSet {
	FINIS, LINEN, ASSIGN, JUMP, JUMPSAVE, RETURNJUMP, CONDJUMP, SAVE, LABEL, TEST, INCR, ADD, SUBT, MULT, DIV, EXP, PLUS, NEG
};

enum class GrammarTokenClass {
	program, program_body, step_part, more_lines, expression, term, factor, primary, E_list, T_list, F_list, REL_OPERATOR, NEXT, GOTO, RIGHT_PAREN, END, TO, delta, ASSIGN, CONDJUMP, SAVE, LABEL, TEST, CHECK, INCR, JUMP, ADD, SUBT, MULT, DIV, EXP, PLUS, NEG
};

enum class SyntaxTokenClass {
	LINE, OPERAND, REL_OPERATOR, NEXT, ASSIGN, FOR, GOTO, GOSUB, LEFT_PAREN, RIGHT_PAREN, IF, RETURN, END, TO, STEP, COMMENT, ERROR, ENDMARKER, PLUS, NEG, MULT, DIV, EXP
};

typedef SymbolicTokenClass Symbol;
typedef LexemeTokenClass Lexeme;
typedef GrammarTokenClass G;
typedef SyntaxTokenClass S;

string symbols[]{
	"Letter", "Digit", "Arithmetic", "Relation", "OpenBracket", "ClosedBracket", "Point", "Space", "LF", "Error", "EndOfFile"
};

string LexemeTokenClassName[]{
	"LINE", "OPERAND", "RELATION", "NEXT", "LET", "FOR", "GOTO", "GOSUB", "OPENBRACKET", "CLOSEDBRACKET", "IF", "RETURN", "END", "TO", "STEP", "COMMENT", "ERROR", "ENDMARK", "ARITHMET"
};

string AtomSetName[]{
	"FINIS\t", "LINEN\t", "ASSIGN\t", "JUMP\t", "JUMPSAVE", "RETURNJUMP", "CONDJUMP", "SAVE\t", "LABEL\t", "TEST\t", "INCR\t", "ADD\t", "SUBT\t", "MULT\t", "DIV\t", "EXP\t", "PLUS\t", "NEG\t"
};

struct SymbolicToken {
	SymbolicTokenClass type;
	int value;
};

struct LexemeToken {
	LexemeTokenClass type;
	int line;
	void* value;

	friend ostream& operator<<(ostream& os, const LexemeToken& lexeme) {
		os << " " << lexeme.line << '\t' << LexemeTokenClassName[(int)lexeme.type] << "\t\t";
		if (lexeme.type == Lexeme::RELATION) {
			os << *(int*)lexeme.value;
		}if (lexeme.type == Lexeme::ARITHMET) {
			os << *(int*)lexeme.value;
		}
		if (lexeme.type == Lexeme::LINE || lexeme.type == Lexeme::OPERAND || lexeme.type == Lexeme::NEXT || lexeme.type == Lexeme::LET || lexeme.type == Lexeme::FOR || lexeme.type == Lexeme::GOTO || lexeme.type == Lexeme::GOSUB) {
			os << lexeme.value;
		}

		return os;
	}
};

struct Variable
{
	string name;
	bool isConst;
	long value;
	Variable() : name(), isConst(true), value(0) {}
	Variable(bool a, double b) : isConst(a), value(b) {}
	Variable(string N) : name(N), isConst(false), value(0) {}
	friend ostream& operator<<(ostream& os, Variable v) {
		if (v.isConst) {
			os << v.value;
		}
		else {
			os << v.name;
		}
	}
};

list<LexemeToken> LexemeList;
vector<Variable*> tableName;
vector<Variable*> tableConst;
vector<int*> tableLabel;

class Parser
{
private:
	SymbolicToken s;
	typedef State(Parser::* function_pointer)();
	map<State, map<SymbolicTokenClass, function_pointer>> automata;

	LexemeToken l;
	LexemeTokenClass lexemeType;
	void* ptr;

	int value;
	int rel;

	bool lineFlag = true;
	int real_line;

	int line;
	int pos;

	string name;
	bool isVariable;

	int num;
	int order;
	int counter;
	int sign;

	list<int> ListPointerTable[101];

	struct SearchTableClass
	{
		char letter;
		int alt;
		function_pointer f;
	};

	int init_vector[26] = { 0, 0, 0, 0, 1, 3, 5, 0, 11, 0, 0, 12, 0, 14, 0, 0, 0, 17, 23, 26, 0, 0, 0, 0, 0, 0 };
	SearchTableClass searchTable[27];

	SymbolicToken transliterator(int ch) {
		SymbolicToken s;
		if (isalpha(ch)) {
			s.type = Symbol::Letter;
			value = ch - 'A';
		}
		else if (isdigit(ch)) {
			s.type = Symbol::Digit;
			value = ch - '0';
		}
		else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^') {
			s.type = Symbol::Arithmet;
		}
		else if (ch == '=' || ch == '!' || ch == '<' || ch == '>') {
			s.type = Symbol::Relation;
		}
		else if (ch == '(') {
			s.type = Symbol::OpenBracket;
		}
		else if (ch == ')') {
			s.type = Symbol::ClosedBracket;
		}
		else if (ch == '.') {
			s.type = Symbol::Point;
		}
		else if (ch == 32) {
			s.type = Symbol::Space;
		}
		else if (ch == 10) {
			s.type = Symbol::LF;
		}
		else if (ch == EOF) {
			s.type = Symbol::EndOfFile;
		}
		else {
			s.type = Symbol::Error;
			s.value = 0;
		}
		if (ch == '=' || ch == '+') {
			value = 1;
		}
		else if (ch == '<' || ch == '-') {
			value = 2;
		}
		else if (ch == '>' || ch == '*') {
			value = 3;
		}
		else if (ch == '/') {
			value = 4;
		}
		else if (ch == '^') {
			value = 5;
		}

		return s;
	}

	State A1() {
		return State::A1;
	}
	State A2() {
		return State::A2;
	}
	State A3() {
		return State::A3;
	}
	State B1() {
		return State::B1;
	}
	State C1() {
		return State::C1;
	}
	State C2() {
		return State::C2;
	}
	State D1() {
		return State::D1;
	}
	State D2() {
		return State::D2;
	}
	State D3() {
		return State::D3;
	}
	State D4() {
		return State::D4;
	}
	State D5() {
		return State::D5;
	}
	State D6() {
		return State::D6;
	}
	State E1() {
		return State::E1;
	}
	State E2() {
		return State::E2;
	}
	State F1() {
		return State::F1;
	}
	State F2() {
		return State::F2;
	}
	State F3() {
		return State::F3;
	}
	State G1() {
		return State::G1;
	}
	State H1() {
		return State::H1;
	}
	State Error1() {
		lexemeType = Lexeme::ERROR;
		CreateLexeme();
		return State::G1;
	}
	State A1a() {
		CreateLexeme();
		return A1();
	}
	State A1b() {
		YES1D();
		return A1a();
	}
	State A1c() {
		YES2D();
		return A1a();
	}
	State A1d() {
		YES3D();
		return A1a();
	}
	State A1e() {
		YES1E();
		return A1a();
	}
	State A2a() {
		lexemeType = Lexeme::ARITHMET;
		return A2b();
	}
	State A2b() {
		CreateLexeme();
		return A2();
	}
	State A2c() {
		YES1D();
		return A2g();
	}
	State A2d() {
		YES2D();
		return A2g();
	}
	State A2e() {
		YES3D();
		return A2g();
	}
	State A2f() {
		YES1E();
		return A2g();
	}
	State A2g() {
		CreateLexeme();
		return A2a();
	}
	State A2h() {
		lexemeType = Lexeme::OPENBRACKET;
		CreateLexeme();
		return A2();
	}
	State A2j() {
		YES1E();
		return A2k();
	}
	State A2l() {
		YES1D();
		return A2k();
	}
	State A2k() {
		CreateLexeme();
		return A2h();
	}
	State A2m() {
		YES2D();
		return A2k();
	}
	State A2n() {
		YES3D();
		return A2k();
	}
	State A2o() {
		if (value != 1) {
			return Error1();
		}
		return A2b();
	}
	State A2p() {
		if (rel == 2 && value == 1) {
			rel = 4;
		}
		else if (rel == 2 && value == 3)
		{
			rel = 6;
		}
		else if (rel == 3 && value == 1)
		{
			rel = 5;
		}
		else {
			rel = 0;
			return Error1();
		}
		return A2b();
	}
	State A2q() {
		lexemeType = Lexeme::END;
		return A2b();
	}
	State A2r() {
		lexemeType = Lexeme::IF;
		return A2b();
	}
	State A2s() {
		lexemeType = Lexeme::RETURN;
		return A2b();
	}
	State A2t() {
		lexemeType = Lexeme::STEP;
		return A2b();
	}
	State A2u() {
		lexemeType = Lexeme::TO;
		return A2b();
	}
	State A3a() {
		name += (char)value + '0';
		CreateLexeme();
		return A3();
	}
	State A3b() {
		lexemeType = Lexeme::CLOSEDBRACKET;
		CreateLexeme();
		return A3();
	}
	State A3c() {
		CreateLexeme();
		return A3b();
	}
	State A3d() {
		YES1D();
		return A3c();
	}
	State A3e() {
		YES2D();
		return A3c();
	}
	State A3f() {
		YES3D();
		return A3c();
	}
	State A3g() {
		YES1E();
		return A3c();
	}
	State B1a() {
		pos = init_vector[value];
		if (pos == 0)
		{
			return Error1();
		}
		return B1();
	}
	State B1b() {
		CreateLexeme();
		return B1a();
	}
	State B1c() {
		YES3D();
		return B1b();
	}
	State B1d() {
		pos++;
		return B1();
	}
	State B1e() {
		YES1E();
		return B1b();
	}
	State C1a() {
		lexemeType = Lexeme::NEXT;
		return C1();
	}
	State C2a() {
		lexemeType = Lexeme::OPERAND;
		return C2d();
	}
	State C2b() {
		CreateLexeme();
		return C2a();
	}
	State C2d() {
		name = (char)value + 'A';
		isVariable = true;
		return C2();
	}
	State D1a() {
		lexemeType = Lexeme::OPERAND;
		num = value;
		return D1();
	}
	State D1b() {
		num = num * 10 + value;
		return D1();
	}
	State D1c() {
		CreateLexeme();
		return D1a();
	}
	State D2a() {
		counter++;
		num = num * 10 + value;
		return D2();
	}
	State D2b() {
		counter = 1;
		num = value;
		return D2();
	}
	State D2c() {
		counter = 0;
		return D2();
	}
	State D3a() {
		counter = 0;
		return D3();
	}
	State D4a() {
		if (value == 1) {
			sign = 1;
		}
		if (value == 2) {
			sign = -1;
		}
		if (value > 2) {
			return Error1();
		}
		return D4();
	}
	State D5a() {
		sign = 1;
		return D5b();
	}
	State D5b() {
		order = value;
		return D5();
	}
	State D5c() {
		order = order * 10 + value;
		return D5();
	}
	State D6a() {
		CreateLexeme();
		return D6();
	}
	State E1a() {
		lexemeType = Lexeme::GOTO;
		lineFlag = false;
		return E1();
	}
	State E1b() {
		lexemeType = Lexeme::GOSUB;
		lineFlag = false;
		return E1();
	}
	State E2a() {
		lexemeType = Lexeme::LINE;
		lineFlag = true;
		return E2b();
	}
	State E2b() {
		line = value;
		if (lineFlag) {
			real_line = line;
		}
		return E2();
	}
	State E2c() {
		line = line * 10 + value;
		if (lineFlag) {
			real_line = line;
		}
		return E2();
	}
	State F1a() {
		lexemeType = Lexeme::LET;
		return F1();
	}
	State F1b() {
		lexemeType = Lexeme::FOR;
		return F1();
	}
	State F2a() {
		name = (char)value + 'A';
		isVariable = true;
		return F2();
	}
	State F3a() {
		name += (char)value + '0';
		return F3();
	}
	State G1a() {
		lexemeType = Lexeme::COMMENT;
		CreateLexeme();
		return G1();
	}
	State H1a() {
		rel = value;
		lexemeType = Lexeme::RELATION;
		return H1();
	}
	State H1b() {
		CreateLexeme();
		return H1a();
	}
	State H1c() {
		YES1D();
		return H1b();
	}
	State H1d() {
		YES2D();
		return H1b();
	}
	State H1e() {
		YES3D();
		return H1b();
	}
	State H1f() {
		YES1E();
		return H1b();
	}
	State M1() {
		if (searchTable[pos].letter == value + 'A')
		{
			return (this->*searchTable[pos].f)();
		}
		else {
			pos = searchTable[pos].alt;
			if (pos == 0) {
				return Error1();
			}
			return M1();
		}
	}
	State M2() {
		if (value == 'E' - 'A') {
			return D3a();
		}
		YES1D();
		return B1b();
	}
	State M3() {
		if (value == 'E' - 'A') {
			return D3();
		}
		YES2D();
		return B1b();
	}
	void YES1E() {
		for (int* v : tableLabel) {
			if (*v == line) {
				ptr = v;
				return;
			}
		}
		tableLabel.push_back(new int(line));
		ptr = tableLabel.back();
	}
	void YES1D() {
		order = 0;
		AddConst();
	}
	void YES2D() {
		order = counter * (-1);
		AddConst();
	}
	void YES3D() {
		if (sign == -1) {
			order = (-1) * order;
		}
		order = order - counter;
		AddConst();
	}
	State Exit1() {
		lexemeType = Lexeme::ENDMARKER;
		CreateLexeme();
		return State::Exit;
	}
	State Exit2() {
		CreateLexeme();
		return Exit1();
	}
	State Exit3() {
		YES1D();
		return Exit2();
	}
	State Exit4() {
		YES2D();
		return Exit2();
	}
	State Exit5() {
		YES3D();
		return Exit2();
	}
	State Exit6() {
		YES1E();
		return Exit2();
	}

	void CreateLexeme() {
		LexemeToken lexeme;
		lexeme.type = lexemeType;
		lexeme.line = real_line;

		if (isVariable) {
			AddVariable();
		}

		if (lexeme.type == Lexeme::RELATION) {
			lexeme.value = new int(rel);
		}
		if (lexeme.type == Lexeme::ARITHMET) {
			lexeme.value = new int(value);
		}
		if (lexeme.type == Lexeme::LINE || lexeme.type == Lexeme::OPERAND || lexeme.type == Lexeme::NEXT || lexeme.type == Lexeme::LET || lexeme.type == Lexeme::FOR || lexeme.type == Lexeme::GOTO || lexeme.type == Lexeme::GOSUB) {
			lexeme.value = ptr;
		}

		LexemeList.push_back(lexeme);
	}
	void AddVariable() {
		isVariable = false;
		for (Variable* v : tableName) {
			if (v->name == name) {
				ptr = v;
				return;
			}
		}
		tableName.push_back(new Variable(name));
		ptr = tableName.back();
	}
	void AddConst() {
		double c = num;
		if (order > 0) {
			for (int i = 0; i < order; i++) {
				c *= 10;
			}
		}
		if (order < 0) {
			for (int i = 0; i > order; i--) {
				c /= 10;
			}
		}
		for (auto i : tableConst) {
			if (c == i->value) {
				ptr = i;
				return;
			}
		}
		tableConst.push_back(new Variable(true, c));
		ptr = tableConst.back();
	}
	void PrintList() {
		cout << "LexicalBlock Output: " << endl << endl;
		cout << "Lexeme Table" << endl << "Line\t" << "Name\t\t" << "Adress" << endl;
		for (LexemeToken l : LexemeList) {
			cout << l << endl;
		}

		cout << endl << "Const Table" << endl;
		cout << "Adress\t\t" << "Value" << endl;
		for (Variable* v : tableConst) {
			cout << v << '\t' << v->value << endl;
		}

		cout << endl << "Varible Name Table" << endl;
		cout << "Adress\t\t" << "Value" << endl;
		for (Variable* v : tableName) {
			cout << v << '\t' << v->name << endl;
		}

		cout << endl << "Label Table" << endl;
		cout << "Adress\t\t" << "Value" << endl;
		for (int* i : tableLabel) {
			cout << i << '\t' << *i << endl;
		}
	}

public:
	Parser() {
		automata = {
			{State::A1,	{									{Symbol::Digit, &Parser::E2a},																																															{Symbol::Space, &Parser::A1},	{Symbol::LF, &Parser::A1},	{Symbol::EndOfFile, &Parser::Exit1}	}},
			{State::A2,	{{Symbol::Letter, &Parser::C2a},	{Symbol::Digit, &Parser::D1a},	{Symbol::Arithmet, &Parser::A2a},	{Symbol::Relation, &Parser::H1a},	{Symbol::OpenBracket, &Parser::A2h},	{Symbol::ClosedBracket, &Parser::A3b},	{Symbol::Point, &Parser::D6},	{Symbol::Space, &Parser::A2},	{Symbol::LF, &Parser::A1},	{Symbol::EndOfFile, &Parser::Exit1}	}},
			{State::A3,	{{Symbol::Letter, &Parser::B1a},	{Symbol::Digit, &Parser::D1a},	{Symbol::Arithmet, &Parser::A2a},	{Symbol::Relation, &Parser::H1a},	{Symbol::OpenBracket, &Parser::A2h},	{Symbol::ClosedBracket, &Parser::A3b},	{Symbol::Point, &Parser::D6},	{Symbol::Space, &Parser::A3},	{Symbol::LF, &Parser::A1},	{Symbol::EndOfFile, &Parser::Exit1}	}},
			{State::B1,	{{Symbol::Letter, &Parser::M1},																																																								{Symbol::Space, &Parser::B1},																	}},
			{State::C1,	{{Symbol::Letter, &Parser::C2d},																																																							{Symbol::Space, &Parser::C1},																	}},
			{State::C2,	{{Symbol::Letter, &Parser::B1b},	{Symbol::Digit, &Parser::A3a},	{Symbol::Arithmet, &Parser::A2g},	{Symbol::Relation, &Parser::H1b},	{Symbol::OpenBracket, &Parser::A2k},	{Symbol::ClosedBracket, &Parser::A3c},									{Symbol::Space, &Parser::C2},	{Symbol::LF, &Parser::A1a},	{Symbol::EndOfFile, &Parser::Exit2}	}},
			{State::D1,	{{Symbol::Letter, &Parser::M2},		{Symbol::Digit, &Parser::D1b},	{Symbol::Arithmet, &Parser::A2c},	{Symbol::Relation, &Parser::H1c},	{Symbol::OpenBracket, &Parser::A2l},	{Symbol::ClosedBracket, &Parser::A3d},	{Symbol::Point, &Parser::D2c},	{Symbol::Space, &Parser::D1},	{Symbol::LF, &Parser::A1b},	{Symbol::EndOfFile, &Parser::Exit3}	}},
			{State::D2,	{{Symbol::Letter, &Parser::M3},		{Symbol::Digit, &Parser::D2a},	{Symbol::Arithmet, &Parser::A2d},	{Symbol::Relation, &Parser::H1d},	{Symbol::OpenBracket, &Parser::A2m},	{Symbol::ClosedBracket, &Parser::A3e},									{Symbol::Space, &Parser::D2},	{Symbol::LF, &Parser::A1c},	{Symbol::EndOfFile, &Parser::Exit4}	}},
			{State::D3,	{									{Symbol::Digit, &Parser::D5a},	{Symbol::Arithmet, &Parser::D4a},																																						{Symbol::Space, &Parser::D3},																	}},
			{State::D4,	{									{Symbol::Digit, &Parser::D5b},																																															{Symbol::Space, &Parser::D4},																	}},
			{State::D5,	{{Symbol::Letter, &Parser::B1c},	{Symbol::Digit, &Parser::D5c},	{Symbol::Arithmet, &Parser::A2e},	{Symbol::Relation, &Parser::H1e},	{Symbol::OpenBracket, &Parser::A2n},	{Symbol::ClosedBracket, &Parser::A3f},									{Symbol::Space, &Parser::D5},	{Symbol::LF, &Parser::A1d},	{Symbol::EndOfFile, &Parser::Exit5}	}},
			{State::D6,	{									{Symbol::Digit, &Parser::D2b},																																															{Symbol::Space, &Parser::D6},																	}},
			{State::E1,	{									{Symbol::Digit, &Parser::E2b},																																															{Symbol::Space, &Parser::E1},																	}},
			{State::E2,	{{Symbol::Letter, &Parser::B1e},	{Symbol::Digit, &Parser::E2c},	{Symbol::Arithmet, &Parser::A2f},	{Symbol::Relation, &Parser::H1f},	{Symbol::OpenBracket, &Parser::A2j},	{Symbol::ClosedBracket, &Parser::A3g},									{Symbol::Space, &Parser::E2},	{Symbol::LF, &Parser::A1e},	{Symbol::EndOfFile, &Parser::Exit6}	}},
			{State::F1,	{{Symbol::Letter, &Parser::F2a},																																																							{Symbol::Space, &Parser::F1},																	}},
			{State::F2,	{									{Symbol::Digit, &Parser::F3a},										{Symbol::Relation, &Parser::A2o},																													{Symbol::Space, &Parser::F2},																	}},
			{State::F3,	{																										{Symbol::Relation, &Parser::A2o},																													{Symbol::Space, &Parser::F3},																	}},
			{State::G1,	{{Symbol::Letter, &Parser::G1},		{Symbol::Digit, &Parser::G1},	{Symbol::Arithmet, &Parser::G1},	{Symbol::Relation, &Parser::G1},	{Symbol::OpenBracket, &Parser::G1},		{Symbol::ClosedBracket, &Parser::G1},	{Symbol::Point, &Parser::G1},	{Symbol::Space, &Parser::G1},	{Symbol::LF, &Parser::A1},	{Symbol::EndOfFile, &Parser::Exit1}	}},
			{State::H1,	{{Symbol::Letter, &Parser::C2b},	{Symbol::Digit, &Parser::D1c},	{Symbol::Arithmet, &Parser::A2g},	{Symbol::Relation, &Parser::A2p},	{Symbol::OpenBracket, &Parser::A2k},	{Symbol::ClosedBracket, &Parser::A3c},	{Symbol::Point, &Parser::D6a},	{Symbol::Space, &Parser::H1},	{Symbol::LF, &Parser::A1a},	{Symbol::EndOfFile, &Parser::Exit2}	}}
		};

		searchTable[1].letter = 'N';	searchTable[1].f = &Parser::B1d;
		searchTable[2].letter = 'D';	searchTable[2].f = &Parser::A2q;
		searchTable[3].letter = 'O';	searchTable[3].f = &Parser::B1d;
		searchTable[4].letter = 'R';	searchTable[4].f = &Parser::F1b;
		searchTable[5].letter = 'O';	searchTable[5].f = &Parser::B1d;
		searchTable[6].letter = 'T';	searchTable[6].f = &Parser::B1d;	searchTable[6].alt = 8;
		searchTable[7].letter = 'O';	searchTable[7].f = &Parser::E1a;
		searchTable[8].letter = 'S';	searchTable[8].f = &Parser::B1d;
		searchTable[9].letter = 'U';	searchTable[9].f = &Parser::B1d;
		searchTable[10].letter = 'B';	searchTable[10].f = &Parser::E1b;
		searchTable[11].letter = 'F';	searchTable[11].f = &Parser::A2r;
		searchTable[12].letter = 'E';	searchTable[12].f = &Parser::B1d;
		searchTable[13].letter = 'T';	searchTable[13].f = &Parser::F1a;
		searchTable[14].letter = 'E';	searchTable[14].f = &Parser::B1d;
		searchTable[15].letter = 'X';	searchTable[15].f = &Parser::B1d;
		searchTable[16].letter = 'T';	searchTable[16].f = &Parser::C1a;
		searchTable[17].letter = 'E';	searchTable[17].f = &Parser::B1d;
		searchTable[18].letter = 'T';	searchTable[18].f = &Parser::B1d;	searchTable[18].alt = 22;
		searchTable[19].letter = 'U';	searchTable[19].f = &Parser::B1d;
		searchTable[20].letter = 'R';	searchTable[20].f = &Parser::B1d;
		searchTable[21].letter = 'N';	searchTable[21].f = &Parser::A2s;
		searchTable[22].letter = 'M';	searchTable[22].f = &Parser::G1a;
		searchTable[23].letter = 'T';	searchTable[23].f = &Parser::B1d;
		searchTable[24].letter = 'E';	searchTable[24].f = &Parser::B1d;
		searchTable[25].letter = 'P';	searchTable[25].f = &Parser::A2t;
		searchTable[26].letter = 'O';	searchTable[26].f = &Parser::A2u;
	}

	void parse(string filename) {
		ifstream f(filename);
		if (!f) {
			cout << "File doesn't open" << endl;
			return;
		}
		int ch;
		State state = State::A1;
		while (state != State::Exit) {
			ch = f.get();
			s = transliterator(ch);
			if (!automata[state].count(s.type)) {
				state = Error1();
			}
			else {
				state = (this->*automata[state][s.type])();
			}
		}
		PrintList();
		cout << endl;
	}
};

struct Pointer {
	void* ptr;
	Pointer() { ptr = nullptr; };
	Pointer(void* p) : ptr(p) {};
	void setValue(void* ptr) {
		this->ptr = ptr;
	}
	void* getValue() {
		return ptr;
	}
};

list<Pointer*> PointerList;

Pointer* getNewPointer() {
	PointerList.push_back(new Pointer());
	return PointerList.back();
}
Pointer* getNewPointer(void* ptr) {
	PointerList.push_back(new Pointer(ptr));
	return PointerList.back();
}

struct Atom {
	AtomSet type;
	Pointer* ptr1;
	Pointer* ptr2;
	Pointer* ptr3;
	Pointer* ptr4;

	friend ostream& operator<<(ostream& os, Atom a) {
		os << AtomSetName[(int)a.type] << "\t";
		if (a.type == AtomSet::LINEN) {
			os << *(int*)a.ptr1->getValue();
		}
		if ((int)a.ptr1 != 0xcccccccc) {
			os << "\t" << a.ptr1->getValue();
		}
		if ((int)a.ptr2 != 0xcccccccc) {
			os << "\t" << a.ptr2->getValue();
		}
		if ((int)a.ptr3 != 0xcccccccc) {
			os << "\t" << a.ptr3->getValue();
		}
		if ((int)a.ptr4 != 0xcccccccc) {
			os << "\t" << a.ptr4->getValue();
		}
		return os;
	}
};

struct SyntaxElements {
	GrammarTokenClass type;
	Pointer* ptr1;
	Pointer* ptr2;
	Pointer* ptr3;
	Pointer* ptr4;
	SyntaxElements(GrammarTokenClass type, Pointer* ptr1 = getNewPointer(nullptr), Pointer* ptr2 = getNewPointer(nullptr), Pointer* ptr3 = getNewPointer(nullptr), Pointer* ptr4 = getNewPointer(nullptr)) {
		this->type = type;
		this->ptr1 = ptr1;
		this->ptr2 = ptr2;
		this->ptr3 = ptr3;
		this->ptr4 = ptr4;
	}
};

list<Atom> AtomList;
list<Variable*> interimTable;
list<int*> markTable;

bool statusError = false;

class SyntexAnalyzer {
private:
	typedef SyntexAnalyzer SA;
	typedef void(SA::* function_pointer)();

	map<GrammarTokenClass, map<SyntaxTokenClass, function_pointer>> SyntaxBox;

	list<LexemeToken>::iterator currentLexeme;

	stack<SyntaxElements> magazine;

	SyntaxTokenClass type;
	Pointer* line;

	int t = 1;
	int e = 1;
	int k = 1;

	bool isExit = false;

	void* NEWTSR() {
		interimTable.push_back(new Variable("e" + to_string(e++)));
		return interimTable.back();
	}
	void* NEWTL() {
		for (void* v : markTable) {
			if (v == &line) {
				return v;
			}
		}
		markTable.push_back(new int(9000 + k++));
		return markTable.back();
	}
	void* NEWTR() {
		tableName.push_back(new Variable("t" + to_string(t++)));
		return tableName.back();
	}

	void shift() {
		if (currentLexeme->type == Lexeme::ARITHMET) {
			switch (*(int*)currentLexeme->value) {
			case 1: type = S::PLUS; break;
			case 2: type = S::NEG;	break;
			case 3: type = S::MULT; break;
			case 4: type = S::DIV;	break;
			case 5: type = S::EXP;	break;
			}
		}
		else {
			type = (SyntaxTokenClass)(currentLexeme->type);
		}
	}

	void rule1() {
		magazine.pop();
		magazine.push(SyntaxElements(G::END));
		magazine.push(SyntaxElements(G::program_body));

		line = getNewPointer(currentLexeme->value);

		Atom atom;
		atom.type = AtomSet::LINEN;
		atom.ptr1 = line;
		AtomList.push_back(atom);

		currentLexeme++;
		shift();
	}
	void rule2() {
		magazine.pop();
	}
	void rule3() {
		line = getNewPointer(currentLexeme->value);

		Atom atom;
		atom.type = AtomSet::LINEN;
		atom.ptr1 = line;
		AtomList.push_back(atom);

		currentLexeme++;
		shift();
	}
	void rule4() {
		magazine.pop();
		Pointer* ptr = getNewPointer();
		magazine.push(SyntaxElements(G::more_lines));
		magazine.push(SyntaxElements(G::ASSIGN, getNewPointer(currentLexeme->value), ptr));
		magazine.push(SyntaxElements(G::expression, ptr));

		currentLexeme++;
		shift();
	}
	void rule5() {
		magazine.pop();
		magazine.push(SyntaxElements(G::more_lines));

		Atom atom;
		atom.type = AtomSet::JUMP;
		atom.ptr1 = getNewPointer(currentLexeme->value);
		AtomList.push_back(atom);

		currentLexeme++;
		shift();
	}
	void rule6() {
		magazine.pop();

		Pointer* ptr1 = getNewPointer();
		Pointer* ptr2 = getNewPointer();
		Pointer* ptr3 = getNewPointer();
		Pointer* ptr4 = getNewPointer();

		magazine.push(SyntaxElements(G::more_lines));
		magazine.push(SyntaxElements(G::CONDJUMP, ptr1, ptr2, ptr3, ptr4));
		magazine.push(SyntaxElements(G::GOTO, ptr4));
		magazine.push(SyntaxElements(G::expression, ptr2));
		magazine.push(SyntaxElements(G::REL_OPERATOR, ptr3));
		magazine.push(SyntaxElements(G::expression, ptr1));

		currentLexeme++;
		shift();
	}
	void rule7() {
		magazine.pop();
		magazine.push(SyntaxElements(G::more_lines));

		Atom atom;
		atom.type = AtomSet::JUMPSAVE;
		atom.ptr1 = getNewPointer(currentLexeme->value);
		AtomList.push_back(atom);

		currentLexeme++;
		shift();
	}
	void rule8() {
		magazine.pop();
		magazine.push(SyntaxElements(G::more_lines));

		Atom atom;
		atom.type = AtomSet::RETURNJUMP;
		AtomList.push_back(atom);

		currentLexeme++;
		shift();
	}
	void rule9() {
		magazine.pop();

		Pointer* u = getNewPointer(NEWTL());
		Pointer* v = getNewPointer(NEWTL());
		Pointer* t = getNewPointer(NEWTSR());
		Pointer* s = getNewPointer(NEWTSR());
		Pointer* p = getNewPointer(currentLexeme->value);
		Pointer* ptr1 = getNewPointer();
		Pointer* ptr2 = getNewPointer();
		Pointer* ptr3 = getNewPointer();
		Pointer* ptr4 = getNewPointer();

		magazine.push(SyntaxElements(G::more_lines));
		magazine.push(SyntaxElements(G::LABEL, v));
		magazine.push(SyntaxElements(G::JUMP, u));
		magazine.push(SyntaxElements(G::INCR, p, t));
		magazine.push(SyntaxElements(G::CHECK, p, ptr1, line));
		magazine.push(SyntaxElements(G::NEXT, ptr1));
		magazine.push(SyntaxElements(G::more_lines));
		magazine.push(SyntaxElements(G::TEST, p, s, t, v));
		magazine.push(SyntaxElements(G::LABEL, u));
		magazine.push(SyntaxElements(G::SAVE, ptr2, t));
		magazine.push(SyntaxElements(G::step_part, ptr2));
		magazine.push(SyntaxElements(G::SAVE, ptr3, s));
		magazine.push(SyntaxElements(G::expression, ptr3));
		magazine.push(SyntaxElements(G::TO));
		magazine.push(SyntaxElements(G::ASSIGN, p, ptr4));
		magazine.push(SyntaxElements(G::expression, ptr4));

		currentLexeme++;
		shift();
	}
	void rule10() {
		SyntaxElements tmp = magazine.top();
		magazine.pop();
		magazine.push(SyntaxElements(G::expression, tmp.ptr1));

		currentLexeme++;
		shift();
	}
	void rule11() {
		magazine.top().ptr1->setValue(new Variable(true, 1));
		magazine.pop();
	}
	void rule12() {
		magazine.pop();
		magazine.push(SyntaxElements(G::more_lines));

		currentLexeme++;
		shift();
	}
	void rule13() {
		magazine.pop();
		magazine.push(SyntaxElements(G::program_body));

		line = getNewPointer(currentLexeme->value);

		Atom atom;
		atom.type = AtomSet::LINEN;
		atom.ptr1 = line;
		AtomList.push_back(atom);

		currentLexeme++;
		shift();
	}
	void rule14() {
		SyntaxElements tmp = magazine.top();
		magazine.pop();
		Pointer* ptr = getNewPointer();
		magazine.push(SyntaxElements(G::E_list, ptr, tmp.ptr1));
		magazine.push(SyntaxElements(G::term, ptr));
	}
	void rule15() {
		SyntaxElements tmp = magazine.top();
		magazine.pop();
		Pointer* ptr = getNewPointer();
		magazine.push(SyntaxElements(G::E_list, getNewPointer(NEWTR()), tmp.ptr1));
		magazine.push(SyntaxElements(G::PLUS, ptr, magazine.top().ptr1));
		magazine.push(SyntaxElements(G::term, ptr));

		currentLexeme++;
		shift();
	}
	void rule16() {
		SyntaxElements tmp = magazine.top();
		magazine.pop();
		Pointer* ptr = getNewPointer();
		magazine.push(SyntaxElements(G::E_list, getNewPointer(NEWTR()), tmp.ptr1));
		magazine.push(SyntaxElements(G::NEG, ptr, magazine.top().ptr1));
		magazine.push(SyntaxElements(G::term, ptr));

		currentLexeme++;
		shift();
	}
	void rule17() {
		SyntaxElements tmp = magazine.top();
		magazine.pop();
		Pointer* ptr = getNewPointer();
		magazine.push(SyntaxElements(G::E_list, getNewPointer(NEWTR()), tmp.ptr2));
		magazine.push(SyntaxElements(G::ADD, tmp.ptr1, ptr, magazine.top().ptr1));
		magazine.push(SyntaxElements(G::term, ptr));

		currentLexeme++;
		shift();
	}
	void rule18() {
		SyntaxElements tmp = magazine.top();
		magazine.pop();
		Pointer* ptr = getNewPointer();
		magazine.push(SyntaxElements(G::E_list, getNewPointer(NEWTR()), tmp.ptr2));
		magazine.push(SyntaxElements(G::SUBT, tmp.ptr1, ptr, magazine.top().ptr1));
		magazine.push(SyntaxElements(G::term, ptr));

		currentLexeme++;
		shift();
	}
	void rule19() {
		magazine.top().ptr2->setValue(magazine.top().ptr1->getValue());
		magazine.pop();
	}
	void rule20() {
		SyntaxElements tmp = magazine.top();
		magazine.pop();
		Pointer* ptr = getNewPointer();
		magazine.push(SyntaxElements(G::T_list, ptr, tmp.ptr1));
		magazine.push(SyntaxElements(G::factor, ptr));
	}
	void rule21() {
		SyntaxElements tmp = magazine.top();
		magazine.pop();
		Pointer* ptr = getNewPointer();
		magazine.push(SyntaxElements(G::T_list, getNewPointer(NEWTR()), tmp.ptr2));
		magazine.push(SyntaxElements(G::MULT, tmp.ptr1, ptr, magazine.top().ptr1));
		magazine.push(SyntaxElements(G::factor, ptr));

		currentLexeme++;
		shift();
	}
	void rule22() {
		SyntaxElements tmp = magazine.top();
		magazine.pop();
		Pointer* ptr = getNewPointer();
		magazine.push(SyntaxElements(G::T_list, getNewPointer(NEWTR()), tmp.ptr2));
		magazine.push(SyntaxElements(G::DIV, tmp.ptr1, ptr, magazine.top().ptr1));
		magazine.push(SyntaxElements(G::factor, ptr));

		currentLexeme++;
		shift();
	}
	void rule23() {
		magazine.top().ptr2->setValue(magazine.top().ptr1->getValue());
		magazine.pop();
	}
	void rule24() {
		SyntaxElements tmp = magazine.top();
		magazine.pop();
		Pointer* ptr = getNewPointer();
		magazine.push(SyntaxElements(G::F_list, ptr, tmp.ptr1));
		magazine.push(SyntaxElements(G::primary, ptr));
	}
	void rule25() {
		SyntaxElements tmp = magazine.top();
		magazine.pop();
		Pointer* ptr = getNewPointer();
		magazine.push(SyntaxElements(G::F_list, getNewPointer(NEWTR()), tmp.ptr2));
		magazine.push(SyntaxElements(G::EXP, tmp.ptr1, ptr, magazine.top().ptr1));
		magazine.push(SyntaxElements(G::primary, ptr));

		currentLexeme++;
		shift();
	}
	void rule26() {
		magazine.top().ptr2->setValue(magazine.top().ptr1->getValue());
		magazine.pop();
	}
	void rule27() {
		SyntaxElements tmp = magazine.top();
		magazine.pop();
		magazine.push(SyntaxElements(G::RIGHT_PAREN));
		magazine.push(SyntaxElements(G::expression, tmp.ptr1));

		currentLexeme++;
		shift();
	}
	void rule28() {
		magazine.top().ptr1->setValue(currentLexeme->value);
		magazine.pop();

		currentLexeme++;
		shift();
	}

	void ruleA() {
		magazine.pop();

		currentLexeme++;
		shift();
	}
	void ruleB() {
		magazine.top().ptr1->setValue(currentLexeme->value);
		magazine.pop();

		currentLexeme++;
		shift();
	}
	void ruleC() {
		Atom atom;
		atom.type = AtomSet::FINIS;
		AtomList.push_back(atom);
		isExit = true;
	}
	void ruleD() {
		Atom atom;
		atom.type = AtomSet::ASSIGN;
		atom.ptr1 = magazine.top().ptr1;
		atom.ptr2 = magazine.top().ptr2;
		AtomList.push_back(atom);

		magazine.pop();
	}
	void ruleE() {
		Atom atom;
		atom.type = AtomSet::CONDJUMP;
		atom.ptr1 = magazine.top().ptr1;
		atom.ptr2 = magazine.top().ptr2;
		atom.ptr3 = magazine.top().ptr3;
		atom.ptr4 = magazine.top().ptr4;
		AtomList.push_back(atom);

		magazine.pop();
	}
	void ruleF() {
		Atom atom;
		atom.type = AtomSet::SAVE;
		atom.ptr1 = magazine.top().ptr1;
		atom.ptr2 = magazine.top().ptr2;
		AtomList.push_back(atom);

		magazine.pop();
	}
	void ruleG() {
		Atom atom;
		atom.type = AtomSet::LABEL;
		atom.ptr1 = magazine.top().ptr1;
		AtomList.push_back(atom);

		magazine.pop();
	}
	void ruleH() {
		Atom atom;
		atom.type = AtomSet::TEST;
		atom.ptr1 = magazine.top().ptr1;
		atom.ptr2 = magazine.top().ptr2;
		atom.ptr3 = magazine.top().ptr3;
		atom.ptr4 = magazine.top().ptr4;
		AtomList.push_back(atom);

		magazine.pop();
	}
	void ruleI() {
		if (magazine.top().ptr1->getValue() != magazine.top().ptr2->getValue()) {
			cout << "NEXT VARIABLE DIFFERS FROM VARIABLE ON LINE " << magazine.top().ptr3->getValue() << " - ASSUMED FOR VARIABLE INTENDENT";
		}
		magazine.pop();
	}
	void ruleJ() {
		Atom atom;
		atom.type = AtomSet::INCR;
		atom.ptr1 = magazine.top().ptr1;
		atom.ptr2 = magazine.top().ptr2;
		AtomList.push_back(atom);

		magazine.pop();
	}
	void ruleK() {
		Atom atom;
		atom.type = AtomSet::JUMP;
		atom.ptr1 = magazine.top().ptr1;
		AtomList.push_back(atom);

		magazine.pop();
	}
	void ruleL() {
		Atom atom;
		atom.type = AtomSet::ADD;
		atom.ptr1 = magazine.top().ptr1;
		atom.ptr2 = magazine.top().ptr2;
		atom.ptr3 = magazine.top().ptr3;
		AtomList.push_back(atom);

		magazine.pop();
	}
	void ruleM() {
		Atom atom;
		atom.type = AtomSet::SUBT;
		atom.ptr1 = magazine.top().ptr1;
		atom.ptr2 = magazine.top().ptr2;
		atom.ptr3 = magazine.top().ptr3;
		AtomList.push_back(atom);

		magazine.pop();
	}
	void ruleN() {
		Atom atom;
		atom.type = AtomSet::MULT;
		atom.ptr1 = magazine.top().ptr1;
		atom.ptr2 = magazine.top().ptr2;
		atom.ptr3 = magazine.top().ptr3;
		AtomList.push_back(atom);

		magazine.pop();
	}
	void ruleO() {
		Atom atom;
		atom.type = AtomSet::DIV;
		atom.ptr1 = magazine.top().ptr1;
		atom.ptr2 = magazine.top().ptr2;
		atom.ptr3 = magazine.top().ptr3;
		AtomList.push_back(atom);

		magazine.pop();
	}
	void ruleP() {
		Atom atom;
		atom.type = AtomSet::EXP;
		atom.ptr1 = magazine.top().ptr1;
		atom.ptr2 = magazine.top().ptr2;
		atom.ptr3 = magazine.top().ptr3;
		AtomList.push_back(atom);

		magazine.pop();
	}
	void ruleQ() {
		Atom atom;
		atom.type = AtomSet::PLUS;
		atom.ptr1 = magazine.top().ptr1;
		atom.ptr2 = magazine.top().ptr2;
		AtomList.push_back(atom);

		magazine.pop();
	}
	void ruleR() {
		Atom atom;
		atom.type = AtomSet::NEG;
		atom.ptr1 = magazine.top().ptr1;
		atom.ptr2 = magazine.top().ptr2;
		AtomList.push_back(atom);

		magazine.pop();
	}

	void ruleAlpha() {
		ErrorRoutine("PROGRAM BIGINS INCORRECTLY");
	}
	void ruleBeta() {
		ErrorRoutine("STATEMENT BEGINS INCORRECTLY");
	}
	void ruleGamma1() {
		ErrorRoutine("UNEXPECTED ", true, " NEAR END OF FOR STATEMENT");
	}
	void ruleGamma2() {
		ErrorRoutine("EXTRA RIGHT PARENTHESIS IN EXPRESSION AFTER TO");
	}
	void ruleDelta() {
		ErrorRoutine("UNEXPECTED ", true, " AFTER STATEMENT COMPLETE");
	}
	void ruleZeta1() {
		ErrorRoutine("EXPRESSION INCOMPLETE - MISSING OPERAND");
	}
	void ruleZeta2() {
		ErrorRoutine("STATEMENT INCOMPLETE");
	}
	void ruleZeta3() {
		ErrorRoutine("MISSING OPERAND IN EXPRESSION");
	}
	void ruleZeta4() {
		ErrorRoutine("UNEXPECTED ", true, " FOLLOWS EXPRESSION");
	}
	void ruleZeta5() {
		ErrorRoutine("EXPRESSION BEGINS WITH ", true);
	}
	void ruleZeta6() {
		ErrorRoutine("TWO OPERATORS A ROW IN EXPRESSION	");
	}
	void ruleZeta7() {
		ErrorRoutine("OPERATOR MISSING IN EXPRESSION");
	}
	void ruleTheta1() {
		ErrorRoutine("UNEXEPECTED ", true, " IN IF STATEMENT");
	}
	void ruleTheta2() {
		ErrorRoutine("IF STATEMENT INCOMPLETE");
	}
	void ruleTheta3() {
		ErrorRoutine("EXTRA RIGHT PARENTHESIS IN EXPRESSION AFTER IF");
	}
	void ruleLambda() {
		ErrorRoutine("FOR STATEMENTS INCORRECTLY NESTED - MISSING NEXT");
	}
	void ruleMu1() {
		ErrorRoutine("UNEXPECTED ", true, " IN IF STATEMENT");
	}
	void ruleMu2() {
		ErrorRoutine("IF STATEMENT INCOMPLETE");
	}
	void ruleMu3() {
		ErrorRoutine("TWO RELATIONAL OPERATORS IN IF STATEMENT");
	}
	void ruleMu4() {
		ErrorRoutine("EXTRA RIGHT PARENTHESIS IN EXPRESSION AFTER RELATIONAL OPERATOR");
	}
	void ruleNu1() {
		ErrorRoutine("MISSING RIGHT PARENTHESIS BEFORE ", true);
	}
	void ruleNu2() {
		ErrorRoutine("MISSING RIGHT PARENTHESIS AT THE END OF LINE");
	}
	void ruleXi() {
		ErrorRoutine("FOR STATEMENT INCORRECTLY NESTED - EXTRA NEXT");
	}
	void rulePi1() {
		ErrorRoutine("UNEXPRECTED ", true, " IN FOR STATEMENT");
	}
	void rulePi2() {
		ErrorRoutine("FOR STATEMENT INCOMPLETE");
	}
	void rulePi3() {
		ErrorRoutine("EXTRA RIGHT PARENTHESIS IN EXPRESSION AFTER =");
	}
	void rulePi4() {
		ErrorRoutine("MISSING OR MISPLACED TO IN FOR STATEMENT");
	}
	void ruleRho() {
		ErrorRoutine("PROGRAM CONTINUES AFTER END STATEMENT");
	}
	void ruleSigma() {
		ErrorRoutine("LexicalBlock Error");
	}
	void ruleTau1() {
		ErrorRoutine("NO PROGRAM");
	}
	void ruleTau2() {
		ErrorRoutine("MISSING END STATEMENT");
	}
	void ruleTau3() {
		ErrorRoutine("PROGRAM ENDS IN MIDDLE OF STATEMENT");
	}
	void ruleOmega() {
		ErrorRoutine("COMPILER ERROR");
	}

	void ErrorRoutine(string ErrorMessagePt1, bool f = false, string ErrorMessagePt2 = "") {
		cout << ErrorMessagePt1;
		if (f) {
			cout << LexemeTokenClassName[(int)currentLexeme->type];
		}
		cout << ErrorMessagePt2 << endl;

		statusError = true;

		while (currentLexeme->type != Lexeme::LINE && currentLexeme->type != Lexeme::ENDMARKER) {
			currentLexeme++;
			shift();
		}
		if (currentLexeme->type == Lexeme::ENDMARKER) {
			isExit = true;
			return;
		}
		if (magazine.top().type == G::END || magazine.top().type == G::delta) {
			magazine.push(G::program_body);
		}
		else {
			while (magazine.top().type != G::more_lines && magazine.top().type != G::program_body && magazine.top().type != G::program) {
				magazine.pop();
			}
		}
	}
	

public:
	SyntexAnalyzer() {
		SyntaxBox = {
			{G::program,		{	{S::LINE, &SA::rule1},		{S::OPERAND, &SA::ruleAlpha},	{S::REL_OPERATOR, &SA::ruleAlpha},	{S::NEXT, &SA::ruleAlpha},	{S::ASSIGN, &SA::ruleAlpha},	{S::FOR, &SA::ruleAlpha},	{S::GOTO, &SA::ruleAlpha},	{S::GOSUB, &SA::ruleAlpha},	{S::LEFT_PAREN, &SA::ruleAlpha},	{S::RIGHT_PAREN, &SA::ruleAlpha},	{S::IF, &SA::ruleAlpha},	{S::RETURN, &SA::ruleAlpha},	{S::END, &SA::ruleAlpha},	{S::TO, &SA::ruleAlpha},	{S::STEP, &SA::ruleAlpha},	{S::COMMENT, &SA::ruleAlpha},	{S::PLUS, &SA::ruleAlpha},	{S::NEG, &SA::ruleAlpha},	{S::MULT, &SA::ruleAlpha},	{S::DIV, &SA::ruleAlpha},	{S::EXP, &SA::ruleAlpha},	{S::ERROR, &SA::ruleSigma},	{S::ENDMARKER, &SA::ruleTau1},	} },
			{G::program_body,	{	{S::LINE, &SA::rule3},		{S::OPERAND, &SA::ruleBeta},	{S::REL_OPERATOR, &SA::ruleBeta},	{S::NEXT, &SA::rule2},		{S::ASSIGN, &SA::rule4},		{S::FOR, &SA::rule9},		{S::GOTO, &SA::rule5},		{S::GOSUB, &SA::rule7},		{S::LEFT_PAREN, &SA::ruleBeta},		{S::RIGHT_PAREN, &SA::ruleBeta},	{S::IF, &SA::rule6},		{S::RETURN, &SA::rule8},		{S::END, &SA::rule2},		{S::TO, &SA::ruleBeta},		{S::STEP, &SA::ruleBeta},	{S::COMMENT, &SA::rule12},		{S::PLUS, &SA::ruleBeta},	{S::NEG, &SA::ruleBeta},	{S::MULT, &SA::ruleBeta},	{S::DIV, &SA::ruleBeta},	{S::EXP, &SA::ruleBeta},	{S::ERROR, &SA::ruleSigma},	{S::ENDMARKER, &SA::ruleTau2},	} },
			{G::step_part,		{	{S::LINE, &SA::rule11},		{S::OPERAND, &SA::ruleGamma1},	{S::REL_OPERATOR, &SA::ruleGamma1},	{S::NEXT, &SA::ruleGamma1},	{S::ASSIGN, &SA::ruleGamma1},	{S::FOR, &SA::ruleGamma1},	{S::GOTO, &SA::ruleGamma1},	{S::GOSUB, &SA::ruleGamma1},{S::LEFT_PAREN, &SA::ruleGamma1},	{S::RIGHT_PAREN, &SA::ruleGamma2},	{S::IF, &SA::ruleGamma1},	{S::RETURN, &SA::ruleGamma1},	{S::END, &SA::ruleGamma1},	{S::TO, &SA::ruleGamma1},	{S::STEP, &SA::rule10},		{S::COMMENT, &SA::ruleGamma1},	{S::PLUS, &SA::ruleGamma1},	{S::NEG, &SA::ruleGamma1},	{S::MULT, &SA::ruleGamma1},	{S::DIV, &SA::ruleGamma1},	{S::EXP, &SA::ruleGamma1},	{S::ERROR, &SA::ruleSigma},	{S::ENDMARKER, &SA::ruleTau2},	} },
			{G::more_lines,		{	{S::LINE, &SA::rule13},		{S::OPERAND, &SA::ruleDelta},	{S::REL_OPERATOR, &SA::ruleDelta},	{S::NEXT, &SA::ruleDelta},	{S::ASSIGN, &SA::ruleDelta},	{S::FOR, &SA::ruleDelta},	{S::GOTO, &SA::ruleDelta},	{S::GOSUB, &SA::ruleDelta},	{S::LEFT_PAREN, &SA::ruleDelta},	{S::RIGHT_PAREN, &SA::ruleDelta},	{S::IF, &SA::ruleDelta},	{S::RETURN, &SA::ruleDelta},	{S::END, &SA::ruleDelta},	{S::TO, &SA::ruleDelta},	{S::STEP, &SA::ruleDelta},	{S::COMMENT, &SA::ruleDelta},	{S::PLUS, &SA::ruleDelta},	{S::NEG, &SA::ruleDelta},	{S::MULT, &SA::ruleDelta},	{S::DIV, &SA::ruleDelta},	{S::EXP, &SA::ruleDelta},	{S::ERROR, &SA::ruleSigma},	{S::ENDMARKER, &SA::ruleTau2},	} },
			{G::expression,		{	{S::LINE, &SA::ruleZeta2},	{S::OPERAND, &SA::rule14},		{S::REL_OPERATOR, &SA::ruleZeta1},	{S::NEXT, &SA::ruleZeta4},	{S::ASSIGN, &SA::ruleZeta4},	{S::FOR, &SA::ruleZeta4},	{S::GOTO, &SA::ruleZeta1},	{S::GOSUB, &SA::ruleZeta4},	{S::LEFT_PAREN, &SA::rule14},		{S::RIGHT_PAREN, &SA::ruleZeta3},	{S::IF, &SA::ruleZeta4},	{S::RETURN, &SA::ruleZeta4},	{S::END, &SA::ruleZeta4},	{S::TO, &SA::ruleZeta1},	{S::STEP, &SA::ruleZeta1},	{S::COMMENT, &SA::ruleZeta4},	{S::PLUS, &SA::rule15},		{S::NEG, &SA::rule16},		{S::MULT, &SA::ruleZeta5},	{S::DIV, &SA::ruleZeta5},	{S::EXP, &SA::ruleZeta5},	{S::ERROR, &SA::ruleSigma},	{S::ENDMARKER, &SA::ruleTau3},	} },
			{G::term,			{	{S::LINE, &SA::ruleZeta2},	{S::OPERAND, &SA::rule20},		{S::REL_OPERATOR, &SA::ruleZeta1},	{S::NEXT, &SA::ruleZeta4},	{S::ASSIGN, &SA::ruleZeta4},	{S::FOR, &SA::ruleZeta4},	{S::GOTO, &SA::ruleZeta1},	{S::GOSUB, &SA::ruleZeta4},	{S::LEFT_PAREN, &SA::rule20},		{S::RIGHT_PAREN, &SA::ruleZeta3},	{S::IF, &SA::ruleZeta4},	{S::RETURN, &SA::ruleZeta4},	{S::END, &SA::ruleZeta4},	{S::TO, &SA::ruleZeta1},	{S::STEP, &SA::ruleZeta1},	{S::COMMENT, &SA::ruleZeta4},	{S::PLUS, &SA::ruleZeta6},	{S::NEG, &SA::ruleZeta6},	{S::MULT, &SA::ruleZeta6},	{S::DIV, &SA::ruleZeta6},	{S::EXP, &SA::ruleZeta6},	{S::ERROR, &SA::ruleSigma},	{S::ENDMARKER, &SA::ruleTau3},	} },
			{G::factor,			{	{S::LINE, &SA::ruleZeta2},	{S::OPERAND, &SA::rule24},		{S::REL_OPERATOR, &SA::ruleZeta1},	{S::NEXT, &SA::ruleZeta4},	{S::ASSIGN, &SA::ruleZeta4},	{S::FOR, &SA::ruleZeta4},	{S::GOTO, &SA::ruleZeta1},	{S::GOSUB, &SA::ruleZeta4},	{S::LEFT_PAREN, &SA::rule24},		{S::RIGHT_PAREN, &SA::ruleZeta3},	{S::IF, &SA::ruleZeta4},	{S::RETURN, &SA::ruleZeta4},	{S::END, &SA::ruleZeta4},	{S::TO, &SA::ruleZeta1},	{S::STEP, &SA::ruleZeta1},	{S::COMMENT, &SA::ruleZeta4},	{S::PLUS, &SA::ruleZeta6},	{S::NEG, &SA::ruleZeta6},	{S::MULT, &SA::ruleZeta6},	{S::DIV, &SA::ruleZeta6},	{S::EXP, &SA::ruleZeta6},	{S::ERROR, &SA::ruleSigma},	{S::ENDMARKER, &SA::ruleTau3},	} },
			{G::primary,		{	{S::LINE, &SA::ruleZeta2},	{S::OPERAND, &SA::rule28},		{S::REL_OPERATOR, &SA::ruleZeta1},	{S::NEXT, &SA::ruleZeta4},	{S::ASSIGN, &SA::ruleZeta4},	{S::FOR, &SA::ruleZeta4},	{S::GOTO, &SA::ruleZeta1},	{S::GOSUB, &SA::ruleZeta4},	{S::LEFT_PAREN, &SA::rule27},		{S::RIGHT_PAREN, &SA::ruleZeta3},	{S::IF, &SA::ruleZeta4},	{S::RETURN, &SA::ruleZeta4},	{S::END, &SA::ruleZeta4},	{S::TO, &SA::ruleZeta1},	{S::STEP, &SA::ruleZeta1},	{S::COMMENT, &SA::ruleZeta4},	{S::PLUS, &SA::ruleZeta6},	{S::NEG, &SA::ruleZeta6},	{S::MULT, &SA::ruleZeta6},	{S::DIV, &SA::ruleZeta6},	{S::EXP, &SA::ruleZeta6},	{S::ERROR, &SA::ruleSigma},	{S::ENDMARKER, &SA::ruleTau3},	} },
			{G::E_list,			{	{S::LINE, &SA::rule19},		{S::OPERAND, &SA::ruleZeta7},	{S::REL_OPERATOR, &SA::rule19},		{S::NEXT, &SA::ruleOmega},	{S::ASSIGN, &SA::ruleOmega},	{S::FOR, &SA::ruleOmega},	{S::GOTO, &SA::rule19},		{S::GOSUB, &SA::ruleOmega},	{S::LEFT_PAREN, &SA::ruleZeta7},	{S::RIGHT_PAREN, &SA::rule19},		{S::IF, &SA::ruleOmega},	{S::RETURN, &SA::ruleOmega},	{S::END, &SA::ruleOmega},	{S::TO, &SA::rule19},		{S::STEP, &SA::rule19},		{S::COMMENT, &SA::ruleOmega},	{S::PLUS, &SA::rule17},		{S::NEG, &SA::rule18},		{S::MULT, &SA::ruleOmega},	{S::DIV, &SA::ruleOmega},	{S::EXP, &SA::ruleOmega},	{S::ERROR, &SA::ruleOmega},	{S::ENDMARKER, &SA::ruleOmega}, } },
			{G::T_list,			{	{S::LINE, &SA::rule23},		{S::OPERAND, &SA::ruleZeta7},	{S::REL_OPERATOR, &SA::rule23},		{S::NEXT, &SA::ruleOmega},	{S::ASSIGN, &SA::ruleOmega},	{S::FOR, &SA::ruleOmega},	{S::GOTO, &SA::rule23},		{S::GOSUB, &SA::ruleOmega},	{S::LEFT_PAREN, &SA::ruleZeta7},	{S::RIGHT_PAREN, &SA::rule23},		{S::IF, &SA::ruleOmega},	{S::RETURN, &SA::ruleOmega},	{S::END, &SA::ruleOmega},	{S::TO, &SA::rule23},		{S::STEP, &SA::rule23},		{S::COMMENT, &SA::ruleOmega},	{S::PLUS, &SA::rule23},		{S::NEG, &SA::rule23},		{S::MULT, &SA::rule21},		{S::DIV, &SA::rule22},		{S::EXP, &SA::ruleOmega},	{S::ERROR, &SA::ruleOmega},	{S::ENDMARKER, &SA::ruleOmega}, } },
			{G::F_list,			{	{S::LINE, &SA::rule26},		{S::OPERAND, &SA::ruleZeta7},	{S::REL_OPERATOR, &SA::rule26},		{S::NEXT, &SA::ruleZeta4},	{S::ASSIGN, &SA::ruleZeta4},	{S::FOR, &SA::ruleZeta4},	{S::GOTO, &SA::rule26},		{S::GOSUB, &SA::ruleZeta4},	{S::LEFT_PAREN, &SA::ruleZeta7},	{S::RIGHT_PAREN, &SA::rule26},		{S::IF, &SA::ruleZeta4},	{S::RETURN, &SA::ruleZeta4},	{S::END, &SA::ruleZeta4},	{S::TO, &SA::rule26},		{S::STEP, &SA::rule26},		{S::COMMENT, &SA::ruleZeta4},	{S::PLUS, &SA::rule26},		{S::NEG, &SA::rule26},		{S::MULT, &SA::rule26},		{S::DIV, &SA::rule26},		{S::EXP, &SA::rule25},		{S::ERROR, &SA::ruleSigma},	{S::ENDMARKER, &SA::ruleTau2},	} },
			{G::REL_OPERATOR,	{	{S::LINE, &SA::ruleTheta2},	{S::OPERAND, &SA::ruleTheta1},	{S::REL_OPERATOR, &SA::ruleB},		{S::NEXT, &SA::ruleTheta1},	{S::ASSIGN, &SA::ruleTheta1},	{S::FOR, &SA::ruleTheta1},	{S::GOTO, &SA::ruleTheta1},	{S::GOSUB, &SA::ruleTheta1},{S::LEFT_PAREN, &SA::ruleTheta1},	{S::RIGHT_PAREN, &SA::ruleTheta3},	{S::IF, &SA::ruleTheta1},	{S::RETURN, &SA::ruleTheta1},	{S::END, &SA::ruleTheta1},	{S::TO, &SA::ruleTheta1},	{S::STEP, &SA::ruleTheta1},	{S::COMMENT, &SA::ruleTheta1},	{S::PLUS, &SA::ruleTheta1},	{S::NEG, &SA::ruleTheta1},	{S::MULT, &SA::ruleTheta1},	{S::DIV, &SA::ruleTheta1},	{S::EXP, &SA::ruleTheta1},	{S::ERROR, &SA::ruleSigma},	{S::ENDMARKER, &SA::ruleTau3},	} },
			{G::NEXT,			{	{S::LINE, &SA::ruleOmega},	{S::OPERAND, &SA::ruleOmega},	{S::REL_OPERATOR, &SA::ruleOmega},	{S::NEXT, &SA::ruleB},		{S::ASSIGN, &SA::ruleOmega},	{S::FOR, &SA::ruleOmega},	{S::GOTO, &SA::ruleOmega},	{S::GOSUB, &SA::ruleOmega},	{S::LEFT_PAREN, &SA::ruleOmega},	{S::RIGHT_PAREN, &SA::ruleOmega},	{S::IF, &SA::ruleOmega},	{S::RETURN, &SA::ruleOmega},	{S::END, &SA::ruleLambda},	{S::TO, &SA::ruleOmega},	{S::STEP, &SA::ruleOmega},	{S::COMMENT, &SA::ruleOmega},	{S::PLUS, &SA::ruleOmega},	{S::NEG, &SA::ruleOmega},	{S::MULT, &SA::ruleOmega},	{S::DIV, &SA::ruleOmega},	{S::EXP, &SA::ruleOmega},	{S::ERROR, &SA::ruleOmega},	{S::ENDMARKER, &SA::ruleOmega}, } },
			{G::GOTO,			{	{S::LINE, &SA::ruleMu2},	{S::OPERAND, &SA::ruleMu1},		{S::REL_OPERATOR, &SA::ruleMu3},	{S::NEXT, &SA::ruleMu1},	{S::ASSIGN, &SA::ruleMu1},		{S::FOR, &SA::ruleMu1},		{S::GOTO, &SA::ruleB},		{S::GOSUB, &SA::ruleMu1},	{S::LEFT_PAREN, &SA::ruleMu1},		{S::RIGHT_PAREN, &SA::ruleMu4},		{S::IF, &SA::ruleMu1},		{S::RETURN, &SA::ruleMu1},		{S::END, &SA::ruleMu1},		{S::TO, &SA::ruleMu1},		{S::STEP, &SA::ruleMu1},	{S::COMMENT, &SA::ruleMu1},		{S::PLUS, &SA::ruleMu1},	{S::NEG, &SA::ruleMu1},		{S::MULT, &SA::ruleMu1},	{S::DIV, &SA::ruleMu1},		{S::EXP, &SA::ruleMu1},		{S::ERROR, &SA::ruleSigma},	{S::ENDMARKER, &SA::ruleTau3},	} },
			{G::RIGHT_PAREN,	{	{S::LINE, &SA::ruleNu2},	{S::OPERAND, &SA::ruleNu1},		{S::REL_OPERATOR, &SA::ruleNu1},	{S::NEXT, &SA::ruleNu1},	{S::ASSIGN, &SA::ruleNu1},		{S::FOR, &SA::ruleNu1},		{S::GOTO, &SA::ruleNu1},	{S::GOSUB, &SA::ruleNu1},	{S::LEFT_PAREN, &SA::ruleNu1},		{S::RIGHT_PAREN, &SA::ruleA},		{S::IF, &SA::ruleNu1},		{S::RETURN, &SA::ruleNu1},		{S::END, &SA::ruleNu1},		{S::TO, &SA::ruleNu1},		{S::STEP, &SA::ruleNu1},	{S::COMMENT, &SA::ruleNu1},		{S::PLUS, &SA::ruleNu1},	{S::NEG, &SA::ruleNu1},		{S::MULT, &SA::ruleNu1},	{S::DIV, &SA::ruleNu1},		{S::EXP, &SA::ruleNu1},		{S::ERROR, &SA::ruleSigma},	{S::ENDMARKER, &SA::ruleTau3},	} },
			{G::END,			{	{S::LINE, &SA::ruleOmega},	{S::OPERAND, &SA::ruleOmega},	{S::REL_OPERATOR, &SA::ruleOmega},	{S::NEXT, &SA::ruleXi},		{S::ASSIGN, &SA::ruleOmega},	{S::FOR, &SA::ruleOmega},	{S::GOTO, &SA::ruleOmega},	{S::GOSUB, &SA::ruleOmega},	{S::LEFT_PAREN, &SA::ruleOmega},	{S::RIGHT_PAREN, &SA::ruleOmega},	{S::IF, &SA::ruleOmega},	{S::RETURN, &SA::ruleOmega},	{S::END, &SA::ruleA},		{S::TO, &SA::ruleOmega},	{S::STEP, &SA::ruleOmega},	{S::COMMENT, &SA::ruleOmega},	{S::PLUS, &SA::ruleOmega},	{S::NEG, &SA::ruleOmega},	{S::MULT, &SA::ruleOmega},	{S::DIV, &SA::ruleOmega},	{S::EXP, &SA::ruleOmega},	{S::ERROR, &SA::ruleOmega},	{S::ENDMARKER, &SA::ruleOmega}, } },
			{G::TO,				{	{S::LINE, &SA::rulePi2},	{S::OPERAND, &SA::rulePi1},		{S::REL_OPERATOR, &SA::rulePi1},	{S::NEXT, &SA::rulePi1},	{S::ASSIGN, &SA::rulePi1},		{S::FOR, &SA::rulePi1},		{S::GOTO, &SA::rulePi1},	{S::GOSUB, &SA::rulePi1},	{S::LEFT_PAREN, &SA::rulePi1},		{S::RIGHT_PAREN, &SA::rulePi3},		{S::IF, &SA::rulePi1},		{S::RETURN, &SA::rulePi1},		{S::END, &SA::rulePi1},		{S::TO, &SA::ruleA},		{S::STEP, &SA::rulePi4},	{S::COMMENT, &SA::rulePi1},		{S::PLUS, &SA::rulePi1},	{S::NEG, &SA::rulePi1},		{S::MULT, &SA::rulePi1},	{S::DIV, &SA::rulePi1},		{S::EXP, &SA::rulePi1},		{S::ERROR, &SA::ruleSigma},	{S::ENDMARKER, &SA::ruleTau3},	} },
			{G::delta,			{	{S::LINE, &SA::ruleRho},	{S::OPERAND, &SA::ruleRho},		{S::REL_OPERATOR, &SA::ruleRho},	{S::NEXT, &SA::ruleRho},	{S::ASSIGN, &SA::ruleRho},		{S::FOR, &SA::ruleAlpha},	{S::GOTO, &SA::ruleAlpha},	{S::GOSUB, &SA::ruleAlpha},	{S::LEFT_PAREN, &SA::ruleRho},		{S::RIGHT_PAREN, &SA::ruleRho},		{S::IF, &SA::ruleRho},		{S::RETURN, &SA::ruleRho},		{S::END, &SA::ruleRho},		{S::TO, &SA::ruleRho},		{S::STEP, &SA::ruleRho},	{S::COMMENT, &SA::ruleRho},		{S::PLUS, &SA::ruleRho},	{S::NEG, &SA::ruleRho},		{S::MULT, &SA::ruleRho},	{S::DIV, &SA::ruleRho},		{S::EXP, &SA::ruleRho},		{S::ERROR, &SA::ruleSigma},	{S::ENDMARKER, &SA::ruleC},		} },
			{G::ASSIGN,			{	{S::LINE, &SA::ruleD},		{S::OPERAND, &SA::ruleD},		{S::REL_OPERATOR, &SA::ruleD},		{S::NEXT, &SA::ruleD},		{S::ASSIGN, &SA::ruleD},		{S::FOR, &SA::ruleD},		{S::GOTO, &SA::ruleD},		{S::GOSUB, &SA::ruleD},		{S::LEFT_PAREN, &SA::ruleD},		{S::RIGHT_PAREN, &SA::ruleD},		{S::IF, &SA::ruleD},		{S::RETURN, &SA::ruleD},		{S::END, &SA::ruleD},		{S::TO, &SA::ruleD},		{S::STEP, &SA::ruleD},		{S::COMMENT, &SA::ruleD},		{S::PLUS, &SA::ruleD},		{S::NEG, &SA::ruleD},		{S::MULT, &SA::ruleD},		{S::DIV, &SA::ruleD},		{S::EXP, &SA::ruleD},		{S::ERROR, &SA::ruleD},		{S::ENDMARKER, &SA::ruleD},		} },
			{G::CONDJUMP,		{	{S::LINE, &SA::ruleE},		{S::OPERAND, &SA::ruleE},		{S::REL_OPERATOR, &SA::ruleE},		{S::NEXT, &SA::ruleE},		{S::ASSIGN, &SA::ruleE},		{S::FOR, &SA::ruleE},		{S::GOTO, &SA::ruleE},		{S::GOSUB, &SA::ruleE},		{S::LEFT_PAREN, &SA::ruleE},		{S::RIGHT_PAREN, &SA::ruleE},		{S::IF, &SA::ruleE},		{S::RETURN, &SA::ruleE},		{S::END, &SA::ruleE},		{S::TO, &SA::ruleE},		{S::STEP, &SA::ruleE},		{S::COMMENT, &SA::ruleE},		{S::PLUS, &SA::ruleE},		{S::NEG, &SA::ruleE},		{S::MULT, &SA::ruleE},		{S::DIV, &SA::ruleE},		{S::EXP, &SA::ruleE},		{S::ERROR, &SA::ruleE},		{S::ENDMARKER, &SA::ruleE},		} },
			{G::SAVE,			{	{S::LINE, &SA::ruleF},		{S::OPERAND, &SA::ruleF},		{S::REL_OPERATOR, &SA::ruleF},		{S::NEXT, &SA::ruleF},		{S::ASSIGN, &SA::ruleF},		{S::FOR, &SA::ruleF},		{S::GOTO, &SA::ruleF},		{S::GOSUB, &SA::ruleF},		{S::LEFT_PAREN, &SA::ruleF},		{S::RIGHT_PAREN, &SA::ruleF},		{S::IF, &SA::ruleF},		{S::RETURN, &SA::ruleF},		{S::END, &SA::ruleF},		{S::TO, &SA::ruleF},		{S::STEP, &SA::ruleF},		{S::COMMENT, &SA::ruleF},		{S::PLUS, &SA::ruleF},		{S::NEG, &SA::ruleF},		{S::MULT, &SA::ruleF},		{S::DIV, &SA::ruleF},		{S::EXP, &SA::ruleF},		{S::ERROR, &SA::ruleF},		{S::ENDMARKER, &SA::ruleF},		} },
			{G::LABEL,			{	{S::LINE, &SA::ruleG},		{S::OPERAND, &SA::ruleG},		{S::REL_OPERATOR, &SA::ruleG},		{S::NEXT, &SA::ruleG},		{S::ASSIGN, &SA::ruleG},		{S::FOR, &SA::ruleG},		{S::GOTO, &SA::ruleG},		{S::GOSUB, &SA::ruleG},		{S::LEFT_PAREN, &SA::ruleG},		{S::RIGHT_PAREN, &SA::ruleG},		{S::IF, &SA::ruleG},		{S::RETURN, &SA::ruleG},		{S::END, &SA::ruleG},		{S::TO, &SA::ruleG},		{S::STEP, &SA::ruleG},		{S::COMMENT, &SA::ruleG},		{S::PLUS, &SA::ruleG},		{S::NEG, &SA::ruleG},		{S::MULT, &SA::ruleG},		{S::DIV, &SA::ruleG},		{S::EXP, &SA::ruleG},		{S::ERROR, &SA::ruleG},		{S::ENDMARKER, &SA::ruleG},		} },
			{G::TEST,			{	{S::LINE, &SA::ruleH},		{S::OPERAND, &SA::ruleH},		{S::REL_OPERATOR, &SA::ruleH},		{S::NEXT, &SA::ruleH},		{S::ASSIGN, &SA::ruleH},		{S::FOR, &SA::ruleH},		{S::GOTO, &SA::ruleH},		{S::GOSUB, &SA::ruleH},		{S::LEFT_PAREN, &SA::ruleH},		{S::RIGHT_PAREN, &SA::ruleH},		{S::IF, &SA::ruleH},		{S::RETURN, &SA::ruleH},		{S::END, &SA::ruleH},		{S::TO, &SA::ruleH},		{S::STEP, &SA::ruleH},		{S::COMMENT, &SA::ruleH},		{S::PLUS, &SA::ruleH},		{S::NEG, &SA::ruleH},		{S::MULT, &SA::ruleH},		{S::DIV, &SA::ruleH},		{S::EXP, &SA::ruleH},		{S::ERROR, &SA::ruleH},		{S::ENDMARKER, &SA::ruleH},		} },
			{G::CHECK,			{	{S::LINE, &SA::ruleI},		{S::OPERAND, &SA::ruleI},		{S::REL_OPERATOR, &SA::ruleI},		{S::NEXT, &SA::ruleI},		{S::ASSIGN, &SA::ruleI},		{S::FOR, &SA::ruleI},		{S::GOTO, &SA::ruleI},		{S::GOSUB, &SA::ruleI},		{S::LEFT_PAREN, &SA::ruleI},		{S::RIGHT_PAREN, &SA::ruleI},		{S::IF, &SA::ruleI},		{S::RETURN, &SA::ruleI},		{S::END, &SA::ruleI},		{S::TO, &SA::ruleI},		{S::STEP, &SA::ruleI},		{S::COMMENT, &SA::ruleI},		{S::PLUS, &SA::ruleI},		{S::NEG, &SA::ruleI},		{S::MULT, &SA::ruleI},		{S::DIV, &SA::ruleI},		{S::EXP, &SA::ruleI},		{S::ERROR, &SA::ruleI},		{S::ENDMARKER, &SA::ruleI},		} },
			{G::INCR,			{	{S::LINE, &SA::ruleJ},		{S::OPERAND, &SA::ruleJ},		{S::REL_OPERATOR, &SA::ruleJ},		{S::NEXT, &SA::ruleJ},		{S::ASSIGN, &SA::ruleJ},		{S::FOR, &SA::ruleJ},		{S::GOTO, &SA::ruleJ},		{S::GOSUB, &SA::ruleJ},		{S::LEFT_PAREN, &SA::ruleJ},		{S::RIGHT_PAREN, &SA::ruleJ},		{S::IF, &SA::ruleJ},		{S::RETURN, &SA::ruleJ},		{S::END, &SA::ruleJ},		{S::TO, &SA::ruleJ},		{S::STEP, &SA::ruleJ},		{S::COMMENT, &SA::ruleJ},		{S::PLUS, &SA::ruleJ},		{S::NEG, &SA::ruleJ},		{S::MULT, &SA::ruleJ},		{S::DIV, &SA::ruleJ},		{S::EXP, &SA::ruleJ},		{S::ERROR, &SA::ruleJ},		{S::ENDMARKER, &SA::ruleJ},		} },
			{G::JUMP,			{	{S::LINE, &SA::ruleK},		{S::OPERAND, &SA::ruleK},		{S::REL_OPERATOR, &SA::ruleK},		{S::NEXT, &SA::ruleK},		{S::ASSIGN, &SA::ruleK},		{S::FOR, &SA::ruleK},		{S::GOTO, &SA::ruleK},		{S::GOSUB, &SA::ruleK},		{S::LEFT_PAREN, &SA::ruleK},		{S::RIGHT_PAREN, &SA::ruleK},		{S::IF, &SA::ruleK},		{S::RETURN, &SA::ruleK},		{S::END, &SA::ruleK},		{S::TO, &SA::ruleK},		{S::STEP, &SA::ruleK},		{S::COMMENT, &SA::ruleK},		{S::PLUS, &SA::ruleK},		{S::NEG, &SA::ruleK},		{S::MULT, &SA::ruleK},		{S::DIV, &SA::ruleK},		{S::EXP, &SA::ruleK},		{S::ERROR, &SA::ruleK},		{S::ENDMARKER, &SA::ruleK},		} },
			{G::ADD,			{	{S::LINE, &SA::ruleL},		{S::OPERAND, &SA::ruleL},		{S::REL_OPERATOR, &SA::ruleL},		{S::NEXT, &SA::ruleL},		{S::ASSIGN, &SA::ruleL},		{S::FOR, &SA::ruleL},		{S::GOTO, &SA::ruleL},		{S::GOSUB, &SA::ruleL},		{S::LEFT_PAREN, &SA::ruleL},		{S::RIGHT_PAREN, &SA::ruleL},		{S::IF, &SA::ruleL},		{S::RETURN, &SA::ruleL},		{S::END, &SA::ruleL},		{S::TO, &SA::ruleL},		{S::STEP, &SA::ruleL},		{S::COMMENT, &SA::ruleL},		{S::PLUS, &SA::ruleL},		{S::NEG, &SA::ruleL},		{S::MULT, &SA::ruleL},		{S::DIV, &SA::ruleL},		{S::EXP, &SA::ruleL},		{S::ERROR, &SA::ruleL},		{S::ENDMARKER, &SA::ruleL},		} },
			{G::SUBT,			{	{S::LINE, &SA::ruleM},		{S::OPERAND, &SA::ruleM},		{S::REL_OPERATOR, &SA::ruleM},		{S::NEXT, &SA::ruleM},		{S::ASSIGN, &SA::ruleM},		{S::FOR, &SA::ruleM},		{S::GOTO, &SA::ruleM},		{S::GOSUB, &SA::ruleM},		{S::LEFT_PAREN, &SA::ruleM},		{S::RIGHT_PAREN, &SA::ruleM},		{S::IF, &SA::ruleM},		{S::RETURN, &SA::ruleM},		{S::END, &SA::ruleM},		{S::TO, &SA::ruleM},		{S::STEP, &SA::ruleM},		{S::COMMENT, &SA::ruleM},		{S::PLUS, &SA::ruleM},		{S::NEG, &SA::ruleM},		{S::MULT, &SA::ruleM},		{S::DIV, &SA::ruleM},		{S::EXP, &SA::ruleM},		{S::ERROR, &SA::ruleM},		{S::ENDMARKER, &SA::ruleM},		} },
			{G::MULT,			{	{S::LINE, &SA::ruleN},		{S::OPERAND, &SA::ruleN},		{S::REL_OPERATOR, &SA::ruleN},		{S::NEXT, &SA::ruleN},		{S::ASSIGN, &SA::ruleN},		{S::FOR, &SA::ruleN},		{S::GOTO, &SA::ruleN},		{S::GOSUB, &SA::ruleN},		{S::LEFT_PAREN, &SA::ruleN},		{S::RIGHT_PAREN, &SA::ruleN},		{S::IF, &SA::ruleN},		{S::RETURN, &SA::ruleN},		{S::END, &SA::ruleN},		{S::TO, &SA::ruleN},		{S::STEP, &SA::ruleN},		{S::COMMENT, &SA::ruleN},		{S::PLUS, &SA::ruleN},		{S::NEG, &SA::ruleN},		{S::MULT, &SA::ruleN},		{S::DIV, &SA::ruleN},		{S::EXP, &SA::ruleN},		{S::ERROR, &SA::ruleN},		{S::ENDMARKER, &SA::ruleN},		} },
			{G::DIV,			{	{S::LINE, &SA::ruleO},		{S::OPERAND, &SA::ruleO},		{S::REL_OPERATOR, &SA::ruleO},		{S::NEXT, &SA::ruleO},		{S::ASSIGN, &SA::ruleO},		{S::FOR, &SA::ruleO},		{S::GOTO, &SA::ruleO},		{S::GOSUB, &SA::ruleO},		{S::LEFT_PAREN, &SA::ruleO},		{S::RIGHT_PAREN, &SA::ruleO},		{S::IF, &SA::ruleO},		{S::RETURN, &SA::ruleO},		{S::END, &SA::ruleO},		{S::TO, &SA::ruleO},		{S::STEP, &SA::ruleO},		{S::COMMENT, &SA::ruleO},		{S::PLUS, &SA::ruleO},		{S::NEG, &SA::ruleO},		{S::MULT, &SA::ruleO},		{S::DIV, &SA::ruleO},		{S::EXP, &SA::ruleO},		{S::ERROR, &SA::ruleO},		{S::ENDMARKER, &SA::ruleO},		} },
			{G::EXP,			{	{S::LINE, &SA::ruleP},		{S::OPERAND, &SA::ruleP},		{S::REL_OPERATOR, &SA::ruleP},		{S::NEXT, &SA::ruleP},		{S::ASSIGN, &SA::ruleP},		{S::FOR, &SA::ruleP},		{S::GOTO, &SA::ruleP},		{S::GOSUB, &SA::ruleP},		{S::LEFT_PAREN, &SA::ruleP},		{S::RIGHT_PAREN, &SA::ruleP},		{S::IF, &SA::ruleP},		{S::RETURN, &SA::ruleP},		{S::END, &SA::ruleP},		{S::TO, &SA::ruleP},		{S::STEP, &SA::ruleP},		{S::COMMENT, &SA::ruleP},		{S::PLUS, &SA::ruleP},		{S::NEG, &SA::ruleP},		{S::MULT, &SA::ruleP},		{S::DIV, &SA::ruleP},		{S::EXP, &SA::ruleP},		{S::ERROR, &SA::ruleP},		{S::ENDMARKER, &SA::ruleP},		} },
			{G::PLUS,			{	{S::LINE, &SA::ruleQ},		{S::OPERAND, &SA::ruleQ},		{S::REL_OPERATOR, &SA::ruleQ},		{S::NEXT, &SA::ruleQ},		{S::ASSIGN, &SA::ruleQ},		{S::FOR, &SA::ruleQ},		{S::GOTO, &SA::ruleQ},		{S::GOSUB, &SA::ruleQ},		{S::LEFT_PAREN, &SA::ruleQ},		{S::RIGHT_PAREN, &SA::ruleQ},		{S::IF, &SA::ruleQ},		{S::RETURN, &SA::ruleQ},		{S::END, &SA::ruleQ},		{S::TO, &SA::ruleQ},		{S::STEP, &SA::ruleQ},		{S::COMMENT, &SA::ruleQ},		{S::PLUS, &SA::ruleQ},		{S::NEG, &SA::ruleQ},		{S::MULT, &SA::ruleQ},		{S::DIV, &SA::ruleQ},		{S::EXP, &SA::ruleQ},		{S::ERROR, &SA::ruleQ},		{S::ENDMARKER, &SA::ruleQ},		} },
			{G::NEG,			{	{S::LINE, &SA::ruleR},		{S::OPERAND, &SA::ruleR},		{S::REL_OPERATOR, &SA::ruleR},		{S::NEXT, &SA::ruleR},		{S::ASSIGN, &SA::ruleR},		{S::FOR, &SA::ruleR},		{S::GOTO, &SA::ruleR},		{S::GOSUB, &SA::ruleR},		{S::LEFT_PAREN, &SA::ruleR},		{S::RIGHT_PAREN, &SA::ruleR},		{S::IF, &SA::ruleR},		{S::RETURN, &SA::ruleR},		{S::END, &SA::ruleR},		{S::TO, &SA::ruleR},		{S::STEP, &SA::ruleR},		{S::COMMENT, &SA::ruleR},		{S::PLUS, &SA::ruleR},		{S::NEG, &SA::ruleR},		{S::MULT, &SA::ruleR},		{S::DIV, &SA::ruleR},		{S::EXP, &SA::ruleR},		{S::ERROR, &SA::ruleR},		{S::ENDMARKER, &SA::ruleR},		} },
		};
	}
	void PrintAtoms() {
		cout << "SyntaxBlock Output:" << endl << endl;
		cout << "Atom\t\t" << "Line\t" << "Ptr1\t\t" << "Ptr2\t\t" << "Ptr3\t\t" << "Ptr4" << endl;
		for (Atom atom : AtomList) {
			cout << atom << endl;
		}

		cout << endl << "interimTable" << endl;
		for (void* v : interimTable) {
			cout << v << endl;
		}
		cout << endl << "markTable" << endl;
		for (void* v : markTable) {
			cout << v << endl;
		}

	}
	void parse() {
		magazine.push(G::delta);
		magazine.push(G::program);

		currentLexeme = LexemeList.begin();
		shift();

		while (!isExit && !magazine.empty()) {
			(this->*SyntaxBox[magazine.top().type][type])();
		}
		PrintAtoms();
	}


};

class CodeGenerator {

	ofstream out;
	list<string> program;
	list<void*> subroutine;
	
public:
	CodeGenerator() {
		out.open("out.txt");

		program.push_back(".386\n");
		program.push_back(".model flat, stdcall\n");
		program.push_back("option casemap: none\n");
		program.push_back("include \\masm32\\include\\msvcrt.inc\n");
		program.push_back("includelib \\masm32\\lib\\msvcrt.lib\n\n");
		program.push_back(".data\n");
		program.push_back("format1 db \"x = % d\", 10, 0\n");
		

		for (Variable* v : tableName) {
			program.push_back(v->name + " dd ?\n");
		}
		for (Variable* v : interimTable) {
			program.push_back(v->name + " dd ?\n");
		}

		program.push_back("\n.code\n");
		program.push_back("start:\n");
	}

	void gen(Atom atom) {
		
	}

	string print(Pointer* ptr) {
		stringstream ss;
		if (((Variable*)(ptr->getValue()))->isConst) {
			ss << ((Variable*)(ptr->getValue()))->value;
		}
		else {
			ss << ((Variable*)(ptr->getValue()))->name;
		}
		return ss.str();
	}

	void generate(bool status) {
		int counter = 0;
		for (Atom atom : AtomList) {
			stringstream ss;
			switch (atom.type)
			{
			case AtomSet::LINEN:
				ss << endl << "l" << *(int*)(atom.ptr1->getValue()) << ":" << endl;
				program.push_back(ss.str());
				break;
			case AtomSet::ASSIGN:
				ss  << "mov eax, " << print(atom.ptr2) << endl << "mov " << print(atom.ptr1) << ", eax" << endl;
				program.push_back(ss.str());
				break;
			case AtomSet::ADD:
				ss << "mov eax, " << print(atom.ptr2) << endl << "add eax, " << print(atom.ptr1) << endl << "mov " << print(atom.ptr3) << ", eax" << endl;
				program.push_back(ss.str());
				break;
			case AtomSet::SUBT:
				ss << "mov eax, " << print(atom.ptr1) << endl << "sub eax, " << print(atom.ptr2) << endl << "mov " << print(atom.ptr3) << ", eax" << endl;
				program.push_back(ss.str());
				break;
			case AtomSet::MULT:
				ss << "mov eax, " << print(atom.ptr2) << endl << "mov ebx, " << print(atom.ptr1) << endl << "mul ebx" << endl << "mov " << print(atom.ptr3) << ", eax" << endl;
				program.push_back(ss.str());
				break;
			case AtomSet::DIV:
				ss << "mov eax, " << print(atom.ptr1) << endl << "mov edx, 0" << endl << "mov ebx, " << print(atom.ptr2) << endl << "div ebx" <<  endl << "mov " << print(atom.ptr3) << ", eax" << endl;
				program.push_back(ss.str());
				break;
			case AtomSet::EXP:
				ss << "mov ebx, " << print(atom.ptr1) << endl << "mov ecx, " << print(atom.ptr2) << endl << "call exp" << endl << "mov " << print(atom.ptr3) << ", eax" << endl;
				program.push_back(ss.str());
				break;
			case AtomSet::CONDJUMP:
				ss << "mov eax, " << print(atom.ptr1) << endl << "cmp eax, " << print(atom.ptr2) << endl;
				switch (*(int*)atom.ptr3->getValue())
				{
				case 1: ss << "je "; break;
				case 2: ss << "jl "; break;
				case 3: ss << "jg "; break;
				case 4: ss << "jle "; break;
				case 5: ss << "jge "; break;
				case 6: ss << "jne "; break;	
				}
				ss << "l" << *(int*)(atom.ptr4->getValue()) << endl;
				program.push_back(ss.str());
				break;
			case AtomSet::JUMP:
				ss << "jmp l" <<  *(int*)(atom.ptr1->getValue());
				program.push_back(ss.str());
				break;
			case AtomSet::JUMPSAVE:
				ss << "call l" << *(int*)(atom.ptr1->getValue()) << endl;
				program.push_back(ss.str());
				break;
			case AtomSet::RETURNJUMP:
				ss << "ret" << endl;
				program.push_back(ss.str());
				break;
			case AtomSet::NEG:
				ss << "mov eax, " << print(atom.ptr1) << endl << "neg eax" << endl << "mov " << print(atom.ptr2) << ", eax" << endl;
				program.push_back(ss.str());
				break;
			case AtomSet::PLUS:
				ss << "mov eax, " << print(atom.ptr1) << endl << "mov " << print(atom.ptr2) << ", eax" << endl;
				program.push_back(ss.str());
				break;
			case AtomSet::INCR:
				ss << "mov eax, " << print(atom.ptr1) << endl << "add eax, " << print(atom.ptr2) << endl << "mov " << print(atom.ptr1) << ", eax" << endl;
				program.push_back(ss.str());
				break;
			case AtomSet::SAVE:
				ss << "mov eax, " << print(atom.ptr1) << endl << "mov " << print(atom.ptr2) << ", eax" << endl;
				program.push_back(ss.str());
				break;
			case AtomSet::LABEL:
				ss << endl << "l" << *(int*)(atom.ptr1->getValue()) << ":" << endl;
				program.push_back(ss.str());
				break;
			case AtomSet::TEST:
				ss << "mov eax, " << print(atom.ptr3) << endl << "cmp eax, 0" << endl << "jg greater" << ++counter << endl;
				ss << "mov eax, " << print(atom.ptr1) << endl << "cmp eax, " << print(atom.ptr2) << endl << "jl l" << *(int*)(atom.ptr4->getValue()) << endl << "jmp ld" << counter << endl;
				ss << "greater" << counter << ":" << endl << "mov eax, " << print(atom.ptr1) << endl << "cmp eax, " << print(atom.ptr2) << endl << "jg l" << *(int*)(atom.ptr4->getValue()) << endl << "ld" << counter << ":" << endl;
				program.push_back(ss.str());
				break;
			case AtomSet::FINIS:
				ss << "push A" << endl << "push offset format1" << endl << "call crt_printf" << endl << "add esp, 7"<< endl;
				ss << "ret" << endl << "exp proc" << endl << "mov eax, 1" << endl << "cycle:" << endl << "cmp ecx, 0" << endl << "jle stop" << endl;
				ss << "mul ebx" << endl << "dec ecx" << endl << "jmp cycle" << endl << "stop: " << endl << "ret" << endl << "exp endp" << endl << "end start" << endl;
				program.push_back(ss.str());
				break;
			}

		}

		for (string s : program) {
			out << s;
		}

		
	}
};

int main() {
	Parser p;
	p.parse("f.txt");

	SyntexAnalyzer sa;
	sa.parse();

	CodeGenerator cd;
	cd.generate(statusError);
}