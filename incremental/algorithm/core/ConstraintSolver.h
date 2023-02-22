#ifndef CONSTRAINT_SOLVER_H
#define CONSTRAINT_SOLVER_H

#include "core/Solver.h"
#include <vector>
#include "mtl/Matrix.h"
#include <stdexcept>
#include <map>

//------------------------
// Constraint Solver types
//------------------------

typedef int IntVarRef;
typedef int BoolVarRef;
typedef Glucose::lbool lbool_t;

int lbool2int(lbool_t b);

struct IntVar  {
    int lb;
    int ub;
    std::vector < BoolVarRef > bools;
};
/*
enum LexConstraintStrategy {
    AND_DECOMPOSITION,
    AND_RECURSIVE 
};
*/

class ConstraintSolver
{

public: 
    ConstraintSolver ( );

    virtual ~ConstraintSolver ( );

    BoolVarRef newBoolVar ( );

    BoolVarRef trueConst ( );
    
    BoolVarRef falseConst ( );
    
    BoolVarRef neg ( BoolVarRef b );

    void setBoolVar ( BoolVarRef b, bool val );

    void boolArrayOr ( const std::vector<BoolVarRef>& bools );

    void boolOr ( BoolVarRef b1, BoolVarRef b2 );

    void boolOr ( BoolVarRef b1, BoolVarRef b2, BoolVarRef b3);

    void atLeastOne ( const std::vector<BoolVarRef>& bools );

    void atMostOne ( const std::vector<BoolVarRef>& bools );
    
    void exactlyOne ( const std::vector<BoolVarRef>& bools );

    /****************************************************
     *  Reifs
     * **************************************************/

    void boolIffReif ( BoolVarRef b1, BoolVarRef b2, BoolVarRef t );

    BoolVarRef boolIffReif ( BoolVarRef b1, BoolVarRef b2);

    void boolAndReif ( BoolVarRef b1, BoolVarRef b2, BoolVarRef t );

    void boolAndReif ( BoolVarRef b1, BoolVarRef b2,  BoolVarRef b3, BoolVarRef t );
    
    void boolArrayAndReif ( const std::vector< BoolVarRef >& bvars, BoolVarRef t ) ;

    void boolOrReif ( BoolVarRef b1, BoolVarRef b2, BoolVarRef t );

    void boolOrReif ( BoolVarRef b1, BoolVarRef b2, BoolVarRef b3, BoolVarRef t );

    void boolArrayOrReif ( const std::vector< BoolVarRef >& bvars , BoolVarRef t );

    /****************************************************
     *  Bool Arrays order
     * **************************************************/
    
    void boolArraysLex ( const std::vector< BoolVarRef >& arr1, 
                         const std::vector< BoolVarRef >& arr2 );

    void boolArraysLexLt ( const std::vector< BoolVarRef >& arr1, 
                           const std::vector< BoolVarRef >& arr2 );

    void boolArraysLexImplication ( const std::vector< BoolVarRef >& arr1, 
                                    const std::vector< BoolVarRef >& arr2,
                                    int length );

    void boolArraysLexLtImplication ( const std::vector< BoolVarRef >& arr1, 
                                    const std::vector< BoolVarRef >& arr2,
                                    int length );

    /****************************************************
     *  Ints (direct representation)
     ****************************************************/

    IntVarRef newIntVar ( int lb, int ub );

    std::vector< IntVarRef > newPermutation ( int n );

    BoolVarRef intEqReif ( IntVarRef vi, int c );


    /****************************************************
     * Adj. matrix
     * *************************************************/

    Matrix< BoolVarRef > newAdjMatrix ( int n );

    // smaller isomorphic of a given matrix 
    std::vector<IntVarRef> notLexLeader ( const Matrix< lbool_t >&  adjMatrix );

    void isomorphic ( const Matrix< BoolVarRef >& g1, 
                      const Matrix< BoolVarRef >& g2, 
                      const std::vector<IntVarRef>& perm );


    /****************************************************
     *  Solver search methods 
     ****************************************************/

    lbool_t solve ( );

    lbool_t getBoolAssignment ( BoolVarRef b );

    int getIntAssignment ( IntVarRef v );

    std::vector<int> decodeIntArray ( std::vector<IntVarRef> arr );

    int countSolutions ( std::vector<BoolVarRef> projected );

    void newModel( );

    /****************************************************
     *  Solver encoding strategies 
     ****************************************************/

    void setLexConstraintStrategy();


private:

    Glucose::Lit toLit ( BoolVarRef b );
    

// members

    Glucose::Solver*                        m_satSolver;
    int                                     m_nextBoolID;
    std::vector < IntVar >                  m_ints; 
    int                                     m_nextIntID;
    std::map< std::pair<int,int>, int >     m_iffReifCache;

    //LexConstraintStrategy    m_lexConstrStrategy;
};

#endif