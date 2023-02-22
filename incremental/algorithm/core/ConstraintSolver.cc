#include "core/ConstraintSolver.h"
#include "core/SolverTypes.h"


int lbool2int(lbool_t b) {
    if (b == l_True) {
        return 1;
    }
    if (b == l_False) {
        return 0;
    }
    return -1;
}

ConstraintSolver::ConstraintSolver() :
    m_nextBoolID( 1 ),
    m_nextIntID ( 0 ),
    m_satSolver( new Glucose::Solver() ),
    m_ints(0)
{
    BoolVarRef trueConst = newBoolVar();
    m_satSolver->addClause( toLit( trueConst ) );
}

ConstraintSolver::~ConstraintSolver() {
    delete m_satSolver;
}

BoolVarRef ConstraintSolver::newBoolVar() {
    m_satSolver->newVar();
    return m_nextBoolID++;
}

BoolVarRef ConstraintSolver::neg( BoolVarRef b ) {
    return -b;
}

BoolVarRef ConstraintSolver::trueConst ( ) {
    return 1;
}

BoolVarRef ConstraintSolver::falseConst ( ) {
    return -1;
}

void ConstraintSolver::boolOr(BoolVarRef b1, BoolVarRef b2) {
    m_satSolver->addClause( toLit(b1), toLit(b2) );
}


void ConstraintSolver::boolOr(BoolVarRef b1, BoolVarRef b2, BoolVarRef b3) {
    m_satSolver->addClause( toLit(b1), toLit(b2), toLit(b3) );
}

void ConstraintSolver::boolArrayOr( const std::vector< BoolVarRef >& bools ) {
    atLeastOne( bools );
}

void ConstraintSolver::boolIffReif( BoolVarRef b1, BoolVarRef b2, BoolVarRef t ) {
    boolOr( b1,      b2,      t      );
    boolOr( neg(b1), neg(b2), t      );
    boolOr( b1,      neg(b2), neg(t) );
    boolOr( neg(b1), b2,      neg(t) );
}

BoolVarRef ConstraintSolver::boolIffReif( BoolVarRef b1, BoolVarRef b2 ) {
    std::pair<int, int> p1(b1,b2);
    std::pair<int, int> p2(b2,b1);
    if ( m_iffReifCache.find(p1)  !=  m_iffReifCache.end()) {
        return m_iffReifCache[p1];
    }
    if (m_iffReifCache.find(p2) !=  m_iffReifCache.end() ) {
        return m_iffReifCache[p2];
    }
    BoolVarRef t = newBoolVar();
    boolIffReif(b1, b2, t);
    m_iffReifCache[p1] = t;
    
    return t;
}


void ConstraintSolver::atLeastOne( const std::vector< BoolVarRef >& bools ) {
    Glucose::vec< Glucose::Lit > lits;
    for (int i = 0; i < bools.size(); ++i) {
        lits.push( toLit( bools.at(i) ) );     
    }
    m_satSolver->addClause(lits);
}

void ConstraintSolver::atMostOne( const std::vector< BoolVarRef >& bools ) {
    for (int i = 0; i < bools.size(); ++i) {
        for (int j = i + 1 ; j < bools.size(); ++j) {
            m_satSolver->addClause( toLit( neg(bools.at(i)) ), toLit( neg(bools.at(j)) ) );
        }
    }
}

void ConstraintSolver::exactlyOne( const std::vector< BoolVarRef >& bools ) {
    atLeastOne( bools );
    atMostOne( bools );
}

Glucose::Lit ConstraintSolver::toLit( BoolVarRef b ) {
    int var = abs(b)-1;
    Glucose::Lit p = (b > 0) ? Glucose::mkLit( var ) : ~Glucose::mkLit( var );    
    return p;
}


void ConstraintSolver::boolArraysLex( const std::vector< BoolVarRef>& arr1, 
                                      const std::vector< BoolVarRef>& arr2 ) {
    if (arr1.size() != arr2.size()) {
        throw std::runtime_error("boolArraysLex: input arrays with different length!");
    }
    int n = arr1.size();
    std::vector< BoolVarRef > reifs;
    // cache
    for (int i = 0; i < n - 1; ++i) {
        BoolVarRef t = newBoolVar();
        boolIffReif( arr1[i], arr2[i], t );
        reifs.push_back( t );
    }
    // horn clauses
    std::vector< BoolVarRef > c;
    for (int i = 0; i < n ; ++i) { // for each implication
        for (int j = 0; j < i; ++j) { //LHS
            c.push_back( neg(reifs[j]) );
        }
        c.push_back( neg(arr1[i]) );
        c.push_back( arr2[i] ); 
        boolArrayOr( c );
        c.clear();
    }
}
void ConstraintSolver::boolArraysLexImplication( const std::vector< BoolVarRef>& arr1,
                                                 const std::vector< BoolVarRef>& arr2,
                                                 int length ) {
    if (arr1.size() != arr2.size()) {
        throw std::runtime_error("boolArraysLexImplication: input arrays with different length!");
    }
    std::vector< BoolVarRef > c;
    for (int i = 0; i < length - 1; ++i) {
        BoolVarRef t = boolIffReif( arr1[i], arr2[i] );
        c.push_back( neg(t) );
    }
    c.push_back( neg(arr1[length-1]) );
    c.push_back( arr2[length-1] ); 
    boolArrayOr( c );
}

void ConstraintSolver::boolArraysLexLtImplication( const std::vector< BoolVarRef>& arr1,
                                                   const std::vector< BoolVarRef>& arr2,
                                                   int length ) {
    if (arr1.size() != arr2.size()) {
        throw std::runtime_error("boolArraysLexImplication: input arrays with different length!");
    }
    for (int i = 0; i < length - 1; ++i) {
        BoolVarRef t = newBoolVar();
        boolIffReif( arr1[i], arr2[i], t );
        setBoolVar( t, true );

    }
    setBoolVar( arr1[length-1], false );
    setBoolVar( arr2[length-1], true  );
}

/*
void ConstraintSolver::boolArraysLex( const std::vector< BoolVarRef>& arr1, 
                                      const std::vector< BoolVarRef>& arr2 ) {
    if (arr1.size() != arr2.size()) {
        throw std::runtime_error("boolArraysLex: input arrays with different length!");
    }
    int n = arr1.size();
    BoolVarRef tcurr = trueConst();
    BoolVarRef tnext = newBoolVar();    
    for (int i = 0; i < n; ++i) {
        if ( i < n - 1 ){
            BoolVarRef t1 = newBoolVar();
            BoolVarRef t2 = newBoolVar();
            BoolVarRef t3 = newBoolVar();
            boolOrReif(-arr1[i], arr2[i], t1); // t1 <-> xi <= yi
            boolAndReif(-arr1[i], arr2[i], t2); // t2 <-> xi < yi
            boolOrReif(t2, tnext, t3),        // t3 <-> t2 -> tnext
            boolAndReif(t1, t3, tcurr);          // tcurr <-> t1 /\ t3
            tcurr = tnext;
            tnext = newBoolVar();
        }
        else {
            boolOrReif(-arr1[i],arr2[i], tcurr);
        }

    }

}
*/

void ConstraintSolver::boolArraysLexLt( const std::vector< BoolVarRef>& arr1, const std::vector< BoolVarRef>& arr2) {
    if (arr1.size() != arr2.size()) {
        throw std::runtime_error("got arrays with different length");
    }
    int n = arr1.size();
    std::vector< BoolVarRef > eqReifs;
    // cache
    for (int i = 0; i < n - 1; ++i) {
        BoolVarRef t = newBoolVar();
        boolIffReif( arr1[i], arr2[i], t );
        eqReifs.push_back( t );
    }
    std::vector< BoolVarRef > ts;
    for (int i = 0; i < n ; ++i) {
        ts.push_back( newBoolVar() );
    }
    for (int i = 0; i < n; i++) {
        std::vector< BoolVarRef > bvars;
        for (int j = 0; j < i; ++j) {
            bvars.push_back( eqReifs[j] );
        }
        bvars.push_back( neg(arr1[i]) );
        bvars.push_back( arr2[i]      );
        boolArrayAndReif( bvars, ts[i] );
    }
    boolArrayOr(ts);
}


void ConstraintSolver::boolAndReif( BoolVarRef b1, BoolVarRef b2, BoolVarRef t ) {
    boolOr( b1,      neg(t)     );
    boolOr( b2,      neg(t)     );
    boolOr( neg(b1), neg(b2), t );
    
}

void ConstraintSolver::boolAndReif( BoolVarRef b1, BoolVarRef b2, BoolVarRef b3, BoolVarRef t ) {
    boolOr( b1, neg(t) );
    boolOr( b2, neg(t) );
    boolOr( b3, neg(t) );
    std::vector< BoolVarRef > bools;
    bools.push_back( neg(b1) );
    bools.push_back( neg(b2) );
    bools.push_back( neg(b3) );
    bools.push_back( t );
    boolArrayOr( bools );
}

void ConstraintSolver::boolArrayAndReif( const std::vector< BoolVarRef >& bvars, BoolVarRef t ) {
    BoolVarRef ti = bvars[0];
    for (int i = 1; i < bvars.size() - 1 ; i++) {
        BoolVarRef ti1 = newBoolVar();
        boolAndReif(ti, bvars[i], ti1);
        ti = ti1;
    }
    boolAndReif( bvars[bvars.size()-1], ti, t );
}

IntVarRef ConstraintSolver::newIntVar( int lb, int ub ) {
    IntVar v;
    v.lb = lb;
    v.ub = ub;
    IntVarRef vRef = m_nextIntID++;
    for (int i = lb; i <= ub; ++i){
        v.bools.push_back( newBoolVar() );
    }
    exactlyOne( v.bools );
    m_ints.push_back( v );
    return vRef;
}

void ConstraintSolver::boolOrReif( BoolVarRef b1, BoolVarRef b2, BoolVarRef t ) {
    boolOr( b1, b2, neg(t) );
    boolOr( b1, neg(b2), t );
    boolOr( neg(b1), b2, t );
}

void ConstraintSolver::boolOrReif( BoolVarRef b1, BoolVarRef b2, BoolVarRef b3, BoolVarRef t ) {
    BoolVarRef t1 = newBoolVar();
    boolOrReif( b1, b2, t1 );
    boolOrReif( t1, b3, t );
}

void ConstraintSolver::boolArrayOrReif( const std::vector< BoolVarRef >& bvars, BoolVarRef t ) {
    BoolVarRef ti = bvars[0];
    for (int i = 1; i < bvars.size() - 1 ; i++) {
        BoolVarRef ti1 = newBoolVar();
        boolOrReif(ti, bvars[i], ti1);
        ti = ti1;
    }
    boolOrReif( bvars[bvars.size()-1], ti, t );
}

std::vector<IntVarRef> ConstraintSolver::newPermutation( int n ) {
    std::vector<IntVarRef> perm;
    // Generate n int vars. Each of which in range [0...n-1]
    for (int i = 0; i < n ; ++i){
        IntVarRef v = newIntVar( 0, n-1 );
        perm.push_back( v );
    }
    // All diff: each num appears exactly once
    std::vector< BoolVarRef > col;
    for (int num = 0; num < n; ++num){
        for (size_t i = 0; i < perm.size(); ++i) {
            IntVar& vi = m_ints.at( perm.at(i) );
            BoolVarRef viEqualsNum = vi.bools.at( num ); 
            col.push_back( viEqualsNum );
        }
        exactlyOne( col );
        col.clear();
    }
    return perm;
}

BoolVarRef ConstraintSolver::intEqReif( IntVarRef vi, int c) {
    int lb = m_ints[vi].lb;
    return m_ints[vi].bools[c - lb];
}

Matrix< BoolVarRef > ConstraintSolver::newAdjMatrix( int n ) {
    Matrix<BoolVarRef> adjMatrix( n, n );
    for (int i = 0 ; i < n; i++){
        adjMatrix.set( i, i, neg( trueConst() ) );
        for (int j = i+1; j < n; j++) {
            BoolVarRef xij = newBoolVar();  
            adjMatrix.set( i, j, xij );
            adjMatrix.set( j, i, xij );
        }
    }
    return adjMatrix;
}

std::vector < IntVarRef > ConstraintSolver::notLexLeader( const Matrix< lbool_t >& g1) {
    int n  = g1.nrows();
    std::vector<IntVarRef> perm = newPermutation( n );
    Matrix <BoolVarRef> g2 = newAdjMatrix( n );

    // encode isomorphism
    for (int i2 = 0; i2 < n; ++i2){
        for (int j2 = i2 + 1; j2 < n; ++j2) {
            std::vector< BoolVarRef > tsTrue;
            std::vector< BoolVarRef > tsFalse;
            for (int i1 = 0; i1 < n; ++i1) {
                for (int j1 = 0; j1 < n; ++j1) {
                    if (i1 != j1) {
                        BoolVarRef t = newBoolVar();
                        BoolVarRef t1 = intEqReif( perm.at(i1), i2 ); // perm(i1) = i2 <=> t1
                        BoolVarRef t2 = intEqReif( perm.at(j1), j2 ); // perm(j1) = j2 <=> t2
                        boolAndReif( t1, t2, t );   // t1 /\ t2 <=> t
                        if (g1.get(i1,j1) == l_False) {
                            tsFalse.push_back( t );
                        }
                        else {
                            tsTrue.push_back( t );
                        }
                    }        
                }
            }
            tsTrue.push_back( neg( g2.get(i2,j2) ) ); 
            tsFalse.push_back( g2.get(i2,j2) );
            boolArrayOr(tsTrue);
            boolArrayOr(tsFalse);
        }
    }
    std::vector< BoolVarRef > stringG1;
    std::vector< BoolVarRef > stringG2;
    bool grounded = true;
    for (int i = 0; i < n && grounded; ++i) {
        for (int j = i + 1; j < n && grounded; ++j ){
            if (g1.get(i,j) != l_Undef) {
                stringG1.push_back(g1.get(i,j) == l_True ? trueConst() : falseConst() );
                stringG2.push_back(g2.get(i,j));
            }
            else{
                grounded = false;
            }
        }
    }
    // impose lexLt on the grounded prefix
    boolArraysLexLt(stringG2, stringG1);

    return perm; 
}

void ConstraintSolver::isomorphic( const Matrix< BoolVarRef >& g1, 
                                       const Matrix< BoolVarRef >& g2, 
                                       const std::vector<IntVarRef>& perm ) {
    int n = g1.nrows();
    std::vector<BoolVarRef> ts;
    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            ts.clear();
            for (int k = 0; k < n; ++k) {
                for (int l = 0; l < n; ++l) {
                    if (k != l){
                        BoolVarRef t1 = intEqReif( perm.at(k), i ); // perm(i) = k <=> t1
                        BoolVarRef t2 = intEqReif( perm.at(l), j ); // perm(j) = l <=> t2
                        BoolVarRef xkl = g2.get(k,l);
                        BoolVarRef t = newBoolVar();
                        boolAndReif(t1, t2, xkl, t);
                        ts.push_back(t);
                    }
                }
            }
            BoolVarRef xij = g1.get(i,j);
            boolArrayOrReif(ts, xij);
        }
    }
}



void ConstraintSolver::setBoolVar( BoolVarRef b, bool val) {
    if (val) {
        m_satSolver->addClause(toLit(b));
    }
    else{
        m_satSolver->addClause(~toLit(b));
    }
}

lbool_t ConstraintSolver::solve() {
    /*
        printf("-----------------------------\n");
        printf("Solving model using glucose 3\n");
        printf("Number of variables:  %12d\n", m_satSolver->nVars());
        printf("Number of clauses:    %12d\n", m_satSolver->nClauses());
    */ 
        Glucose::vec<Glucose::Lit> dummy;
        return m_satSolver->solveLimited( dummy );
}

lbool_t ConstraintSolver::getBoolAssignment( BoolVarRef b ){
    return m_satSolver->modelValue( toLit(b) );
}

int ConstraintSolver::getIntAssignment( IntVarRef vref ) {
    IntVar &v = m_ints[vref];
    int val = v.lb;
    for (size_t i = 0; i < v.bools.size(); ++i){
        if ( getBoolAssignment( v.bools[i] ) == l_True) {
                val = v.lb + i;
                break;
        }
    }
    return val;
}


std::vector<int> ConstraintSolver::decodeIntArray( std::vector<IntVarRef> arr ) {
    std::vector <int> ans;
    for (int i=0; i < arr.size(); ++i) {
        ans.push_back( getIntAssignment(arr[i]) );
    }
    return ans;
}


void ConstraintSolver::newModel( ) {
    m_ints.clear();
    m_iffReifCache.clear();
    m_nextBoolID = 1;
    m_nextIntID = 0;
    delete m_satSolver;
    m_satSolver = new Glucose::Solver();
    BoolVarRef trueConst = newBoolVar();
    m_satSolver->addClause( toLit( trueConst ) );
}




int ConstraintSolver::countSolutions ( std::vector<BoolVarRef> projected ) {
    int count = 0;
    while (solve() == l_True) {
        std::vector<BoolVarRef> blockingClause;
        for (int i = 0; i < projected.size(); ++i) {
            lbool_t v = getBoolAssignment(projected[i]);
            blockingClause.push_back(v == l_True ? -projected[i] : projected[i]);
        }
        boolArrayOr(blockingClause);
        ++count;
    }
    return count;
}
