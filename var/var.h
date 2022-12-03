#if !defined(VAR_H_INCL)
#define VAR_H_INCL

#include <stdbool.h>
#include <stddef.h>

/// @brief enum containing the type which can be represented in var
enum VAR_TYPES {
    VAR_NONE,
    /// @brief var_type indicating the type is boolean
    VAR_BOOL,
    /// @brief var_type indicating the type is character
    VAR_CHAR,
    /// @brief var_type indicating the type is short
    VAR_SHORT,
    /// @brief var_type indicating the type is unsigned short
    VAR_USHORT,
    /// @brief var_type indicating the type is int
    VAR_INT,
    /// @brief var_type indicating the type is unsigned int
    VAR_UINT,
    /// @brief var_type indicating the type is long
    VAR_LONG,
    /// @brief var_type indicating the type is unsigned long
    VAR_ULONG,
    /// @brief var_type indicating the type is float
    VAR_FLOAT,
    /// @brief var_type indicating the type is double
    VAR_DOUBLE,
    /// @brief var_type indicating the type is string
    VAR_STRING,
    /// @brief var_type indicating the type is a custom type
    VAR_CUSTOM
};


/// @brief the actual var definition
typedef struct var {
    /// @brief memory where the data is stored.
    void * mem;
    /// @brief current size of allocated memory
    size_t size;
    /// @brief curremt type of data stored in var
    enum VAR_TYPES type;
} var;

enum VAR_ERRORS {
    /// @brief var_error indicating no error
    VAR_NO_ERROR,
    /// @brief var_error indicating memory error
    VAR_MEMORY_ERROR,
    /// @brief var error indicating warning error [meaning that warn is equal to error & warning was produced]
    VAR_WARN_ERROR,
    /// @brief var error indicating type error.
    VAR_TYPE_ERROR,
    /// @brief var error indicating that some variables are not cleared to free memory.
    VAR_NOT_CLEARED,
    /// @brief var error indicating unknown error
    VAR_UNKNOWN_ERROR
};


// ------------------------------ Function Definitions -----------------------------------

/// @brief get the string representation of type
/// @param _var variable to get type from
/// @return the type of variable as string
const char * var_get_str_type(var _var);

/// @brief to be used in end of program, to clear all variables which are left to be cleared.
void var_mem_clear_end();

/// @brief used to get the count of non cleared variable
/// @return no of variables that are not cleared.
size_t var_not_freed_count();

/// @brief Returns a null, but initialized var variable
/// @return initialized var variable
var var_default();

/// @brief prints the content of var as necessary
/// @param _var the var _var to print content
/// @return the value returned by printf when printing the content.
int var_print(var _var);

/// @brief debug print of var variable
/// @param _var the variable to debug print
/// @return the value returned by var_print when printing the content
int var_debug_print(var _var);

/// @brief Changes size of the var variabled [ doesn't affect content as long as it fits ]
/// @param _var the variable to change size.
/// @param size the size to change to. [ can't be zero ]
/// @return the var of given size.
var var_change_size(var _var, size_t size);

// ----------- Set Functions ------------

/// @brief set the value to boolean
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_bool(var _var, bool val);

/// @brief set the value to char
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_char(var _var, char val);

/// @brief set the value to short
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_short(var _var, short val);

/// @brief set the value to unsigned short
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_ushort(var _var, unsigned short val);

/// @brief set the value to int
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_int(var _var, int val);

/// @brief set the value to unsigned int
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_uint(var _var, unsigned int val);

/// @brief set the value to long
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_long(var _var, long val);

/// @brief set the value to unsigned long
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_ulong(var _var, unsigned long val);

/// @brief set the value to float
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_float(var _var, float val);

/// @brief set the value to double
/// @param _var the var variable to set to
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_double(var _var, double val);

/// @brief set the value to string
/// @param _var the var variable to set var_clearto
/// @param val the value to set to
/// @return the var variable with value as given
var var_set_string(var _var, char * val);

/// @brief clear the variable, i.e set it to none
/// @param _var the variable to clear
/// @return the cleared variable
var var_clear(var _var);

/// @brief set the value to custom type value
/// @param _var the var variable to set to
/// @param valPointer the pointer of the custom type value
/// @param size the size of the custom type variable 
/// @return the var variable with value of custom type
var var_set_custom(var _var, void * valPointer, size_t size);

// ----------- Get Functions ------------

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as a boolean
bool var_get_bool(var _var);

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as a char
char var_get_char(var _var);

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as a short
short var_get_short(var _var);

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as unsigned short
unsigned short var_get_ushort(var _var);

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as int
int var_get_int(var _var);

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as unsigned int
unsigned int var_get_uint(var _var);

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as long
long var_get_long(var _var);

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as unsigned long
unsigned long var_get_ulong(var _var);

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as a float
float var_get_float(var _var);

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as a double
double var_get_double(var _var);

/// @brief get the value of var variable
/// @param _var the var variable to get from
/// @return the value of given var variable as a string
char * var_get_string(var _var);

#endif // VAR_H_INCL