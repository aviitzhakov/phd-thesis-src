package uniqueHamGraphs;

import java.util.Iterator;
import java.util.NoSuchElementException;

public class PathIterator implements Iterator<int[]> {

    private Graph graph;
    private int length;
    private int v;
    private int[] currentPath;
    private int[] nextPath;
    private boolean[] visited;
    private int i;
    private int[] inds;

    public PathIterator(Graph graph, int v, int length) {
        this.graph = graph;
        this.length = length;
        this.v = v;
        visited = new boolean[graph.order()];
        currentPath = new int[length];
        currentPath[0] = v;
        i = 0;
        visited[v] = true;
        inds = new int[graph.order()]; // inds[i] indicates next index of i's neighbor
        prepareNext();
    }

    @Override
    public boolean hasNext() {
        return nextPath != null;
    }

    @Override
    public int[] next() {
        if (!hasNext()) throw new NoSuchElementException();
        int[] path = nextPath;
        prepareNext();
        return path;
    }


    public void prepareNext() {
        boolean foundNext;
        nextPath = null;
        while (i > -1) {
            v = currentPath[i];
            if (i == length - 1) {
                if (currentPath[0] < v) {
                    nextPath = currentPath.clone();
                }
                visited[v] = false;
                i--;
                v = currentPath[i];
                if (nextPath != null) {
                    return;
                }
            }
            foundNext = false;
            while (inds[v] < graph.order() && !foundNext) {
                int u = inds[v];
                if (graph.hasEdge(v,u) && !visited[u]) {
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
}
