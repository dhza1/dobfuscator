#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "funcs.h"
#include "structs.h"

int main() {
    obfuscate("def hello():\n\tprint('Hello, world!')\n\tx = 5\n\tprint(x)");
}
