#include <dtype.h>
#include <stdio.h>

typedef struct {
    int x;
    int y;
} custom;

int main()
{
    dtype var = dtype_default();
    var = dtype_set_string(var, "Hello World");
}