
#include "core/LexImplicationsReducer.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <math.h>

LexImplicationsReducer::LexImplicationsReducer ( int n, CanonicalIndexing indexing ) :
    m_reducerPath ("./glucoseReducer"),
    m_nextBoolID(1),
    m_adjMatrix(n,n),
    m_indexing(indexing),
    m_subgraphStr("")
{
    char pid[50]; 
    sprintf(pid, "%d", getpid() );
    m_impFileName = std::string("implications_") + std::string(pid) + std::string(".dimacs");
    m_tsFileName = std::string("tseitens_") + std::string(pid) + std::string(".dimacs");
    m_outFileName = std::string("reduced_") + std::string(pid) + std::string(".indices");
    // init adjacency matrix
    for ( int i = 0; i < n; ++i ) {
        for ( int j = i+1; j < n; ++j ) {
            m_adjMatrix.set(i, j, m_nextBoolID);
            m_adjMatrix.set(j, i, m_nextBoolID);
            m_nextBoolID++;
        }
    }
}

LexImplicationsReducer::LexImplicationsReducer ( int n, CanonicalIndexing indexing, const std::string& subgraphStr ) :
    LexImplicationsReducer(n, indexing) {
        m_subgraphStr = subgraphStr.c_str();
}


LexImplicationsReducer::~LexImplicationsReducer ( ) {

}

std::vector < int > LexImplicationsReducer::indexing( Matrix <int> g ) {
    if (m_indexing == UPPER_TRIANGLE_ROWS) {
        return g.upperTriangle();
    }
    return g.lowerTriangle();
}


std::vector< LexImplication > LexImplicationsReducer::reduce ( const std::vector <LexImplication>& leximps ) {
    writeTseitenCnfFile( leximps );
    writeImplicationsCnfFile( leximps );
    std::vector<int> res = runReducer();
    std::vector< LexImplication > reduced;
    for (int i = 0; i < res.size(); ++i) {
        reduced.push_back( leximps[ res[i] ] ); 
    }

    return reduced;

}


    std::vector< LexImplication > LexImplicationsReducer::reduceDelta ( const std::vector <LexImplication>& leximps,
                                                                        const std::vector <LexImplication>& delta ) {
        writeTseitenCnfFile( leximps, delta );
        writeImplicationsCnfFile( leximps, delta );
        std::vector<int> res = runReducer();
        std::vector< LexImplication > reduced;
        for (int i = 0; i < res.size(); ++i) {
            reduced.push_back( delta[ res[i] ] ); 
        }

        return reduced;    
    }


void LexImplicationsReducer::writeTseitenCnfFile( const std::vector<LexImplication>& leximps ) {

    std::vector< int > arr1 = indexing(m_adjMatrix);
    std::vector< int > arr2;
    // create cache
    std::ofstream tsFile;
    tsFile.open(m_tsFileName.c_str());

    for ( int i = 0; i < leximps.size(); ++i ) {
        arr2 = indexing( m_adjMatrix.permute( leximps[i].perm ) );
        for (int j = 0; j < leximps[i].length - 1; ++j) {
            std::pair< int, int > p1( arr1[j], arr2[j] );
            std::pair< int, int > p2( arr2[j], arr1[j] );
            if ( m_iffReifCache.find(p1)  ==  m_iffReifCache.end() && m_iffReifCache.find(p2) ==  m_iffReifCache.end()) {
                m_iffReifCache[ p1 ] = m_nextBoolID;
                tsFile <<   arr1[j] << " " <<  arr2[j] << " " <<  m_nextBoolID << " 0" << std::endl;
                tsFile <<  -arr1[j] << " " << -arr2[j] << " " <<  m_nextBoolID << " 0" << std::endl;
                tsFile <<   arr1[j] << " " << -arr2[j] << " " << -m_nextBoolID << " 0" << std::endl;
                tsFile <<  -arr1[j] << " " <<  arr2[j] << " " << -m_nextBoolID << " 0" << std::endl;
                m_nextBoolID++;               
            } 
        }    
    }
    tsFile.close();
}



void LexImplicationsReducer::writeTseitenCnfFile( const std::vector<LexImplication>& leximps,  
                                                  const std::vector<LexImplication>& delta) {

    std::vector< int > arr1 = indexing(m_adjMatrix);
    std::vector< int > arr2;

    // create cache
    std::ofstream tsFile;
    tsFile.open(m_tsFileName.c_str());

    std::vector < int > clause;
    for ( int i = 0; i < leximps.size(); ++i ) {
        arr2 = indexing( m_adjMatrix.permute( leximps[i].perm ) );
        for (int j = 0; j < leximps[i].length - 1; ++j) {
            std::pair< int, int > p1( arr1[j], arr2[j] );
            std::pair< int, int > p2( arr2[j], arr1[j] );
            if ( m_iffReifCache.find(p1)  ==  m_iffReifCache.end() && m_iffReifCache.find(p2) ==  m_iffReifCache.end()) {
                m_iffReifCache[ p1 ] = m_nextBoolID;
                tsFile <<   arr1[j] << " " <<  arr2[j] << " " <<  m_nextBoolID << " 0" << std::endl;
                tsFile <<  -arr1[j] << " " << -arr2[j] << " " <<  m_nextBoolID << " 0" << std::endl;
                tsFile <<   arr1[j] << " " << -arr2[j] << " " << -m_nextBoolID << " 0" << std::endl;
                tsFile <<  -arr1[j] << " " <<  arr2[j] << " " << -m_nextBoolID << " 0" << std::endl;
                m_nextBoolID++;               
            } 
        }
        clause = boolArraysLexImplication( arr1, arr2, leximps[i].length );
        for ( int j = 0; j < clause.size(); ++j) {
            tsFile << clause[j] << " ";
        }
        tsFile << "0" << std::endl;    
    }

    for ( int i = 0; i < delta.size(); ++i ) {
        arr2 = indexing( m_adjMatrix.permute( delta[i].perm ) );
        for (int j = 0; j < delta[i].length - 1; ++j) {
            std::pair< int, int > p1( arr1[j], arr2[j] );
            std::pair< int, int > p2( arr2[j], arr1[j] );
            if ( m_iffReifCache.find(p1)  ==  m_iffReifCache.end() && m_iffReifCache.find(p2) ==  m_iffReifCache.end()) {
                m_iffReifCache[ p1 ] = m_nextBoolID;
                tsFile <<   arr1[j] << " " <<  arr2[j] << " " <<  m_nextBoolID << " 0" << std::endl;
                tsFile <<  -arr1[j] << " " << -arr2[j] << " " <<  m_nextBoolID << " 0" << std::endl;
                tsFile <<   arr1[j] << " " << -arr2[j] << " " << -m_nextBoolID << " 0" << std::endl;
                tsFile <<  -arr1[j] << " " <<  arr2[j] << " " << -m_nextBoolID << " 0" << std::endl;
                m_nextBoolID++;               
            } 
        }    
    }
    
    //embed subgraph
    int k = (int)sqrt(m_subgraphStr.size());
    for (int j = 0; j < m_subgraphStr.size(); ++j) {
        int val = m_subgraphStr[j] == '0' ? -1 :  1;
        int x = m_adjMatrix.get(j/k, j%k);
        if (x != 0){
            tsFile << val * x << " 0" << std::endl;
        }
    }
    tsFile.close();
}


std::vector<int> LexImplicationsReducer::runReducer() {
    std::string command = m_reducerPath + std::string(" -verb=0");
    command += std::string(" ") + m_tsFileName ;
    command += std::string(" ") + m_impFileName ;
    command += std::string(" ") + m_outFileName ;
    
    system( command.c_str() );
    std::vector<int> ans;

    std::ifstream infile;
    infile.open(m_outFileName.c_str());
    std::string line;
    while (std::getline(infile, line)) {
        line = line.substr(0, line.size()-1);
        ans.push_back( atoi(line.c_str()) - 1 );
    }
    infile.close();

    return ans;
}


void LexImplicationsReducer::writeImplicationsCnfFile( const std::vector<LexImplication>& leximps ) {
    std::ofstream impFile;
    impFile.open(m_impFileName.c_str());
    int vars = m_nextBoolID - 1;
    int k = (int)sqrt(m_subgraphStr.size());
    int clauses = m_iffReifCache.size() * 4 + leximps.size() + (m_subgraphStr.size()-k);
    impFile << "p cnf " << vars  << " " << clauses << std::endl; 
    std::vector< int > clause;
    std::vector< int > arr1 = indexing( m_adjMatrix );
    std::vector< int > arr2;
    for ( int i = 0; i < leximps.size(); ++i ) {
        arr2 = indexing( m_adjMatrix.permute( leximps[i].perm ) );
        clause = boolArraysLexImplication( arr1, arr2, leximps[i].length );
        for ( int j = 0; j < clause.size(); ++j) {
            impFile << clause[j] << " ";
        }
        impFile << "0" << std::endl;
    }
    impFile.close();
}

void LexImplicationsReducer::writeImplicationsCnfFile( const std::vector<LexImplication>& leximps, 
                                                       const std::vector<LexImplication>& delta ) {
    std::ofstream impFile;
    impFile.open(m_impFileName.c_str());
    int vars = m_nextBoolID - 1;
    int clauses = m_iffReifCache.size() * 4 + leximps.size() + delta.size();
    impFile << "p cnf " << vars  << " " << clauses << std::endl; 
    std::vector< int > clause;
    std::vector< int > arr1 = indexing( m_adjMatrix );
    std::vector< int > arr2;
    for ( int i = 0; i < delta.size(); ++i ) {
        arr2 = indexing( m_adjMatrix.permute( delta[i].perm ) );
        clause = boolArraysLexImplication( arr1, arr2, delta[i].length );
        for ( int j = 0; j < clause.size(); ++j) {
            impFile << clause[j] << " ";
        }
        impFile << "0" << std::endl;
    }
    impFile.close();
}


std::vector<int> LexImplicationsReducer::boolArraysLexImplication( const std::vector< int >& arr1,
                                                                   const std::vector< int >& arr2,
                                                                   int length ) {
    std::vector< int > c;
    for (int i = 0; i < length - 1; ++i) {
        std::pair<int, int> p1( arr1[i], arr2[i] );
        std::pair<int, int> p2( arr2[i], arr1[i] );
        int t;
        if ( m_iffReifCache.find(p1)  !=  m_iffReifCache.end()) {
            t = m_iffReifCache[p1];
        }
        else if (m_iffReifCache.find(p2) !=  m_iffReifCache.end() ) {
            t = m_iffReifCache[p2];
        }
        else {
            t = m_nextBoolID++;
            m_iffReifCache[ p1 ] = t;
        }
        c.push_back( -t );
    }
    c.push_back( -arr1[length-1] );
    c.push_back( arr2[length-1] ); 

    return c;
}