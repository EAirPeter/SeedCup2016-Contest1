# 其他队都错只有我们队
## 编译和运行
* 编译
  * 使用GCC-5.1.0或者更高版本GCC进行编译。
  * 编译命令`g++ -o SeedCup2016.exe -std=c++14 -O2 -DNDEBUG SeedCup2016.cpp`。
* 运行
  * `Bin`文件夹中的`SeedCup2016.exe`是Windows平台下32位的可执行文件，如果需要其他平台下的可执行文件请自行编译。 
  * 因为本程序仅使用了标准库，所以本程序的代码可以在任何能够编译ISO C++14的平台下编译运行。
  * 本程序将从当前目录下的`input.txt`中读入需要处理的C语言代码并进行分析，然后会将运行顺序输出到当前目录下的`output.txt`。

## 程序功能
* 简单说就是题目要求支持的都支持了。还有一些其他的，具体见下文。
* 表达式
  * 支持一元后置操作符，包括：`++`、`--`。
  * 支持一元前置操作符，包括：`+`、`-`、`++`、`--`、`!`、`~`。
  * 支持二元算术操作符，包括：`+`、`-`、`*`、`/`、`%`。
  * 支持二元位运算操作符，包括：`<<`、`>>`、`&`、`^`、`|`。
  * 支持二元关系操作符，包括：`<`、`>`、`<=`、`>=`、`==`、`!=`。
  * 支持二元逻辑操作符，包括：`&&`、`||`。
  * 支持三元条件操作符：`e1 ? e2 : e3`。
  * 支持赋值操作符：`=`、`+=`、`-=`、`*=`、`/=`、`%=`、`<<=`、`>>=`、`&=`、`^=`、`|=`。
  * 支持逗号操作符：`e1, e2`。
* 语句
  * 支持复合语句。
  * 支持表达式语句或空语句。
  * 支持条件语句：if语句。
  * 支持迭代语句：while语句、do语句、for语句。
  * 支持跳转语句：continue语句、break语句。
* 声明
  * 支持声明及初始化。
* 需要注意的地方
  * 支持括号嵌套。
  * 支持复合赋值。
  * 逻辑运算结果真对应值`1`，假对应值`0`。
  * 字符串字面量转换为整数的结果是不确定的。比如：`"same" == "same"`不一定为真，`int c = "example";`中`c`的值不确定。
  * 字符串字面量中的转义序列会被忽略，换句话说就是不支持`\"`转义序列。
  * 常量仅支持十进制整数常量。
  * 函数调用仅支持`printf`函数。
  * 未显式初始化的变量会被隐式初始化为`0`。
  * 对于声明，只要存在至少一个初始化，就会输出其行号。
  * 对于表达式语句，无论是否影响变量值或程序执行顺序，只要不是空语句，就会输出其行号。
  * 对于while语句和do语句，每一轮循环都会输出`while ( e )`部分的行号。
  * 对于for语句，每一轮循环都会输出`for ( p1 ; p2 ; p3 )`部分的行号。
  * 输出一行若干个十进制正整数，代表按照顺序执行的行号，相邻两个整数用一个空格字符隔开，行末有一个换行符（保证行末没有空格字符）。
  * 如果输入文件为空，那么输出仅一个换行符。
  * 对于非法的输入，程序可能会崩溃且不给出错误信息。

## 对支持的输入的严格描述
### 输入文件
* _`input-text`_
  * _`block-item-list(opt)`_

### 语句
* _`statement`_
  * _`compound-statement`_
  * _`expression-statement`_
  * _`if-statement`_
  * _`while-statement`_
  * _`do-statement`_
  * _`for-statement`_
  * _`jump-statement`_

### 复合语句
* _`compound-statement`_
  * `{` _`block-item-list(opt)`_ `}`
* _`block-item-list`_
  * _`block-item`_
  * _`block-item-list`_ _`block-item`_
* _`block-item`_
  * _`declaration`_
  * _`statement`_

### 表达式语句与空语句
* _`expression-statement`_
  * _`expression(opt)`_ `;`

### 条件语句
* _`if-statement`_
  * `if` `(` _`expression`_ `)` _`statement`_
  * `if` `(` _`expression`_ `)` _`statement`_ `else` _`statement`_

### 迭代语句
* _`while-statement`_
  * `while` `(` _`expression`_ `)` _`statement`_
* _`do-statement`_
  * `do` _`statement`_ `while` `(` _`expression`_ `)` `;`
* _`for-statement`_
  * `for` `(` _`expression(opt)`_ `;` _`expression(opt)`_ `;` _`expression(opt)`_ `)` _`statement`_
  * `for` `(` _`declaration`_ _`expression(opt)`_ `;` _`expression(opt)`_ `)` _`statement`_

### 跳转语句
* _`jump-statement`_
  * `continue` `;`
  * `break` `;`

### 声明
* _`declaration`_
  * `int` _`init-declarator-list(opt)`_
* _`init-declarator-list`_
  * _`init-declarator`_
  * _`init-declarator-list`_ `init_declarator`
* _`init-declarator`_
  * _`identifier`_
  * _`identifier`_ `=` _`assignment-expression`_

### 表达式
* _`expression`_
  * _`assignment-expression`_
  * _`expression`_ `,` _`assignment-expression`_
* _`assignment-expression`_
  * _`conditional-expression`_
  * _`unary-expression`_ _`assignment-operator`_ _`assignment-expression`_
* _`assignment-operator`_
  * one of: `=` `*=` `/=` `%=` `+=` `-=` `<<=` `>>=` `&=` `^=` `|=`
* _`conditional-expression`_
  * _`logical-OR-expression`_
  * _`logical-OR-expression`_ `?` _`expression`_ : _`conditional-expression`_
* _`logical-OR-expression`_
  * _`logical-AND-expression`_
  * _`logical-OR-expression`_ `||` _`logical-AND-expression`_
* _`logical-AND-expression`_
  * _`inclusive-OR-expression`_
  * _`logical-AND-expression`_ `&&` _`inclusive-OR-expression`_
* _`inclusive-OR-expression`_
  * _`exclusive-OR-expression`_
  * _`inclusive-OR-expression`_ `|` _`exclusive-OR-expression`_
* _`exclusive-OR-expression`_
  * _`AND-expression`_
  * _`exclusive-OR-expression`_ `^` _`AND-expression`_
* _`AND-expression`_
  * _`equality-expression`_
  * _`AND-expression`_ `&` _`equality-expression`_
* _`equality-expression`_
  * _`relational-expression`_
  * _`equality-expression`_ `==` _`relational-expression`_
  * _`equality-expression`_ `!=` _`relational-expression`_
* _`relational-expression`_
  * _`shift-expression`_
  * _`relational-expression`_ `<` _`shift-expression`_
  * _`relational-expression`_ `>` _`shift-expression`_
  * _`relational-expression`_ `<=` _`shift-expression`_
  * _`relational-expression`_ `>=` _`shift-expression`_
* _`shift-expression`_
  * _`additive-expression`_
  * _`shift-expression`_ `<<` _`additive-expression`_
  * _`shift-expression`_ `>>` _`additive-expression`_
* _`additive-expression`_
  * _`additive-expression`_
  * _`additive-expression`_ `+` _`multiplicative-expression`_
  * _`additive-expression`_ `-` _`multiplicative-expression`_
* _`multiplicative-expression`_
  * _`unary-expression`_
  * _`multiplicative-expression`_ `*` _`unary-expression`_
  * _`multiplicative-expression`_ `/` _`unary-expression`_
  * _`multiplicative-expression`_ `%` _`unary-expression`_
* _`unary-expression`_
  * _`postfix-expression`_
  * `++` _`unary-expression`_
  * `--` _`unary-expression`_
  * _`unary-operator`_ _`unary-expression`_
* _`unary-operator`_
  * one of: `+` `-` `!` `~`
* _`postfix-expression`_
  * _`primary-expression`_
  * _`postfix-expression`_ `(` _`argument-expression-list(opt)`_ `)`
  * _`postfix-expression`_ `++`
  * _`postfix-expression`_ `--`
* _`argument-expresison-list`_
  * _`assignment-expression`_
  * _`argument-expresison-list`_ _`assignment-expression`_ 
* _`primary-expression`_
  * _`identifier`_
  * _`decimal-constant`_
  * _`string-literal`_
  * `(` _`expression`_ `)`

### 标识符
* _`identifier`_
  * _`nondigit`_
  * _`identifier`_ _`nondigit`_
  * _`identifier`_ _`digit`_
* _`nondigit`_
  * one of: `_` `a` `b` ... `z` `A` `B` ... `Z`
* _`digit`_
  * one of: `0` `1` `2` `3` `4` `5` `6` `7` `8` `9`

### 常量
* _`decimal-constant`_
  * _`nonzero-digit`_
  * _`decimal-constant`_ _`digit`_
* _`nonzero-digit`_
  * one of: `1` `2` `3` `4` `5` `6` `7` `8` `9`

### 字符串字面量
* _`string-literal`_
  * `"` _`s-char-sequence(opt)`_ `"`
* _`s-char-sequence`_
  * _`s-char`_
  * _`s-char-sequence`_ _`s-char`_
* _`s-char`_
  * any member of the source character set except the double-quote `"`

## 数据结构和类型
* 基础数据结构和类型
  * `struct Pos`：表示文件中一个字符或者Token的位置。
  * `struct Char`：记录字符和它的位置。
  * `enum TokenType`：表示一个Token的类型。
  * `struct Token`：表示一个Token。
  * `struct Scope`：表示一个作用域。
* Syntax相关的数据结构和类型
  * `struct BlockItem`：表示一个_`block-item`_（可以执行）。
  * `struct Stmt`：表示一个_`statement`_。
  * `struct StmtComp`：表示一个_`compound-statement`_。
  * `struct StmtExpr`：表示一个_`expression-statement`_。
  * `struct StmtIf`：表示一个_`if-statement`_。
  * `struct StmtWhile`：表示一个_`while-statement`_。
  * `struct StmtDo`：表示一个_`do-statement`_。
  * `struct StmtFor`：表示一个_`for-statement`_。
  * `struct StmtJump`：表示一个_`jump-statement`_。
  * `struct Decl`：表示一个_`declaration`_。
  * `struct ExprBase`：所有表达式的基类（可以求值）。
  * `struct Expr`：表示一个_`expression`_。
  * `struct ExprAssig`：表示一个_`assignment-expression`_。
  * `struct ExprCondi`：表示一个_`conditional-expression`_。
  * `struct ExprLogor`：表示一个_`logical-OR-expression`_。
  * `struct ExprLogan`：表示一个_`logical-AND-expression`_。
  * `struct ExprBitor`：表示一个_`inclusive-OR-expression`_。
  * `struct ExprBitxo`：表示一个_`exclusive-OR-expression`_。
  * `struct ExprBitan`：表示一个_`AND-expression`_。
  * `struct ExprEquty`：表示一个_`equality-expression`_。
  * `struct ExprRelat`：表示一个_`relational-expression`_。
  * `struct ExprShift`：表示一个_`shift-expression`_。
  * `struct ExprAddit`：表示一个_`additive-expression`_。
  * `struct ExprMulti`：表示一个_`multiplicative-expression`_。
  * `struct ExprUnary`：表示一个_`unary-expression`_。
  * `struct ExprPofix`：表示一个_`postfix-expression`_。
  * `struct ExprPrima`：表示一个_`primary-expression`_。

## 程序结构
* 关于主要流程
  * 由7个Processing Phase(PP)组成
    * PP0：初始化与字符类型相关的函数所需要的资源。
    * PP1：从`input.txt`中读取所有字符并记录其位置信息。
    * PP2：将所有注释替换为一个空格字符。
    * PP3：将字符序列转换为Token序列。
    * PP4：Syntax分析，建立数据结构。
    * PP5：模拟程序执行。
    * PP6：将结果输出至`output.txt`。
  * 每一个PP结束后都会释放后面不会用到的资源。
* 关于Syntax分析
  * 首先在整个Token序列两端分别加上`{` `}`，然后在最前端加上`int printf;`，接下来再在两端分别加上`{` `}`，最后把新的Token序列当做一个_`compound-statement`_来解析。
  * 每一个`StmtComp`或`StmtFor`同时也是一个变量作用域，因此它们都有一个`Scope`基类。
  * 对于每一个元素都有一个对应的Parse函数，从而实现对整个Token序列的解析。
  * Parse函数是按照上文中“对支持的输入的严格描述”来调用的。
* 关于模拟程序执行
  * 每一个`Scope`中都有一个`std::unordered_map<std::string, int>`来实现变量的“标识符-值”的映射。
  * 对于_`identifier`_的查找是递归进行的，首先在当前`Scope`中查找，如果不存在则在父级`Scope`中查找。因此前文中“伪造”一个`printf`的原因就是输入代码本身并没有给出`printf`的声明。
  * 语句、声明的执行是通过`BlockItem::Exec()`来实现的，这是一个虚函数，通过C++多态性来实现不同类型的语句、声明的执行。
  * 表达式的求值是通过`ExprBase::Eval()`来实现的，这同样是一个虚函数，同样通过C++多态性来实现对不同类型的表达式的求值。
  * 处理函数调用仅对`(` `)`之间的表达式进行了求值，并没有实际调用函数。
  * 记录行号的时候如果当前行号与上次记录的行号相同，那么这次记录就会被忽略。
* 关于调试代码
  * 如果`NDEBUG`宏没被定义，那么程序的会将答案输出到标准输出流而不是文件中，然而输入仍然来自文件。 
  * 代码中还有一大堆调试用的函数`DbgOut`、`DbgFmt`、`DbgPrint`和`Base::DbgPrint`，作用是将传入的对象的信息输出到标准错误流，当然它们也仅存在于`NDEBUG`宏未定义的情况下。
  * 请慎用`void DbgPrint(const std::list<Char> &)`（因为它会产生很大量的输出），因此提交的代码中没有保留调用这个函数的语句。

## 一些支持的样例
### 例1（求最大公约数）
```
int x, y;
int a = x = 36;
int b = y = 120;
for (int c; ; a = b, b = c) {
    if (!b)
        break;
    c = a % b;
}
if (a == 12)
    printf("gcd of %d and %d is %d\n", x, y, a);
else
    printf("something wrong\n");
```

### 例2（利用矩阵求Fibonacci数列第n项）
```
int c11 = 1, c12 = 0, c21 = 0, c22 = 1;
int d11 = 0, d12 = 1, d21 = 1, d22 = 1;
int n = 40;
for (int m = n; m; m >>= 1) {
    if (m & 1) {
        int e11 = c11 * d11 + c12 * d21;
        int e12 = c11 * d12 + c12 * d22;
        int e21 = c21 * d11 + c22 * d21;
        int e22 = c21 * d12 + c22 * d22;
        c11 = e11, c12 = e12, c21 = e21, c22 = e22;
    }
    int f11 = d11 * d11 + d12 * d21;
    int f12 = d11 * d12 + d12 * d22;
    int f21 = d21 * d11 + d22 * d21;
    int f22 = d21 * d12 + d22 * d22;
    d11 = f11, d12 = f12, d21 = f21, d22 = f22;
}
if (c12 != 102334155)
    printf("something wrong\n");
else
    printf("the %dth fibonacci number is %d\n", n, c12);
```

### 例3（素数统计）
```
int n;
int m = n = 100;
int p = 0;
do {
    int i = 2;
    while (i * i <= m) {
        if (!(m % i))
            break;
        ++i;
    }
    p += i * i > m;
} while (--m > 1);
if (p != 25/* there are 25 primes between 1 and 100 */)
    printf("something wrong");
else
    printf("there are %d primes between 1 and %d\n", p, n);
```

## 参考资料
* 2016“亚马逊•种子杯” 编程PK赛-初赛题目
* [ISO/IEC JTC1-SC22-WG14-N1570](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf)
* [cppreference.com](http://en.cppreference.com/w/)
