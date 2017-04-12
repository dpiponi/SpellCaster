#ifndef READ_FILE_H
#define READ_FILE_H

#include <fstream>
#include <sstream>

using std::ifstream;
using std::ostringstream;

static
string read_file(const char *filename) {
    ifstream in(filename);
    ostringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}

#endif
