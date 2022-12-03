#include <stdio.h>
#include "var.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

/// @brief array of strings mapping to string versions of typecode
const char * VAR_STR_TYPES[] = {
    "none",
    "boolean",
    "character",
    "short",
    "unsigned short",
    "int",
    "unsigned int",
    "long",
    "unsigned long",
    "float",
    "double",
    "string",
    "other (custom type)"
};

typedef struct var_count {
    struct var_count * next;
    void * var_pointer;
} var_count;

var_count VAR_TOTAL = {NULL, NULL};

void var__register_alloc( void * vp) {
    if(vp == NULL) { return; }
    // register the allocation
    if ( VAR_TOTAL.var_pointer == NULL ) {
        VAR_TOTAL.var_pointer = vp;
    } else if ( VAR_TOTAL.next == NULL ) {
        var_count * v = calloc(1, sizeof(var_count));
        v -> next = NULL;
        v -> var_pointer = vp;
        VAR_TOTAL.next = v;
    } else {
        var_count  *v = calloc(1, sizeof(var_count)), *p = &VAR_TOTAL;
        v -> next = NULL;
        v -> var_pointer = vp;
        while(p -> var_pointer) {
            if ( p -> next == NULL) {
                // last element
                p -> next = v;
                break;
            }
            p = p -> next;
        }
    }
}

void var__deregister_alloc( void * vp ) {
    if(vp == NULL) { return; }
    // attempt to deregister the allocation, else discard silently
    var_count *v = &VAR_TOTAL;
    while(v -> var_pointer ) {
        if ( v -> next == NULL ) {
            break;
        }
        if ( v -> next -> var_pointer == vp) {
            // tmp here
            var_count * tmp = v -> next;
            // deregistered here
            v -> next = v -> next -> next;
            // freed here
            free(tmp -> var_pointer);
            free(tmp);
            break;
        } else {
            v = v -> next;
        }
    }
}

/// @brief to be used in end of program, to clear all variables which are left to be cleared.
void var_mem_clear_end() {
    // all clear then return
    if ( VAR_TOTAL.var_pointer == NULL ) { return; }
    var_count * v = VAR_TOTAL.next;
    var_count * tmp;
    // free all unfreed variables
    while ( v && v -> var_pointer) {
        tmp = v -> next;
        free(v -> var_pointer);
        free(v);
        v = tmp;
    }
    // finally free the base
    if ( VAR_TOTAL.var_pointer != NULL ) {
        free(VAR_TOTAL.var_pointer);
        VAR_TOTAL.var_pointer = NULL;
    }
}

/// @brief used to get the count of non cleared variable
/// @return no of variables that are not cleared.
size_t var_not_freed_count() {
    size_t count = 0;
    var_count *v = &VAR_TOTAL;
    while ( v && v -> var_pointer) {
        count++;
        v = v -> next;
    }
    return count;
}

// -------------------------------- Flags ----------------------------------------------

/// @brief throw / show errors or not.
bool VAR_ERROR_THROW = true;

/// @brief throw / show warnings or not.
bool VAR_WARN_THROW = true;

/// @brief if warning should be taken as error
bool VAR_WARN_EQ_ERROR = false;

/// @brief exit on error.
bool VAR_EXIT_ON_ERROR = false;



/// @brief function to toggle VAR_WARN_THROW flag [ not accessible directly ].
/// @param val the value to set (in boolean)
void var_warn_throw(bool val) { VAR_WARN_THROW = val; }

/// @brief function to toggle VAR_ERROR_THROW flag [ not accessible directly ].
/// @param val the value to set (in boolean)
void var_error_throw(bool val) { VAR_ERROR_THROW = val; }

/// @brief function to toggle VAR_WARN_EQ_ERROR flag [ not accessible directly ].
/// @param val the value to set (in boolean)
void var_warn_eq_error(bool val) { VAR_WARN_EQ_ERROR = val; }

/// @brief function to toggle VAR_EXIT_ON_ERROR flag [ not accessible directly ].
/// @param val the value to set (in boolean)
void var_exit_on_error(bool val) { VAR_EXIT_ON_ERROR = val; }

// -------------------------------- Internal Functions ----------------------------------------------

/// @brief Error raising function for internal use.
/// @param func the function in which error occured.
/// @param msg the error message
/// @param errcode the errorcode
/// @return true if error is displayed, false if not. 
/// [ Note: Program directly exits with `errcode` as return value if VAR_EXIT_ON_ERROR is set ]
bool var__raise(const char * func, const char * msg, enum VAR_ERRORS errcode)
{
    // if error throw is set and there is  a valid errcode
    if ( VAR_ERROR_THROW && (errcode > VAR_NO_ERROR && errcode <= VAR_UNKNOWN_ERROR ) ) {
        // throw the error to stderr
        fprintf(
            stderr,
            "\n----- Var Error -----\nErrcode: %d\nin function `%s`: %s", errcode, func, msg
        );
        if ( VAR_EXIT_ON_ERROR ) { exit(errcode); }
        return true;
    }
    return false;
}

/// @brief memory error raising for internal use
/// @param size size for which memory was to be allocated.
/// @param func function which caused the memory allocation to happen.
void var__mem_error(size_t size, const char * func)
{
    if ( var__raise(func, "Couldn't allocate memory for size", VAR_MEMORY_ERROR))
    {
        fprintf(stderr, ": %lu\n", size);
    }
}

/// @brief var__typecheck function for internal use
/// @param _var the var variable to check
/// @param type type to check
/// @return true if warning was displayed, else false.
bool var__typecheck (var _var, enum VAR_TYPES type) {
    // if the type is not within type range
    if ( _var.type < VAR_NONE || _var.type > VAR_CUSTOM ) {
        if ( var__raise("var_typecheck", "Invalid type, type is corrupted.", VAR_TYPE_ERROR) ) {
            fprintf(stderr, " type `%d` is not within ( %d >= type >= %d )\n", _var.type, VAR_NONE, VAR_CUSTOM);
        }
        return true;
    }
    // if type are same or if warn throw is not set, return false
    if (!VAR_WARN_THROW || _var.type == type) {
        return false;
    }
    // output the warning to stderr
    fprintf(
        stderr,
        "\n----- Var Warning ------\n %s : `%s` [typecode : %d ] from `%s` [typecode : %d ]\n",
        "Type mismatch while getting", VAR_STR_TYPES[type], type, var_get_str_type(_var), _var.type
    );
    return true;
}

/// @brief memory allocator for internal use
/// @param size size to allocate
/// @return pointer to the calloc'd memory.
void * var__mem_alloc(size_t size)
{
    void * vp = calloc(1, size);
    // register allocation
    var__register_alloc(vp);
    return vp;
}

/// @brief memory refresher for internal use
/// @param _var variable to refresh memory
/// @param size new size of memory
/// @param func function name which is requesting to refresh
/// @return the var variable with refreshed memory.
var var__mem_refresh(var _var, size_t size, const char * func)
{
    if (_var.mem!=NULL) {
        // deregister allocation
        var__deregister_alloc(_var.mem);
        free(_var.mem);
    }
    _var.mem = size ? var__mem_alloc(size): NULL;
    _var.size = (_var.mem != NULL) ? size : 0;
    _var.type = VAR_NONE;
    if(!_var.size && size) {
        var__mem_error(size, func);
    }
    return _var;
}

// -------------------------------- External Functions ----------------------------------------------

/// @brief get the string representation of type
/// @param _var variable to get type from
/// @return the type of variable as string
const char * var_get_str_type(var _var)
{
    return VAR_STR_TYPES[_var.type];
}

/// @brief Returns a null, but initialized var variable
/// @return initialized var variable
var var_default() {
    var _var;
    _var.size = 0;
    _var.mem = NULL;
    _var.type = VAR_NONE;
    return _var;
}

/// @brief Changes size of the var variabled [ doesn't affect content as long as it fits ]
/// @param _var the variable to change size.
/// @param size the size to change to. [ can't be zero ]
/// @return the var of given size.
var var_change_size(var _var, size_t size)
{
    _var.mem = realloc(_var.mem, size);
    return _var;
}

// ----------------- Set Functions ----------------

/// @brief set the value to boolean
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_bool(var _var, bool val)
{
    _var = var__mem_refresh(_var, sizeof(bool), "var_set_bool");
    // copy only if size of _var is not zero else do nothing
    _var.size ? memcpy(_var.mem, &val, sizeof(bool)) : 0;
    _var.type = VAR_BOOL;
    return _var;
}

/// @brief set the value to char
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_char(var _var, char val)
{
    _var = var__mem_refresh(_var, sizeof(char), "var_set_char");
    // copy only if size of _var is not zero else do nothing
    _var.size ? memcpy(_var.mem, &val, sizeof(char)) : 0;
    _var.type = VAR_CHAR;
    return _var;
}

/// @brief set the value to short
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_short(var _var, short val)
{
    _var = var__mem_refresh(_var, sizeof(short), "var_set_short");
    // copy only if size of _var is not zero else do nothing
    _var.size ? memcpy(_var.mem, &val, sizeof(short)) : 0;
    _var.type = VAR_SHORT;
    return _var;
}

/// @brief set the value to unsigned short
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_ushort(var _var, unsigned short val)
{
    _var = var__mem_refresh(_var, sizeof(unsigned short), "var_set_ushort");
    // copy only if size of _var is not zero else do nothing
    _var.size ? memcpy(_var.mem, &val, sizeof(unsigned short)) : 0;
    _var.type = VAR_USHORT;
    return _var;
}

/// @brief set the value to int
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_int(var _var, int val)
{
    _var = var__mem_refresh(_var, sizeof(int), "var_set_int");
    // copy only if size of _var is not zero else do nothing
    _var.size ? memcpy(_var.mem, &val, sizeof(int)) : 0;
    _var.type = VAR_INT;
    return _var;
}

/// @brief set the value to unsigned int
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_uint(var _var, unsigned int val)
{
    _var = var__mem_refresh(_var, sizeof(unsigned int), "var_set_uint");
    // copy only if size of _var is not zero else do nothing
    _var.size ? memcpy(_var.mem, &val, sizeof(unsigned int)) : 0;
    _var.type = VAR_UINT;
    return _var;
}

/// @brief set the value to long
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_long(var _var, long val)
{
    _var = var__mem_refresh(_var, sizeof(long), "var_set_long");
    // copy only if size of _var is not zero else do nothing
    _var.size ? memcpy(_var.mem, &val, sizeof(long)) : 0;
    _var.type = VAR_LONG;
    return _var;
}

/// @brief set the value to unsigned long
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_ulong(var _var, unsigned long val)
{
    _var = var__mem_refresh(_var, sizeof(unsigned long), "var_set_ulong");
    // copy only if size of _var is not zero else do nothing
    _var.size ? memcpy(_var.mem, &val, sizeof(unsigned long)) : 0;
    _var.type = VAR_ULONG;
    return _var;
}

/// @brief set the value to float
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_float(var _var, float val)
{
    _var = var__mem_refresh(_var, sizeof(float), "var_set_float");
    // copy only if size of _var is not zero else do nothing
    _var.size ? memcpy(_var.mem, &val, sizeof(float)) : 0;
    _var.type = VAR_FLOAT;
    return _var;
}

/// @brief set the value to double
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_double(var _var, double val)
{
    _var = var__mem_refresh(_var, sizeof(double), "var_set_double");
    // copy only if size of _var is not zero else do nothing
    _var.size ? memcpy(_var.mem, &val, sizeof(double)) : 0;
    _var.type = VAR_DOUBLE;
    return _var;
}

/// @brief set the value to string
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_string(var _var, char * val)
{
    _var = var__mem_refresh(_var, strlen(val)+1, "var_set_string");
    // copy only if size of _var is not zero else do nothing
    _var.size ? memcpy(_var.mem, val, strlen(val)+1) : 0;
    _var.type = VAR_STRING;
    return _var;
}

/// @brief clear the variable, i.e set it to none
/// @param _var the variable to clear
/// @return the cleared variable
var var_clear(var _var)
{
    _var = var__mem_refresh(_var, 0, "var_clear");
    _var.type = VAR_NONE;
    return _var;
}

/// @brief set the value to custom type value
/// @param _var the var variable to set to
/// @param valPointer the pointer of the custom type value
/// @param size the size of the custom type variable 
/// @return the var variable with value of custom type
var var_set_custom(var _var, void * valPointer, size_t size) {
    _var = var__mem_refresh(_var, size, "var_set_custom");
    // copy only if size of _var is not zero else do nothing
    _var.size ? memcpy(_var.mem, valPointer, size) : 0;
    return _var;
}

//----------------- Get Functions ----------------

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as a boolean
bool var_get_bool(var _var)
{
    // type mismatch check
    if ( var__typecheck(_var, VAR_BOOL) && VAR_WARN_EQ_ERROR ) {
        var__raise(
            "var_get_bool", "All warnings treated as errors, Error produced due to type mismatch.",
            VAR_WARN_ERROR
        );
    }
    // return the value
    return *( (bool*) _var.mem );
}

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as a char
char var_get_char(var _var)
{
    // type mismatch check
    if ( var__typecheck(_var, VAR_CHAR) && VAR_WARN_EQ_ERROR ) {
        var__raise(
            "var_get_char", "All warnings treated as errors, Error produced due to type mismatch.",
            VAR_WARN_ERROR
        );
    }
    // return the value
    return *( (char*) _var.mem );
}

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as a short
short var_get_short(var _var)
{
    // type mismatch check
    if ( var__typecheck(_var, VAR_SHORT) && VAR_WARN_EQ_ERROR ) {
        var__raise(
            "var_get_short", "All warnings treated as errors, Error produced due to type mismatch.",
            VAR_WARN_ERROR
        );
    }
    // return the value
    return *( (short*) _var.mem );
}

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as unsigned short
unsigned short var_get_ushort(var _var)
{
    // type mismatch check
    if ( var__typecheck(_var, VAR_USHORT) && VAR_WARN_EQ_ERROR ) {
        var__raise(
            "var_get_ushort", "All warnings treated as errors, Error produced due to type mismatch.",
            VAR_WARN_ERROR
        );
    }
    // return the value
    return *( (unsigned short*) _var.mem );
}

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as int
int var_get_int(var _var)
{
    // type mismatch check
    if ( var__typecheck(_var, VAR_INT) && VAR_WARN_EQ_ERROR ) {
        var__raise(
            "var_get_int", "All warnings treated as errors, Error produced due to type mismatch.",
            VAR_WARN_ERROR
        );
    }
    // return the value
    return *( (int*) _var.mem );
}

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as unsigned int
unsigned int var_get_uint(var _var)
{
    // type mismatch check
    if ( var__typecheck(_var, VAR_UINT) && VAR_WARN_EQ_ERROR ) {
        var__raise(
            "var_get_uint", "All warnings treated as errors, Error produced due to type mismatch.",
            VAR_WARN_ERROR
        );
    }
    // return the value
    return *( (unsigned int*) _var.mem );
}

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as long
long var_get_long(var _var)
{
    // type mismatch check
    if ( var__typecheck(_var, VAR_LONG) && VAR_WARN_EQ_ERROR ) {
        var__raise(
            "var_get_long", "All warnings treated as errors, Error produced due to type mismatch.",
            VAR_WARN_ERROR
        );
    }
    // return the value
    return *( (long*) _var.mem );
}

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as unsigned long
unsigned long var_get_ulong(var _var)
{
    // type mismatch check
    if ( var__typecheck(_var, VAR_ULONG) && VAR_WARN_EQ_ERROR ) {
        var__raise(
            "var_get_ulong", "All warnings treated as errors, Error produced due to type mismatch.",
            VAR_WARN_ERROR
        );
    }
    // return the value
    return *( (unsigned long*) _var.mem );
}

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as a float
float var_get_float(var _var)
{
    // type mismatch check
    if ( var__typecheck(_var, VAR_FLOAT) && VAR_WARN_EQ_ERROR ) {
        var__raise(
            "var_get_float", "All warnings treated as errors, Error produced due to type mismatch.",
            VAR_WARN_ERROR
        );
    }
    // return the value
    return *( (float*) _var.mem );
}

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as a double
double var_get_double(var _var)
{
    // type mismatch check
    if ( var__typecheck(_var, VAR_DOUBLE) && VAR_WARN_EQ_ERROR ) {
        var__raise(
            "var_get_double", "All warnings treated as errors, Error produced due to type mismatch.",
            VAR_WARN_ERROR
        );
    }
    // return the value
    return *( (double*) _var.mem );
}

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as a string
char * var_get_string(var _var)
{
    // type mismatch check
    if ( var__typecheck(_var, VAR_STRING) && VAR_WARN_EQ_ERROR ) {
        var__raise(
            "var_get_string", "All warnings treated as errors, Error produced due to type mismatch.",
            VAR_WARN_ERROR
        );
    }
    // return the value
    return ( char * )( _var.mem );
}

/// @brief prints the content of var as necessary
/// @param _var the var _var to print content
/// @return the value returned by printf when printing the content.
int var_print(var _var)
{
    switch ( _var.type )
    {
        case VAR_NONE: return printf("none");
        case VAR_BOOL: return printf("%s", var_get_bool(_var) ? "true" : "false");
        case VAR_CHAR: return printf("%c", var_get_char(_var));
        case VAR_SHORT: return printf("%hi", var_get_short(_var));
        case VAR_USHORT: return printf("%hu", var_get_ushort(_var));
        case VAR_INT: return printf("%d", var_get_int(_var));
        case VAR_UINT: return printf("%u", var_get_uint(_var));
        case VAR_LONG: return printf("%ld", var_get_long(_var));
        case VAR_ULONG: return printf("%lu", var_get_ulong(_var));
        case VAR_FLOAT: return printf("%.15g", var_get_float(_var));
        case VAR_DOUBLE: return printf("%.15g", var_get_double(_var));
        case VAR_STRING: return printf("%s", var_get_string(_var));
        case VAR_CUSTOM: return printf("var_custom_variable");
        default: return var__raise("var_print", "Invalid type to print.", VAR_TYPE_ERROR);
    }
}

/// @brief debug print of var variable
/// @param _var the variable to debug print
/// @return the value returned by var_print when printing the content
int var_debug_print(var _var)
{
    if ( _var.type < VAR_NONE || _var.type > VAR_CUSTOM ) {
        return var__raise("var_debug_print", "Invalid type to print.", VAR_TYPE_ERROR);
    }
    printf("\n{ typecode = %d, type = `%s`, size = `%lu`, content = `", _var.type, var_get_str_type(_var), _var.size);
    int ret = var_print(_var);
    printf("` }\n");
    return ret;
}