package uniqueHamGraphs;

import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Collection;

public class ExperimentStats {


    private double time;
    private Collection<Graph> solutions;

    public ExperimentStats() {
        time = 0;
        solutions = new ArrayList<>();
    }

    public void addTime(double t) {
        time += t;
    }

    public void addSolution(Graph sol) {
        Graph g = new UndirectedGraph(sol.order());
        for (int i = 0; i < sol.order(); i++) {
            for (int j = i + 1; j < sol.order(); j++) {
                if (sol.hasEdge(i,j)) {
                    g.addEdge(i,j);
                }
            }
        }
        solutions.add(g);
    }

    public double getTotalTime() {
        return time;
    }

    public void print(PrintStream stream) {
        stream.format("total time: %.2f sec.\n", time);
        stream.println("solutions: " + solutions.size());
        int k = 1;
        for (Graph g : solutions) {
            stream.println("SOL #" + k + ":" + g);
            k++;
        }
    }

}
