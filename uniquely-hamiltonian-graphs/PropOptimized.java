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
import org.chocosolver.util.objects.graphs.UndirectedGraph;

import java.util.*;


public class PropOptimized extends Propagator<BoolVar> {

    private UnknownGraph graph;
    public static long sumPaths = 0;
    public static long executions = 0;
    private Collection<int[]> Dn;
    private int[] currentPath ;
    private int[] degrees;
    private boolean[] visited;

    public PropOptimized(UnknownGraph graph) {
        super(graph.allVars(), PropagatorPriority.VERY_SLOW, true);
        this.graph = graph;
        currentPath = new int[graph.order()];
        degrees = new int[graph.order()];
        visited = new boolean[graph.order()];
        Dn =  Utils.dihedralGroupPermutations(graph.order());
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

        if (Utils.cycleDist(e[0],e[1],graph.order()) == 1) return;

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
        //Collection<int[]> aut = automorphisms(graph.getCurrentFixedGraph());
        //Collection<int[]> autE = automorphisms(graph.getCurrentFixedGraph(),e);

        //DEBUG
        //executions++;
        Arrays.fill(visited, false);
        visited[e[0]] = true;
        visited[e[1]] = true;
        Arrays.fill(currentPath, 0);
        currentPath[0] = e[0];
        currentPath[1] = e[1];

        //int[] connectivity  = new int[n];

        //Arrays.fill(connectivity, -1);
        /////////////////////////////////////////////////////////
        // compute the adjacency list representation of the graph
        int[][] adjList = new int[n][];

        for (int i = 0; i < n; ++i) {
            adjList[i] = graph.neighborsOf(i).toArray();
            degrees[i] = adjList[i].length;
        }
        for (int x : adjList[e[0]]) { if (!visited[x]) --degrees[x]; }
        for (int x : adjList[e[1]]) { if (!visited[x]) --degrees[x]; }

        for (int i = 0; i < n; i++) {
            sortByDegrees(adjList[i], degrees);
        }

        int i = 1;
        int[] inds = new int[n];
        boolean foundNext;
        int v;
        int[] ns;

        while (i > 0) {
            v = currentPath[i];
            // if found hamiltonian path - force it not to be a hamiltonian cycle
            if (i == n - 1) {
                // debug
                //sumPaths++;
                graph.fixEdgeValue(currentPath[0], currentPath[i], false, this);
                //for (int[] perm : aut) {
                //    graph.fixEdgeValue(perm[currentPath[0]], perm[currentPath[i]], false, this);
                //}
                visited[v] = false;
                --i;
                v = currentPath[i];
            }
            foundNext = false;
            ns = adjList[v];

            //if (connectivity[i] == -1)
            //    connectivity[i] =  isSinlgeComponent(graph.getCurrentFixedGraph(), currentPath, i + 1) ? 1 : 0;
/*
            boolean minPath = true;
            int[] perm = null;
            for (Iterator<int[]> it = autE.iterator(); minPath && it.hasNext();) {
                perm = it.next();
                for (int j = 2; j <= i; j++) {
                    if (perm[currentPath[j]] < currentPath[j]) {
                        minPath = false;
                        break;
                    }
                    if (perm[currentPath[j]] > currentPath[j]) {
                        break;
                    }
                }
            }
*/

            if (i < n-2) {
                int count = 0;
                int w1 = -1;
                int w2 = -1;
                for (int x : ns) {
                    if (!visited[x] && degrees[x] == 0) {
                        inds[v] = ns.length;
                        break;
                    }
                    else if (!visited[x] && degrees[x] == 1) {
                        count++;
                        if (count >= 3) {
                            inds[v] = ns.length;
                            break;
                        }
                        if (w1 == -1) {
                            w1 = x;
                        }
                        else if (w2 == -1) {
                            w2 = x;
                        }
                    }
                }
                if (count == 2) {
                    ns = new int[]{w1,w2};
                }
            }

            while (inds[v] < ns.length && !foundNext) {
                int u = ns[inds[v]];
                if (!visited[u] && (i >=  n - 3 || !makesNeighborUnreachable(u, visited, degrees))) {
                    ++i;
                    visited[u] = true;
                    currentPath[i] = u;
                    inds[u] = 0;
                    foundNext = true;
                    for (int x : adjList[u]) { if (!visited[x]) --degrees[x]; }
                }
                ++inds[v]; // next
            }

            if(!foundNext) {
                visited[v] = false;
                currentPath[i] = 0;
                //connectivity[i] = -1;
                --i;
                for (int x : adjList[v]) {  if (!visited[x]) ++degrees[x]; }
            }
        }
    }

    private void sortByDegrees(int[] arr, int[] degrees) {
        int i, j, temp;
        boolean swapped;
        for (i = 0; i < arr.length; i++) {
            swapped = false;
            for (j = 0; j < arr.length - i - 1; j++) {
                if (degrees[arr[j]] < degrees[arr[j + 1]]) {
                    temp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = temp;
                    swapped = true;
                }
            }
            if (swapped == false)
                break;
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
                    System.out.println(Arrays.toString(cycle));
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

    private boolean makesNeighborUnreachable(int v, boolean[] visited, int[] degrees) {
        for (int u : graph.neighborsOf(v)) {
            if (!visited[u] && degrees[u] < 2) {
                return true;
            }
        }
        return false;
    }

    @Override
    public void explain(ExplanationForSignedClause explanation, ValueSortedMap<IntVar> front, Implications ig, int p) {
        super.explain(explanation, front, ig, p);
    }


    public Collection<int[]> automorphisms(UndirectedGraph g) {
        int n  = g.getNbMaxNodes();
        Collection<int[]> aut = new ArrayList<>();
        for (int[] perm : Dn) {
            boolean isAut = true;
            for (int i = 0; i < n && isAut; ++i) {
                for (int j : g.getNeighOf(i)) {
                    if (i < j && !g.edgeExists(perm[i], perm[j])) {
                        isAut = false;
                    }
                }
            }
            if (isAut) {
                aut.add(perm);
            }
        }
        return aut;
    }

    // automorphisms which map e to e
    public Collection<int[]> automorphisms(UndirectedGraph g, int[] e) {
        int n  = g.getNbMaxNodes();
        Collection<int[]> aut = new ArrayList<>();
        for (int[] perm : Dn) {
            if (perm[e[0]] == e[0] && perm[e[1]] == e[1]) {
                boolean isAut = true;
                for (int i = 0; i < n && isAut; ++i) {
                    for (int j : g.getNeighOf(i)) {
                        if (!g.edgeExists(perm[i], perm[j])) {
                            isAut = false;
                        }
                    }
                }

                if (isAut) {
                    aut.add(perm);
                }
            }
        }
        return aut;
    }


    public boolean isSinlgeComponent(UndirectedGraph g, int[] currentPath, int len) {
        int n = g.getNbMaxNodes();
        boolean[] visited = new boolean[n];
        boolean[] deleted = new boolean[n];

        int v = -1;
        for (int i = 0; i < len; ++i) {
            deleted[currentPath[i]] = true;
        }

        for (int i = 0; v == -1 && i < n; ++i) {
            if (!deleted[i]) {
                v = i;
            }
        }
        Queue<Integer> q = new ArrayDeque<>();

        q.add(v);

        while (!q.isEmpty()) {
            v = q.remove();
            visited[v] = true;
            for (int u : g.getNeighOf(v)) {
                if (!visited[u] && !deleted[u]) {
                    q.add(u);
                }
            }
        }

        for (int i = 0; i < n; ++i) {
            if (visited[i] == false && !deleted[i]) {
                    return false;
                }
        }

        return true;
    }

}
