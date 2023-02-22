#ifndef RamseyGraph_H
#define RamseyGraph_H

#include "core/Matrix.h"

class RamseyGraph {

public:

    RamseyGraph ( int s, int t, int n );

    ~RamseyGraph ();

    Matrix<int> find ();

    noClique ( int size );

    noIndependentSet ( int size );

private:
    


};

#endif