package uniqueHamGraphs;

import java.util.List;

public interface Graph {

    boolean hasEdge(int u, int v);

    void addEdge(int u, int v);

    void removeEdge(int u, int v);

    int degree(int v);

    List<Integer> neighbors(int v);

    int order();
}
