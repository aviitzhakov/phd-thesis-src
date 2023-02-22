#ifndef CanonizingPermutationSetAlg_H
#define CanonizingPermutationSetAlg_H

#include <vector>
#include "core/ConstraintSolver.h"
#include "core/defs.h"

class CanonizingPermutationSetAlg {

public:

    CanonizingPermutationSetAlg ( int n, CanonicalIndexing indexing);
    
    ~CanonizingPermutationSetAlg ( );
    
    std::vector< std::vector<int> > compute ();

    std::vector< std::vector<int> > reduce ( std::vector< std::vector<int> > prems);

private:
    int                 m_nvertices;
    ConstraintSolver    m_solver;
    CanonicalIndexing   m_indexing;
};

#endif