The symmetry breaking constraints in this folder are the ones that were computed in the paper:

Codish, M., Ehlers, T., Gange, G., Itzhakov, A., Stuckey, P.J.: Breaking
  symmetries with lex implications. In: Gallagher, J.P., Sulzmann, M. (eds.)
  Functional and Logic Programming - 14th International Symposium, FLOPS
  2018, Nagoya, Japan, May 9-11, 2018, Proceedings. Lecture Notes in Computer
  Science, vol. 10818, pp. 182--197. Springer (2018)

The zip file contains the CNF representation of the complete and compact symmetry breaking constraints in dimacs format.
Each symmetry breaking constraint for order 4 <= n <= 11 graphs is associated with a file g<n>_sb_leximps.dimacs.

The header of the file details the graph variables (commented out to keep the dimacs format).
The first line specifies the order of the graph, the next six lines specify the graph variables in the format "e i j k", meaning that variable k indicates whether the edge (i,j) is present in the solution.

For example, the following is the content of the file g4_sb_leximps.dimacs:

c v 4
c e 0 1 3
c e 0 2 1
c e 0 3 6
c e 1 2 8
c e 1 3 4
c e 2 3 10
p cnf 10 22
-1 2 -3 0
1 -2 -3 0
1 2 3 0
-1 -2 3 0
-4 5 -3 0
4 -5 -3 0
4 5 3 0
-4 -5 3 0
-6 7 -1 0
6 -7 -1 0
6 7 1 0
-6 -7 1 0
-8 9 -1 0
8 -9 -1 0
8 9 1 0
-8 -9 1 0
3 -6 -5 -9 0
-8 4 -7 0
-4 10 -2 0
-3 1 0
-1 6 0
-1 8 0

