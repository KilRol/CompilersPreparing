#include <iostream>
#include <map>
#include <stack>

using namespace std;

enum States { s_A1, s_A2, s_A3, s_A4, s_Exit };
enum SymbolicTokenClass { CONST_0, CONST_1, ERROR, ENDMARK, ENDFILE };

struct SymbolicToken {
    SymbolicTokenClass token_class;
    int value;
};

class Parser {
private:
    SymbolicToken s;

    typedef int(Parser::* function_pointer)();
    map<int, map<int, map < int, function_pointer>>> MegaMap;

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

    int ERR() {
        cout << "Error!" << endl;
        return s_Exit;
    }

    int ERR_IN() {
        cout << "Invalid input" << endl;
        return s_Exit;
    }

    int Exit() {
        cout << "Ok!" << endl;
        return s_Exit;
    }

    SymbolicToken transliterator(int ch) {
        SymbolicToken s;
        if (ch == '0') {
            s.token_class = CONST_0;
        }
        else if (ch == '1') {
            s.token_class = CONST_1;
        }
        else if (ch == EOF || ch == 0) {
            s.token_class = ENDFILE;
        }
        else {
            s.token_class = ERROR;
        }
        return s;
    }

    map<int, map<int, function_pointer>> table_1_1 = {
            {s_A1, {{CONST_0, &Parser::A2}, {CONST_1, &Parser::A3}, {ERROR, &Parser::ERR_IN}}},
            {s_A2, {{CONST_0, &Parser::A1}, {CONST_1, &Parser::A4}, {ERROR, &Parser::ERR_IN}, {ENDFILE, &Parser::Exit}}},
            {s_A3, {{CONST_0, &Parser::A4}, {CONST_1, &Parser::A1}, {ERROR, &Parser::ERR_IN}}},
            {s_A4, {{CONST_0, &Parser::A3}, {CONST_1, &Parser::A2}, {ERROR, &Parser::ERR_IN}}}
    };

    map<int, map<int, function_pointer>> table_1_2 = {
            {s_A1, {{CONST_0, &Parser::A1}, {CONST_1, &Parser::A2},     {ERROR,   &Parser::ERR_IN}, {ENDFILE, &Parser::Exit}}},
            {s_A2, {{CONST_0, &Parser::A3}, {CONST_1, &Parser::A2},     {ERROR,   &Parser::ERR_IN}, {ENDFILE, &Parser::Exit}}},
            {s_A3, {{CONST_0, &Parser::A2}, {ERROR,   &Parser::ERR_IN}, {ENDFILE, &Parser::Exit}}}
    };

    map<int, map<int, function_pointer>> table_1_3 = {
            {s_A1, {{CONST_0, &Parser::A1}, {CONST_1, &Parser::A2}, {ERROR, &Parser::ERR_IN}, {ENDFILE, &Parser::Exit}}},
            {s_A2, {{CONST_0, &Parser::A1}, {CONST_1, &Parser::A3}, {ERROR, &Parser::ERR_IN}, {ENDFILE, &Parser::Exit}}},
            {s_A3, {{CONST_0, &Parser::A1}, {ERROR,   &Parser::ERR_IN}}}
    };

    map<int, map<int, function_pointer>> table_1_4 = {
            {s_A1, {{CONST_0, &Parser::A2}, {CONST_1, &Parser::A2},     {ERROR,   &Parser::ERR_IN}, {ENDFILE, &Parser::Exit}}},
            {s_A2, {{CONST_0, &Parser::A3}, {CONST_1, &Parser::A3},     {ERROR,   &Parser::ERR_IN}, {ENDFILE, &Parser::Exit}}},
            {s_A3, {{CONST_1, &Parser::A1}, {ERROR,   &Parser::ERR_IN}, {ENDFILE, &Parser::Exit}}}
    };

    stack<int> stickyStack;

    map<int, map<int, function_pointer>> table_2_1 = {
            {s_A1, {{CONST_0, &Parser::A1b}, {CONST_1, &Parser::A1a}, {ERROR, &Parser::ERR_IN}, {ENDFILE, &Parser::A1c}}}
    };

    int A1a() {
        if (stickyStack.top() == -1) {
            stickyStack.push(1);
        }
        else if (stickyStack.top() == 0) {
            stickyStack.pop();
            stickyStack.push(2);
        }
        else {
            return ERR();
        }
        return s_A1;
    }

    int A1b() {
        if (stickyStack.top() != 1) {
            return ERR();
        }
        stickyStack.pop();
        stickyStack.push(0);
        return s_A1;
    }

    int A1c() {
        if (stickyStack.top() != 2) {
            return ERR();
        }
        stickyStack.pop();
        return Exit();

    }

    map<int, map<int, function_pointer>> table_2_2 = {
            {s_A1, {{CONST_0, &Parser::A1d}, {CONST_1, &Parser::A1e},    {ERROR,   &Parser::ERR_IN}, {ENDFILE, &Parser::A2g}}},
            {s_A2, {{CONST_0, &Parser::A2f}, {ERROR,   &Parser::ERR_IN}, {ENDFILE, &Parser::A2g}}}
    };

    int A1d() {
        if (stickyStack.top() != -1) {
            return ERR();
        }
        stickyStack.push(0);
        return s_A1;
    }

    int A1e() {
        if (stickyStack.top() != 0) {
            return ERR();
        }
        stickyStack.pop();
        stickyStack.push(1);
        return s_A2;
    }

    int A2f() {
        if (stickyStack.top() == 0) {
            stickyStack.pop();
        }
        else if (stickyStack.top() == 1) {
            stickyStack.pop();
            stickyStack.push(0);
        }
        else {
            return ERR();
        }
        return s_A2;
    }

    int A2g() {
        if (stickyStack.top() == 1) {
            stickyStack.pop();
        }
        if (stickyStack.top() == -1) {
            return Exit();
        }
        return ERR();
    }

    map<int, map<int, function_pointer>> table_2_3 = {
            {s_A1, {{CONST_0, &Parser::A1h}, {CONST_1, &Parser::A1i}, {ERROR, &Parser::ERR_IN}, {ENDFILE, &Parser::A1j}}}
    };

    int A1h() {
        if (stickyStack.top() == -1) {
            stickyStack.push(0);
            return s_A1;
        }
        if (stickyStack.top() == 0) {
            return s_A1;
        }
        if (stickyStack.top() == 1) {
            stickyStack.pop();
            stickyStack.push(0);
            return s_A1;
        }
        return ERR();
    }

    int A1i() {
        if (stickyStack.top() == 0) {
            stickyStack.pop();
            stickyStack.push(1);
            return s_A1;
        }
        if (stickyStack.top() == 1) {
            return s_A1;
        }
        return ERR();

    }

    int A1j() {
        if (stickyStack.top() != 1) {
            return ERR();
        }
        stickyStack.pop();
        return Exit();
    }

    map<int, map<int, function_pointer>> table_2_4 = {
            {s_A1, {{CONST_0, &Parser::A1k}, {CONST_1, &Parser::A1l}, {ERROR, &Parser::ERR_IN}, {ENDFILE, &Parser::A1m}}}
    };

    int A1k() {
        if (stickyStack.top() == -1) {
            stickyStack.push(0);
            return s_A1;
        }
        if (stickyStack.top() == 0) {
            return s_A1;
        }
        if (stickyStack.top() == 1) {
            stickyStack.pop();
            stickyStack.push(0);
            return s_A1;
        }
        return ERR();
    }

    int A1l() {
        if (stickyStack.top() != 0) {
            return ERR();
        }
        stickyStack.pop();
        stickyStack.push(1);
        return s_A1;
    }

    int A1m() {
        if (stickyStack.top() == 1) {
            return ERR();
        }
        stickyStack.pop();
        return Exit();
    }

public:
    Parser() {
        stickyStack.push(-1);

        MegaMap = {
                {1, table_1_1},
                {2, table_1_2},
                {3, table_1_3},
                {4, table_1_4},
                {5, table_2_1},
                {6, table_2_2},
                {7, table_2_3},
                {8, table_2_4},
        };
    }

    void parse(string str, int pointer) {
        int ch;
        int state = s_A1;
        for (int i = 0; state != s_Exit; i++) {
            ch = str[i];
            s = transliterator(ch);
            if (!MegaMap[pointer][state].count(s.token_class)) {
                state = ERR();
            }
            else {
                state = (this->*MegaMap[pointer][state][s.token_class])();
            }
        }
        while (stickyStack.size() > 1) stickyStack.pop();
    }
};

int main() {
    Parser p;
    int pointer;
    cout << "Enter the task number: ";
    cin >> pointer;
    for (;;) {
        string str;
        cin >> str;
        p.parse(str, pointer);
    }

}