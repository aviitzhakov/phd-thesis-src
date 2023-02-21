package uniqueHamGraphs;

import org.chocosolver.solver.ICause;
import org.chocosolver.solver.Model;
import org.chocosolver.solver.exception.ContradictionException;
import org.chocosolver.solver.variables.BoolVar;
import org.chocosolver.util.ESat;
import org.chocosolver.util.objects.graphs.UndirectedGraph;
import org.chocosolver.util.objects.setDataStructures.ISet;
import org.chocosolver.util.objects.setDataStructures.SetType;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

public class UnknownGraph implements Graph, ICause{

    private AdjMatrixBoolVar[][] adjMatrix;
    private Model model;
    private UndirectedGraph currentFixedGraph;

    public UnknownGraph(int n, Model model) {
        adjMatrix = new AdjMatrixBoolVar[n][n];
        this.model = model;
        this.currentFixedGraph = new UndirectedGraph(model, n, SetType.LINKED_LIST, true);
        for (int i = 0; i < n; i++) {
            for (int j = i + 1 ; j < n; j++) {
                adjMatrix[i][j] = new AdjMatrixBoolVar(i, j, model, currentFixedGraph);
                adjMatrix[j][i] = adjMatrix[i][j];
            }
        }
    }

    public int order() {
        return adjMatrix.length;
    }

    public ESat edgeStatus(int i, int j) {
        if (i == j) {
            return ESat.FALSE;
        }
        return adjMatrix[i][j].getBooleanValue();
    }

    public void fixEdgeValue(int i, int j, boolean val, ICause cause) throws ContradictionException {
        adjMatrix[i][j].instantiateTo(val ? 1 : 0, cause);
    }

    public BoolVar getEdgeVar(int i, int j) {
        return adjMatrix[i][j];
    }


    public int[] varToEdge(BoolVar x) {
        return ((AdjMatrixBoolVar)x).getEdge();
    }

    public BoolVar[] allVars() {
        BoolVar[] vars = new BoolVar[(adjMatrix.length * (adjMatrix.length - 1))/2];
        int k = 0;
        for (int i = 0; i < adjMatrix.length; i++) {
            for (int j = i + 1; j < adjMatrix.length; j++) {
                vars[k++] = adjMatrix[i][j];
            }
        }
        return vars;
    }

    @Override
    public boolean hasEdge(int u, int v) {
        return currentFixedGraph.edgeExists(u,v);
    }

    @Override
    public void addEdge(int u, int v) {
        try {
            fixEdgeValue(u,v,true, this);
        } catch (ContradictionException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void removeEdge(int u, int v) {
        try {
            fixEdgeValue(u,v,false, this);
        } catch (ContradictionException e) {
            e.printStackTrace();
        }
    }

    public int degree(int v) {
        return currentFixedGraph.getNeighOf(v).size();
    }

    @Override
    public List<Integer> neighbors(int v) {
        throw  new UnsupportedOperationException();
    }

    public ISet neighborsOf(int v) {
        return currentFixedGraph.getNeighOf(v);
    }


    public UndirectedGraph getCurrentFixedGraph() {
        return currentFixedGraph;
    }

}
