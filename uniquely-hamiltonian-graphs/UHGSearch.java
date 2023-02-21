package uniqueHamGraphs;

import org.chocosolver.solver.ICause;
import org.chocosolver.solver.Model;
import org.chocosolver.solver.Solver;
import org.chocosolver.solver.constraints.Constraint;
import org.chocosolver.solver.exception.ContradictionException;
import org.chocosolver.solver.variables.BoolVar;
import org.chocosolver.util.ESat;

import java.io.PrintStream;
import java.util.*;

public class UHGSearch implements ICause{


    protected int n;
    protected Model model;
    protected UnknownGraph graph;
    protected boolean show;
    protected int splitLevel;
    protected int split;


    public UHGSearch(int n) {
        this.n = n;
        this.model = new Model("Unique Hamiltonian");
        this.graph = new UnknownGraph(n, model);
        show = false;
        fixHamCycle();
        //model.post(new Constraint("unique hamiltonian cycle" + n, new PropNoHamCycleStartsWithEdge(graph)));
        model.post(new Constraint("unique hamiltonian cycle" + n, new PropOptimized(graph)));
        splitLevel = 3;
        split = -1;
    }

    private void fixHamCycle(){
        for (int i = 0; i <= n - 1; i++) {
            graph.addEdge(i % n, (i + 1) % n);
        }
    }

    public void girthAtLeast(int k) {
        for (int i = 3; i < k; i++) {
            model.post(new Constraint("no " + i + "-cycle", new PropNoCycle(graph, i)));
        }
    }


    public void maxNumberOfChords(int m) {
        BoolVar[] vars = new BoolVar[(n*(n-1))/2 - n];
        int index = 0;
        for (BoolVar v : graph.allVars()) {
            if (!v.isInstantiated()) {
                vars[index++] = v;
            }
        }
        model.post(model.sum(vars, "<",  m + 1));
    }


    public void minNumberOfChords(int m) {

        BoolVar[] vars = new BoolVar[(n*(n-1))/2 - n];
        int index = 0;
        for (BoolVar v : graph.allVars()) {
            if (!v.isInstantiated()) {
                vars[index++] = v;
            }
        }
        model.post(model.sum(vars, ">", m - 1));
    }

    public void numberOfChords(int m) {
        BoolVar[] vars = new BoolVar[(n*(n-1))/2 - n];
        int index = 0;
        for (BoolVar v : graph.allVars()) {
            if (!v.isInstantiated()) {
                vars[index++] = v;
            }
        }
        model.post(model.sum(vars, "=", m));
    }

    public void symmetryBreaking() {
        Collection<int[]> perms = Utils.dihedralGroupPermutations(n);
        for (int[] p : perms) {
            BoolVar[] vec1 = new BoolVar[(n*(n-1))/2];
            BoolVar[] vec2 = new BoolVar[(n*(n-1))/2];
            int k = 0;
            for (int i = 0; i < n; i++) {
                for (int j = i+1; j < n; j++) {
                    vec1[k] = graph.getEdgeVar(i,j);
                    vec2[k] = graph.getEdgeVar(p[i], p[j]);
                    k++;
                }
            }
            model.lexLessEq(vec1, vec2).post();
        }
    }



    public void shortestChord(int k){
        // no chord of length < k
        for (int len = 2; len < k; len++) {
            for (int i = 0; i < graph.order(); i++) {
                int a = (i - len + n) % n;
                int b = i;
                try {
                    graph.fixEdgeValue(a,b,false, this);
                } catch (ContradictionException e) {
                    e.printStackTrace();
                }
            }
        }

        // at least one chord of length k
        Collection<BoolVar> kchords = new ArrayList<>();
        for (int i = 0; i < graph.order(); i++) {
            int a = (i - k + n) % n;
            int b = i;
            kchords.add(graph.getEdgeVar(a,b));
        }
        BoolVar[] vars = new BoolVar[kchords.size()];
        int index = 0;
        for (BoolVar b : kchords) {
            vars[index++] = b;
        }
        model.or(vars).post();
    }

    public void longestChord(int k){

        // no chord of length > k
        for (int len = k + 1; len <= graph.order()/ 2 ; len++) {
            for (int i = 0; i < graph.order(); i++) {
                int a = (i - len + n) % n;
                int b = i;
                try {
                    graph.fixEdgeValue(a,b,false, this);
                } catch (ContradictionException e) {
                    e.printStackTrace();
                }
            }
        }

        // at least one chord of length k
        Collection<BoolVar> kchords = new ArrayList<>();
        for (int i = 0; i < graph.order(); i++) {
            int a = (i - k + n) % n;
            int b = i;
            kchords.add(graph.getEdgeVar(a,b));
        }
        BoolVar[] vars = new BoolVar[kchords.size()];
        int index = 0;
        for (BoolVar b : kchords) {
            vars[index++] = b;
        }
        model.or(vars).post();
    }


    public void atMostOneChord(int k) {
        // at most one chord of length k
        Collection<BoolVar> kchords = new ArrayList<>();
        for (int i = 0; i < graph.order(); i++) {
            int a = (i - k + n) % n;
            int b = i;
            kchords.add(graph.getEdgeVar(a,b));
        }
        BoolVar[] vars = new BoolVar[kchords.size()];
        int index = 0;
        for (BoolVar b : kchords) {
            vars[index++] = b;
        }
        model.post(model.sum(vars,"<=", 1));
    }


    public void noSplittingChord(int e0, int e1) {
        int k = e1 - e0;
        if (k < 2 || k == graph.order() - 1) return;
        int cycleLen  = k + 1;
        if (cycleLen % 2 == 0) {
            int h = cycleLen/2;
            for (int i = e0; i + h <= e1; i++) {
                BoolVar[] bin = new BoolVar[2];
                bin[0] = graph.getEdgeVar(e0,e1);
                bin[1] = graph.getEdgeVar(i,i+h);
                model.post(model.sum(bin,"<=",1));
                //graph.fixEdgeValue(i, i+h, false, this);
            }

        }

        k = e0 + n - e1;
        cycleLen = k + 1;
        if (cycleLen % 2 == 0) {
            int h = cycleLen/2;
            for (int i = e1; (i + h ) % n < e0 && i < graph.order(); i++) {
                BoolVar[] bin = new BoolVar[2];
                bin[0] = graph.getEdgeVar(e0,e1);
                bin[1] = graph.getEdgeVar(i,(i+h) % n);
                model.post(model.sum(bin,"<=",1));
                //graph.fixEdgeValue(i, (i+h) % n, false, this);
            }

        }
    }


    public int solveAll(PrintStream printStream) {

        Solver s = model.getSolver();
        s.setOut(printStream);
        s.setSearch(new SearchStrategyUHG(graph));
        int cnt = 0;
        if (show) s.showSolutions();

        while(s.solve()) {
            cnt++;
            //System.out.println(graph.getCurrentFixedGraph().toString());
        }

        if (split > - 1) {
            printStream.println("case #" + split + ", " + "time = " + s.getTimeCount() + ", " + "solutions = " + s.getSolutionCount());
        }
        else {
            s.printStatistics();
        }
        return cnt;
    }

    public boolean solve(PrintStream printStream) {

        Solver s = model.getSolver();
        s.setOut(printStream);
        s.setSearch(new SearchStrategyUHG(graph));

        boolean ans = s.solve();
        s.printStatistics();

        return ans;
    }

    public void fixDegrees(int[] ds) {
        int k;
        for (int i = 0; i < n; i++) {
            BoolVar[] row = new BoolVar[n-3];
            k = 0;
            for (int j = 0; j < n; j++) {
                if (i != j && graph.edgeStatus(i,j) == ESat.UNDEFINED) {
                    row[k++] = graph.getEdgeVar(i,j);
                }
            }
            model.sum(row, "=", ds[i] - 2).post();
        }
    }

    public void minDegree(int d) {
        int k;
        for (int i = 0; i < n; i++) {
            BoolVar[] row = new BoolVar[n-3];
            k = 0;
            for (int j = 0; j < n; j++) {
                if (i != j && graph.edgeStatus(i,j) == ESat.UNDEFINED) {
                    row[k++] = graph.getEdgeVar(i,j);
                }
            }
            model.sum(row, ">", d - 3).post();
        }
    }

    public void maxDegree(int d) {
        int k;
        for (int i = 0; i < n; i++) {
            BoolVar[] row = new BoolVar[n-3];
            k = 0;
            for (int j = 0; j < n; j++) {
                if (i != j && graph.edgeStatus(i,j) == ESat.UNDEFINED) {
                    row[k++] = graph.getEdgeVar(i,j);
                }
            }
            model.sum(row, "<", d - 1).post();
        }
    }

    public void setCase(int num) {
        split = num;
        int[] vec = numToChordSeq(num);
        //System.out.println(Arrays.toString(vec));
        int k = 2;
        for (int count : vec) {
            numberOfChordsOfLengthK(k,count);
            k++;
        }
    }

    private int[] numToChordSeq(int num) {
        int[] v = new int[splitLevel];
        int i = splitLevel - 1;
        int base = graph.order() + 1;
        while (num > 0) {
            v[i--] = num % base;
            num = num/base;
        }
        return v;
    }


    private void numberOfChordsOfLengthK(int k, int cnt) {
        Set<BoolVar> kchords = new HashSet<>();
        for (int i = 0; i < graph.order(); i++) {
            int a = (i - k + n) % n;
            int b = i;
            kchords.add(graph.getEdgeVar(a,b));
        }
        BoolVar[] vars = new BoolVar[kchords.size()];
        int index = 0;
        for (BoolVar b : kchords) {
            vars[index++] = b;
        }
        model.sum(vars,"=", cnt).post();
    }


    public void showSolutions() {
        show = true;
    }

    public void setSplitLevel(int val) {
        splitLevel = val;
    }

    public void regular(int deg) {
        int[] ds = new int[graph.order()];
        for (int i = 0; i < ds.length; i++) {
            ds[i] = deg;
        }
        fixDegrees(ds);
    }
}
