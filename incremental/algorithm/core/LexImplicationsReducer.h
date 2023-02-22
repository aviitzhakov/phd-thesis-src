#ifndef LexImplicationsReducer_H
#define LexImplicationsReducer_H

#include <map>
#include <vector>
#include <string>
#include "core/Matrix.h"
#include "core/defs.h"

class LexImplicationsReducer {


public:

    LexImplicationsReducer ( int n, CanonicalIndexing indexing );

    LexImplicationsReducer ( int n, CanonicalIndexing indexing, const std::string& subgraphStr );

    ~LexImplicationsReducer ();

    std::vector< LexImplication > reduce ( const std::vector <LexImplication>& leximps );

    std::vector< LexImplication > reduceDelta ( const std::vector <LexImplication>& leximps,
                                                const std::vector <LexImplication>& delta );

private:

    void writeImplicationsCnfFile ( const std::vector<LexImplication>& leximps );

    void writeImplicationsCnfFile ( const std::vector<LexImplication>& leximps,
                                    const std::vector <LexImplication>& delta );

    void writeTseitenCnfFile ( const std::vector<LexImplication>& leximps );

    void writeTseitenCnfFile ( const std::vector<LexImplication>& leximps,
                               const std::vector <LexImplication>& delta );


    std::vector<int> boolArraysLexImplication( const std::vector< int >& arr1,
                                               const std::vector< int >& arr2,
                                               int length );

    std::vector < int > indexing( Matrix <int> g ); 

    std::vector<int> runReducer();

    std::map< std::pair<int,int>, int >     m_iffReifCache;
    std::string                             m_impFileName;
    std::string                             m_tsFileName;
    std::string                             m_outFileName;
    std::string                             m_reducerPath;
    int                                     m_nextBoolID;
    Matrix <int>                            m_adjMatrix;
    CanonicalIndexing                       m_indexing;
    std::string                             m_subgraphStr;
};

#endif
