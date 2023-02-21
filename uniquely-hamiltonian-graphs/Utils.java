package uniqueHamGraphs;

import java.util.*;
import org.chocosolver.util.objects.graphs.UndirectedGraph;

public class Utils {

    public static Collection<int[]> dihedralGroupPermutations(int n) {
        Collection<int[]> perms = new ArrayList<>();
        for (int i = 0; i < n; i++) {
            int[] p1 = new int[n];
            int[] p2 = new int[n];
            p1[0] = i;
            p2[0] = i;
            for (int j = 1; j < n; j++) {
                p1[j] = Utils.mod(i + j, n);
                p2[j] = Utils.mod(i - j, n);
            }
            perms.add(p1);
            perms.add(p2);
        }
        return perms;
    }


    public static int mod(int x, int n) {
        x = x % n;
        if (x < 0) x = x + n;
        return x;
    }


    public static Collection<int[]> paths(Graph graph, int length) {
        Collection<int[]> all = new ArrayDeque<>();
        for (int i = 0; i < graph.order() - 1; i++) {
            pathsIter(graph, i, length, all);
        }
        return all;
    }


    public static Collection<int[]> pathsWithEdge(Graph graph, int length, int[] e) {
        Collection<int[]> all = new ArrayDeque<>();
        for (int i = 0; i < graph.order() - 1; i++) {
            pathsIter(graph, i, length, e, all);
        }
        return all;
    }

    public static void pathsIter(Graph graph, int v, int length, int[] e, Collection<int[]> allPaths) {
        boolean[] visited = new boolean[graph.order()];
        visited[v] = true;
        int[] currentPath = new int[length];
        currentPath[0] = v;
        int i = 0;
        int[] inds = new int[graph.order()]; // inds[i] indicates next index of i's neighbor
        boolean foundNext;
        while (i > -1) {
            v = currentPath[i];
            if (i == length - 1) {
                if (currentPath[0] < v && visited[e[0]] && visited[e[1]]) {
                    allPaths.add(currentPath.clone());
                }
                visited[v] = false;
                i--;
                v = currentPath[i];
            }
            foundNext = false;
            while (inds[v] < graph.order() && !foundNext) {
                int u = inds[v];
                if (v == e[0] && !visited[e[1]]) {
                    u = e[1];
                    inds[v] = graph.order();
                }
                else if (v == e[1] && !visited[e[0]]) {
                    u = e[0];
                    inds[v] = graph.order();
                }
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

    public static void pathsIter(Graph graph, int v, int length, Collection<int[]> allPaths) {
        boolean[] visited = new boolean[graph.order()];
        visited[v] = true;
        int[] currentPath = new int[length];
        currentPath[0] = v;
        int i = 0;
        int[] inds = new int[graph.order()]; // inds[i] indicates next index of i's neighbor
        boolean foundNext;

        while (i > -1) {
            v = currentPath[i];
            if (i == length - 1) {
                if (currentPath[0] < v) {
                    allPaths.add(currentPath.clone());
                }
                visited[v] = false;
                i--;
                v = currentPath[i];
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


/*

    public static Collection<int[]> paths(uniqueHamGraphs.Graph graph, int length) {
        Collection<int[]> allPaths = new ArrayList<>();
        int[] currentPath = new int[length];
        boolean[] visited = new boolean[graph.order()];
        for (int i = 0; i < graph.order(); i++) {
            pathsFromRec(graph, i, 0, currentPath, visited, allPaths);
        }
        return allPaths;
    }
*/
    private static void pathsFromRec(Graph graph, int v, int index, int[] currentPath, boolean[] visited, Collection<int[]> allPaths) {
        visited[v] = true;
        currentPath[index] = v;
        if (index == currentPath.length - 1) {
            allPaths.add(currentPath.clone());
        }
        else {
            for (int u : graph.neighbors(v)) {
                if (!visited[u]) {
                    pathsFromRec(graph, u, index + 1, currentPath, visited, allPaths);
                }
            }
        }
        visited[v] = false;
    }


    public static boolean isSameCycle(int[] c1, int[] c2) {
        int minIndex1 = 0;
        int minIndex2 = 0;
        if (c1.length != c2.length) {
            return false;
        }

        int k = c1.length;

        for (int i = 0; i < k; i++) {
            if (c1[minIndex1] > c1[i]) {
                minIndex1 = i;
            }
            if (c2[minIndex2] > c2[i]) {
                minIndex2 = i;
            }
        }
        int s1 =  c1[mod(minIndex1-1,k)] < c1[mod(minIndex1+1,k)] ? 1 : -1;
        int s2 =  c2[mod(minIndex2-1,k)] < c2[mod(minIndex2+1,k)] ? 1 : -1;
        int j1 = minIndex1;
        int j2 = minIndex2;

        for (int i = 0; i < k; i++) {
            if (c1[j1] != c2[j2]){
                return false;
            }
            j1 = mod(j1 + s1 ,k);
            j2 = mod(j2 + s2 ,k);
        }
        return true;
    }

    public static Collection<int[]> nearlyCubicDS(int n) {
        Collection<int[]> all = new ArrayList<>();
        int[] ds = new int[n];
        for (int i = 0; i < n; i++) {
            ds[i] = 3;
        }
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                ds[i] = ds[j] = 4;
                all.add(ds.clone());
                ds[i] = ds[j] = 3;
            }
        }
        return all;
    }

    public static boolean hasHamCycleWithEdge(Graph graph, int[] e) {
        int length = graph.order();
        boolean[] visited = new boolean[length];
        visited[e[0]] = true;
        visited[e[1]] = true;
        int[] currentPath = new int[length];
        currentPath[0] = e[0];
        currentPath[1] = e[1];
        int i = 1;
        int[] inds = new int[graph.order()]; // inds[i] indicates next index of i's neighbor
        boolean foundNext;
        int v;
        while (i > 0) {
            v = currentPath[i];
            if (i == length - 1) {
                if (graph.hasEdge(currentPath[0], currentPath[i])) {
                    return true;
                }
                visited[v] = false;
                i--;
                v = currentPath[i];
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

        return false;
    }



    public static Collection<int[]> pathsFromEdge(Graph graph, int k, int[] e) {
        Collection<int[]> paths = new ArrayDeque<>();
        int length = k;
        boolean[] visited = new boolean[graph.order()];
        visited[e[0]] = true;
        visited[e[1]] = true;
        int[] currentPath = new int[length];
        currentPath[0] = e[0];
        currentPath[1] = e[1];
        int i = 1;
        int[] inds = new int[graph.order()]; // inds[i] indicates next index of i's neighbor
        boolean foundNext;
        int v;
        while (i > 0) {
            v = currentPath[i];
            if (i == length - 1) {
                paths.add(currentPath.clone());
                visited[v] = false;
                i--;
                v = currentPath[i];
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
        return paths;
    }



    public static void main(String[] args) {
        Graph g = new uniqueHamGraphs.UndirectedGraph(5);
        int v = 0;
        g.addEdge(0,1);
        g.addEdge(1,2);
        g.addEdge(2,3);
        g.addEdge(2,4);
        g.addEdge(3,4);
        g.addEdge(4,0);


    }




    public static BitSetGraph toBitSetGraph(Graph g) {
        BitSetGraph bg = new BitSetGraph(g.order());
        for (int i = 0; i < g.order(); i++) {
            for (int j = i+1; j < g.order(); j++) {
                if (g.hasEdge(i,j)){
                    bg.addEdge(i,j);
                }
            }
        }
        return bg;
    }

    public static int cycleDist(int i, int j, int n) {
        return Math.min(Utils.mod(i - j, n) , Utils.mod(j- i, n));
    }



}



