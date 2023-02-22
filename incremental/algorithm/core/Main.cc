
#include <vector>
#include <unistd.h>
#include <math.h>  
#include "core/CanonizingPermutationSetAlg.h"
#include "core/CanonizingLexImplicationsAlg.h"
#include "core/LexImplicationsReducer.h"


void runIncremental(int n, CanonicalIndexing indexing) {
    std::vector< LexImplication > canset;

    for (int i = 3; i <= n; ++i) {
        CanonizingLexImplicationsAlg alg( i, indexing );
        std::vector< LexImplication > delta = alg.extend(canset);
        for (int j = 0; j < delta.size() ; ++j) {
            canset.push_back( delta[j] );
        }
    }
}


void runIncrementalSubgraph(int n, CanonicalIndexing indexing, const std::string& subgraphStr) {
    std::vector< LexImplication > canset;

    for (int i = (int)sqrt(subgraphStr.size()) + 1; i <= n; ++i) {
        CanonizingLexImplicationsAlg alg( i, indexing);
        std::vector< LexImplication > delta = alg.extend(canset,subgraphStr);
        for (int j = 0; j < delta.size() ; ++j) {
            canset.push_back( delta[j] );
        }
    }
}

void runDirect(int n, CanonicalIndexing indexing) {
    CanonizingLexImplicationsAlg alg( n, indexing );
    alg.compute();
}


void help( ) {
    printf("complexi: generate compact canonizing lex-implications set for graph search problems. by AVI ITZHAKOV.\n");
    printf("OPTIONS:\n");
    printf("---------------------------------------------------------------------------\n");
    printf("\t-i <max>       | incremental computation\n");
    printf("\t-d <n>         | direct computation\n");
    printf("\t-c             | use upper triangle columns for canonical indexing\n");
    printf("\t-r             | use upper triangle rows for canonical indexing\n");
    printf("\t-g <adjmatrix> | embed sub-graph given in adjmatrix (concatenation of adjacency matrix rows)\n");
    printf("\t-h             | help\n");
    printf("---------------------------------------------------------------------------\n");
}


int main(int argc, char** argv) {
    
    const char* options = "i:d:g:crh";
    char opt;
    int n = 0;
    std::string subgraphStr("");
    CanonicalIndexing indexing = UPPER_TRIANGLE_ROWS;
    CompType compType = DIRECT;
    // parse input
    while ( (opt = getopt(argc, argv ,options)) != -1 ) {
        switch (opt) {
            case 'i':
                n = atoi(optarg);
                compType = INCREMENTAL;
                break;   
            case 'd':
                n = atoi(optarg);
                compType = DIRECT; 
                break;
            case 'c':
                indexing = UPPER_TRIANGLE_COLS;
                break;
            case 'r':
                indexing = UPPER_TRIANGLE_ROWS;
                break;
            case 'g':
                subgraphStr = optarg;
                break;
            case 'h':
                help();
                return 0;
            default:
                break;
        }
    }
    if ( n == 0 ) {
        printf("Invalid input. Exiting..\n");
        return 1;
    }
    //run experiment

    if (subgraphStr.empty()) {
        if (compType == DIRECT) {
            runDirect(n, indexing);
        }
        else if (compType == INCREMENTAL){
            runIncremental(n, indexing);
        }
    }
    else {
        runIncrementalSubgraph(n, indexing, subgraphStr);
    }

    return 0;
}



/*
int countGraphs( int n ) {
    ConstraintSolver s;
    Matrix< lbool_t > g(n,n);
    std::vector < IntPair > map;
    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            if (i == j) {
                g.set(i,j,l_False);
            }
            else{
                g.set(i,j,l_Undef);
                g.set(j,i,l_Undef);
                IntPair p;
                p.first = i;
                p.second = j;
                map.push_back(p);
            }
        }
    }
    int nvars = n*(n-1)/2;
    int v = 0;
    int i,j;
    int nsols = 0;

    while ( v > -1 ) {
        i = map.at(v).first;
        j = map.at(v).second;
        if (g.get(i, j) == l_Undef) {
            s.newModel();
            //printf("assign g(%d,%d)=0 \n",i,j);
            g.set(i, j, l_False); // try 0
            g.set(j, i, l_False); 
            s.notLexLeader(g);
            if (s.solve() == l_False) {
                v++;
                if (v == nvars) {
                    nsols++;
                    printf("%d\n",nsols);
                    v--;
                }
                continue;
            }
        }
        if (g.get(i, j) == l_False) {
            s.newModel();
            //printf("assign g(%d,%d)=1 \n",i,j);
            g.set(i, j, l_True); // try 1
            g.set(j, i, l_True); 
            s.notLexLeader(g);
            if (s.solve() == l_False) {
                v++;
                if (v == nvars) {
                    nsols++;
                    printf("%d\n",nsols);
                    v--;
                }
                continue;
            }
        }
        //printf("backtrack \n",i,j);
        g.set(i, j, l_Undef); // backtrack
        g.set(j, i, l_Undef);
        v--;
    }
    return nsols;
}


void computeCanonizingPermutationSet(int n) {
    CanonicalIndexing indexing = UPPER_TRIANGLE_ROWS;
    CanonizingPermutationSetAlg alg(n, indexing) ;
    std::vector< std::vector<int> > canset = alg.compute();
    printf("n = %d, |canset| = %d\n",n, canset.size());
    
    ConstraintSolver s;
    Matrix<BoolVarRef> g = s.newAdjMatrix(n);
    std::vector<BoolVarRef> varsG = g.upperTriangle();
    for (int i = 0 ;i < canset.size(); ++i) {
        std::vector<BoolVarRef> stringPermutedG = g.permute(canset[i]).upperTriangle();
        s.boolArraysLex(varsG, stringPermutedG);
    }
    printf("#graphs = %d\n", s.countSolutions(varsG));    
    
}


void computeCanonizingLexImplicationsSet(int n) {
    CanonicalIndexing indexing = UPPER_TRIANGLE_COLS;
    CanonizingLexImplicationsAlg alg( n, indexing );
    std::vector< LexImplication > canset = alg.compute();

    CanonizingLexImplicationsAlg alg2( n + 1, indexing );
    std::vector< LexImplication > delta = alg2.extend(canset);

    ConstraintSolver s;
    Matrix<BoolVarRef> g = s.newAdjMatrix(n+1);
    std::vector<BoolVarRef> stringG = g.lowerTriangle();
    for (int i = 0 ;i < canset.size(); ++i) {
        std::vector<BoolVarRef> stringPermutedG = g.permute(canset[i].perm).lowerTriangle();
        s.boolArraysLexImplication(stringG, stringPermutedG, canset[i].length);
    }
    for (int i = 0 ;i < delta.size(); ++i) {
        std::vector<BoolVarRef> stringPermutedG = g.permute(delta[i].perm).lowerTriangle();
        s.boolArraysLexImplication(stringG, stringPermutedG, delta[i].length);
    }
    printf("#graphs = %d\n", s.countSolutions(stringG));    

}
*/
