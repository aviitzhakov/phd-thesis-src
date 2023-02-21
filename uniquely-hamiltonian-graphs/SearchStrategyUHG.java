package uniqueHamGraphs;

import org.chocosolver.solver.search.strategy.assignments.DecisionOperator;
import org.chocosolver.solver.search.strategy.assignments.DecisionOperatorFactory;
import org.chocosolver.solver.search.strategy.decision.Decision;
import org.chocosolver.solver.search.strategy.decision.IntDecision;
import org.chocosolver.solver.search.strategy.strategy.AbstractStrategy;
import org.chocosolver.solver.variables.BoolVar;
import org.chocosolver.solver.variables.IntVar;
import org.chocosolver.util.ESat;
import org.chocosolver.util.PoolManager;

import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;
import java.util.Vector;

public class SearchStrategyUHG extends AbstractStrategy<IntVar> {

    private UnknownGraph graph;
    private PoolManager<IntDecision> pool;

    private static final int MAX_DEGREE = 0;
    private static final int MIN_DEGREE = 1;
    private static final int MAX_SUM_CYCLE_NEIGHBORS_DEGREES = 2;
    private static final int MIN_SUM_CYCLE_NEIGHBORS_DEGREES = 3;
    private static final int COMMON_NEIGHBORS = 4;
    private static final int VEC_SIZE = 5;


    public SearchStrategyUHG(UnknownGraph graph) {
        super(graph.allVars());
        pool = new PoolManager();
        this.graph = graph;
    }


    @Override
    public Decision getDecision() {
        IntDecision d = pool.getE();
        if(d == null) {
            d = new IntDecision(pool);
        }
        BoolVar next = null;
        int n = graph.order();

        int[] sumDegressPS = new int[n];
        for (int i = 0; i < n; i++) {
            sumDegressPS[i] = graph.degree(Utils.mod(i-1,n)) + graph.degree(Utils.mod(i+1,n));
        }

        int[] min_vector = new int[VEC_SIZE];
        int[] temp_vector = new int[VEC_SIZE];

        for (int i = 0; i < n; i++) {
            for(int j = i + 2; j < n; j++) {
                if (graph.edgeStatus(i,j) == ESat.UNDEFINED) {
                    temp_vector[MAX_DEGREE]  = Math.max(graph.degree(i), graph.degree(j));
                    temp_vector[MIN_DEGREE]  = Math.min(graph.degree(i), graph.degree(j));
                    temp_vector[MAX_SUM_CYCLE_NEIGHBORS_DEGREES] = -Math.max(sumDegressPS[j], sumDegressPS[i]);
                    temp_vector[MIN_SUM_CYCLE_NEIGHBORS_DEGREES] = Math.min(sumDegressPS[j], sumDegressPS[i]);
                    temp_vector[COMMON_NEIGHBORS] = 0;
                    for (int k : graph.neighborsOf(i)) {
                        if (graph.hasEdge(j,k)) {
                            temp_vector[COMMON_NEIGHBORS]++;
                        }
                    }

                    if (next == null || lexGt(min_vector, temp_vector)) {
                        for (int k = 0; k < VEC_SIZE; k++) {
                            min_vector[k] = temp_vector[k];
                        }
                        next = graph.getEdgeVar(i,j);
                    }
                }
            }
        }
        if (next == null) {
            return null;
        }
        else {
            d.set(next, next.getUB(), DecisionOperatorFactory.makeIntEq());
        }

        return d;
    }


    private boolean lexGt(int[] vec1, int[] vec2) {
        for (int i = 0; i < VEC_SIZE; i++) {
            if (vec1[i] > vec2[i]) {
                return true;
            }
            if (vec1[i] < vec2[i]) {
                return false;
            }
        }
        return false;
    }


}
