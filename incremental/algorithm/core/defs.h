#ifndef DEFS_H
#define DEFS_H

#include <vector>

enum CanonicalIndexing {
    UPPER_TRIANGLE_ROWS ,
    UPPER_TRIANGLE_COLS
};

struct LexImplication {
    std::vector <int> perm;
    int length;
};

enum CompType {
    INCREMENTAL,
    DIRECT
};

struct IntPair {
    int first;
    int second;
};



#endif