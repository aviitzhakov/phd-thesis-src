package uniqueHamGraphs;

import java.util.BitSet;
import java.util.List;

public class BitSetGraph implements Graph{

    private BitSet adjMatrix;
    private int n;

    public BitSetGraph(int n) {
        adjMatrix = new BitSet(n*n);
        this.n = n;
    }

    @Override
    public boolean hasEdge(int i, int j) {
        return adjMatrix.get(i*n + j);
    }

    @Override
    public void addEdge(int i, int j) {
        adjMatrix.set(i*n+j, true);
        adjMatrix.set(j*n+i, true);
    }

    @Override
    public void removeEdge(int i, int j) {
        adjMatrix.set(i*n+j, false);
        adjMatrix.set(j*n+i, false);
    }

    @Override
    public int degree(int v) {
        return 0;
    }

    @Override
    public List<Integer> neighbors(int v) {
        return null;
    }

    @Override
    public int order() {
        return n;
    }


}
