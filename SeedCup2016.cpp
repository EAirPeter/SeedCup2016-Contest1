// SeedCup2016.cpp
// Author:      EAirPeter(violetcrestfall@hotmail.com)
// Date:        10/25/2016
// Compilation: g++ -o SeedCup2016.exe -std=c++14 -O2 -DNDEBUG SeedCup2016.cpp
// Description: This code is used for participating SeedCup2016.

#ifndef NDEBUG
#   define NDEBUG
#endif

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <functional>
#include <iterator>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

using namespace std;

// Processing phase 0: Initialization of character types
namespace pp0 {
    constexpr auto C_DIGIT = 0x01U;
    constexpr auto C_IDENT = 0x02U;
    constexpr auto C_WHSPC = 0x04U;

    unsigned TYPES[256];

    inline bool IsDigit(char c) {
        return TYPES[(unsigned) c] & C_DIGIT;
    }
    
    inline bool IsIdent(char c) {
        return TYPES[(unsigned) c] & C_IDENT;
    }
    
    inline bool IsWhspc(char c) {
        return TYPES[(unsigned) c] & C_WHSPC;
    }
    
    void Perform() {
        for (unsigned i = 'A'; i <= 'Z'; ++i)
            TYPES[i] |= C_IDENT;
        for (unsigned i = 'a'; i <= 'z'; ++i)
            TYPES[i] |= C_IDENT;
        for (unsigned i = '0'; i <= '9'; ++i)
            TYPES[i] |= C_IDENT | C_DIGIT;
        TYPES['_'] |= C_IDENT;
        TYPES[' '] |= C_WHSPC;
        TYPES['\n'] |= C_WHSPC;
        TYPES['\t'] |= C_WHSPC;
    }
}

// Pos: Indicating the position of an object in source file
struct Pos {
    constexpr Pos() : uRow(), uCol() {
    };
    constexpr Pos(const Pos &rhs) : uRow(rhs.uRow), uCol(rhs.uCol) {
    }
    constexpr Pos(unsigned uRow_, unsigned uCol_) : uRow(uRow_), uCol(uCol_){
    }
    unsigned uRow, uCol;
};

// Char: A wrapped class storing a character with its position
struct Char {
    constexpr Char(char ch_, const Pos &ps_) : ch(ch_), ps(ps_) {
    }
    constexpr operator char() const {
        return ch;
    }
    char ch;
    Pos ps;
};

// Processing phase 1: Reading source file and mapping all chars
namespace pp1 {
    auto pli = new list<Char>;
    auto &li = *pli;
    
    void Perform() {
        FILE *pFile = fopen("input.txt", "r");
        assert(pFile);
        Pos ps(1, 0);
        int n;
        while ((n = fgetc(pFile)) != EOF) {
            ++ps.uCol;
            li.emplace_back((char) n, ps);
            if (n == '\n') {
                ++ps.uRow;
                ps.uCol = 0;
            }
        }
        fclose(pFile);
    }    
}

// Processing phase 2: Replacing any comment with a space character
namespace pp2 {
    auto pli = new list<Char>;
    auto &li = *pli;
    
    void Perform() {
        auto &li1 = *pp1::pli;
        for (auto i = li1.begin(); i != li1.end(); ) {
            if (*i == '\"') {
                do {
                    li.emplace_back(*i++);
                }
                while (i != li1.end() && *i != '\"');
                if (i != li1.end())
                    li.emplace_back(*i++);
                continue;
            }
            if (*i == '/') {
                auto in = next(i);
                if (in != li1.end()) {
                    auto ps = i->ps;
                    if (*in == '*') {
                        i = next(in);
                        while (i != li1.end()) {
                            if (*i == '*') {
                                in = next(i);
                                if (in != li1.end() && *in == '/') {
                                    i = next(in);
                                    break;
                                }
                                i = in;
                            }
                            else
                                ++i;
                        }
                        li.emplace_back(' ', ps);
                        continue;
                    }
                    if (*in == '/') {
                        i = next(in);
                        while (i != li1.end() && *i != '\n')
                            ++i;
                        li.emplace_back(' ', ps);
                        continue;                        
                    }
                }
            }
            li.emplace_back(*i++);
        }
        delete pp1::pli;
    }
}

enum TokenType: unsigned {
    // identifier, constant, string-literal
    T_IDN, T_CON, T_STR,
    // Post increment and decrement operators
    T_INC, T_DEC,
    // Prefix increment and decrement operators
    //T_INC, T_DEC,
    // Unary operators
    T_NEG, T_ADD, T_SUB, T_TIL,
    // Multiplicative operators
    T_MUL, T_DIV, T_MOD,
    // Additive operators
    //T_ADD, T_SUB,
    // Bitwise shift operators
    T_SHL, T_SHR,
    // Relational operators
    T_LES, T_GRE, T_LEQ, T_GEQ,
    // Equality operators
    T_EQU, T_NEQ,
    // Bitwise AND operator
    T_BAN,
    // Bitwise exclusive OR operator
    T_BXO,
    // Bitwise inclusive OR operator
    T_BOR,
    // Logical AND operator
    T_LAN,
    // Logical OR operator
    T_LOR,
    // Conditional operator
    T_QUE, T_COL,
    // Assignment operators
    T_ASG, T_AMU, T_ADI, T_AMO, T_AAD, T_ASU, T_ASL, T_ASR, T_AAN, T_AXO, T_AOR,
    // Comma operator
    T_CMA,
    // Parentheses
    T_LPA, T_RPA,
    // Braces
    T_LBR, T_RBR,
    // Semicolon
    T_SMC,
    // Keywords
    T_KIN, T_KIF, T_KEL, T_KWH, T_KDO, T_KFO, T_KCO, T_KBR,
    // Count of token types
    TCOUNT,
};

// Token: Having the same meaning with its name
struct Token {
    Token(TokenType tp_, const string &s_, const Pos &ps_) :
        tp(tp_), s(s_), n(), ps(ps_)
    {
        if (tp != T_IDN)
            return;
        if (pp0::IsDigit(s.front())) {
            tp = T_CON;
            for (auto c : s)
                n = n * 10 + (c & 0x0f);
        }
        else if (s == "int")
            tp = T_KIN;
        else if (s == "if")
            tp = T_KIF;
        else if (s == "else")
            tp = T_KEL;
        else if (s == "while")
            tp = T_KWH;
        else if (s == "do")
            tp = T_KDO;
        else if (s == "for")
            tp = T_KFO;
        else if (s == "continue")
            tp = T_KCO;
        else if (s == "break")
            tp = T_KBR;
    }
    operator TokenType() const {
        return tp;
    }
    TokenType tp;
    string s;
    int n;
    Pos ps;
};

// Processing phase 3: Tokenization
namespace pp3 {
    auto pli = new list<Token>;
    auto &li = *pli;
    
    void Perform() {
        auto &li2 = *pp2::pli;
        for (auto i = li2.begin(); i != li2.end(); ) {
            while (i != li2.end() && pp0::IsWhspc(*i))
                ++i;
            if (i == li2.end())
                break;
            auto ps = i->ps;
            auto in = next(i);
            string s;
            switch (i->ch) {
            case '+':
                if (in != li2.end()) {
                    if (*in == '+') {
                        li.emplace_back(T_INC, "++", ps);
                        i = next(in);
                        break;
                    }
                    if (*in == '=') {
                        li.emplace_back(T_AAD, "+=", ps);
                        i = next(in);
                        break;
                    }
                }
                li.emplace_back(T_ADD, "+", ps);
                i = in;
                break;
            case '-':
                if (in != li2.end()) {
                    if (*in == '-') {
                        li.emplace_back(T_DEC, "--", ps);
                        i = next(in);
                        break;
                    }
                    if (*in == '=') {
                        li.emplace_back(T_ASU, "-=", ps);
                        i = next(in);
                        break;
                    }
                }
                li.emplace_back(T_SUB, "-", ps);
                i = in;
                break;
            case '*':
                if (in != li2.end()) {
                    if (*in == '=') {
                        li.emplace_back(T_AMU, "*=", ps);
                        i = next(in);
                        break;
                    }
                }
                li.emplace_back(T_MUL, "*", ps);
                i = in;
                break;
            case '/':
                if (in != li2.end()) {
                    if (*in == '=') {
                        li.emplace_back(T_ADI, "/=", ps);
                        i = next(in);
                        break;
                    }
                }
                li.emplace_back(T_DIV, "/", ps);
                i = in;
                break;
            case '%':
                if (in != li2.end()) {
                    if (*in == '=') {
                        li.emplace_back(T_AMO, "%=", ps);
                        i = next(in);
                        break;
                    }
                }
                li.emplace_back(T_MOD, "%", ps);
                i = in;
                break;
            case '~':
                li.emplace_back(T_TIL, "~", ps);
                i = in;
                break;
            case '&':
                if (in != li2.end()) {
                    if (*in == '&') {
                        li.emplace_back(T_LAN, "&&", ps);
                        i = next(in);
                        break;
                    }
                    if (*in == '=') {
                        li.emplace_back(T_AAN, "&=", ps);
                        i = next(in);
                        break;
                    }
                }
                li.emplace_back(T_BAN, "&", ps);
                i = in;
                break;
            case '|':
                if (in != li2.end()) {
                    if (*in == '|') {
                        li.emplace_back(T_LOR, "||", ps);
                        i = next(in);
                        break;
                    }
                    if (*in == '=') {
                        li.emplace_back(T_AOR, "|=", ps);
                        i = next(in);
                        break;
                    }
                }
                li.emplace_back(T_BOR, "|", ps);
                i = in;
                break;
            case '^':
                if (in != li2.end()) {
                    if (*in == '=') {
                        li.emplace_back(T_AXO, "^=", ps);
                        i = next(in);
                        break;
                    }
                }
                li.emplace_back(T_BXO, "^", ps);
                i = in;
                break;
            case '=':
                if (in != li2.end()) {
                    if (*in == '=') {
                        li.emplace_back(T_EQU, "==", ps);
                        i = next(in);
                        break;
                    }
                }
                li.emplace_back(T_ASG, "=", ps);
                i = in;
                break;
            case '!':
                if (in != li2.end()) {
                    if (*in == '=') {
                        li.emplace_back(T_NEQ, "!=", ps);
                        i = next(in);
                        break;
                    }
                }
                li.emplace_back(T_NEG, "!", ps);
                i = in;
                break;
            case '<':
                if (in != li2.end()) {
                    auto inn = next(in);
                    if (*in == '<') {
                        if (inn != li2.end() && *inn == '=') {
                            li.emplace_back(T_ASL, "<<=", ps);
                            i = next(inn);
                            break;
                        }
                        li.emplace_back(T_SHL, "<<", ps);
                        i = inn;
                        break;
                    }
                    if (*in == '=') {
                        li.emplace_back(T_LEQ, "<=", ps);
                        i = inn;
                        break;
                    }
                }
                li.emplace_back(T_LES, "<", ps);
                i = in;
                break;
            case '>':
                if (in != li2.end()) {
                    auto inn = next(in);
                    if (*in == '>') {
                        if (inn != li2.end() && *inn == '=') {
                            li.emplace_back(T_ASR, ">>=", ps);
                            i = next(inn);
                            break;
                        }
                        li.emplace_back(T_SHR, ">>", ps);
                        i = inn;
                        break;
                    }
                    if (*in == '=') {
                        li.emplace_back(T_GEQ, ">=", ps);
                        i = inn;
                        break;
                    }
                }
                li.emplace_back(T_GRE, ">", ps);
                i = in;
                break;
            case '?':
                li.emplace_back(T_QUE, "?", ps);
                i = in;
                break;
            case ':':
                li.emplace_back(T_COL, ":", ps);
                i = in;
                break;
            case '(':
                li.emplace_back(T_LPA, "(", ps);
                i = in;
                break;
            case ')':
                li.emplace_back(T_RPA, ")", ps);
                i = in;
                break;
            case '{':
                li.emplace_back(T_LBR, "{", ps);
                i = in;
                break;
            case '}':
                li.emplace_back(T_RBR, "}", ps);
                i = in;
                break;
            case ',':
                li.emplace_back(T_CMA, ",", ps);
                i = in;
                break;
            case ';':
                li.emplace_back(T_SMC, ";", ps);
                i = in;
                break;
            case '\"':
                do {
                    s.push_back(*i++);
                } while (i != li2.end() && *i != '\"');
                if (i != li2.end())
                    s.push_back(*i++);
                li.emplace_back(T_STR, s, ps);
                break;
            default:
                assert(pp0::IsIdent(*i));
                string s;
                do {
                    s.push_back(*i++);
                } while (pp0::IsIdent(*i));
                li.emplace_back(T_IDN, s, ps);
                break;
            }
        }
        delete pp2::pli;
    }
}

struct Scope;
struct BlockItem;
struct Stmt;
struct StmtComp;
struct StmtExpr;
struct StmtIf;
struct StmtWhile;
struct StmtDo;
struct StmtFor;
struct StmtJump;
struct Decl;
struct ExprBase;
struct Expr;
struct ExprAssig;
struct ExprCondi;
struct ExprLogor;
struct ExprLogan;
struct ExprBitor;
struct ExprBitxo;
struct ExprBitan;
struct ExprEquty;
struct ExprRelat;
struct ExprShift;
struct ExprAddit;
struct ExprMulti;
struct ExprUnary;
struct ExprPofix;
struct ExprPrima;

// Scope: A scope
struct Scope {
    Scope(Scope *pScope_) : pScope(pScope_) {
    }
    virtual ~Scope() {
    }
    unordered_map<string, int>::iterator Lookup(const string &sId) {
        auto i = mst.find(sId);
        if (i != mst.end())
            return i;
        assert(pScope);
        return pScope->Lookup(sId);
    }
    Scope *pScope;
    unordered_map<string, int> mst;
};

// BlockItem: A block-item
struct BlockItem {
    BlockItem(const Pos &ps_) : ps(ps_) {
    }
    virtual ~BlockItem() {
    }
    virtual bool Exec() = 0;
#ifndef NDEBUG
    virtual void DbgPrint() = 0;
#endif
    Pos ps;
};

// Stmt: A statement
struct Stmt : BlockItem {
    using BlockItem::BlockItem;
    virtual ~Stmt() {
    }
};

// StmtComp: A compound-statement
struct StmtComp : Stmt, Scope {
    StmtComp(Scope *pScope_, const Pos &ps_) : Stmt(ps_), Scope(pScope_) {
    }
    ~StmtComp();
    bool Exec() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    list<BlockItem *> li;
};

// StmtExpr: An expression-statement
struct StmtExpr : Stmt {
    using Stmt::Stmt;
    ~StmtExpr();
    bool Exec() override;
#ifndef NDEBUG
    virtual void DbgPrint() override;
#endif
    Expr *pExpr = nullptr;
};

// StmtIf: An if-statement
struct StmtIf : Stmt {
    using Stmt::Stmt;
    ~StmtIf();
    bool Exec() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    Expr *pExpr = nullptr;
    Stmt *pStmt1 = nullptr;
    Stmt *pStmt2 = nullptr;
};

// StmtWhile: A while-statement
struct StmtWhile : Stmt {
    using Stmt::Stmt;
    ~StmtWhile();
    bool Exec() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    Expr *pExpr = nullptr;
    Stmt *pStmt = nullptr;
};

// StmtDo: A do-statement
struct StmtDo : Stmt {
    using Stmt::Stmt;
    ~StmtDo();
    bool Exec() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    Stmt *pStmt = nullptr;
    Expr *pExpr = nullptr;
};

// StmtFor: A for-statement
struct StmtFor : Stmt, Scope {
    StmtFor(Scope *pScope_, const Pos &ps_) : Stmt(ps_), Scope(pScope_) {
    }
    ~StmtFor();
    bool Exec() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    BlockItem *pClause1 = nullptr;
    Expr *pExpr2 = nullptr;
    Expr *pExpr3 = nullptr;
    Stmt *pStmt = nullptr;
};

// StmtJump: A jump-statement
struct StmtJump : Stmt {
    using Stmt::Stmt;
    ~StmtJump() {
    }
    bool Exec() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    TokenType tp;
};

// Decl: An init-declarator
struct Decl : Stmt {
    Decl(Scope *pScope_, const Pos &ps_) : Stmt(ps_), pScope(pScope_) {
    }
    ~Decl();
    bool Exec() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    Scope *pScope;
    list<pair<string, ExprAssig *>> li;
};

// ExprBase: Base class for expressions
struct ExprBase {
    ExprBase(Scope *pScope_) : pScope(pScope_) {
    }
    virtual ~ExprBase() {
    }
    virtual int Eval() = 0;
    unordered_map<string, int>::iterator Lookup(const string &sId) {
        assert(pScope);
        return pScope->Lookup(sId);
    }
#ifndef NDEBUG
    virtual void DbgPrint() = 0;
#endif
    Scope *pScope;
};

// Expr: An expression
struct Expr : ExprBase {
    using ExprBase::ExprBase;
    ~Expr();
    int Eval() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    ExprAssig *pExprAsg = nullptr;
    list<ExprAssig *> li;
};

// ExprAssig: An assignment-expression
struct ExprAssig : ExprBase {
    using ExprBase::ExprBase;
    ~ExprAssig();
    int Eval() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    list<pair<ExprUnary *, TokenType>> li;
    ExprCondi *pExprCon = nullptr;
};

// ExprCondi: A conditional-expression
struct ExprCondi : ExprBase {
    using ExprBase::ExprBase;
    ~ExprCondi();
    int Eval() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    ExprLogor *pExprLor = nullptr;
    list<pair<Expr *, ExprLogor *>> li;
};

// ExprLogor: A logical-OR-expression
struct ExprLogor : ExprBase {
    using ExprBase::ExprBase;
    ~ExprLogor();
    int Eval() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    ExprLogan *pExprLan = nullptr;
    list<ExprLogan *> li;
};

// ExprLogan: A logical-AND-expression
struct ExprLogan : ExprBase {
    using ExprBase::ExprBase;
    ~ExprLogan();
    int Eval() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    ExprBitor *pExprBor = nullptr;
    list<ExprBitor *> li;
};

// ExprBitor: An inclusive-OR-expression
struct ExprBitor : ExprBase {
    using ExprBase::ExprBase;
    ~ExprBitor();
    int Eval() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    ExprBitxo *pExprBxo = nullptr;
    list<ExprBitxo *> li;
};

// ExprBitxo: An exclusive-OR-expression
struct ExprBitxo : ExprBase {
    using ExprBase::ExprBase;
    ~ExprBitxo();
    int Eval() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    ExprBitan *pExprBan = nullptr;
    list<ExprBitan *> li;
};

// ExprBitan: An AND-expression
struct ExprBitan : ExprBase {
    using ExprBase::ExprBase;
    ~ExprBitan();
    int Eval() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    ExprEquty *pExprEqu = nullptr;
    list<ExprEquty *> li;
};

// ExprEquty: An equality-expression
struct ExprEquty : ExprBase {
    using ExprBase::ExprBase;
    ~ExprEquty();
    int Eval() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    ExprRelat *pExprRel = nullptr;
    list<pair<ExprRelat *, TokenType>> li;
};

// ExprRelat: A relational-expression
struct ExprRelat : ExprBase {
    using ExprBase::ExprBase;
    ~ExprRelat();
    int Eval() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    ExprShift *pExprSft = nullptr;
    list<pair<ExprShift *, TokenType>> li;
};

// ExprShift: A shift-expression
struct ExprShift : ExprBase {
    using ExprBase::ExprBase;
    ~ExprShift();
    int Eval() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    ExprAddit *pExprAdd = nullptr;
    list<pair<ExprAddit *, TokenType>> li;
};

// ExprAddit: An additive-expression
struct ExprAddit : ExprBase {
    using ExprBase::ExprBase;
    ~ExprAddit();
    int Eval() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    ExprMulti *pExprMul = nullptr;
    list<pair<ExprMulti *, TokenType>> li;
};

// ExprMulti: A multiplicative-expression
struct ExprMulti : ExprBase {
    using ExprBase::ExprBase;
    ~ExprMulti();
    int Eval() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    ExprUnary *pExprUry = nullptr;
    list<pair<ExprUnary *, TokenType>> li;
};

// ExprUnary: An unary-expression
struct ExprUnary : ExprBase {
    using ExprBase::ExprBase;
    ~ExprUnary();
    int Eval() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    list<TokenType> li;
    ExprPofix *pExprPfx = nullptr;
};

// ExprPofix: A postfix-expression
struct ExprPofix : ExprBase {
    using ExprBase::ExprBase;
    ~ExprPofix();
    int Eval() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    ExprPrima *pExprPri = nullptr;
    list<TokenType> li;
    list<Expr *> lie;
};

// ExprPrima: A priamary-expression
struct ExprPrima : ExprBase {
    using ExprBase::ExprBase;
    ~ExprPrima();
    int Eval() override;
#ifndef NDEBUG
    void DbgPrint() override;
#endif
    TokenType tp;
    string s;
    int n;
    Expr *pExpr = nullptr;
};

StmtComp::~StmtComp() {
    for (auto p : li)
        delete p;
}

StmtExpr::~StmtExpr() {
    delete pExpr;
}

StmtIf::~StmtIf() {
    delete pExpr;
    delete pStmt1;
    delete pStmt2;
}

StmtWhile::~StmtWhile() {
    delete pExpr;
    delete pStmt;
}

StmtDo::~StmtDo() {
    delete pStmt;
    delete pExpr;
}

StmtFor::~StmtFor() {
    delete pExpr2;
    delete pExpr3;
    delete pStmt;
}

Decl::~Decl() {
    for (auto &p : li)
        delete p.second;
}

Expr::~Expr() {
    delete pExprAsg;
    for (auto p : li)
        delete p;
}

ExprAssig::~ExprAssig() {
    for (auto &p : li)
        delete p.first;
    delete pExprCon;
}

ExprCondi::~ExprCondi() {
    delete pExprLor;
    for (auto &p : li) {
        delete p.first;
        delete p.second;
    }
}

ExprLogor::~ExprLogor() {
    delete pExprLan;
    for (auto p : li)
        delete p;
}

ExprLogan::~ExprLogan() {
    delete pExprBor;
    for (auto p : li)
        delete p;
}

ExprBitor::~ExprBitor() {
    delete pExprBxo;
    for (auto p : li)
        delete p;
}

ExprBitxo::~ExprBitxo() {
    delete pExprBan;
    for (auto p : li)
        delete p;
}

ExprBitan::~ExprBitan() {
    delete pExprEqu;
    for (auto p : li)
        delete p;
}

ExprEquty::~ExprEquty() {
    delete pExprRel;
    for (auto &p : li)
        delete p.first;
}

ExprRelat::~ExprRelat() {
    delete pExprSft;
    for (auto &p : li)
        delete p.first;
}

ExprShift::~ExprShift() {
    delete pExprAdd;
    for (auto &p : li)
        delete p.first;
}

ExprAddit::~ExprAddit() {
    delete pExprMul;
    for (auto &p : li)
        delete p.first;
}

ExprMulti::~ExprMulti() {
    delete pExprUry;
    for (auto &p : li)
        delete p.first;
}

ExprUnary::~ExprUnary() {
    delete pExprPfx;
}

ExprPofix::~ExprPofix() {
    delete pExprPri;
    for (auto p : lie)
        delete p;
}

ExprPrima::~ExprPrima() {
    delete pExpr;
}

// Processing phase 4: Parsing and constructing data structures
namespace pp4 {
    auto &li = *pp3::pli;
    decay_t<decltype(li)>::iterator ci;
    StmtComp *pStmtComp = nullptr;
    
    // Parsing functions, every function parses an object.
    
    Stmt *          ParseStmt       (Scope *pScope);
    StmtComp *      ParseStmtComp   (Scope *pScope);
    StmtExpr *      ParseStmtExpr   (Scope *pScope);
    StmtIf *        ParseStmtIf     (Scope *pScope);
    StmtWhile *     ParseStmtWhile  (Scope *pScope);
    StmtDo *        ParseStmtDo     (Scope *pScope);
    StmtFor *       ParseStmtFor    (Scope *pScope);
    StmtJump *      ParseStmtJump   (Scope *pScope);
    Decl *          ParseDecl       (Scope *pScope);
    Expr *          ParseExpr       (Scope *pScope);
    ExprAssig *     ParseExprAssig  (Scope *pScope);
    ExprCondi *     ParseExprCondi  (Scope *pScope);
    ExprLogor *     ParseExprLogor  (Scope *pScope);
    ExprLogan *     ParseExprLogan  (Scope *pScope);
    ExprBitor *     ParseExprBitor  (Scope *pScope);
    ExprBitxo *     ParseExprBitxo  (Scope *pScope);
    ExprBitan *     ParseExprBitan  (Scope *pScope);
    ExprEquty *     ParseExprEquty  (Scope *pScope);
    ExprRelat *     ParseExprRelat  (Scope *pScope);
    ExprShift *     ParseExprShift  (Scope *pScope);
    ExprAddit *     ParseExprAddit  (Scope *pScope);
    ExprMulti *     ParseExprMulti  (Scope *pScope);
    ExprUnary *     ParseExprUnary  (Scope *pScope);
    ExprPofix *     ParseExprPofix  (Scope *pScope);
    ExprPrima *     ParseExprPrima  (Scope *pScope);
    
    // statement
    // > compound-statement
    // > expression-statement
    // > selection-statement
    //   > if-statement
    // > iteration-statement
    //   > while-statement
    //   > do-statement
    //   > for-statement
    // > jump-statement
    Stmt *ParseStmt(Scope *pScope) {
        if (*ci == T_LBR)
            return ParseStmtComp(pScope);
        if (*ci == T_KIF)
            return ParseStmtIf(pScope);
        if (*ci == T_KFO)
            return ParseStmtFor(pScope);
        if (*ci == T_KWH)
            return ParseStmtWhile(pScope);
        if (*ci == T_KDO)
            return ParseStmtDo(pScope);
        if (*ci == T_KCO || *ci == T_KBR)
            return ParseStmtJump(pScope);
        return ParseStmtExpr(pScope);
    }
    
    // compound-statement
    // > { block-item-list(opt) }
    // block-item-list
    // > block-item
    // > block-item-list block-item
    // block-item
    // > declaration
    // > statement
    StmtComp *ParseStmtComp(Scope *pScope) {
        auto p = new StmtComp(pScope, ci->ps);
        assert(*ci == T_LBR);
        ++ci;
        while (*ci != T_RBR) {
            if (*ci == T_KIN)
                p->li.push_back(ParseDecl(p));
            else
                p->li.push_back(ParseStmt(p));
        }
        assert(*ci == T_RBR);
        ++ci;
        return p;
    }
    
    // expression-statement
    // > expression(opt) ;
    StmtExpr *ParseStmtExpr(Scope *pScope) {
        auto p = new StmtExpr(ci->ps);
        if (*ci != T_SMC) {
            p->pExpr = ParseExpr(pScope);
            assert(p->pExpr);
        }
        assert(*ci == T_SMC);
        ++ci;
        return p;
    }
    
    // if-statement
    // > if ( expression ) statement
    // > if ( expression ) statement else statement
    StmtIf *ParseStmtIf(Scope *pScope) {
        auto p = new StmtIf(ci->ps);
        assert(*ci == T_KIF);
        ++ci;
        assert(*ci == T_LPA);
        ++ci;
        p->pExpr = ParseExpr(pScope);
        assert(*ci == T_RPA);
        ++ci;
        p->pStmt1 = ParseStmt(pScope);
        if (*ci == T_KEL) {
            ++ci;
            p->pStmt2 = ParseStmt(pScope);
        }
        return p;
    }
    
    // while-statement
    // > while ( expression ) statement
    StmtWhile *ParseStmtWhile(Scope *pScope) {
        auto p = new StmtWhile(ci->ps);
        assert(*ci == T_KWH);
        ++ci;
        assert(*ci == T_LPA);
        ++ci;
        p->pExpr = ParseExpr(pScope);
        assert(*ci == T_RPA);
        ++ci;
        p->pStmt = ParseStmt(pScope);
        return p;
    }
    
    // do-statement
    // > do statement while ( expression ) ;
    StmtDo *ParseStmtDo(Scope *pScope) {
        auto p = new StmtDo(Pos {});
        assert(*ci == T_KDO);
        ++ci;
        p->pStmt = ParseStmt(pScope);
        assert(*ci == T_KWH);
        p->ps = ci++->ps;
        assert(*ci == T_LPA);
        ++ci;
        p->pExpr = ParseExpr(pScope);
        assert(*ci == T_RPA);
        ++ci;
        assert(*ci == T_SMC);
        ++ci;
        return p;
    }
    
    // for-statement
    // > for ( expression(opt) ; expression(opt) ; expression(opt) ) statement
    // > for ( declaration expression(opt) ; expression(opt) ) statement
    StmtFor *ParseStmtFor(Scope *pScope) {
        auto p = new StmtFor(pScope, ci->ps);
        assert(*ci == T_KFO);
        ++ci;
        assert(*ci == T_LPA);
        ++ci;
        if (*ci == T_KIN)
            p->pClause1 = ParseDecl(p);
        else
            p->pClause1 = ParseStmtExpr(p); // <=> expression-statement
        if (*ci != T_SMC)
            p->pExpr2 = ParseExpr(p);
        assert(*ci == T_SMC);
        ++ci;
        if (*ci != T_RPA)
            p->pExpr3 = ParseExpr(p);
        assert(*ci == T_RPA);
        ++ci;
        p->pStmt = ParseStmt(p);
        return p;
    }
    
    // jump-statement
    // > continue ;
    // > break ;
    StmtJump *ParseStmtJump(Scope *) {
        auto p = new StmtJump(ci->ps);
        assert(*ci == T_KCO || *ci == T_KBR);
        p->tp = *ci++;
        assert(*ci == T_SMC);
        ++ci;
        return p;
    }
    
    // declaration
    // > declaration-specifiers init-declarator-list(opt)
    // declaration-specifiers
    // > int
    // init-declarator-list
    // > init-declarator
    // > init-declarator-list , init-declarator
    // init-declarator
    // > declarator
    // > declarator = initializer
    // declarator
    // > identifier
    // initializer
    // > assignment-expression
    Decl *ParseDecl(Scope *pScope) {
        auto p = new Decl(pScope, ci->ps);
        assert(*ci == T_KIN);
        ++ci;
        for(;;) {
            assert(*ci == T_IDN);
            auto sId = ci++->s;
            if (*ci == T_ASG) {
                ++ci;
                p->li.emplace_back(sId, ParseExprAssig(pScope));
            }
            else
                p->li.emplace_back(sId, nullptr);
            if (*ci == T_SMC)
                break;
            assert(*ci == T_CMA);
            ++ci;
        }
        assert(*ci == T_SMC);
        ++ci;
        return p;
    }
    
    // expression
    // > assignment-expression
    // > expression , assignment-expression
    Expr *ParseExpr(Scope *pScope) {
        auto p = new Expr(pScope);
        auto ti = ci;
        try {
            if (!(p->pExprAsg = ParseExprAssig(pScope)))
                throw nullptr;
            while (ci != li.end() && *ci == T_CMA) {
                ++ci;
                auto pa = ParseExprAssig(pScope);
                if (!pa)
                    throw nullptr;
                p->li.emplace_back(pa);
            }
            return p;
        }
        catch (...) {
            delete p;
            ci = ti;
        }
        return nullptr;
    }
    
    // assignment-expression
    // > conditional-expression
    // > unary-expression assignment-operator assignment-expression
    // assignment-operator
    // > one of: = *= /= %= += -= <<= >>= &= ^= |=
    ExprAssig *ParseExprAssig(Scope *pScope) {
        static const unordered_set<unsigned> stOpr {
            T_ASG,
            T_AMU, T_ADI, T_AMO,
            T_AAD, T_ASU, 
            T_ASL, T_ASR,
            T_AAN, T_AXO, T_AOR,
        };
        auto p = new ExprAssig(pScope);
        auto ti = ci;
        try {
            for (;;) {
                if (!(p->pExprCon = ParseExprCondi(pScope)))
                    throw nullptr;
                if (ci == li.end())
                    break;
                if (stOpr.find(*ci) == stOpr.end())
                    break;
                TokenType tp = *ci++;
                auto pExprCon = p->pExprCon;
                if (!pExprCon->li.empty() || !pExprCon->pExprLor)
                    throw nullptr;
                auto pExprLor = pExprCon->pExprLor;
                if (!pExprLor->li.empty() || !pExprLor->pExprLan)
                    throw nullptr;
                auto pExprLan = pExprLor->pExprLan;
                if (!pExprLan->li.empty() || !pExprLan->pExprBor)
                    throw nullptr;
                auto pExprBor = pExprLan->pExprBor;
                if (!pExprBor->li.empty() || !pExprBor->pExprBxo)
                    throw nullptr;
                auto pExprBxo = pExprBor->pExprBxo;
                if (!pExprBxo->li.empty() || !pExprBxo->pExprBan)
                    throw nullptr;
                auto pExprBan = pExprBxo->pExprBan;
                if (!pExprBan->li.empty() || !pExprBan->pExprEqu)
                    throw nullptr;
                auto pExprEqu = pExprBan->pExprEqu;
                if (!pExprEqu->li.empty() || !pExprEqu->pExprRel)
                    throw nullptr;
                auto pExprRel = pExprEqu->pExprRel;
                if (!pExprRel->li.empty() || !pExprRel->pExprSft)
                    throw nullptr;
                auto pExprSft = pExprRel->pExprSft;
                if (!pExprSft->li.empty() || !pExprSft->pExprAdd)
                    throw nullptr;
                auto pExprAdd = pExprSft->pExprAdd;
                if (!pExprAdd->li.empty() || !pExprAdd->pExprMul)
                    throw nullptr;
                auto pExprMul = pExprAdd->pExprMul;
                if (!pExprMul->li.empty() || !pExprMul->pExprUry)
                    throw nullptr;
                p->li.emplace_back(pExprMul->pExprUry, tp);
                pExprMul->pExprUry = nullptr;
                delete p->pExprCon;
                p->pExprCon = nullptr;
            }
            if (!p->pExprCon)
                throw nullptr;
            return p;
        }
        catch (...) {
            delete p;
            ci = ti;
        }
        return nullptr;
    }
    
    // conditional-expression
    // > logical-OR-expression
    // > logical-OR-expression ? expression : conditional-expression
    ExprCondi *ParseExprCondi(Scope *pScope) {
        auto p = new ExprCondi(pScope);
        auto ti = ci;
        try {
            if (!(p->pExprLor = ParseExprLogor(pScope)))
                throw nullptr;
            while (ci != li.end() && *ci == T_QUE) {
                ++ci;
                auto q = ParseExpr(pScope);
                if (!q || ci == li.end() || *ci != T_COL)
                    throw nullptr;
                ++ci;
                auto r = ParseExprLogor(pScope);
                if (!r)
                    throw nullptr;
                p->li.emplace_back(q, r);
            }
            return p;
        }
        catch (...) {
            delete p;
            ci = ti;
        }
        return nullptr;
    }
    
    // logical-OR-expression
    // > logical-AND-expression
    // > logical-OR-expression || logical-AND-expression
    ExprLogor *ParseExprLogor(Scope *pScope) {
        auto p = new ExprLogor(pScope);
        auto ti = ci;
        try {
            if (!(p->pExprLan = ParseExprLogan(pScope)))
                throw nullptr;
            while (ci != li.end() && *ci == T_LOR) {
                ++ci;
                auto q = ParseExprLogan(pScope);
                if (!q)
                    throw nullptr;
                p->li.emplace_back(q);
            }
            return p;
        }
        catch (...) {
            delete p;
            ci = ti;
        }
        return nullptr;
    }

    // logical-AND-expression
    // > inclusive-OR-expression
    // > logical-AND-expression && inclusive-OR-expression
    ExprLogan *ParseExprLogan(Scope *pScope) {
        auto p = new ExprLogan(pScope);
        auto ti = ci;
        try {
            if (!(p->pExprBor = ParseExprBitor(pScope)))
                throw nullptr;
            while (ci != li.end() && *ci == T_LAN) {
                ++ci;
                auto q = ParseExprBitor(pScope);
                if (!q)
                    throw nullptr;
                p->li.emplace_back(q);
            }
            return p;
        }
        catch (...) {
            delete p;
            ci = ti;
        }
        return nullptr;
    }
    
    // inclusive-OR-expression
    // > exclusive-OR-expression
    // > inclusive-OR-expression | exclusive-OR-expression
    ExprBitor *ParseExprBitor(Scope *pScope) {
        auto p = new ExprBitor(pScope);
        auto ti = ci;
        try {
            if (!(p->pExprBxo = ParseExprBitxo(pScope)))
                throw nullptr;
            while (ci != li.end() && *ci == T_BOR) {
                ++ci;
                auto q = ParseExprBitxo(pScope);
                if (!q)
                    throw nullptr;
                p->li.emplace_back(q);
            }
            return p;
        }
        catch (...) {
            delete p;
            ci = ti;
        }
        return nullptr;
    }
    
    // exclusive-OR-expression
    // > AND-expression
    // > exclusive-OR-expression ^ AND-expression
    ExprBitxo *ParseExprBitxo(Scope *pScope) {
        auto p = new ExprBitxo(pScope);
        auto ti = ci;
        try {
            if (!(p->pExprBan = ParseExprBitan(pScope)))
                throw nullptr;
            while (ci != li.end() && *ci == T_BXO) {
                ++ci;
                auto q = ParseExprBitan(pScope);
                if (!q)
                    throw nullptr;
                p->li.emplace_back(q);
            }
            return p;
        }
        catch (...) {
            delete p;
            ci = ti;
        }
        return nullptr;
    }
    
    // AND-expression
    // > equality-expression
    // > AND-expression & equality-expression
    ExprBitan *ParseExprBitan(Scope *pScope) {
        auto p = new ExprBitan(pScope);
        auto ti = ci;
        try {
            if (!(p->pExprEqu = ParseExprEquty(pScope)))
                throw nullptr;
            while (ci != li.end() && *ci == T_BAN) {
                ++ci;
                auto q = ParseExprEquty(pScope);
                if (!q)
                    throw nullptr;
                p->li.emplace_back(q);
            }
            return p;
        }
        catch (...) {
            delete p;
            ci = ti;
        }
        return nullptr;
    }
    
    // equality-expression
    // > relational-expression
    // > equality-expression == relational-expression
    // > equality-expression != relational-expression
    ExprEquty *ParseExprEquty(Scope *pScope) {
        static const unordered_set<unsigned> stOpr {
            T_EQU, T_NEQ
        };
        auto p = new ExprEquty(pScope);
        auto ti = ci;
        try {
            if (!(p->pExprRel = ParseExprRelat(pScope)))
                throw nullptr;
            while (ci != li.end() && stOpr.find(*ci) != stOpr.end()) {
                TokenType tp = *ci++;
                auto q = ParseExprRelat(pScope);
                if (!q)
                    throw nullptr;
                p->li.emplace_back(q, tp);
            }
            return p;
        }
        catch (...) {
            delete p;
            ci = ti;
        }
        return nullptr;
    }
    
    // relational-expression
    // > shift-expression
    // > relational-expression <  shift-expression
    // > relational-expression >  shift-expression
    // > relational-expression <= shift-expression
    // > relational-expression >= shift-expression
    ExprRelat *ParseExprRelat(Scope *pScope) {
        static const unordered_set<unsigned> stOpr {
            T_LES, T_GRE, T_LEQ, T_GEQ
        };
        auto p = new ExprRelat(pScope);
        auto ti = ci;
        try {
            if (!(p->pExprSft = ParseExprShift(pScope)))
                throw nullptr;
            while (ci != li.end() && stOpr.find(*ci) != stOpr.end()) {
                TokenType tp = *ci++;
                auto q = ParseExprShift(pScope);
                if (!q)
                    throw nullptr;
                p->li.emplace_back(q, tp);
            }
            return p;
        }
        catch (...) {
            delete p;
            ci = ti;
        }
        return nullptr;
    }
    
    // shift-expression
    // > additive-expression
    // > shift-expression << additive-expression
    // > shift-expression >> additive-expression
    ExprShift *ParseExprShift(Scope *pScope) {
        static const unordered_set<unsigned> stOpr {
            T_SHL, T_SHR,
        };
        auto p = new ExprShift(pScope);
        auto ti = ci;
        try {
            if (!(p->pExprAdd = ParseExprAddit(pScope)))
                throw nullptr;
            while (ci != li.end() && stOpr.find(*ci) != stOpr.end()) {
                TokenType tp = *ci++;
                auto q = ParseExprAddit(pScope);
                if (!q)
                    throw nullptr;
                p->li.emplace_back(q, tp);
            }
            return p;
        }
        catch (...) {
            delete p;
            ci = ti;
        }
        return nullptr;
    }
    
    // additive-expression
    // > multiplicative-expression
    // > additive-expression + multiplicative-expression
    // > additive-expression - multiplicative-expression
    ExprAddit *ParseExprAddit(Scope *pScope) {
        static const unordered_set<unsigned> stOpr {
            T_ADD, T_SUB
        };
        auto p = new ExprAddit(pScope);
        auto ti = ci;
        try {
            if (!(p->pExprMul = ParseExprMulti(pScope)))
                throw nullptr;
            while (ci != li.end() && stOpr.find(*ci) != stOpr.end()) {
                TokenType tp = *ci++;
                auto q = ParseExprMulti(pScope);
                if (!q)
                    throw nullptr;
                p->li.emplace_back(q, tp);
            }
            if (!p->pExprMul)
                throw nullptr;
            return p;
        }
        catch (...) {
            delete p;
            ci = ti;
        }
        return nullptr;
    }
    
    // multiplicative-expression
    // > cast-expression
    // > multiplicative->expression * cast-expression
    // > multiplicative->expression / cast-expression
    // > multiplicative->expression % cast-expression
    // cast-expression
    // > unary-expression
    ExprMulti *ParseExprMulti(Scope *pScope) {
        static const unordered_set<unsigned> stOpr {
            T_MUL, T_DIV, T_MOD
        };
        auto p = new ExprMulti(pScope);
        auto ti = ci;
        try {
            if (!(p->pExprUry = ParseExprUnary(pScope)))
                throw nullptr;
            while (ci != li.end() && stOpr.find(*ci) != stOpr.end()) {
                TokenType tp = *ci++;
                auto q = ParseExprUnary(pScope);
                if (!q)
                    throw nullptr;
                p->li.emplace_back(q, tp);
            }
            if (!p->pExprUry)
                throw nullptr;
            return p;
        }
        catch (...) {
            delete p;
            ci = ti;
        }
        return nullptr;
    }
    
    // unary-expression
    // > postfix-expression
    // > ++ unary-expression
    // > -- unary-expression
    // > unary-operator cast-expression
    // unary-operator
    // > one of: + - ! ~
    ExprUnary *ParseExprUnary(Scope *pScope) {
        static const unordered_set<unsigned> stOpr {
            T_INC, T_DEC, T_ADD, T_SUB, T_NEG, T_TIL
        };
        auto p = new ExprUnary(pScope);
        auto ti = ci;
        while (ci != li.end() && stOpr.find(*ci) != stOpr.end())
            p->li.emplace_back(*ci++);
        if (!(p->pExprPfx = ParseExprPofix(pScope))) {
            delete p;
            ci = ti;
            return nullptr;
        }
        return p;
    }
    
    // postfix-expression
    // > primary-expression
    // > postfix-expression ( argument-expression-list(opt) )
    // > postfix-expression ++
    // > postfix-expression --
    // argument-expression-list
    // > assignment-expression
    // > argument-epression-list , assignment-expression
    ExprPofix *ParseExprPofix(Scope *pScope) {
        static const unordered_set<unsigned> stOpr {
            T_INC, T_DEC, T_LPA
        };
        auto p = new ExprPofix(pScope);
        auto ti = ci;
        try {
            if (!(p->pExprPri = ParseExprPrima(pScope)))
                throw nullptr;
            while (ci != li.end() && stOpr.find(*ci) != stOpr.end()) {
                if (*ci == T_LPA) {
                    ++ci;
                    if (ci == li.end())
                        throw nullptr;
                    p->li.emplace_back(T_LPA);
                    if (*ci == T_RPA) {
                        p->lie.emplace_back(nullptr);
                        ++ci;
                        continue;
                    }
                    auto pe = ParseExpr(pScope); // <=> expression
                    if (!pe)
                        throw nullptr;
                    p->lie.emplace_back(pe);
                    if (ci == li.end() || *ci != T_RPA)
                        throw nullptr;
                    ++ci;
                    continue;
                }
                p->li.emplace_back(*ci++);
            }
            return p;
        }
        catch (...) {
            delete p;
            ci = ti;
        }
        return nullptr;
    }
    
    // primary-expression
    // > identifier
    // > constant
    // > string-literal
    // > ( expression )
    ExprPrima *ParseExprPrima(Scope *pScope) {
        auto p = new ExprPrima(pScope);
        auto ti = ci;
        try {
            if (ci == li.end())
                throw nullptr;
            if (*ci == T_IDN) {
                p->tp = T_IDN;
                p->s = ci->s;
                ++ci;
                return p;
            }
            if (*ci == T_CON) {
                p->tp = T_CON;
                p->n = ci->n;
                ++ci;
                return p;
            }
            if (*ci == T_STR) {
                p->tp = T_STR;
                p->s = ci->s;
                ++ci;
                return p;
            }
            if (*ci == T_LPA) {
                p->tp = T_LPA;
                ++ci;
                if (!(p->pExpr = ParseExpr(pScope)))
                    throw nullptr;
                if (ci == li.end() || *ci != T_RPA)
                    throw nullptr;
                ++ci;
                return p;
            }
        }
        catch (...) {
            delete p;
            ci = ti;
        }
        return nullptr;
    }
    
    void Perform() {
        // Adds a dummy printf identifier and wraps the whole token list.
        // { int printf ; { tokenlist } }
        li.emplace_back(T_RBR, "}", Pos {});
        li.emplace_back(T_RBR, "}", Pos {});
        li.emplace_front(T_LBR, "{", Pos {});
        li.emplace_front(T_SMC, ";", Pos {});
        li.emplace_front(T_IDN, "printf", Pos {});
        li.emplace_front(T_IDN, "int", Pos {});
        li.emplace_front(T_LBR, "{", Pos {});
        ci = li.begin();
        // Parses the token list as a compound-statement
        pStmtComp = ParseStmtComp(nullptr);
        assert(ci == li.end());
        delete pp3::pli;
    }
}

// Processing phase 5: Simulating the execution and recording line numbers
namespace pp5 {
    list<unsigned> li;
    TokenType tpJump;
    int nTmp;
    reference_wrapper<int> refLv(nTmp);
    
    inline void RecLn(unsigned u) {
        if (li.empty() || li.back() != u)
            li.push_back(u);
    }
    
    inline void RecLn(Stmt *p) {
        RecLn(p->ps.uRow);
    }
    
    template<class tExpr>
    int AccuRef(list<pair<tExpr *, TokenType>> &li, int nVal,
        const unordered_map<unsigned, function<void (int &, int)>> &mOpr)
    {
        for (auto &p : li)
            mOpr.at(p.second)(nVal, p.first->Eval());
        return nVal;
    }
    
    template<class tExpr>
    int AccuVal(list<pair<tExpr *, TokenType>> &li, int nVal,
        const unordered_map<unsigned, function<int (int, int)>> &mOpr)
    {
        for (auto &p : li)
            nVal = mOpr.at(p.second)(nVal, p.first->Eval());
        return nVal;
    }
    
    template<class tExpr>
    int AccuRef(list<tExpr *> &li, int nVal,
        const function<void (int &, int)> &fnOpr)
    {
        for (auto &p : li)
            fnOpr(nVal, p->Eval());
        return nVal;
    }
    
    template<class tExpr>
    int AccuVal(list<tExpr *> &li, int nVal,
        const function<int (int, int)> &fnOpr)
    {
        for (auto &p : li)
            nVal = fnOpr(nVal, p->Eval());
        return nVal;
    }
    
    void Perform() {
        pp4::pStmtComp->Exec();
        delete pp4::pStmtComp;
    }
}

bool StmtComp::Exec() {
    mst.clear();
    for (auto p : li)
        if (p->Exec())
            return true;
    return false;
}

bool StmtExpr::Exec() {
    if (pExpr) {
        pp5::RecLn(this);
        pExpr->Eval();
    }
    return false;
}

bool StmtIf::Exec() {
    pp5::RecLn(this);
    if (pExpr->Eval())
        return pStmt1->Exec();
    if (pStmt2)
        return pStmt2->Exec();
    return false;
}

bool StmtWhile::Exec() {
    for (;;) {
        pp5::RecLn(this);
        if (!pExpr->Eval())
            break;
        if (pStmt->Exec() && pp5::tpJump == T_KBR)
            break;
    }
    return false;
}

bool StmtDo::Exec() {
    for (;;) {
        if (pStmt->Exec() && pp5::tpJump == T_KBR)
            break;
        pp5::RecLn(this);
        if (!pExpr->Eval())
            break;
    }
    return false;
}

bool StmtFor::Exec() {
    pp5::RecLn(this);
    pClause1->Exec();
    for (;;) {
        pp5::RecLn(this);
        if (pExpr2 && !pExpr2->Eval())
            break;
        if (pStmt->Exec() && pp5::tpJump == T_KBR)
            break;
        if (pExpr3)
            pExpr3->Eval();
    }
    return false;
}

bool StmtJump::Exec() {
    pp5::RecLn(this);
    pp5::tpJump = tp;
    return true;
}

bool Decl::Exec() {
    for (auto &p : li)
        if (p.second) {
            pp5::RecLn(this);
            pScope->mst.emplace(p.first, p.second->Eval());
        }
        else
            pScope->mst.emplace(p.first, 0);
    return false;
}

int Expr::Eval() {
    int v = pExprAsg->Eval();
    for (auto p : li)
        v = p->Eval();
    return v;
}

int ExprAssig::Eval() {
    static const unordered_map<unsigned, function<void (int &, int)>> mOpr {
        {T_ASG, [] (int &lhs, int rhs) {lhs   = rhs;}},
        {T_AMU, [] (int &lhs, int rhs) {lhs  *= rhs;}},
        {T_ADI, [] (int &lhs, int rhs) {lhs  /= rhs;}},
        {T_AMO, [] (int &lhs, int rhs) {lhs  %= rhs;}},
        {T_AAD, [] (int &lhs, int rhs) {lhs  += rhs;}},
        {T_ASU, [] (int &lhs, int rhs) {lhs  -= rhs;}},
        {T_ASL, [] (int &lhs, int rhs) {lhs <<= rhs;}},
        {T_ASR, [] (int &lhs, int rhs) {lhs >>= rhs;}},
        {T_AAN, [] (int &lhs, int rhs) {lhs  &= rhs;}},
        {T_AXO, [] (int &lhs, int rhs) {lhs  ^= rhs;}},
        {T_AOR, [] (int &lhs, int rhs) {lhs  |= rhs;}},
    };
    int v = pExprCon->Eval();
    for (auto i = li.rbegin(); i != li.rend(); ++i) {
        i->first->Eval();
        mOpr.at(i->second)(pp5::refLv, v);
        v = pp5::refLv;
    }
    return v;
}

int ExprCondi::Eval() {
    int v = pExprLor->Eval();
    for (auto &p : li) {
        if (v) {
            v = p.first->Eval();
            break;
        }
        v = p.second->Eval();
    }
    return v;
}

int ExprLogor::Eval() {
    int v = pExprLan->Eval();
    for (auto i = li.begin(); !v && i != li.end(); ++i)
        v = (*i)->Eval();
    return li.empty() ? v : !!v;
}

int ExprLogan::Eval() {
    int v = pExprBor->Eval();
    for (auto i = li.begin(); v && i != li.end(); ++i)
        v = (*i)->Eval();
    return li.empty() ? v : !!v;
}

int ExprBitor::Eval() {
    return pp5::AccuVal(li, pExprBxo->Eval(), bit_or<int>());
}

int ExprBitxo::Eval() {
    return pp5::AccuVal(li, pExprBan->Eval(), bit_xor<int>());
}

int ExprBitan::Eval() {
    return pp5::AccuVal(li, pExprEqu->Eval(), bit_and<int>());
}

int ExprEquty::Eval() {
    static const unordered_map<unsigned, function<int (int, int)>> mOpr {
        {T_EQU, equal_to<int>()},
        {T_NEQ, not_equal_to<int>()},
    };
    return pp5::AccuVal(li, pExprRel->Eval(), mOpr);
}

int ExprRelat::Eval() {
    static const unordered_map<unsigned, function<int (int, int)>> mOpr {
        {T_LES, less<int>()},
        {T_GRE, greater<int>()},
        {T_LEQ, less_equal<int>()},
        {T_GEQ, greater_equal<int>()},
    };
    return pp5::AccuVal(li, pExprSft->Eval(), mOpr);
}

int ExprShift::Eval() {
    static const unordered_map<unsigned, function<void (int &, int)>> mOpr {
        {T_SHL, [] (int &lhs, int rhs) {return lhs <<= rhs;}},
        {T_SHR, [] (int &lhs, int rhs) {return lhs >>= rhs;}},
    };
    return pp5::AccuRef(li, pExprAdd->Eval(), mOpr);
}

int ExprAddit::Eval() {
    static const unordered_map<unsigned, function<void (int &, int)>> mOpr {
        {T_ADD, [] (int &lhs, int rhs) {lhs += rhs;}},
        {T_SUB, [] (int &lhs, int rhs) {lhs -= rhs;}},
    };
    return pp5::AccuRef(li, pExprMul->Eval(), mOpr);
}

int ExprMulti::Eval() {
    static const unordered_map<unsigned, function<void (int &, int)>> mOpr {
        {T_MUL, [] (int &lhs, int rhs) {lhs *= rhs;}},
        {T_DIV, [] (int &lhs, int rhs) {lhs /= rhs;}},
        {T_MOD, [] (int &lhs, int rhs) {lhs %= rhs;}},
    };
    return pp5::AccuRef(li, pExprUry->Eval(), mOpr);
}

int ExprUnary::Eval() {
    static const unordered_map<unsigned, function<void (int &)>> mOpr {
        {T_ADD, [] (int &) {;}},
        {T_SUB, [] (int &v) {v = -v;}},
        {T_NEG, [] (int &v) {v = !v;}},
        {T_TIL, [] (int &v) {v = ~v;}},
        {T_INC, [&] (int &v) {pp5::refLv.get() = ++v;}},
        {T_DEC, [&] (int &v) {pp5::refLv.get() = --v;}},
    };
    int v = pExprPfx->Eval();
    for (auto i = li.rbegin(); i != li.rend(); ++i)
        mOpr.at(*i)(v);
    return v;
}

int ExprPofix::Eval() {
    int v = pExprPri->Eval();
    auto i = lie.begin();
    for (auto tp : li)
        switch (tp) {
        case T_INC:
            pp5::refLv.get() = v + 1;
            break;
        case T_DEC:
            pp5::refLv.get() = v - 1;
            break;
        case T_LPA:
            (*i++)->Eval();
            v = 0;
            break;
        default:
            assert(false);
        }
    return v;
}

int ExprPrima::Eval() {
    static const unordered_map<unsigned, function<int (ExprPrima *)>> mOpr {
        {T_IDN, [] (ExprPrima *p) -> int
            {return pp5::refLv = p->Lookup(p->s)->second;}
        },
        {T_CON, [] (ExprPrima *p) {return p->n;}},
        {T_STR, [] (ExprPrima *p) {return (int) (intptr_t) p->s.c_str();}},
        {T_LPA, [] (ExprPrima *p) {return p->pExpr->Eval();}},
    };
    return mOpr.at(tp)(this);
}

// Processing phase 6: Ouputing the result
namespace pp6 {
    void Perform() {
#   ifndef NDEBUG
#       define pFile stdout
#   else
        FILE *pFile = fopen("output.txt", "w");
#   endif
        assert(pFile);
        bool bFirst = true;
        for (auto u : pp5::li) {
            if (bFirst)
                bFirst = false;
            else
                fputc(' ', pFile);
            fprintf(pFile, "%u", u);
        }
        fputc('\n', pFile);
        fclose(pFile);
    }
}

// Anything surrounded by #ifndef NDEBUG and #endif is only used when debugging
#ifndef NDEBUG

inline void DbgPut(char c) {
    fputc(c, stderr);
}

inline void DbgPut(const char *s) {
    fputs(s, stderr);
}

inline void DbgPut(const string &s) {
    fputs(s.c_str(), stderr);
}

inline void DbgFmt(const char *sFmt, ...) {
    va_list vl;
    va_start(vl, sFmt);
    vfprintf(stderr, sFmt, vl);
    va_end(vl);
}

void DbgPrint(const list<Char> &l) {
    for (auto &c : l)
        DbgPut(c.ch);
}

void DbgPrint(const list<Token> &l) {
    static string tp[TCOUNT];
    tp[T_NEG] = "OprNeg";
    tp[T_TIL] = "OprTil";
    tp[T_INC] = "OprInc";
    tp[T_DEC] = "OprDec";
    tp[T_MUL] = "OprMul";
    tp[T_DIV] = "OprDiv";
    tp[T_MOD] = "OprMod";
    tp[T_ADD] = "OprAdd";
    tp[T_SUB] = "OprSub";
    tp[T_SHL] = "OprShl";
    tp[T_SHR] = "OprShr";
    tp[T_LES] = "OprLes";
    tp[T_GRE] = "OprGre";
    tp[T_LEQ] = "OprLeq";
    tp[T_GEQ] = "OprGeq";
    tp[T_EQU] = "OprEqu";
    tp[T_NEQ] = "OprNeq";
    tp[T_BAN] = "OprBitAnd";
    tp[T_BXO] = "OprBitXor";
    tp[T_BOR] = "OprBitOr";
    tp[T_LAN] = "OprLogAnd";
    tp[T_LOR] = "OprLogOr";
    tp[T_ASG] = "OprAsg";
    tp[T_AMU] = "OprAsgMul";
    tp[T_ADI] = "OprAsgDiv";
    tp[T_AMO] = "OprAsgMod";
    tp[T_AAD] = "OprAsgAdd";
    tp[T_ASU] = "OprAsgSub";
    tp[T_ASL] = "OprAsgShl";
    tp[T_ASR] = "OprAsgShr";
    tp[T_AAN] = "OprAsgAnd";
    tp[T_AXO] = "OprAsgXor";
    tp[T_AOR] = "OprAsgOr";
    tp[T_QUE] = "PunctQue";
    tp[T_COL] = "PunctCol";
    tp[T_LPA] = "PunctLpa";
    tp[T_RPA] = "PunctRpa";
    tp[T_LBR] = "PunctLbr";
    tp[T_RBR] = "PunctRbr";
    tp[T_CMA] = "PunctCma";
    tp[T_SMC] = "PunctSmc";
    tp[T_IDN] = "Identifier";
    tp[T_CON] = "Constant";
    tp[T_STR] = "StrLiteral";
    tp[T_KIN] = "KwdInt";
    tp[T_KIF] = "KwdIf";
    tp[T_KEL] = "KwdElse";
    tp[T_KWH] = "KwdWhile";
    tp[T_KDO] = "KwdDo";
    tp[T_KFO] = "KwdFor";
    tp[T_KBR] = "KwdBreak";
    for (auto &t : l)
        DbgFmt("%-10s (%-2u, %-2u) %s\n",
            tp[t.tp].c_str(), t.ps.uRow, t.ps.uCol, t.s.c_str());
}

template<class tWhat>
void DbgPrint(tWhat *p) {
    p->DbgPrint();
    DbgPut('\n');
}

void StmtComp::DbgPrint() {
    DbgPut("{ ");
    for (auto p : li)
        p->DbgPrint();
    DbgPut("} ");
}

void StmtExpr::DbgPrint() {
    if (pExpr)
        pExpr->DbgPrint();
    DbgPut("; ");
}

void StmtIf::DbgPrint() {
    DbgPut("if ( ");
    pExpr->DbgPrint();
    DbgPut(") ");
    pStmt1->DbgPrint();
    if (pStmt2) {
        DbgPut("else ");
        pStmt2->DbgPrint();
    }
}

void StmtWhile::DbgPrint() {
    DbgPut("while ( ");
    pExpr->DbgPrint();
    DbgPut(") ");
    pStmt->DbgPrint();
}

void StmtDo::DbgPrint() {
    DbgPut("do ");
    pStmt->DbgPrint();
    DbgPut("while ( ");
    pExpr->DbgPrint();
    DbgPut("); ");
}

void StmtFor::DbgPrint() {
    DbgPut("for ( ");
    pClause1->DbgPrint();
    if (pExpr2)
        pExpr2->DbgPrint();
    DbgPut("; ");
    if (pExpr3)
        pExpr3->DbgPrint();
    DbgPut(") ");
    pStmt->DbgPrint();
}

void StmtJump::DbgPrint() {
    static const unordered_map<unsigned, string> m {
        {T_KCO, "continue"}, {T_KBR, "break"},
    };
    DbgPut(m.at(tp));
    DbgPut(" ; ");
}

void Decl::DbgPrint() {
    DbgPut("int ");
    for (auto i = li.begin(); i != li.end(); ++i) {
        if (i != li.begin())
            DbgPut(", ");
        DbgPut(i->first);
        DbgPut(' ');
        if (i->second) {
            DbgPut("= ");
            i->second->DbgPrint();
        }
    }
    DbgPut("; ");
}

void Expr::DbgPrint() {
    pExprAsg->DbgPrint();
    for (auto p : li) {
        DbgPut(", ");
        p->DbgPrint();
    }
}

void ExprAssig::DbgPrint() {
    static const unordered_map<unsigned, string> m {
        {T_ASG, "="},
        {T_AMU, "*="}, {T_ADI, "/="}, {T_AMO, "%="},
        {T_AAD, "+="}, {T_ASU, "-="},
        {T_ASL, "<<="}, {T_ASR, ">>="},
        {T_AAN, "&="}, {T_AXO, "^="}, {T_AOR, "|="},
    };
    for (auto &p : li) {
        p.first->DbgPrint();
        DbgPut(m.at(p.second));
        DbgPut(' ');
    }
    pExprCon->DbgPrint();
}

void ExprCondi::DbgPrint() {
    pExprLor->DbgPrint();
    for (auto &p : li) {
        DbgPut("? ");
        p.second->DbgPrint();
        DbgPut(": ");
        p.second->DbgPrint();
    }
}

void ExprLogor::DbgPrint() {
    pExprLan->DbgPrint();
    for (auto p : li) {
        DbgPut("|| ");
        p->DbgPrint();
    }
}

void ExprLogan::DbgPrint() {
    pExprBor->DbgPrint();
    for (auto p : li) {
        DbgPut("&& ");
        p->DbgPrint();
    }
}

void ExprBitor::DbgPrint() {
    pExprBxo->DbgPrint();
    for (auto p : li) {
        DbgPut("| ");
        p->DbgPrint();
    }
}

void ExprBitxo::DbgPrint() {
    pExprBan->DbgPrint();
    for (auto p : li) {
        DbgPut("^ ");
        p->DbgPrint();
    }
}

void ExprBitan::DbgPrint() {
    pExprEqu->DbgPrint();
    for (auto p : li) {
        DbgPut("& ");
        p->DbgPrint();
    }
}

void ExprEquty::DbgPrint() {
    static const unordered_map<unsigned, string> m {
        {T_EQU, "=="}, {T_NEQ, "!="},
    };
    pExprRel->DbgPrint();
    for (auto &p : li) {
        DbgPut(m.at(p.second));
        DbgPut(' ');
        p.first->DbgPrint();
    }
}

void ExprRelat::DbgPrint() {
    static const unordered_map<unsigned, string> m {
        {T_LES, "<"}, {T_GRE, ">"}, {T_LEQ, "<="}, {T_GEQ, ">="},
    };
    pExprSft->DbgPrint();
    for (auto &p : li) {
        DbgPut(m.at(p.second));
        DbgPut(' ');
        p.first->DbgPrint();
    }
}

void ExprShift::DbgPrint() {
    static const unordered_map<unsigned, string> m {
        {T_SHL, "<<"}, {T_SHR, ">>"},
    };
    pExprAdd->DbgPrint();
    for (auto &p : li) {
        DbgPut(m.at(p.second));
        DbgPut(' ');
        p.first->DbgPrint();
    }
}

void ExprAddit::DbgPrint() {
    static const unordered_map<unsigned, string> m {
        {T_ADD, "+"}, {T_SUB, "-"},
    };
    pExprMul->DbgPrint();
    for (auto &p : li) {
        DbgPut(m.at(p.second));
        DbgPut(' ');
        p.first->DbgPrint();
    }
}

void ExprMulti::DbgPrint() {
    static const unordered_map<unsigned, string> m {
        {T_MUL, "*"}, {T_DIV, "/"}, {T_MOD, "%"},
    };
    pExprUry->DbgPrint();
    for (auto &p : li) {
        DbgPut(m.at(p.second));
        DbgPut(' ');
        p.first->DbgPrint();
    }
}

void ExprUnary::DbgPrint() {
    static const unordered_map<unsigned, string> m {
        {T_ADD, "+"}, {T_SUB, "-"}, {T_NEG, "!"}, {T_INC, "++"}, {T_DEC, "--"},
    };
    for (auto tp : li) {
        DbgPut(m.at(tp));
        DbgPut(' ');
    }
    pExprPfx->DbgPrint();
}

void ExprPofix::DbgPrint() {
    static const unordered_map<unsigned, string> m {
        {T_INC, "++"}, {T_DEC, "--"},
    };
    pExprPri->DbgPrint();
    auto i = lie.begin();
    for (auto tp : li) {
        if (tp == T_LPA) {
            DbgPut('(');
            (*i++)->DbgPrint();
            DbgPut(')');
        }
        else
            DbgPut(m.at(tp));
        DbgPut(' ');
    }
}

void ExprPrima::DbgPrint() {
    if (tp == T_LPA) {
        DbgPut("( ");
        pExpr->DbgPrint();
        DbgPut(')');
    }
    else if (tp == T_CON)
        DbgFmt("%d", n);
    else
        DbgPut(s);
    DbgPut(' ');
}

#endif

// Main function. It's simple, uh hah~
int main() {
    pp0::Perform();
    pp1::Perform();
    pp2::Perform();
    pp3::Perform();
#ifndef NDEBUG
    DbgPrint(pp3::li);
#endif
    pp4::Perform();
#ifndef NDEBUG
    DbgPrint(pp4::pStmtComp);
#endif
    pp5::Perform();
    pp6::Perform();
    return 0;
}

