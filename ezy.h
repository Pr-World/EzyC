#include <var/var.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#define none var_default()

#define char(x) char_val(x)
#define boolean(x) bool_val(x)
#define string char *

#if __STRICT_ANSI__
# define ARG_LENGTH(...) __builtin_choose_expr(sizeof (#__VA_ARGS__) == 1,  \
        0,                                                                  \
        ARG_LENGTH__(__VA_ARGS__))
#else /* !__STRICT_ANSI__ */
# define ARG_LENGTH(...) ARG_LENGTH__(__VA_ARGS__)
#endif /* !__STRICT_ANSI__ */

# define ARG_LENGTH__(...) ARG_LENGTH_(,##__VA_ARGS__,                         \
    63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45,\
    44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26,\
    25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6,\
    5, 4, 3, 2, 1, 0)
# define ARG_LENGTH_(_, _63, _62, _61, _60, _59, _58, _57, _56, _55, _54, _53, \
    _52, _51, _50, _49, _48, _47, _46, _45, _44, _43, _42, _41, _40, _39, _38, \
    _37, _36, _35, _34, _33, _32, _31, _30, _29, _28, _27, _26, _25, _24, _23, \
    _22, _21, _20, _19, _18, _17, _16, _15, _14, _13, _12, _11, _10, _9, _8,   \
    _7, _6, _5, _4, _3, _2, _1, Count, ...) Count

// ezyC
#define main() int main()
#define clear_all_vars() var_mem_clear_end()
#define die_wrap(v, x, arg...) x
#define die(arg...) clear_all_vars(); return die_wrap(1, ## arg, 0, 0)

// wrap needed for different types, The person who made _Generic was truly awesome! :) hats off
#define int(x) intwrap(x)(x)
#define intwrap(x) _Generic((x),\
    default: 0,\
    int: intvar,\
    double: getintdbl,\
    var: var_get_int\
\
)

double int_to_dbl(int v) { return (double)v; }

// wrap needed for different types!
#define float(x) floatwrap(x)
#define floatwrap(x) _Generic((x),\
    default: double(x),\
    float: floatvar,\
    double: floatvar,\
    var: var_get_float\
\
)

double int_to_double(int x) {
    return (double)x;
}

// wrap needed for different types!
#define double(x) doublewrap(x)(x)
#define doublewrap(x) _Generic((x),\
    default: 0.0f,\
    int: int_to_double,\
    float: doublevar,\
    double: doublevar,\
    var: var_get_double\
\
)

inline string __unsup_str() { return "\nUnsupported type for `str()`\n"; }

// wrap needed for different types!
#define str(x) strwrap(x)(x)
#define strwrap(x) _Generic((x), \
    default: (*__unsup_str),\
    short: __long_to_str,\
    int: __long_to_str,\
    long: __long_to_str,\
    unsigned short: __ulong_to_str,\
    unsigned int: __ulong_to_str,\
    unsigned long: __ulong_to_str,\
    string: str_val,\
    const string: str_val,\
    var: var_get_string\
\
)

#define is_type(x, type) _Generic(x, type: true, default: false)
#define let auto

#define val(x, type) (*((type *)x.mem))
#define strval(x) get_strval(x)

#define clear(variable) variable = var_clear(variable)
#define new(variable) variable = var_clear(variable); variable

#define set(variable, value) variable = setwrap(value)(variable, value)
#define setwrap(x) _Generic((x),\
    default: var_set_custom,\
    short: var_set_short,\
    unsigned short: var_set_ushort,\
    int: var_set_int,\
    unsigned int: var_set_uint,\
    float: var_set_float,\
    double: var_set_double,\
    char: var_set_char,\
    char *: var_set_string,\
    const char *: var_set_string,\
    var: setsamevar\
\
)

#define print__sp printf(" ")

// printf wrappers for print
int print__int(int x) { return printf("%d", x); }
int print__uint(unsigned int x) { return printf("%u", x); }
int print__short(short x) { return printf("%hi", x); }
int print__ushort(unsigned short x) { printf("%hu", x); }
int print__long(long x) { return printf("%li", x); }
int print__ulong(unsigned long x) { return printf("%lu", x); }
int print__double(double x) { return printf("%.15g", x); }
int print__float(float x) { return print__double((double)x); }
int print__string(const string x) { printf("%s", x); }
int print__char(char x) { return printf("%c", x); }
int print__var(var x) { return var_print(x); }

int print__unknown() { return printf("\n`print()` doesn't recognize the type to print.\n"); }

// print function macros
#define print__wn(toPrint) printwrap(toPrint)(toPrint)
#define printwrap(x) _Generic((x),\
    default: (*print__unknown),\
    short: print__short,\
    unsigned short: print__ushort,\
    int: print__int,\
    unsigned int: print__uint,\
    long: print__long,\
    unsigned long: print__ulong,\
    float: print__float,\
    double: print__double,\
    char: print__char,\
    string: print__string,\
    const string: print__string,\
    var: print__var\
)

#define print___wn(v, p) ((v)?(print__wn(p)):(0))
// #define print__wn[n] \
(validate, a, b, c ...i, p, dummyarg...) if v: print__wn[n-1](v,a,b,c,...,h) && print__sp && print__wn(p) else 0

#define print__wn1(v,p,an...) print___wn(v, p)
#define print__wn2(v,a,p,an...) print___wn(v, a) && print__sp && print___wn(v, p)
#define print__wn3(v,a,b,p,an...) print__wn2(v,a,b) && print__sp && print___wn(v, p)
#define print__wn4(v,a,b,c,p,an...) print__wn3(v,a,b,c) && print__sp && print___wn(v,p)
#define print__wn5(v,a,b,c,d,p,an...) print__wn4(v,a,b,c,d) && print__sp && print___wn(v,p)
#define print__wn6(v,a,b,c,d,e,p,an...) print__wn5(v,a,b,c,d,e) && print__sp && print___wn(v,p)
#define print__wn7(v,a,b,c,d,e,f,p,an...) print__wn6(v,a,b,c,d,e,f) && print__sp && print___wn(v,p)
#define print__wn8(v,a,b,c,d,e,f,g,p,an...) print__wn7(v,a,b,c,d,e,f,g) && print__sp && print___wn(v,p)
#define print__wn9(v,a,b,c,d,e,f,g,h,p,an...) print__wn8(v,a,b,c,d,e,f,g,h) && print__sp && print___wn(v,p)
#define print__wn10(v,a,b,c,d,e,f,g,h,i,p,an...) print__wn9(v,a,b,c,d,e,f,g,h,i)&& print__sp && print__wn(v,p)

#define print_wn(args...) print__internal_wn(0, ## args)
#define print(args...) print__internal_wn(0, ## args); printf("\n")

#define print__internal_wn(a, args...)\
(( ARG_LENGTH(1 ,## args) == 1 )                                                   \
        ? (print__wn(""))                                                       \
:(( ARG_LENGTH(1, ## args) == 2 )                                                  \
        ? (print__wn1(ARG_LENGTH(1, ## args)-1 , ##args, 0, 0))                       \
:(( ARG_LENGTH(1, ## args) == 3 )                                                  \
        ? (print__wn2(ARG_LENGTH(1, ## args)-1 , ##args, 0, 0, 0))                    \
:(( ARG_LENGTH(1, ## args) == 4 )                                                  \
        ? (print__wn3(ARG_LENGTH(1, ## args)-1 , ##args, 0, 0, 0, 0))                 \
:(( ARG_LENGTH(1, ## args) == 5 )                                                  \
        ? (print__wn4(ARG_LENGTH(1, ## args)-1 , ##args, 0, 0, 0, 0, 0))             \
:(( ARG_LENGTH(1, ## args) == 6 )                                                  \
        ? (print__wn5(ARG_LENGTH(1, ## args)-1 , ##args, 0, 0, 0, 0, 0, 0))             \
:(( ARG_LENGTH(1, ## args) == 7 )                                                  \
        ? (print__wn6(ARG_LENGTH(1, ## args)-1 , ##args, 0, 0, 0, 0, 0, 0, 0))             \
:(( ARG_LENGTH(1, ## args) == 8 )                                                  \
        ? (print__wn7(ARG_LENGTH(1, ## args)-1 , ##args, 0, 0, 0, 0, 0, 0, 0, 0))             \
:(( ARG_LENGTH(1, ## args) == 9 )                                                  \
        ? (print__wn4(ARG_LENGTH(1, ## args)-1 , ##args, 0, 0, 0, 0, 0, 0, 0, 0, 0))             \
:(( ARG_LENGTH(1, ## args) == 10 )                                                  \
        ? (print__wn4(ARG_LENGTH(1, ## args)-1 , ##args, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0))             \
:(         print__wn3(1, "\nMore than 10 arguments in `print()` is not supported. you provided:",\
                        ARG_LENGTH(1, ## args)-1, "Arguments.\n", 0)\
)))))))))))

var setsamevar(var x, var y) {
    return x;
}

int getintdbl(double x) {
    return (int)x;
}

var intvar(int _val) {
    var v = var_default();
    v = var_set_int(v, _val);
    return v;
}

var floatvar(float _val) {
    var v = var_default();
    v = var_set_float(v, _val);
    return v;
}

var doublevar(double _val) {
    var v = var_default();
    v = var_set_double(v, _val);
    return v;
}

var str_val(char * str) {
    var v = var_default();
    v = var_set_string(v, str);
    return v;
}

var char_val(char c) {
    var v = var_default();
    v = var_set_char(v, c);
    return v;
}

var bool_val(bool _val) {
    var v = var_default();
    v = var_set_bool(v, _val);
    return v;
}

var input(string prompt)
{
    var ret;
    printf("%s", prompt);
    int c; //as getchar() returns `int`
    string tmp = malloc(sizeof(char)); //allocating memory

    tmp[0]='\0';

    for(int i=0; i<1000  && (c=getchar())!='\n' && c != EOF ; i++)
    {
        tmp = realloc(tmp, (i+2)*sizeof(char)); //reallocating memory
        tmp[i] = (char) c; //type casting `int` to `char`
        tmp[i+1] = '\0'; //inserting null character at the end
    }
    ret.mem = tmp;
    ret.size = strlen(tmp) + 1;
    ret.type = VAR_STRING;
    return ret;
}

string long_to_str(long v, bool uns)
{
    char ret[25];
    char * p = ret;
    if(uns)
        snprintf(ret, 25, "%lu", (unsigned long)v);
    else
        snprintf(ret, 25, "%ld", v);
    return p;
}

string double_to_str(double val)
{
    char ret[25];
    char * p = ret;
    snprintf(ret, 25, "%g", val);
    return p;
}

string char_to_str(char val)
{
    char ret[2] = {'\0'};
    char * p = ret;
    ret[0] = val;
    return p;
}

string get_strval(var x)
{
    switch(x.type)
    {
        case VAR_NONE: return "none";
        case VAR_BOOL: return var_get_bool(x)?"true":"false";
        case VAR_SHORT: return long_to_str((long)var_get_short(x), false);
        case VAR_USHORT: return long_to_str((long)var_get_ushort(x), false);
        case VAR_INT: return long_to_str((long)var_get_int(x), false);
        case VAR_UINT: return long_to_str((long)var_get_uint(x), false);
        case VAR_LONG: return long_to_str((long)var_get_long(x), false);
        case VAR_ULONG: return long_to_str((long)var_get_ulong(x), true);
        case VAR_FLOAT: return double_to_str((double)var_get_float(x));
        case VAR_DOUBLE: return double_to_str(var_get_double(x));
        case VAR_CHAR: return char_to_str(var_get_char(x));
        case VAR_STRING: return var_get_string(x);
        case VAR_CUSTOM: return "Custom";
        default: return "Unknown Value";
    }
}

string __ulong_to_str(unsigned long v) { return long_to_str(v, true); } 
string __long_to_str(long v) { return long_to_str(v, false); } 