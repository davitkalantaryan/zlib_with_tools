//
// file:			main_hello_world_test.cpp
// path:			src/tests/main_hello_world_test.cpp
// created on:		2023 Feb 17
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <cinternal/disable_compiler_warnings.h>
#include <stdio.h>
#include <cinternal/undisable_compiler_warnings.h>

#ifdef _MSC_VER
#pragma warning (disable:4710)
#endif

int main(void)
{
    printf("Hello world!\n");

    return 0;
}
