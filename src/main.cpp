#include <iostream>
#include <ctime>

#include "compiler/chunk.h"
#include "linaro_utils/logging.h"
#include "compiler/compiler.h"
#include "vm/vm.h"

using namespace linaro;

void f(std::string& f) {
    std::string pp = "new";
    f = pp;
    printf("%p\n", &f);
    printf("%p\n", &pp);
    std::cout << f;
} 

int main(int argc, char** argv) {
    
    std::string p = "hello there";
    std::string &x = p;
    printf("%p\n", &p);
    printf("%p\n", &x);
    std::cout << x;

   /*
    std::string z = "hello 22";
    x = z;
    printf("%p\n", &z);
    printf("%p\n", &x);
    std::cout << x;
*/


    /*
    uint64_t t1 = 0;
    clock_t begin = clock();
    while(1) {
        t1++;
        if(t1 == 0) {
            clock_t end = clock();
            std::cout << "32bit overflow took " << double(end - begin) / CLOCKS_PER_SEC << "\n";
            begin = clock();
        }
    }
    */
}

