/***************************************************************
 *
 *
 ***************************************************************/

#include <cstdlib>
#include <iostream>

#include <cstdio>
#include <cstdarg>
#include <cmath>

#include <StdCout.hpp>

// **************************************************************
int main(int argc, char *argv[])
{
    std_cout.open("output/log.txt");

    const int a = 2;
    const double b = 3.14;
    const double c = std::acos(-1.0);

    std_cout << "0 TESTING!!!\n";

    log("a = %d\nb = %f  c = %g or %.15g\n", a, b, c, c);

    std_cout << "1 TESTING!!!\n";

    return EXIT_SUCCESS;
}

