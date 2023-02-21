package uniqueHamGraphs;

import org.chocosolver.solver.constraints.Propagator;
import org.chocosolver.solver.constraints.PropagatorPriority;
import org.chocosolver.solver.exception.ContradictionException;
import org.chocosolver.solver.learn.ExplanationForSignedClause;
import org.chocosolver.solver.learn.Implications;
import org.chocosolver.solver.variables.BoolVar;
import org.chocosolver.solver.variables.IntVar;
import org.chocosolver.solver.variables.events.IntEventType;
import org.chocosolver.util.ESat;
import org.chocosolver.util.objects.ValueSortedMap;

import java.util.ArrayList;
import java.util.Arrays;


public class PropOptimizedDC extends Propagator<BoolVar> {

    private UnknownGraph graph;
    public static long sumPaths = 0;
    public static long executions = 0;

    public PropOptimizedDC(UnknownGraph graph) {
        super(graph.allVars(), PropagatorPriority.VERY_SLOW, true);
        this.graph = graph;
    }

    @Override
    public void propagate(int evtmask) throws ContradictionException {
        for (int i = 0; i < vars.length; i++) {
            int[] e  = graph.varToEdge(vars[i]);
            if (vars[i].isInstantiatedTo(1) && Utils.cycleDist(e[0],e[1],graph.order()) != 1) {
                propagate(i, 0);
            }
        }
    }

    @Override
    public int getPropagationConditions(int vIdx) {
        return IntEventType.INCLOW.getMask();
    }

    @Override
    public void propagate(int vIdx, int mask) throws ContradictionException {
        if (!vars[vIdx].isInstantiatedTo(1)) {
            return;
        }

        int[] e = graph.varToEdge(vars[vIdx]);
        int n = graph.order();

        ///////////////////////////////////////
        // rule 1: for a non cycle edge (x,y):
        // (x,y) --> not (x+1, y+1)
        graph.fixEdgeValue(Utils.mod(e[0] + 1, n),Utils.mod(e[1] + 1, n),false,this);

        ///////////////////////////////////////
        // rule 2: for a non cycle edge (x,y):
        // (x,y) /\ (y+k,y-1) --> not (x+1, y+k+1)
        for (int k = 1; Utils.mod(e[1] + k + 1, n) < e[0]; k++) {
            if (graph.hasEdge(Utils.mod(e[1] + k, n), Utils.mod(e[1] - 1, n))) {
                graph.fixEdgeValue(Utils.mod(e[0] + 1, n) ,Utils.mod(e[1] + k + 1, n),false,this);
            }
        }
        ////////////////////////////////////////////////////////////////////
        // rule 3: (symmetric version of rule 2) for a non cycle edge (x,y):
        // (x,y) /\ (y-k,y+1) --> not (x-1, y-k-1)
        for (int k = 1; Utils.mod(e[1] - k - 1, n) > e[0]; k++) {
            if (graph.hasEdge(Utils.mod(e[1] - k, n), Utils.mod(e[1] + 1, n))) {
                graph.fixEdgeValue(Utils.mod(e[0] - 1, n) ,Utils.mod(e[1] - k - 1, n),false,this);
            }
        }

        boolean[] visited = new boolean[n];
        int[] currentPath = new int[n];

        //DEBUG
        executions++;
        /////////////////////////////////////////////////////////
        // compute the adjacency list representation of the graph
        int[][] adjList = new int[n][];

        for (int i = 0; i < n; i++) {
            adjList[i] = graph.neighborsOf(i).toArray();
            sortByDegrees(adjList[i]);
        }

        int i = 0;
        int[] inds = new int[n];
        boolean foundNext;
        int v;
        int[] ns;
        //System.out.println(e[0] + ", " + e[1]);
        currentPath[0] = 1;
        visited[1] = true;
        while (i > -1) {
            v = currentPath[i];
            // if found hamiltonian path - force it not to be a hamiltonian cycle
            if (i == n - 1) {
                //debug
                sumPaths++;
                //System.out.println(Arrays.toString(currentPath));
                graph.fixEdgeValue(currentPath[0], currentPath[i], false, this);
                visited[v] = false;
                i--;
                v = currentPath[i];
            }
            foundNext = false;
            ns = adjList[v];
            while (inds[v] < ns.length && !foundNext) {
                int u = ns[inds[v]];
                if (v == e[0] && !visited[e[1]]) {
                    i++;
                    u = e[1];
                    inds[u] = 0;
                    visited[u] = true;
                    currentPath[i] = u;
                    foundNext = true;
                    inds[v] = n;
                } else if (v == e[1] && !visited[e[0]]) {
                    inds[v] = n;
                    break;
                } else if (!visited[u] && (i >= n - 3 || !makesNeighborUnreachable(u, visited))) {
                    i++;
                    visited[u] = true;
                    currentPath[i] = u;
                    inds[u] = 0;
                    foundNext = true;
                }
                inds[v]++; // next
            }

            if (!foundNext) {
                visited[v] = false;
                i--;
            }
        }
    }
    private void printReason(int[] cycle) {

        String reason = "";
        for (int i = 0; i < cycle.length; i++) {
            int x = cycle[i];
            int y = cycle[(i+1) % graph.order()];
            if (Utils.cycleDist(x, y, graph.order()) > 1) {
                reason = reason + graph.getEdgeVar(x,y) + " && ";
            }
        }
        System.out.println(reason.substring(0,reason.length() - 4));

    }

    private void sortByDegrees(int[] arr) {
        for (int i = 0; i < arr.length; i++) {
            int index = i;
            for (int j = i + 1; j < arr.length; j++) {
                if (graph.degree(arr[index]) < graph.degree(arr[j])) {
                    index = j;
                }
            }
            int temp = arr[i];
            arr[i] = arr[index];
            arr[index] = temp;
        }
    }


    @Override
    public ESat isEntailed() {
        /*
        int numOfCycles = 0;
        int[] cycle = null;
        int k = graph.order();
        Collection<int[]> paths = Utils.paths(graph, k);
        // count k-cycles
        for (int[] p : paths) {
            if (graph.hasEdge(p[0],p[k-1]) && ( cycle == null || !Utils.isSameCycle(cycle,p))) {
                numOfCycles++;
                cycle = p;
                if (numOfCycles > 1) {
                    return ESat.FALSE;
                }
            }
        }
        if (numOfCycles == 1) {
            return ESat.TRUE;
        }
        return ESat.FALSE;
         */
        return ESat.TRUE;
    }

    private boolean makesNeighborUnreachable(int v, boolean[] visited) {
        visited[v] = true;
        boolean ans = false;
        for (int u : graph.neighborsOf(v)) {
            if (!visited[u] && unreachable(u, visited)) {
                ans = true;
                break;
            }
        }
        visited[v] = false;
        return ans;
    }
    ////////////////////////////////////////////////////////////////////
    // if all neighbors of v are already visited then v is unreachable
    private boolean unreachable(int v,  boolean[] visited) {
        for (int u : graph.neighborsOf(v)) {
            if (!visited[u]) {
                return false;
            }
        }
        return true;
    }

    private boolean isCycle(int[] cycle) {
        if (cycle == null) {
            return false;
        }
        for (int i = 0; i < cycle.length; i++) {
            if (!graph.hasEdge(cycle[i], cycle[(i + 1) % cycle.length])) {
                return false;
            }
        }
        return true;
    }


    @Override
    public void explain(ExplanationForSignedClause explanation, ValueSortedMap<IntVar> front, Implications ig, int p) {
        super.explain(explanation, front, ig, p);
    }


}
