#include <iostream>
#include <map>
#include <list>
#include <stack>
#include <string>

using namespace std;

enum States { s_A1, s_A2, s_A3, s_A4, s_C1, s_C2, s_C3, s_C4, s_C5, s_Exit };
enum SymbolicTokenClass { LETTER, DIGIT, CONST_0, CONST_1, COMMA, SPACE, LF, ERROR, ENDFILE };
enum LexemeTokenClass { H, C, N, O, SI, S, CL, SN, Error };

string LexemeTokenClassName[] = { "H", "C", "N", "O", "SI", "S", "CL", "SN", "Error" };

struct SymbolicToken {
	SymbolicTokenClass token_class;
	int value;
};

class Parser {
private:
	typedef int(Parser::* function_pointer)();

	struct SearchTableClass
	{
		char letter;
		int alt;
		function_pointer f;
	};

	struct LexemeToken {
		LexemeTokenClass type;
		int value;
		friend ostream& operator<<(ostream& os, const LexemeToken& lexeme) {
			return os << '\t' << LexemeTokenClassName[lexeme.type] << '\t' << lexeme.value;
		}
	};

	SymbolicToken s;
	LexemeTokenClass lexemeType;
	int value;
	int pos;

	stack<int> magazine;
	list<LexemeToken> lexemeList;

	int init_vector[26] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 };

	SearchTableClass table_search[13];

	map<int, map<int, map < int, function_pointer>>> taskTables;

	int A1() {
		return s_A1;
	}
	int A2() {
		return s_A2;
	}
	int A3() {
		return s_A3;
	}
	int A4() {
		return s_A4;
	}

	int C2() {
		return s_C2;
	}
	int C3() {
		return s_C3;
	}
	int C4() {
		return s_C4;
	}

	SymbolicToken transliterator(int ch) {
		SymbolicToken s;
		if (ch == '0') {
			s.token_class = CONST_0;
			s.value = 0;
		}
		else if (ch == '1') {
			s.token_class = CONST_1;
			s.value = 1;
		}
		else if (isdigit(ch)) {
			s.token_class = DIGIT;
			s.value = ch - '0';
		}
		else if (isalpha(ch)) {
			s.token_class = LETTER;
			s.value = ch;
		}
		else if (ch == ',') {
			s.token_class = COMMA;
			s.value = ch;
		}
		else if (ch == 32) {
			s.token_class = SPACE;
			s.value = ch;
		}
		else if (ch == 10) {
			s.token_class = LF;
			s.value = ch;
		}
		else if (ch == EOF || ch == 0) {
			s.token_class = ENDFILE;
			s.value = ch;
		}
		else {
			s.token_class = ERROR;
		}
		return s;
	}

	int ERR() {
		if (s.token_class != LF) {
			while (cin.get() != 10);
		}
		lexemeList.clear();
		cout << "Error!" << endl;
		return s_Exit;
	}
	int ERR_IN() {
		if (s.token_class != LF) {
			while (cin.get() != 10);
		}
		lexemeList.clear();
		cout << "Invalid input" << endl;
		return s_Exit;
	}
	int Exit() {
		cout << "Ok!" << endl;
		return s_Exit;
	}

	map<int, map<int, function_pointer>> table_1_1 = {
		{s_A1, {{CONST_0, &Parser::A2},     {CONST_1, &Parser::A3},     {ERROR, &Parser::ERR_IN},                                                       }},
		{s_A2, {{CONST_0, &Parser::A1},     {CONST_1, &Parser::A4},     {ERROR, &Parser::ERR_IN},   {LF, &Parser::Exit},   {ENDFILE, &Parser::Exit}     }},
		{s_A3, {{CONST_0, &Parser::A4},     {CONST_1, &Parser::A1},     {ERROR, &Parser::ERR_IN},                                                       }},
		{s_A4, {{CONST_0, &Parser::A3},     {CONST_1, &Parser::A2},     {ERROR, &Parser::ERR_IN},                                                       }}
	};

	map<int, map<int, function_pointer>> table_1_2 = {
		{s_A1, {{CONST_0, &Parser::A1},     {CONST_1, &Parser::A2},     {ERROR, &Parser::ERR_IN},   {LF, &Parser::Exit},    {ENDFILE, &Parser::Exit}    }},
		{s_A2, {{CONST_0, &Parser::A3},     {CONST_1, &Parser::A2},     {ERROR, &Parser::ERR_IN},   {LF, &Parser::Exit},    {ENDFILE, &Parser::Exit}    }},
		{s_A3, {{CONST_0, &Parser::A2},                                 {ERROR, &Parser::ERR_IN},   {LF, &Parser::Exit},    {ENDFILE, &Parser::Exit}    }}
	};

	map<int, map<int, function_pointer>> table_1_3 = {
		{s_A1, {{CONST_0, &Parser::A1},     {CONST_1, &Parser::A2},     {ERROR, &Parser::ERR_IN},   {LF, &Parser::Exit},    {ENDFILE, &Parser::Exit}    }},
		{s_A2, {{CONST_0, &Parser::A1},     {CONST_1, &Parser::A3},     {ERROR, &Parser::ERR_IN},   {LF, &Parser::Exit},    {ENDFILE, &Parser::Exit}    }},
		{s_A3, {{CONST_0, &Parser::A1},                                 {ERROR, &Parser::ERR_IN},   {LF, &Parser::ERR},     {ENDFILE, &Parser::ERR}     }}
	};

	map<int, map<int, function_pointer>> table_1_4 = {
		{s_A1, {{CONST_0, &Parser::A2},     {CONST_1, &Parser::A2},     {ERROR, &Parser::ERR_IN},   {LF, &Parser::Exit},    {ENDFILE, &Parser::Exit}    }},
		{s_A2, {{CONST_0, &Parser::A3},     {CONST_1, &Parser::A3},     {ERROR, &Parser::ERR_IN},   {LF, &Parser::Exit},    {ENDFILE, &Parser::Exit}    }},
		{s_A3, {{CONST_1, &Parser::A1},                                 {ERROR, &Parser::ERR_IN},   {LF, &Parser::Exit},    {ENDFILE, &Parser::Exit}    }}
	};

	map<int, map<int, function_pointer>> table_2_1 = {
		{s_A1, {{CONST_0, &Parser::A1b},    {CONST_1, &Parser::A1a},    {ERROR, &Parser::ERR_IN},   {LF, &Parser::A1c},     {ENDFILE, &Parser::A1c}     }}
	};

	int A1a() {
		if (magazine.top() == -1) {
			magazine.push(1);
		}
		else if (magazine.top() == 0) {
			magazine.pop();
			magazine.push(2);
		}
		else {
			return ERR();
		}
		return s_A1;
	}
	int A1b() {
		if (magazine.top() != 1) {
			return ERR();
		}
		magazine.pop();
		magazine.push(0);
		return s_A1;
	}
	int A1c() {
		if (magazine.top() != 2) {
			return ERR();
		}
		magazine.pop();
		return Exit();

	}

	map<int, map<int, function_pointer>> table_2_2 = {
		{s_A1, {{CONST_0, &Parser::A1d},    {CONST_1, &Parser::A1e},    {ERROR, &Parser::ERR_IN},   {LF, &Parser::A2g},     {ENDFILE,&Parser::A2g}     }},
		{s_A2, {{CONST_0, &Parser::A2f},                                {ERROR, &Parser::ERR_IN},   {LF, &Parser::A2g},     {ENDFILE, &Parser::A2g}    }}
	};

	int A1d() {
		if (magazine.top() != -1) {
			return ERR();
		}
		magazine.push(0);
		return s_A1;
	}

	int A1e() {
		if (magazine.top() != 0) {
			return ERR();
		}
		magazine.pop();
		magazine.push(1);
		return s_A2;
	}
	int A2f() {
		if (magazine.top() == 0) {
			magazine.pop();
		}
		else if (magazine.top() == 1) {
			magazine.pop();
			magazine.push(0);
		}
		else {
			return ERR();
		}
		return s_A2;
	}
	int A2g() {
		if (magazine.top() == 1) {
			magazine.pop();
		}
		if (magazine.top() == -1) {
			return Exit();
		}
		return ERR();
	}

	map<int, map<int, function_pointer>> table_2_3 = {
		{s_A1, {{CONST_0, &Parser::A1h},    {CONST_1, &Parser::A1i},    {ERROR, &Parser::ERR_IN},   {LF, &Parser::A1j},    {ENDFILE, &Parser::A1j}	}}
	};

	int A1h() {
		if (magazine.top() == -1) {
			magazine.push(0);
			return s_A1;
		}
		if (magazine.top() == 0) {
			return s_A1;
		}
		if (magazine.top() == 1) {
			magazine.pop();
			magazine.push(0);
			return s_A1;
		}
		return ERR();
	}
	int A1i() {
		if (magazine.top() == 0) {
			magazine.pop();
			magazine.push(1);
			return s_A1;
		}
		if (magazine.top() == 1) {
			return s_A1;
		}
		return ERR();
	}
	int A1j() {
		if (magazine.top() != 1) {
			return ERR();
		}
		magazine.pop();
		return Exit();
	}

	map<int, map<int, function_pointer>> table_2_4 = {
		{s_A1, {{CONST_0, &Parser::A1k},    {CONST_1, &Parser::A1l},    {ERROR, &Parser::ERR_IN},   {LF, &Parser::A1m},		{ENDFILE, &Parser::A1m}	}}
	};

	int A1k() {
		if (magazine.top() == -1) {
			magazine.push(0);
			return s_A1;
		}
		if (magazine.top() == 0) {
			return s_A1;
		}
		if (magazine.top() == 1) {
			magazine.pop();
			magazine.push(0);
			return s_A1;
		}
		return ERR();
	}
	int A1l() {
		if (magazine.top() != 0) {
			return ERR();
		}
		magazine.pop();
		magazine.push(1);
		return s_A1;
	}
	int A1m() {
		if (magazine.top() == 1) {
			return ERR();
		}
		magazine.pop();
		return Exit();
	}

	map<int, map<int, function_pointer>> table_3_1 = {
		{s_A1, {{LETTER, &Parser::A2a},  {SPACE, &Parser::A1},   {ERROR,&Parser::ERR_IN},   {LF, &Parser::Exit},    {ENDFILE,&Parser::Exit}    }},
		{s_A2, {{LETTER, &Parser::M1},                           {ERROR, &Parser::ERR_IN}                                                      }},
		{s_A3, {                         {SPACE, &Parser::A1},   {ERROR, &Parser::ERR_IN},  {LF,&Parser::Exit},     {ENDFILE,&Parser::Exit}    }}
	};

	int A2a()
	{
		pos = init_vector[s.value - 'A'];
		if (pos == 0) {
			return ERR();
		}
		return A2();
	}
	int A2b()
	{
		pos++;
		return A2();
	}
	int M1()
	{
		if (table_search[pos].letter == s.value) {
			return (this->*table_search[pos].f)();
		}
		else {
			pos = table_search[pos].alt;
			if (pos == 0) {
				return ERR();
			}
			else {
				return M1();
			}
		}
	}

	map<int, map<int, function_pointer>> table_3_2 = {
		  {s_A1, {{LETTER, &Parser::C2a},                                                                                                           {SPACE, &Parser::A1},   {LF, &Parser::Exit1},  {ERROR, &Parser::Error2},     {ENDFILE, &Parser::Exit1}  }},
		  {s_C2, {{LETTER, &Parser::C3a},  {DIGIT, &Parser::C4a},   {CONST_0, &Parser::C4a},    {CONST_1, &Parser::C4a},    {COMMA, &Parser::C1c},                          {LF, &Parser::Exit2},  {ERROR, &Parser::Error2},     {ENDFILE, &Parser::Exit2}  }},
		  {s_C3, {                         {DIGIT, &Parser::C4c},   {CONST_0, &Parser::C4c},    {CONST_1, &Parser::C4c},    {COMMA, &Parser::C1b},                          {LF, &Parser::Exit5},  {ERROR, &Parser::Error2},     {ENDFILE, &Parser::Exit5}  }},
		  {s_C4, {                         {DIGIT, &Parser::C4b},   {CONST_0, &Parser::C4b},    {CONST_1, &Parser::C4b},    {COMMA, &Parser::C1a},                          {LF, &Parser::Exit3},  {ERROR, &Parser::Error2},     {ENDFILE, &Parser::Exit3}  }}
	};

	int C1a() {
		CreateLexeme();
		return A1();
	}
	int C1b() {
		if (lexemeType == Error) {
			return Error1();
		}
		value = 1;
		return C1a();
	}
	int C1c() {
		C2b();
		return C1b();
	}
	int C2a() {
		magazine.push(s.value);
		return C2();
	}
	int C2b() {
		switch (magazine.top()) {
		case 'H': lexemeType = H;
			break;
		case 'C': lexemeType = C;
			break;
		case 'N': lexemeType = N;
			break;
		case 'O': lexemeType = O;
			break;
		case 'S': lexemeType = S;
			break;
		default: lexemeType = Error;
		}
		magazine.pop();
		return 0;
	}
	int C3a() {
		if (magazine.top() == 'S' && s.value == 'I') {
			lexemeType = SI;
		}
		else if (magazine.top() == 'S' && s.value == 'N') {
			lexemeType = SN;
		}
		else if (magazine.top() == 'C' && s.value == 'L') {
			lexemeType = CL;
		}
		else {
			magazine.pop();
			return Error1();
		}
		magazine.pop();
		return C3();
	}
	int C4a() {
		C2b();
		if (lexemeType == Error) {
			return Error1();
		}
		value = s.value;
		return C4();
	}
	int C4b() {
		value = value * 10 + s.value;
		return C4();
	}
	int C4c() {
		value = s.value;
		return C4();
	}
	int Exit1() {
		return s_Exit;
	}
	int Exit2() {
		C1c();
		return Exit1();
	}
	int Exit3() {
		C1a();
		return Exit1();
	}
	int Exit5() {
		C1b();
		return Exit1();
	}
	int Error1() {
		return ERR();
	}
	int Error2() {
		return ERR_IN();

	}

	void CreateLexeme() {
		LexemeToken lexeme;
		lexeme.type = lexemeType;
		lexeme.value = value;

		lexemeList.push_back(lexeme);
	}

	void PrintList() {
		cout << "Element table:" << endl << '\t' << "Name" << '\t' << "Value" << endl;
		for (LexemeToken l : lexemeList) {
			cout << l << endl;
		}
	}
	void PrintFormula() {
		bool first = true;
		cout << "Formula: ";
		for (LexemeToken l : lexemeList) {
			for (int i = 0; i < l.value; i++)
				if (first) {
					cout << LexemeTokenClassName[l.type];
					first = false;
				}
				else {
					cout << '+' << LexemeTokenClassName[l.type];
				}
		}
		cout << endl << endl;
	}

public:
	Parser() {
		magazine.push(-1);

		taskTables = {
				{1, table_1_1},
				{2, table_1_2},
				{3, table_1_3},
				{4, table_1_4},
				{5, table_2_1},
				{6, table_2_2},
				{7, table_2_3},
				{8, table_2_4},
				{9, table_3_1},
				{10, table_3_2},
		};

		//step
		table_search[1].letter = 'T';   table_search[1].f = &Parser::A2b;   table_search[1].alt = 8;
		table_search[2].letter = 'E';   table_search[2].f = &Parser::A2b;   table_search[2].alt = 4;
		table_search[3].letter = 'P';   table_search[3].f = &Parser::A3;
		//string
		table_search[4].letter = 'R';   table_search[4].f = &Parser::A2b;
		table_search[5].letter = 'I';   table_search[5].f = &Parser::A2b;
		table_search[6].letter = 'N';   table_search[6].f = &Parser::A2b;
		table_search[7].letter = 'G';   table_search[7].f = &Parser::A3;
		//switch
		table_search[8].letter = 'W';   table_search[8].f = &Parser::A2b;
		table_search[9].letter = 'I';   table_search[9].f = &Parser::A2b;
		table_search[10].letter = 'T';  table_search[10].f = &Parser::A2b;
		table_search[11].letter = 'C';  table_search[11].f = &Parser::A2b;
		table_search[12].letter = 'H';  table_search[12].f = &Parser::A3;
	}

	void parse(int pointer) {
		int ch;
		int state = s_A1;
		for (int i = 0; state != s_Exit; i++) {
			ch = cin.get();
			s = transliterator(ch);
			if (!taskTables[pointer][state].count(s.token_class)) {
				state = ERR();
			}
			else {
				state = (this->*taskTables[pointer][state][s.token_class])();
			}
		}
		while (magazine.size() > 1) magazine.pop();
	}

	void Print() {
		if (!lexemeList.empty()) {
			PrintList();
			PrintFormula();
			lexemeList.clear();
		}
	}
};

int main() {
	Parser p;
	int pointer;
	cout << "Enter the task number: ";
	cin >> pointer;
	cin.ignore();
	for (;;) {
		p.parse(pointer);
		p.Print();
	}
}