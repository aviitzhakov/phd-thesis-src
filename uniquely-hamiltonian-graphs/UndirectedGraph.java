package uniqueHamGraphs;

import java.util.*;

public class UndirectedGraph implements Graph {

    private boolean[][] adjMatrix;

    public UndirectedGraph(int order) {
        adjMatrix = new boolean[order][order];
    }


    @Override
    public boolean hasEdge(int u, int v) {
        return adjMatrix[u][v];
    }

    @Override
    public void addEdge(int u, int v) {
        adjMatrix[u][v] = true;
        adjMatrix[v][u] = true;
    }

    @Override
    public void removeEdge(int u, int v) {
        adjMatrix[u][v] = false;
        adjMatrix[v][u] = false;
    }

    @Override
    public int degree(int v) {
        int deg = 0;
        for (boolean t : adjMatrix[v]) {
            if (t) {deg++;}
        }
        return deg;
    }

    @Override
    public List<Integer> neighbors(int v) {
        List<Integer> ns = new ArrayList<>();
        for (int u = 0; u < order(); u++) {
            if (adjMatrix[u][v]) {
                ns.add(u);
            }
        }
        return ns;
    }

    @Override
    public int order() {
        return adjMatrix.length;
    }


    public String toString() {

        int[][] intMatrix = new int[order()][order()];
        for (int i = 0; i < order(); i++) {
            for (int j = 0; j < order(); j++) {
                intMatrix[i][j] = hasEdge(i,j)? 1 : 0;
            }
        }
        return Arrays.deepToString(intMatrix);
    }
}
