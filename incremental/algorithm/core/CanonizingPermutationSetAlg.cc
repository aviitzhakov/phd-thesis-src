#include "core/CanonizingPermutationSetAlg.h"
#include <ctime>

CanonizingPermutationSetAlg::CanonizingPermutationSetAlg( int n, CanonicalIndexing indexing) : 
    m_nvertices(n),
    m_indexing(indexing)
{

}


CanonizingPermutationSetAlg::~CanonizingPermutationSetAlg() {

}


std::vector< std::vector<int> > CanonizingPermutationSetAlg::compute() {
    Matrix <BoolVarRef> g1      = m_solver.newAdjMatrix( m_nvertices );
    Matrix <BoolVarRef> g2      = m_solver.newAdjMatrix( m_nvertices );
    std::vector<IntVarRef> perm = m_solver.newPermutation( m_nvertices );
    m_solver.isomorphic(g1, g2, perm);
    
    std::vector<BoolVarRef> stringG1;
    std::vector<BoolVarRef> stringG2;
    
    if (m_indexing == UPPER_TRIANGLE_ROWS) {
        stringG1 = g1.upperTriangle();
        stringG2 = g2.upperTriangle();
    }
    else {
        stringG1 = g1.lowerTriangle();
        stringG2 = g2.lowerTriangle();
    }

    m_solver.boolArraysLexLt(stringG2, stringG1);
    long startTime = time(NULL);


    std::vector< std::vector<int> > canonizingSet;
    while (m_solver.solve() == l_True) {
        std::vector<int> cePerm = m_solver.decodeIntArray(perm);
        for (int i = 0 ; i < cePerm.size(); ++i) {
            printf("%d ",cePerm[i]);
        }
        printf("\n");
        canonizingSet.push_back(cePerm);
        Matrix <BoolVarRef> permutedG1 = g1.permute( cePerm );
        std::vector<BoolVarRef> stringPermutedG1;
        if (m_indexing == UPPER_TRIANGLE_ROWS) {
            stringPermutedG1 = permutedG1.upperTriangle();
        }
        else {
            stringPermutedG1 = permutedG1.lowerTriangle();
        }
        m_solver.boolArraysLex(stringG1, stringPermutedG1);
    }
    long endTime = time(NULL);
    printf("exectution time = %d sec.\n", endTime-startTime);
    m_solver.newModel();
    return canonizingSet;
}
