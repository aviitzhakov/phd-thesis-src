package uniqueHamGraphs;

import org.chocosolver.solver.constraints.Propagator;
import org.chocosolver.solver.constraints.PropagatorPriority;
import org.chocosolver.solver.exception.ContradictionException;
import org.chocosolver.solver.variables.BoolVar;
import org.chocosolver.util.ESat;
import org.chocosolver.util.objects.graphs.UndirectedGraph;

public class PropAtLeastOneCycle extends Propagator<BoolVar> {

    private UnknownGraph graph;
    private int k;
    private int[] cycle;


    public PropAtLeastOneCycle(UnknownGraph graph, int k) {
        super(graph.allVars());
        this.graph = graph;
        this.k = k;
        this.cycle = new int[k];
        for (int i = 0; i < k; i++) {
            cycle[i] = i;
        }
    }

    @Override
    public void propagate(int mask) throws ContradictionException {

        if (isCycle(cycle) || undetermined(cycle)) {
            return;
        }
        cycle = null;

        int n = graph.order();

        for (int s = 0; s < n; s++) {
            boolean[] visited = new boolean[n];
            int[] currentPath = new int[k];
            currentPath[0] = s;
            visited[s] = true;
            int i = 0;
            int[] inds = new int[graph.order()]; // inds[i] indicates next index of i's neighbor
            boolean foundNext;
            int v;
            while (i > -1) {
                v = currentPath[i];
                if (i == k - 1) {
                    if (cycle == null && graph.edgeStatus(currentPath[0], currentPath[i]) != ESat.FALSE) {
                        cycle = currentPath.clone();
                        return;
                    }
                    visited[v] = false;
                    i--;
                    v = currentPath[i];
                }
                foundNext = false;
                while (inds[v] < graph.order() && !foundNext) {
                    int u = inds[v];
                    if (graph.edgeStatus(v,u) != ESat.FALSE && !visited[u]) {
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
        if (cycle == null) {
            throw new ContradictionException();
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


    private boolean undetermined(int[] cycle) {
        if (cycle == null) {
            return false;
        }
        int free = 0;
        for (int i = 0; i < k; i++) {
            if (graph.edgeStatus(cycle[i], cycle[(i + 1) % k]) == ESat.UNDEFINED) {
                free++;
            }
            if (graph.edgeStatus(cycle[i], cycle[(i + 1) % k]) == ESat.FALSE) {
                return false;
            }
        }
        return free > 0;
    }
}
