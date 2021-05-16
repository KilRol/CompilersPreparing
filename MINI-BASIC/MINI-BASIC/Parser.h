#pragma once
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <vector>
#include <map>
using namespace std;

enum SymbolicTokenClass { Letter, Digit, Arithmet, Relation, OpenBracket, ClosedBracket, Point, Space, LF, Error, EndOfFile };
const string symbols[]{ "Letter", "Digit", "Arithmetic", "Relation", "OpenBracket", "ClosedBracket", "Point", "Space", "LF", "Error", "EndOfFile" };

enum States { s_A1, s_A2, s_A3, s_B1, s_C1, s_C2, s_D1, s_D2, s_D3, s_D4, s_D5, s_D6, s_E1, s_E2, s_F1, s_F2, s_F3, s_G1, s_H1, s_Exit };

enum LexemeTokenClass { LINE, OPERAND, ARITHMET, RELATION, OPENBRACKET, CLOSEDBRACKET, ERROR, ENDMARK, NEXT, LET, FOR, GOTO, GOSUB, IF, RETURN, END, TO, STEP, REM, };
const string LexemeTokenClassName[]{ "LINE", "OPERAND", "ARITHMET", "RELATION", "OPENBRACKET", "CLOSEDBRACKET", "ERROR", "ENDMARK", "NEXT", "LET", "FOR", "GOTO", "GOSUB",  "IF", "RETURN", "END", "TO", "STEP", "REM" };

const int init_vector[26] = { 0, 0, 0, 0, 1, 3, 5, 0, 11, 0, 0, 12, 0, 14, 0, 0, 0, 17, 23, 26, 0, 0, 0, 0, 0, 0 };

struct SymbolicToken {
	SymbolicTokenClass type;
	int value;
};

struct LexemeToken {
	LexemeTokenClass type;
	int line;
	void* value;
	friend ostream& operator<<(ostream& os, const LexemeToken& lexeme) {
		return os << " " << lexeme.line << '\t' << LexemeTokenClassName[lexeme.type] << '\t' << lexeme.value;
	}
};

class Parser
{
private:
	

	SymbolicToken transliterator(int ch) {
		SymbolicToken s;
		if (isalpha(ch)) {
			s.type = Letter;
			value = ch - 'A';
		}
		else if (isdigit(ch)) {
			s.type = Digit;
			value = ch - '0';
		}
		else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%') {
			s.type = Arithmet;
		}
		else if (ch == '=' || ch == '!' || ch == '<' || ch == '>') {
			s.type = Relation;
		}
		else if (ch == '(') {
			s.type = OpenBracket;
		}
		else if (ch == ')') {
			s.type = ClosedBracket;
		}
		else if (ch == '.') {
			s.type = Point;
		}
		else if (ch == 32) {
			s.type = Space;
		}
		else if (ch == 10) {
			s.type = LF;
		}
		else if (ch == EOF) {
			s.type = EndOfFile;
		}
		else {
			s.type = Error;
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
		else if (ch == '%') {
			value = 5;
		}

		return s;
	}
	SymbolicToken s;
	typedef int(Parser::* function_pointer)();
	map<int, map<int, function_pointer>> automata;

	LexemeToken l;
	LexemeTokenClass lexemeType;	//–≈√»—“–_ À¿——¿
	void* ptr;						//–≈√»—“–_” ¿«¿“≈Àﬂ

	int value;						//–≈√»—“–_«Õ¿◊≈Õ»ﬂ
	int rel;						//–≈√»—“–_Œ“ÕŒÿ≈Õ»ﬂ
	int line;
	int pos = -1;					//–≈√»—“–_Œ¡Õ¿–”∆≈Õ»ﬂ

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

	int tableName[1000];

	int A1() {
		return s_A1;
	}
	int A2() {
		return s_A2;
	}
	int A3() {
		return s_A3;
	}
	int B1() {
		return s_B1;
	}
	int C1() {
		return s_C1;
	}
	int C2() {
		return s_C2;
	}
	int D1() {
		return s_D1;
	}
	int D2() {
		return s_D2;
	}
	int D3() {
		return s_D3;
	}
	int D4() {
		return s_D4;
	}
	int D5() {
		return s_D5;
	}
	int D6() {
		lexemeType = OPERAND;
		return s_D6;
	}
	int E1() {
		return s_E1;
	}
	int E2() {
		return s_E2;
	}
	int F1() {
		return s_F1;
	}
	int F2() {
		return s_F2;
	}
	int F3() {
		return s_F3;
	}
	int G1() {
		return s_G1;
	}
	int H1() {
		return s_H1;
	}
	int Error1() {
		cout << "ERROR!!!" << endl;
		return s_Exit;
	}

	int A1a() {
		CreateLexeme();
		return A1();
	}
	int A1b() {
		YES1D();
		return A1a();
	}
	int A1c() {
		YES2D();
		return A1a();
	}
	int A1d() {
		YES3D();
		return A1a();
	}
	int A1e() {
		YES1E();
		return A1a();
	}

	int A2a() {
		lexemeType = ARITHMET;
		return A2b();
	}
	int A2b() {
		CreateLexeme();
		return A2();
	}
	int A2c() {
		YES1D();
		return A2g();
	}
	int A2d() {
		YES2D();
		return A2g();
	}
	int A2e() {
		YES3D();
		return A2g();
	}
	int A2f() {
		YES1E();
		return A2g();
	}
	int A2g() {
		CreateLexeme();
		return A2a();
	}
	int A2h() {
		lexemeType = OPENBRACKET;
		CreateLexeme();
		return A2();
	}
	int A2j() {
		YES1E();
		return A2k();
	}
	int A2l() {
		YES1D();
		return A2k();
	}
	int A2k() {
		CreateLexeme();
		return A2h();
	}
	int A2m() {
		YES2D();
		return A2k();
	}
	int A2n() {
		YES3D();
		return A2k();
	}
	int A2o() {
		if (value != 1) {
			return Error1();
		}
		return A2b();
	}
	int A2p() {
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
	int A2q() {
		lexemeType = END;
		return A2b();
	}
	int A2r() {
		lexemeType = IF;
		return A2b();
	}
	int A2s() {
		lexemeType = RETURN;
		return A2b();
	}
	int A2t() {
		lexemeType = STEP;
		return A2b();
	}
	int A2u() {
		lexemeType = TO;
		return A2b();
	}

	int A3a() {
		ptr = &ptr + (value + 1) * 26;
		CreateLexeme();
		return A3();
	}
	int A3b() {
		lexemeType = CLOSEDBRACKET;
		CreateLexeme();
		return A3();
	}
	int A3c() {
		CreateLexeme();
		return A3b();
	}
	int A3d() {
		YES1D();
		return A3c();
	}
	int A3e() {
		YES2D();
		return A3c();
	}
	int A3f() {
		YES3D();
		return A3c();
	}
	int A3g() {
		YES1D();
		return A3c();
	}

	int B1a() {
		pos = init_vector[value];
		if (pos == 0)
		{
			return Error1();
		}
		return B1();
	}
	int B1b() {
		CreateLexeme();
		return B1a();
	}
	int B1c() {
		YES3D();
		return B1b();
	}
	int B1d() {
		pos++;
		return B1();
	}
	int B1e() {
		YES1E();
		return B1b();
	}

	int C1a() {
		lexemeType = NEXT;
		return C1();
	}

	int C2a() {
		lexemeType = OPERAND;
		return C2d();
	}
	int C2b() {
		CreateLexeme();
		return C2a();
	}
	int C2d() {
		ptr = &tableName + value;
		return C2();
	}

	int D1a() {
		lexemeType = OPERAND;
		num = value;
		return D1();
	}
	int D1b() {
		num = num * 10 + value;
		return D1();
	}
	int D1c() {
		CreateLexeme();
		return s_D1;
	}

	int D2a() {
		counter++;
		num = num * 10 + value;
		return D2();
	}
	int D2b() {
		counter = 1;
		num = value;
		return D2();
	}
	int D2c() {
		counter = 0;
		return D2();
	}

	int D3a() {
		counter = 0;
		return D3();
	}

	int D4a() {
		if (value == 2) {
			sign = -1;
		}
		if (value > 2) {
			return Error1();
		}
		return D4();
	}

	int D5a() {
		sign = +1;
		return D5b();
	}
	int D5b() {
		order = value;
		return D5();
	}
	int D5c() {
		order = order * 10 + value;
		return D5();
	}

	int D6a() {
		CreateLexeme();
		return D6();
	}

	int E1a() {
		lexemeType = GOTO;
		return E1();
	}
	int E1b() {
		lexemeType = GOSUB;
		return E1();
	}

	int E2a() {
		lexemeType = LINE;
		return E2b();
	}
	int E2b() {
		line = value;
		return E2();
	}
	int E2c() {
		line = line * 10 + value;
		return E2();
	}

	int F1a() {
		lexemeType = LET;
		return F1();
	}
	int F1b() {
		lexemeType = FOR;
		return F1();
	}

	int F2a() {
		ptr = &tableName + value;
		return F2();
	}

	int F3a() {
		ptr = &ptr + ((value + 1) * 26);
		return F3();
	}

	int G1a() {
		lexemeType = REM;
		CreateLexeme();
		return G1();
	}

	int H1a() {
		rel = value;
		lexemeType = RELATION;
		return H1();
	}
	int H1b() {
		CreateLexeme();
		return H1a();
	}
	int H1c() {
		YES1D();
		return H1b();
	}
	int H1d() {
		YES2D();
		return H1b();
	}
	int H1e() {
		YES3D();
		return H1b();
	}
	int H1f() {
		YES1E();
		return H1b();
	}
	int M1() {
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
	int M2() {
		if (value == 5) {
			return D3a();
		}
		YES1D();
		return B1b();
	}
	int M3() {
		if (value == 5) {
			return D3();
		}
		YES2D();
		return B1b();
	}

	void YES1E() {
		int pointer = line % 101;
		for (int i : ListPointerTable[pointer]) {
			if (i == line) {
				ptr = &ListPointerTable[pointer];
				return;
			}
		}
		ListPointerTable[pointer].push_front(line);
		ptr = &ListPointerTable[pointer];
	}

	void calcConst() {
		if (sign == -1) {
			for (int i = 0; i < order; i++) {
				num = num / 10;
			}
		}
		if (sign == +1) {
			for (int i = 0; i < order; i++) {
				num = num * 10;
			}
		}
		ptr = &num + 286;
	}

	void YES1D() {
		order = 0;
		calcConst();
	}

	void YES2D() {
		order = counter * (-1);
		calcConst();
	}

	void YES3D() {
		if (sign == -1) {
			order = (-1) * order;
			order = order - counter;
		}
		calcConst();
	}

	int Exit1() {
		lexemeType = ENDMARK;
		CreateLexeme();
		return s_Exit;
	}
	int Exit2() {
		CreateLexeme();
		return Exit1();
	}
	int Exit3() {
		YES1D();
		return Exit2();
	}
	int Exit4() {
		YES2D();
		return Exit2();
	}
	int Exit5() {
		YES3D();
		return Exit2();
	}
	int Exit6() {
		YES1E();
		return Exit2();
	}
	list<LexemeToken> listLexeme;
	void CreateLexeme() {
		LexemeToken lexeme;
		lexeme.type = lexemeType;
		lexeme.line = line;
		lexeme.value = ptr;

		listLexeme.push_back(lexeme);
	}
	void PrintList() {
		for (LexemeToken l : listLexeme) {
			cout << l << endl;
		}
	}
	SearchTableClass searchTable[27];

public:
	Parser() {
		automata = {
			{s_A1,	{							{Digit, &Parser::E2a},																																					{Space, &Parser::A1},	{LF, &Parser::A1},	{EndOfFile, &Parser::Exit1}	}},
			{s_A2,	{{Letter, &Parser::C2a},	{Digit, &Parser::D1a},	{Arithmet, &Parser::A2a},	{Relation, &Parser::H1a},	{OpenBracket, &Parser::A2h},	{ClosedBracket, &Parser::A3b},	{Point, &Parser::D6},	{Space, &Parser::A2},	{LF, &Parser::A1},	{EndOfFile, &Parser::Exit1}	}},
			{s_A3,	{{Letter, &Parser::B1a},	{Digit, &Parser::D1a},	{Arithmet, &Parser::A2a},	{Relation, &Parser::H1a},	{OpenBracket, &Parser::A2h},	{ClosedBracket, &Parser::A3b},	{Point, &Parser::D6},	{Space, &Parser::A3},	{LF, &Parser::A1},	{EndOfFile, &Parser::Exit1}	}},
			{s_B1,	{{Letter, &Parser::M1},																																												{Space, &Parser::B1},													}},
			{s_C1,	{{Letter, &Parser::C2d},																																											{Space, &Parser::C1},													}},
			{s_C2,	{{Letter, &Parser::B1b},	{Digit, &Parser::A3a},	{Arithmet, &Parser::A2g},	{Relation, &Parser::H1b},	{OpenBracket, &Parser::A2k},	{ClosedBracket, &Parser::A3c},							{Space, &Parser::C2},	{LF, &Parser::A1a},	{EndOfFile, &Parser::Exit2}	}},
			{s_D1,	{{Letter, &Parser::M2},		{Digit, &Parser::D1b},	{Arithmet, &Parser::A2c},	{Relation, &Parser::H1c},	{OpenBracket, &Parser::A2l},	{ClosedBracket, &Parser::A3d},	{Point, &Parser::D2c},	{Space, &Parser::D1},	{LF, &Parser::A1b},	{EndOfFile, &Parser::Exit3}	}},
			{s_D2,	{{Letter, &Parser::M3},		{Digit, &Parser::D2a},	{Arithmet, &Parser::A2d},	{Relation, &Parser::H1d},	{OpenBracket, &Parser::A2m},	{ClosedBracket, &Parser::A3e},							{Space, &Parser::D2},	{LF, &Parser::A1c},	{EndOfFile, &Parser::Exit4}	}},
			{s_D3,	{							{Digit, &Parser::D5a},	{Arithmet, &Parser::D4a},																														{Space, &Parser::D3},													}},
			{s_D4,	{							{Digit, &Parser::D5b},																																					{Space, &Parser::D4},													}},
			{s_D5,	{{Letter, &Parser::B1c},	{Digit, &Parser::D5c},	{Arithmet, &Parser::A2e},	{Relation, &Parser::H1e},	{OpenBracket, &Parser::A2n},	{ClosedBracket, &Parser::A3f},							{Space, &Parser::D5},	{LF, &Parser::A1d},	{EndOfFile, &Parser::Exit5}	}},
			{s_D6,	{							{Digit, &Parser::D2b},																																					{Space, &Parser::D6},													}},
			{s_E1,	{							{Digit, &Parser::E2b},																																					{Space, &Parser::E1},													}},
			{s_E2,	{{Letter, &Parser::B1e},	{Digit, &Parser::E2c},	{Arithmet, &Parser::A2f},	{Relation, &Parser::H1f},	{OpenBracket, &Parser::A2j},	{ClosedBracket, &Parser::A3g},							{Space, &Parser::E2},	{LF, &Parser::A1e},	{EndOfFile, &Parser::Exit6}	}},
			{s_F1,	{{Letter, &Parser::F2a},																																											{Space, &Parser::F1},													}},
			{s_F2,	{							{Digit, &Parser::F3a},								{Relation, &Parser::A2o},																							{Space, &Parser::F2},													}},
			{s_F3,	{																				{Relation, &Parser::A2o},																							{Space, &Parser::F3},													}},
			{s_G1,	{{Letter, &Parser::G1},		{Digit, &Parser::G1},	{Arithmet, &Parser::G1},	{Relation, &Parser::G1},	{OpenBracket, &Parser::G1},		{ClosedBracket, &Parser::G1},	{Point, &Parser::G1},	{Space, &Parser::G1},	{LF, &Parser::A1},	{EndOfFile, &Parser::Exit1}	}},
			{s_H1,	{{Letter, &Parser::C2b},	{Digit, &Parser::D1c},	{Arithmet, &Parser::A2g},	{Relation, &Parser::A2p},	{OpenBracket, &Parser::A2k},	{ClosedBracket, &Parser::A3c},	{Point, &Parser::D6a},	{Space, &Parser::A1},	{LF, &Parser::A1a},	{EndOfFile, &Parser::Exit2}	}},

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
		int state = s_A1;
		while (state != s_Exit) {
			ch = f.get();
			s = transliterator(ch);
			if (!automata[state].count(s.type)) {
				state = Error1();
			}
			else {
				state = (this->*automata[state][s.type])();
			}
		}
		cout << "&tableName: " << &tableName << endl;
		PrintList();
		cout << endl;
		printTable();
	}
};

