#include <ezy.h>

main()
{
    var a = str("Hello World!");
    print(a);
    new(a) = int(10);
    print(val(a, int));
    return 0;
}