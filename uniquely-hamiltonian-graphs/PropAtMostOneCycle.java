package uniqueHamGraphs;

import org.chocosolver.solver.constraints.Propagator;
import org.chocosolver.solver.constraints.PropagatorPriority;
import org.chocosolver.solver.exception.ContradictionException;
import org.chocosolver.solver.variables.BoolVar;
import org.chocosolver.util.ESat;

public class PropAtMostOneCycle extends Propagator<BoolVar> {

    private UnknownGraph graph;
    private int k;
    private int[] cycle;


    public PropAtMostOneCycle(UnknownGraph graph, int k) {
        super(graph.allVars(), PropagatorPriority.VERY_SLOW, true);
        this.graph = graph;
        this.k = k;
        this.cycle = null;
    }

    @Override
    public void propagate(int mask) throws ContradictionException {

    }


    @Override
    public void propagate(int vIdx, int mask) throws ContradictionException {

        if (vars[vIdx].isInstantiatedTo(0)) {
            return;
        }

        if (!isCycle(cycle)) {
            cycle = null;
        }

        int[] e = graph.varToEdge(vars[vIdx]);
        int n = graph.order();

        boolean[] visited = new boolean[n];
        visited[e[0]] = true;
        visited[e[1]] = true;
        int[] currentPath = new int[k];

        currentPath[0] = e[0];
        currentPath[1] = e[1];

        /////////////////////////////////////////////////////////
        // compute the adjacency list representation of the graph
        int[][] adjList = new int[n][];

        for (int i = 0; i < n; i++) {
            adjList[i] = graph.neighborsOf(i).toArray();
        }

        int i = 1;
        int[] inds = new int[n];
        boolean foundNext;
        int v;
        int[] ns;
        while (i > 0) {
            v = currentPath[i];
            if (i == k - 1) {
                if (cycle == null && graph.hasEdge(currentPath[0], currentPath[i])) {
                    cycle = currentPath.clone();
                }
                else if (cycle != null && !Utils.isSameCycle(cycle, currentPath)) {
                    graph.fixEdgeValue(currentPath[0], currentPath[i], false, this);
                }
                visited[v] = false;
                i--;
                v = currentPath[i];
            }
            foundNext = false;
            ns = adjList[v];
            while (inds[v] < ns.length && !foundNext) {
                int u = ns[inds[v]];
                if (!visited[u]) {
                    i++;
                    visited[u] = true;
                    currentPath[i] = u;
                    inds[u] = 0;
                    foundNext = true;
                }
                inds[v]++; // next
            }

            if(!foundNext) {
                visited[v] = false;
                i--;
            }
        }



    }

    @Override
    public ESat isEntailed() {
        return ESat.TRUE;
    }

    private boolean isCycle(int[] cycle) {
        if (cycle == null) {
            return false;
        }
        for (int i = 0; i < k; i++) {
            if (!graph.hasEdge(cycle[i], cycle[(i + 1) % k])) {
                return false;
            }
        }
        return true;
    }
}
