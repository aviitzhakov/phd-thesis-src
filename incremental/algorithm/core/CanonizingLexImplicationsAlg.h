#ifndef  CanonizingLexImplicationsAlg_H
#define  CanonizingLexImplicationsAlg_H

#include <map>
#include <vector>
#include "core/ConstraintSolver.h"
#include "core/defs.h"
#include <string>


class CanonizingLexImplicationsAlg {

public:
    CanonizingLexImplicationsAlg ( int n,  CanonicalIndexing indexing ) ;
    
    ~CanonizingLexImplicationsAlg ( );

    std::vector <LexImplication> compute ( );

    std::vector <LexImplication> extend ( const std::vector <LexImplication>& impSet );

    std::vector <LexImplication> extend ( const std::vector <LexImplication>& impSet, const std::string& subgraphStr );

private:

    std::vector <BoolVarRef> indexing ( Matrix<BoolVarRef> g ) const;

    int                                     m_nvertices;
    std::map< std::pair<int,int>, int >     m_reifs;
    ConstraintSolver                        m_solver;
    CanonicalIndexing                       m_indexing;
    std::string                             m_incFileName;
    std::string                             m_directFileName;
};

#endif