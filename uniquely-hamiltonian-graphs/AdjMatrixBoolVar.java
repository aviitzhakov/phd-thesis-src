package uniqueHamGraphs;

import org.chocosolver.solver.ICause;
import org.chocosolver.solver.Model;
import org.chocosolver.solver.exception.ContradictionException;
import org.chocosolver.solver.variables.impl.BoolVarImpl;
import org.chocosolver.util.objects.graphs.UndirectedGraph;

public class AdjMatrixBoolVar extends BoolVarImpl {

    private UndirectedGraph graph;
    private int[] e;

    public AdjMatrixBoolVar(int i, int j, Model model, UndirectedGraph btGraph) {
        super("X" + i + "," + j, model);
        this.graph = btGraph;
        e = new int[] {i,j};
    }

    public int[] getEdge(){
        return e;
    }

    @Override
    public boolean instantiateTo(int value, ICause cause) throws ContradictionException {
        boolean ans =  super.instantiateTo(value, cause);
        if (value == 1){
            graph.addEdge(e[0],e[1]);
        }
        return ans;
    }

}
