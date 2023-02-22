
#include "core/CanonizingLexImplicationsAlg.h"
#include "core/LexImplicationsReducer.h"
#include "core/LexImplicationsFileWriter.h"
#include <ctime>
#include <math.h>  
#include <stdio.h>

CanonizingLexImplicationsAlg::CanonizingLexImplicationsAlg ( int n, CanonicalIndexing indexing ) :
    m_nvertices (n),
    m_indexing(indexing),
    m_directFileName(""),
    m_incFileName("")
{
    char buff[16];
    sprintf(buff, "%d", m_nvertices);
    std::string prefix("leximps_");
    prefix += std::string(buff);
    if (indexing == UPPER_TRIANGLE_ROWS) {
        prefix += std::string("_rowise");
    }
    else {
        prefix += std::string("_colwise");
    }
    m_directFileName += prefix;
    m_directFileName += "_direct.pl";
    m_incFileName += prefix;
    m_incFileName += "_inc.pl";
}

CanonizingLexImplicationsAlg::~CanonizingLexImplicationsAlg() {

}

std::vector<BoolVarRef> CanonizingLexImplicationsAlg::indexing(Matrix <BoolVarRef> g) const {
    if (m_indexing == UPPER_TRIANGLE_ROWS) {
        return g.upperTriangle();
    }
    return g.lowerTriangle();
}


std::vector <LexImplication> CanonizingLexImplicationsAlg::compute () {

    std::vector <LexImplication> canonizingSet;
    int maxLen = ((m_nvertices * (m_nvertices-1)) / 2) - 1;
    timespec tsStart;
    clock_gettime(CLOCK_REALTIME, &tsStart);
    
    for (int i = 1; i <= maxLen; ++i) {
        printf("n=%d, length=%d",m_nvertices, i);
        m_solver.newModel();
        Matrix <BoolVarRef> g1      = m_solver.newAdjMatrix( m_nvertices );
        Matrix <BoolVarRef> g2      = m_solver.newAdjMatrix( m_nvertices );
        std::vector<IntVarRef> perm = m_solver.newPermutation( m_nvertices );
        m_solver.isomorphic(g1, g2, perm);
        
        std::vector<BoolVarRef> stringG1 = indexing(g1);
        std::vector<BoolVarRef> stringG2 = indexing(g2);

        m_solver.boolArraysLexLtImplication(stringG2, stringG1, i);
        
        // encode lex implications from previous steps
        std::vector<BoolVarRef> stringPermutedG1;

        for (int j = 0; j < canonizingSet.size(); ++j) {
            stringPermutedG1 = indexing( g1.permute( canonizingSet[j].perm ) );
            m_solver.boolArraysLexImplication( stringG1, stringPermutedG1, canonizingSet[j].length);
        }
        int stepCount = 0;
        // find implications of length i
        while (m_solver.solve() == l_True) {
            std::vector<int> cePerm = m_solver.decodeIntArray(perm);
            stringPermutedG1 = indexing(g1.permute( cePerm ));
            m_solver.boolArraysLexImplication( stringG1, stringPermutedG1, i);
            LexImplication leximp;
            leximp.perm = cePerm;
            leximp.length = i;
            canonizingSet.push_back(leximp);
            stepCount++;
        }
        printf(", implications: [step = %d, total = %d]\n",stepCount, canonizingSet.size());
        if ( i % 5 == 0 || i == maxLen ) {
            LexImplicationsReducer reducer(m_nvertices, m_indexing);
            printf("Reduce:");
            int before = canonizingSet.size();
            canonizingSet = reducer.reduce( canonizingSet );
            int after = canonizingSet.size();
            printf(" %d / %d,\n", after, before);
        }
    }
    timespec tsEnd;
    clock_gettime(CLOCK_REALTIME, &tsEnd);

    double elapsed = (tsEnd.tv_sec - tsStart.tv_sec) + (tsEnd.tv_nsec - tsStart.tv_nsec) / 1000000000.0 ;
    printf("exectution time = %lf sec\n", elapsed);

    printf("|implications|=%d\n", canonizingSet.size());

    LexImplicationsFileWriter writer;
    writer.write(m_nvertices, canonizingSet, m_directFileName, elapsed, DIRECT, m_indexing);

    return canonizingSet;
}




std::vector <LexImplication> CanonizingLexImplicationsAlg::extend( const std::vector<LexImplication>& impSet ) {
    std::vector< LexImplication > delta;
    int maxLen = ((m_nvertices * (m_nvertices-1)) / 2) - 1;
    long startTime = time(NULL);
    
    timespec tsStart;
    clock_gettime(CLOCK_REALTIME, &tsStart);
    
    for (int i = 1; i <= maxLen; ++i) {
        printf("n=%d, length=%d",m_nvertices, i);
        m_solver.newModel();
        Matrix <BoolVarRef> g1      = m_solver.newAdjMatrix( m_nvertices );
        Matrix <BoolVarRef> g2      = m_solver.newAdjMatrix( m_nvertices );
        std::vector<IntVarRef> perm = m_solver.newPermutation( m_nvertices );
        m_solver.isomorphic(g1, g2, perm);
        
        std::vector<BoolVarRef> stringG1 = indexing(g1);
        std::vector<BoolVarRef> stringG2 = indexing(g2);

        m_solver.boolArraysLexLtImplication(stringG2, stringG1, i);
    
        std::vector<BoolVarRef> stringPermutedG1;
        // encode given set of lex implications 
        for (int j = 0; j < impSet.size(); ++j) {
            stringPermutedG1 = indexing( g1.permute( impSet[j].perm ) );
            m_solver.boolArraysLexImplication( stringG1, stringPermutedG1, impSet[j].length);
        }
        // encode lex implications from previous steps
        for (int j = 0; j < delta.size(); ++j) {
            stringPermutedG1 = indexing( g1.permute( delta[j].perm ) );
            m_solver.boolArraysLexImplication( stringG1, stringPermutedG1, delta[j].length);
        }
        int stepCount = 0;
        // find implications of length i
        while (m_solver.solve() == l_True) {
            std::vector<int> cePerm = m_solver.decodeIntArray(perm);
            stringPermutedG1 = indexing(g1.permute( cePerm ));
            m_solver.boolArraysLexImplication( stringG1, stringPermutedG1, i);
            LexImplication leximp;
            leximp.perm = cePerm;
            leximp.length = i;
            delta.push_back(leximp);
            stepCount++;
        }
        printf(", implications: [step = %d, total = %d]\n",stepCount, delta.size());
        if ( i % 5 == 0 || i == maxLen ) {
            LexImplicationsReducer reducer(m_nvertices, m_indexing);
            printf("Reduce delta:");
            int before = delta.size();
            delta = reducer.reduceDelta( impSet, delta );
            int after = delta.size();
            printf(" %d / %d,\n", after, before);
        }
    }

    timespec tsEnd;
    clock_gettime(CLOCK_REALTIME, &tsEnd);

    double elapsed = (tsEnd.tv_sec - tsStart.tv_sec) + (tsEnd.tv_nsec - tsStart.tv_nsec) / 1000000000.0 ;
    printf("exectution time = %lf sec\n", elapsed);
    printf("|delta|=%d\n", delta.size());

    LexImplicationsFileWriter writer;
    writer.write(m_nvertices, delta, m_incFileName, elapsed, INCREMENTAL, m_indexing);

    return delta;
}





////////////
// SUBGRAPH
std::vector <LexImplication> CanonizingLexImplicationsAlg::extend( const std::vector<LexImplication>& impSet, 
                                                                   const std::string& subgraphStr) {
    std::vector< LexImplication > delta;
    int maxLen = ((m_nvertices * (m_nvertices-1)) / 2) - 1;
    long startTime = time(NULL);
    
    timespec tsStart;
    clock_gettime(CLOCK_REALTIME, &tsStart);
    
    int k = (int)sqrt(subgraphStr.size());

    for (int i = 1; i <= maxLen; ++i) {
        printf("n=%d, length=%d",m_nvertices, i);
        m_solver.newModel();
        Matrix <BoolVarRef> g1      = m_solver.newAdjMatrix( m_nvertices );
        Matrix <BoolVarRef> g2      = m_solver.newAdjMatrix( m_nvertices );
        std::vector<IntVarRef> perm = m_solver.newPermutation( m_nvertices );
        m_solver.isomorphic(g1, g2, perm);
        
        // embed subgraph
        for (int j = 0; j < subgraphStr.size(); ++j) {
            bool val = subgraphStr[j] == '0' ? false :  true;
            m_solver.setBoolVar(g1.get(j/k, j%k), val);
        }

        std::vector<BoolVarRef> stringG1 = indexing(g1);
        std::vector<BoolVarRef> stringG2 = indexing(g2);

        m_solver.boolArraysLexLtImplication(stringG2, stringG1, i);
    
        std::vector<BoolVarRef> stringPermutedG1;
        // encode given set of lex implications 
        for (int j = 0; j < impSet.size(); ++j) {
            stringPermutedG1 = indexing( g1.permute( impSet[j].perm ) );
            m_solver.boolArraysLexImplication( stringG1, stringPermutedG1, impSet[j].length);
        }
        // encode lex implications from previous steps
        for (int j = 0; j < delta.size(); ++j) {
            stringPermutedG1 = indexing( g1.permute( delta[j].perm ) );
            m_solver.boolArraysLexImplication( stringG1, stringPermutedG1, delta[j].length);
        }
        int stepCount = 0;
        // find implications of length i
        while (m_solver.solve() == l_True) {
            std::vector<int> cePerm = m_solver.decodeIntArray(perm);
            stringPermutedG1 = indexing(g1.permute( cePerm ));
            m_solver.boolArraysLexImplication( stringG1, stringPermutedG1, i);
            LexImplication leximp;
            leximp.perm = cePerm;
            leximp.length = i;
            delta.push_back(leximp);
            stepCount++;
        }
        printf(", implications: [step = %d, total = %d]\n",stepCount, delta.size());
        if ( i % 5 == 0 || i == maxLen ) {
            LexImplicationsReducer reducer(m_nvertices, m_indexing, subgraphStr);
            printf("Reduce delta:");
            int before = delta.size();
            delta = reducer.reduceDelta( impSet, delta );
            int after = delta.size();
            printf(" %d / %d,\n", after, before);
        }
    }

    timespec tsEnd;
    clock_gettime(CLOCK_REALTIME, &tsEnd);

    double elapsed = (tsEnd.tv_sec - tsStart.tv_sec) + (tsEnd.tv_nsec - tsStart.tv_nsec) / 1000000000.0 ;
    printf("exectution time = %lf sec\n", elapsed);
    printf("|delta|=%d\n", delta.size());

    LexImplicationsFileWriter writer;
    std::string incFileName = "leximps_subgraph_";
    incFileName += subgraphStr.c_str();
    incFileName += ".pl";
    writer.write(m_nvertices, delta, incFileName, elapsed, INCREMENTAL, m_indexing, subgraphStr);

    return delta;
}

