:- module(pair_invariants,
    [
	inv_min_deg/3, 
	inv_max_deg/3, 
	inv_common/3, 
	inv_minus_common/3,
	inv_anti_common/3,
	inv_adj/3,
	inv_len3_paths/3, 
	inv_triangles/3, 
	inv_dist2/3,
	inv_minus_dist2/3,
	inv_sum_degrees_common_neighbors/4, 
	inv_matrices_lexstar/2,
	inv_matrices_lexplus/2,
	cond_inv_matrices_lexstar/3,
	cond_inv_matrices_lexplus/3
    ]).


:- use_module(bee('beeCompiler.lib/auxs.Avi/graphSearch'),[]).


isomorphic_solutions(N) :-
    auxAdjacencyMatrix:createAdjacencyMatrix(N,-1,G),
    auxAdjacencyMatrix:createAdjacencyMatrix(N,-1,H),
    pair_inv_sb(G,Cs-Cs1),
    pair_inv_sb(H,Cs1-Cs2),
    isomorphism:isomorphic(G,H,_,Cs2-Cs3),
    lex_permutations:matrix_upper_triangle(G,Xs),
    lex_permutations:matrix_upper_triangle(H,Ys),
    Cs3 = [bool_arrays_lexLt(Xs,Ys)],
    bCompiler:bCompile(Cs,CNF),
    satsolver:sat(CNF),
    auxMatrix:matrixPrint(G),
    writeln('------'),
    auxMatrix:matrixPrint(H).

test_symbreak(N) :-
    auxAdjacencyMatrix:createAdjacencyMatrix(N,-1,AdjMatrix),
    pair_inv_sb(AdjMatrix,Cs-[]),
    graphSearch:findall_graphs(N,Graphs,[adj(AdjMatrix),constraints(Cs)]),
    maplist(densenauty:canonic_graph(N), Graphs, GraphsTmp1),
    sort(GraphsTmp1, SolsModIso),
    length(SolsModIso,Count),
    writeln(Count).


pair_inv_sb(AdjMatrix,Cs1-Cs7) :-
    auxAdjacencyMatrix:degrees(AdjMatrix,Ds,Cs1-Cs2),
    inv_min_deg(Ds,MinDegMatrix,Cs2-Cs3),
    inv_max_deg(Ds,MaxDegMatrix,Cs3-Cs4),
    inv_common(AdjMatrix,CommonMatrix,Cs4-Cs5),
    inv_adj(AdjMatrix,InvAdjMatrix,Cs5-Cs6),
    inv_matrices_lexstar([MinDegMatrix,MaxDegMatrix,InvAdjMatrix,CommonMatrix],Cs6-Cs7).
        



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% min between degree of i and j
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
inv_min_deg(Ds,MinDegMatrix,Cs1-Cs2) :-
    length(Ds,N),
    auxAdjacencyMatrix:createAdjacencyMatrix(N,0,MinDegMatrix),
    pairs(N,Pairs),
    inv_min_deg(Pairs,Ds,MinDegMatrix,Cs1-Cs2).
    
inv_min_deg([],_,_,Cs-Cs) :- !.    
inv_min_deg([p(I,J)|Pairs],Ds,Matrix,Cs1-Cs3) :-
    auxMatrix:matrixGetCell(Matrix,I,J,Xmin),
    length(Ds,N),
    nth1(I,Ds,Di),
    nth1(J,Ds,Dj),
    N1 is N-1,
    Cs1 = [new_int(Xmin,0,N1),int_min(Di,Dj,Xmin)|Cs2],!,
    inv_min_deg(Pairs,Ds,Matrix,Cs2-Cs3).
    
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% max between degree of i and j
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
inv_max_deg(Ds,MinDegMatrix,Cs1-Cs2) :-
    length(Ds,N),
    auxAdjacencyMatrix:createAdjacencyMatrix(N,0,MinDegMatrix),
    pairs(N,Pairs),
    inv_max_deg(Pairs,Ds,MinDegMatrix,Cs1-Cs2).
    
inv_max_deg([],_,_,Cs-Cs) :- !.    
inv_max_deg([p(I,J)|Pairs],Ds,Matrix,Cs1-Cs3) :-
    auxMatrix:matrixGetCell(Matrix,I,J,Xmax),
    length(Ds,N),
    nth1(I,Ds,Di),
    nth1(J,Ds,Dj),
    N1 is N-1,
    Cs1 = [new_int(Xmax,0,N1),int_max(Di,Dj,Xmax)|Cs2],!,
    inv_max_deg(Pairs,Ds,Matrix,Cs2-Cs3).
    
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% common neighbors of i and j
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
inv_non_common(AdjMatrix,CommonMatrix,Cs1-Cs2) :-
    length(AdjMatrix,N),
    auxAdjacencyMatrix:createAdjacencyMatrix(N,0,CommonMatrix),
    pairs(N,Pairs),
    inv_non_common(Pairs,AdjMatrix,CommonMatrix,Cs1-Cs2).
        
        
inv_non_common([],_,_,Cs-Cs) :- !.    
inv_non_common([p(I,J)|Pairs],AdjMatrix,Matrix,Cs1-Cs4) :-
    auxMatrix:matrixGetCell(Matrix,I,J,Xcommon),
    length(AdjMatrix,N),
    nth1(I,AdjMatrix,Ri),
    nth1(J,AdjMatrix,Rj),
    shorten(I,J,Ri,Vec1),
    shorten(I,J,Rj,Vec2),
    bitwise_nand(Vec1,Vec2,Xs,Cs1-Cs2),
    N1 is N-1,
    Cs2 = [
    	new_int(Xcommon,0,N1),
	bool_array_sum_eq(Xs,Xcommon)
    |Cs3],!,
    inv_non_common(Pairs,AdjMatrix,Matrix,Cs3-Cs4).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% common neighbors of i and j
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
inv_common(AdjMatrix,CommonMatrix,Cs1-Cs2) :-
    length(AdjMatrix,N),
    auxAdjacencyMatrix:createAdjacencyMatrix(N,0,CommonMatrix),
    pairs(N,Pairs),
    inv_common(Pairs,AdjMatrix,CommonMatrix,Cs1-Cs2).
        
        
inv_common([],_,_,Cs-Cs) :- !.    
inv_common([p(I,J)|Pairs],AdjMatrix,Matrix,Cs1-Cs4) :-
    auxMatrix:matrixGetCell(Matrix,I,J,Xcommon),
    length(AdjMatrix,N),
    nth1(I,AdjMatrix,Ri),
    nth1(J,AdjMatrix,Rj),
%     shorten(I,J,Ri,Vec1),
%     shorten(I,J,Rj,Vec2),
    bitwise_and(Ri,Rj,Xs,Cs1-Cs2),
    N1 is N-1,
    Cs2 = [
    	new_int(Xcommon,0,N1),
	bool_array_sum_eq(Xs,Xcommon)
    |Cs3],!,
    inv_common(Pairs,AdjMatrix,Matrix,Cs3-Cs4).


inv_minus_common(AdjMatrix,MinusCommonMatrix,Cs1-Cs3):-
     inv_common(AdjMatrix,CommonMatrix,Cs1-Cs2),
     multiply_matrix_by_const(CommonMatrix,-1,MinusCommonMatrix,Cs2-Cs3).
    
    
multiply_matrix_by_const([],_,[],Cs-Cs) :- !.
multiply_matrix_by_const([R|Rs],C,[R1|Rs1],Cs1-Cs3):-
    multiply_row_by_const(R,C,R1,Cs1-Cs2),!,
    multiply_matrix_by_const(Rs,C,Rs1,Cs2-Cs3).
    

multiply_row_by_const([],_,[],Cs-Cs) :-!.
multiply_row_by_const([I|Is],C,[J|Js],Cs1-Cs3) :-
    Cs1 = [new_int_mulK(J,I,-1)|Cs2],!,
    multiply_row_by_const(Is,C,Js,Cs2-Cs3).
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% adjacency
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
inv_adj(AdjMatrix,Matrix,Cs1-Cs2) :-
    length(AdjMatrix,N),
    auxAdjacencyMatrix:createAdjacencyMatrix(N,0,Matrix),
    pairs(N,Pairs),
    inv_adj(Pairs,AdjMatrix,Matrix,Cs1-Cs2).
        
        
inv_adj([],_,_,Cs-Cs) :- !.    
inv_adj([p(I,J)|Pairs],AdjMatrix,Matrix,Cs1-Cs3) :-
    auxMatrix:matrixGetCell(Matrix,I,J,Xadj),
    auxMatrix:matrixGetCell(AdjMatrix,I,J,Xij),
    
    Cs1 = [
    	new_int(Xadj,0,1),
    	int_eq_reif(Xadj,1,Xij)
    |Cs2],!,
    inv_adj(Pairs,AdjMatrix,Matrix,Cs2-Cs3).
    
    
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% common non-neighbors of i and j
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

inv_anti_common(AdjMatrix,CommonMatrix,Cs1-Cs2) :-
    inv_common_non_neighbors(AdjMatrix,CommonMatrix,Cs1-Cs2).

inv_common_non_neighbors(AdjMatrix,CommonMatrix,Cs1-Cs2) :-
    length(AdjMatrix,N),
    auxAdjacencyMatrix:createAdjacencyMatrix(N,0,CommonMatrix),
    pairs(N,Pairs),
    inv_common_non_neighbors(Pairs,AdjMatrix,CommonMatrix,Cs1-Cs2).
        
        
inv_common_non_neighbors([],_,_,Cs-Cs) :- !.    
inv_common_non_neighbors([p(I,J)|Pairs],AdjMatrix,Matrix,Cs1-Cs4) :-
    auxMatrix:matrixGetCell(Matrix,I,J,Xcommon),
    length(AdjMatrix,N),
    nth1(I,AdjMatrix,Ri),
    nth1(J,AdjMatrix,Rj),
    shorten(I,J,Ri,Vec1),
    shorten(I,J,Rj,Vec2),
    bitwise_nor(Vec1,Vec2,Xs,Cs1-Cs2),
    N1 is N-1,
    Cs2 = [
    	new_int(Xcommon,0,N1),
	bool_array_sum_eq(Xs,Xcommon)
    |Cs3],!,
    inv_common_non_neighbors(Pairs,AdjMatrix,Matrix,Cs3-Cs4).
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% sum of degrees of common neighbors of i and j
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
inv_sum_degrees_common_neighbors(AdjMatrix,Ds,CommonMatrix,Cs1-Cs2) :-
    length(AdjMatrix,N),
    auxAdjacencyMatrix:createAdjacencyMatrix(N,0,CommonMatrix),
    pairs(N,Pairs),
    inv_sum_degrees_common_neighbors(Pairs,AdjMatrix,Ds,CommonMatrix,Cs1-Cs2).
        
        
inv_sum_degrees_common_neighbors([],_,_,_,Cs-Cs) :- !.    
inv_sum_degrees_common_neighbors([p(I,J)|Pairs],AdjMatrix,Ds,Matrix,Cs1-Cs4) :-
    auxMatrix:matrixGetCell(Matrix,I,J,Xsum),
    length(AdjMatrix,N),
    nth1(I,AdjMatrix,Ri),
    nth1(J,AdjMatrix,Rj),
    shorten(I,J,Ri,Vec1),
    shorten(I,J,Rj,Vec2),
    shorten(I,J,Ds,VecDs),
    bitwise_nand(Vec1,Vec2,Xs,Cs1-Cs2),
    N1 is (N-1)*(N-1),
    Cs2 = [
    	new_int(Xsum,0,N1),
    	int_array_sumCond_eq(Xs,VecDs,Xsum)
    |Cs3],!,
    inv_sum_degrees_common_neighbors(Pairs,AdjMatrix,Ds,Matrix,Cs3-Cs4).
    
    
    
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% len3 paths
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
inv_len3_paths(AdjMatrix,Len3PathsMatrix,Cs1-Cs2) :-
    length(AdjMatrix,N),
    auxAdjacencyMatrix:createAdjacencyMatrix(N,0,Len3PathsMatrix),
    pairs(N,Pairs),
    inv_len3_paths(Pairs,AdjMatrix,Len3PathsMatrix,Cs1-Cs2).
        
        
inv_len3_paths([],_,_,Cs-Cs) :- !.    
inv_len3_paths([p(I,J)|Pairs],AdjMatrix,Matrix,Cs1-Cs4) :-
    auxMatrix:matrixGetCell(Matrix,I,J,Xcnt),
    length(AdjMatrix,N),
    findall(p(I,A,B,J),(between(1,N,A),between(1,N,B),A \= B, A \= I, A \= J, B \= I, B \= J),Paths),
    len3_paths_reifs(Paths,AdjMatrix,Ts,Cs1-Cs2),
    negate(Ts,NegTs),
    N2 is N*N,
    Cs2 = [new_int(Xcnt,0,N2), bool_array_sum_eq(NegTs,Xcnt)|Cs3],
    inv_len3_paths(Pairs,AdjMatrix,Matrix,Cs3-Cs4).

    
len3_paths_reifs([],_,[],Cs-Cs) :-!.
len3_paths_reifs([p(I,A,B,J)|Paths],AdjMatrix,[T|Ts],Cs1-Cs3) :-
    auxMatrix:matrixGetCell(AdjMatrix,I,A,X1),
    auxMatrix:matrixGetCell(AdjMatrix,A,B,X2),
    auxMatrix:matrixGetCell(AdjMatrix,B,J,X3),
    Cs1 = [bool_array_and_reif([X1,X2,X3],T)|Cs2],!,
    len3_paths_reifs(Paths,AdjMatrix,Ts,Cs2-Cs3).
    
    
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% adjacent common neighbors
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
inv_adjacent_common_neighbors(AdjMatrix,Matrix,Cs1-Cs2) :-
    length(AdjMatrix,N),
    auxAdjacencyMatrix:createAdjacencyMatrix(N,0,Matrix),
    pairs(N,Pairs),
    inv_adjacent_common_neighbors(Pairs,AdjMatrix,Matrix,Cs1-Cs2).
        
        
inv_adjacent_common_neighbors([],_,_,Cs-Cs) :- !.    
inv_adjacent_common_neighbors([p(I,J)|Pairs],AdjMatrix,Matrix,Cs1-Cs4) :-
    auxMatrix:matrixGetCell(Matrix,I,J,Xcnt),
    length(AdjMatrix,N),
    findall(p(I,A,B,J),(between(1,N,A),between(A,N,B),A < B, A \= I, A \= J, B \= I, B \= J),Paths),
    adjacent_common_neighbors_reifs(Paths,AdjMatrix,Ts,Cs1-Cs2),
    negate(Ts,NegTs),
    N2 is ((N-2)*(N-3))/2,
    Cs2 = [new_int(Xcnt,0,N2), bool_array_sum_eq(NegTs,Xcnt)|Cs3],
    inv_adjacent_common_neighbors(Pairs,AdjMatrix,Matrix,Cs3-Cs4).

    
adjacent_common_neighbors_reifs([],_,[],Cs-Cs) :-!.
adjacent_common_neighbors_reifs([p(I,A,B,J)|Paths],AdjMatrix,[T|Ts],Cs1-Cs3) :-
    auxMatrix:matrixGetCell(AdjMatrix,I,A,X1),
    auxMatrix:matrixGetCell(AdjMatrix,I,B,X2),
    auxMatrix:matrixGetCell(AdjMatrix,J,A,X3),
    auxMatrix:matrixGetCell(AdjMatrix,J,B,X4),
    auxMatrix:matrixGetCell(AdjMatrix,A,B,X5),
    Cs1 = [bool_array_and_reif([X1,X2,X3,X4,X5],T)|Cs2],!,
    adjacent_common_neighbors_reifs(Paths,AdjMatrix,Ts,Cs2-Cs3).
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% triangles
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

inv_triangles(AdjMatrix,Matrix,Cs1-Cs3) :-
    triangles_reif_map(AdjMatrix,TrianglesMap,Cs1-Cs2),
    length(AdjMatrix,N),
    auxAdjacencyMatrix:createAdjacencyMatrix(N,0,Matrix),
    pairs(N,Pairs),
    inv_triangles(Pairs,TrianglesMap,Matrix,Cs2-Cs3).
    
inv_triangles([],_,_,Cs-Cs) :-!.
inv_triangles([p(I,J)|Pairs],TrianglesMap,Matrix,Cs1-Cs3) :-
    length(Matrix,N),
    auxMatrix:matrixGetCell(Matrix,I,J,X),
    findall(t(A,B,C),(between(1,N,A),between(A,N,B), A<B,between(B,N,C), B < C, (member(I,[A,B,C]) ; member(J,[A,B,C]))),Keys),
    triangles_map_get_values(TrianglesMap,Keys,Vars),
    N3 is (N*(N-1)*(N-2))/3,
    negate(Vars,NegVars),
    Cs1 = [new_int(X,0,N3),bool_array_sum_eq(NegVars,X)|Cs2],!,
    inv_triangles(Pairs,TrianglesMap,Matrix,Cs2-Cs3).



inv_max_triangles(AdjMatrix,TrDegrees,Matrix,Cs1-Cs2) :-
    length(AdjMatrix,N),
    auxAdjacencyMatrix:createAdjacencyMatrix(N,0,Matrix),
    pairs(N,Pairs),
    inv_max_triangles_(Pairs,TrDegrees,Matrix,Cs1-Cs2).
        
        
inv_max_triangles_([],_,_,Cs-Cs) :- !.    
inv_max_triangles_([p(I,J)|Pairs],Ds,Matrix,Cs1-Cs3) :-
    auxMatrix:matrixGetCell(Matrix,I,J,Xmin),
    length(Ds,N),
    nth1(I,Ds,Di),
    nth1(J,Ds,Dj),
    N3 is (N*(N-1)*(N-2))/3,
    Cs1 = [new_int(Xmin,0,N3),int_max(Di,Dj,Xmin)|Cs2],!,
    inv_max_triangles_(Pairs,Ds,Matrix,Cs2-Cs3).
    
    
inv_min_triangles(AdjMatrix,TrDegrees,Matrix,Cs1-Cs2) :-
    length(AdjMatrix,N),
    auxAdjacencyMatrix:createAdjacencyMatrix(N,0,Matrix),
    pairs(N,Pairs),
    inv_min_triangles_(Pairs,TrDegrees,Matrix,Cs1-Cs2).
        
        
inv_min_triangles_([],_,_,Cs-Cs) :- !.    
inv_min_triangles_([p(I,J)|Pairs],Ds,Matrix,Cs1-Cs3) :-
    auxMatrix:matrixGetCell(Matrix,I,J,Xmin),
    length(Ds,N),
    nth1(I,Ds,Di),
    nth1(J,Ds,Dj),
    N3 is (N*(N-1)*(N-2))/3,
    Cs1 = [new_int(Xmin,0,N3),int_min(Di,Dj,Xmin)|Cs2],!,
    inv_min_triangles_(Pairs,Ds,Matrix,Cs2-Cs3).
    
triangles_degrees(AdjMatrix,TrDegrees,Cs1-Cs3) :-
    triangles_reif_map(AdjMatrix,TrianglesMap,Cs1-Cs2),
    length(AdjMatrix,N),
    length(TrDegrees,N),
    triangles_degrees_iter(1,AdjMatrix,TrianglesMap,TrDegrees,Cs2-Cs3).
    
triangles_degrees_iter(_,_,_,[],Cs-Cs) :- !.
triangles_degrees_iter(I,AdjMatrix,TrianglesMap,[D|TrDegrees],Cs1-Cs3) :-
    length(AdjMatrix,N),
    findall(t(A,B,C),(between(1,N,A),between(A,N,B), A<B,between(B,N,C), B < C, member(I,[A,B,C])),Keys),
    triangles_map_get_values(TrianglesMap,Keys,Vars),
    N3 is (N*(N-1)*(N-2))/3,
    Cs1 = [new_int(D,0,N3),bool_array_sum_eq(Vars,D)|Cs2],
    I1 is I+1,!,
    triangles_degrees_iter(I1,AdjMatrix,TrianglesMap,TrDegrees,Cs2-Cs3).
    
    
triangles_reif_map(AdjMatrix,Map,Cs1-Cs2) :-
    length(AdjMatrix,N),
    findall(t(I,J,K)-_,(between(1,N,I),between(I,N,J), I<J,between(J,N,K), J < K),Map),
    triangles_reif_map_iter(AdjMatrix,Map,Cs1-Cs2).
    
triangles_reif_map_iter(_,[],Cs-Cs) :- !.
triangles_reif_map_iter(AdjMatrix,[t(I,J,K)-T|Map],Cs1-Cs3) :-
    auxMatrix:matrixGetCell(AdjMatrix,I,J,X),
    auxMatrix:matrixGetCell(AdjMatrix,I,K,Y),
    auxMatrix:matrixGetCell(AdjMatrix,J,K,Z),
    Cs1 = [bool_array_and_reif([X,Y,Z],T)|Cs2],!,
    triangles_reif_map_iter(AdjMatrix,Map,Cs2-Cs3).
    
triangles_map_get_values(_,[],[]) :- !.
triangles_map_get_values(Map,[K|Keys],[V|Values]) :-
    triangles_map_get_value(Map,K,V),!,
    triangles_map_get_values(Map,Keys,Values).
    
triangles_map_get_value([Key-X|_],Key,X) :- !.
triangles_map_get_value([_|Map],Key,X) :-
    triangles_map_get_value(Map,Key,X).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% vertices at distance at most 2
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
inv_minus_dist2(AdjMatrix,MinusInvMatrix,Cs1-Cs3) :-
    inv_dist2(AdjMatrix,InvMatrix,Cs1-Cs2),
    multiply_matrix_by_const(InvMatrix,-1,MinusInvMatrix,Cs2-Cs3).

inv_dist2(AdjMatrix,InvMatrix,Cs1-Cs3) :-
    length(AdjMatrix,N),
    dist2_matrix(AdjMatrix,D2Matrix,Cs1-Cs2),
    pairs(N,Pairs),
    auxAdjacencyMatrix:createAdjacencyMatrix(N,0,InvMatrix),
    inv_dist2(Pairs,D2Matrix,InvMatrix,Cs2-Cs3).

        
inv_dist2([],_,_,Cs-Cs) :- !.    
inv_dist2([p(I,J)|Pairs],D2Matrix,Matrix,Cs1-Cs4) :-
    auxMatrix:matrixGetCell(Matrix,I,J,Xcount),
    length(D2Matrix,N),
    nth1(I,D2Matrix,Ri),
    nth1(J,D2Matrix,Rj),
    shorten(I,J,Ri,Vec1),
    shorten(I,J,Rj,Vec2),
    bitwise_or(Vec1,Vec2,Xs,Cs1-Cs2),
    Cs2 = [
    	new_int(Xcount,0,N),
	bool_array_sum_eq(Xs,Xcount)
    |Cs3],!,
    inv_dist2(Pairs,D2Matrix,Matrix,Cs3-Cs4).
    

dist2_matrix(AdjMatrix,D2Matrix,Cs1-Cs2) :-
    length(AdjMatrix,N),
    pairs(N,Pairs),
    auxAdjacencyMatrix:createAdjacencyMatrix(N,-1,D2Matrix),
    dist2_matrix(Pairs,AdjMatrix,D2Matrix,Cs1-Cs2).


        
dist2_matrix([],_,_,Cs-Cs) :- !.    
dist2_matrix([p(I,J)|Pairs],AdjMatrix,Matrix,Cs1-Cs4) :-
    auxMatrix:matrixGetCell(Matrix,I,J,Y),
    auxMatrix:matrixGetCell(AdjMatrix,I,J,Xij),
    nth1(I,AdjMatrix,Ri),
    nth1(J,AdjMatrix,Rj),
    shorten(I,J,Ri,Vec1),
    shorten(I,J,Rj,Vec2),
    bitwise_and(Vec1,Vec2,Xs,Cs1-Cs2),
    Cs2 = [bool_array_or_reif([Xij|Xs],Y)|Cs3],!,
    dist2_matrix(Pairs,AdjMatrix,Matrix,Cs3-Cs4).

    
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% neighbors of i or j
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
inv_neighbors(AdjMatrix,CommonMatrix,Cs1-Cs2) :-
    length(AdjMatrix,N),
    auxAdjacencyMatrix:createAdjacencyMatrix(N,0,CommonMatrix),
    pairs(N,Pairs),
    inv_neighbors(Pairs,AdjMatrix,CommonMatrix,Cs1-Cs2).
        
        
inv_neighbors([],_,_,Cs-Cs) :- !.    
inv_neighbors([p(I,J)|Pairs],AdjMatrix,Matrix,Cs1-Cs4) :-
    auxMatrix:matrixGetCell(Matrix,I,J,Xcommon),
    length(AdjMatrix,N),
    nth1(I,AdjMatrix,Ri),
    nth1(J,AdjMatrix,Rj),
    shorten(I,J,Ri,Vec1),
    shorten(I,J,Rj,Vec2),
    bitwise_or(Vec1,Vec2,Xs,Cs1-Cs2),
    N1 is N-1,
    Cs2 = [
    	new_int(Xcommon,0,N1),
	bool_array_sum_eq(Xs,Xcommon)
    |Cs3],!,
    inv_neighbors(Pairs,AdjMatrix,Matrix,Cs3-Cs4).

    
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% shortest path between i and j
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
inv_shortest_path(AdjMatrix,SPMartrix,Cs1-Cs3) :-
    encode_shortest_paths(AdjMatrix,Map,Cs1-Cs2),
    length(AdjMatrix,N),
    auxAdjacencyMatrix:createAdjacencyMatrix(N,0,SPMartrix),
    findall(p(I,J),(between(1,N,I),between(I,N,J),I<J),Pairs),
    shortest_paths_matrix(Pairs,Map,SPMartrix,Cs2-Cs3).    


shortest_paths_matrix([],_,_,Cs-Cs) :-!.
shortest_paths_matrix([p(I,J)|Pairs],Map,SPMartrix,Cs1-Cs4) :-
    length(SPMartrix,N),
    N1 is N-1,
    findall(p(I,K,N1),(between(1,N,K), K \= I, K\= J),Keys1),
    findall(p(J,K,N1),(between(1,N,K), K \= I, K\= J),Keys2),
    map_get_values(Keys1,Map,Vars1),
    map_get_values(Keys2,Map,Vars2),
    
%     negate(Vars,NegVars),
    bitwise_and(Vars1,Vars2,Ts,Cs1-Cs2),
%     negate(Ts,NegTs),
    auxMatrix:matrixGetCell(SPMartrix,I,J,DistIJ),
    Cs2 = [new_int(DistIJ,0,N1),bool_array_sum_eq(Ts,DistIJ)|Cs3],!,
    shortest_paths_matrix(Pairs,Map,SPMartrix,Cs3-Cs4).
    


encode_shortest_paths(AdjMatrix,Map, Cs1-Cs2) :-
    length(AdjMatrix,N),
    map_create(N,Map),
    assoc_to_list(Map,List),
    pairs_keys_values(List,Keys,_),
    encode_shortest_paths_iter(Keys,AdjMatrix, Map, Cs1-Cs2).


encode_shortest_paths_iter([],_, _, Cs-Cs) :-!.

encode_shortest_paths_iter([p(I,J,1)|Keys],AdjMatrix, Map, Cs1-Cs3) :- !,
    map_get_value(p(I,J,1),Map, X),
    auxMatrix:matrixGetCell(AdjMatrix,I,J,Eij),
    Cs1 = [bool_array_or([-X,Eij]),bool_array_or([X,-Eij])|Cs2],!,
    encode_shortest_paths_iter(Keys, AdjMatrix, Map, Cs2-Cs3).        
    
    
encode_shortest_paths_iter([p(I,J,L)|Keys], AdjMatrix, Map, Cs1-Cs4) :-
    length(AdjMatrix,N),
    L1 is L-1,
    findall(p(I,K,L1)-e(K,J), (between(1,N,K), K \= I, K \= J), Paths),
%     encode_clauses(p(I,J,L),Ks,AdjMatrix,Map,Cs1-Cs2),!,
    paths_reifs(Paths,AdjMatrix,Map,Ts,Cs1-Cs2),
    map_get_value(p(I,J,L1),Map,Pijl1),
    map_get_value(p(I,J,L),Map,Pijl),
    Cs2 = [bool_array_or_reif([Pijl1|Ts],Pijl)|Cs3],!,
    encode_shortest_paths_iter(Keys,AdjMatrix,Map,Cs3-Cs4).
    
paths_reifs([],_,_,[],Cs-Cs) :-!.
paths_reifs([p(I,K,L)-e(K,J)|Paths],AdjMatrix,Map,[T|Ts],Cs1-Cs3) :- 
    map_get_value(p(I,K,L),Map,Pikl),
    auxMatrix:matrixGetCell(AdjMatrix,K,J,Ekj),
    Cs1 = [bool_array_and_reif([Pikl,Ekj],T)|Cs2],!,
    paths_reifs(Paths,AdjMatrix,Map,Ts,Cs2-Cs3).



% encode_clauses(_,[],_,_,Cs-Cs) :-!.
% encode_clauses(p(I,J,L),[K|Ks],AdjMatrix,Map,Cs1-Cs3) :-
%     L1 is L-1,
%     map_get_value(p(I,J,L), Map, Pijl),
%     map_get_value(p(I,K,L1), Map, Pikl1),
%     auxMatrix:matrixGetCell(AdjMatrix,K, J, Ekj),
%     Cs1 = [bool_array_or([-Pikl1,-Ekj,Pijl])|Cs2],!,
%     encode_clauses(p(I,J,L),Ks,AdjMatrix,Map,Cs2-Cs3).
    

map_create(N,Map) :-
    N1 is N-1,
    findall(p(I,J,K), 
	(
	    between(1,N,I), 
	    between(I,N,J), 
	    I < J, 
	    between(1,N1,K)
	), Keys),
    length(Keys, Size),
    length(Vars, Size),
    pairs_keys_values(List, Keys, Vars),
    list_to_assoc(List,Map).


map_get_value(p(I,J,K),Map,X) :-
    MinIJ is min(I,J),
    MaxIJ is max(I,J),
    get_assoc(p(MinIJ,MaxIJ,K),Map,X).
    
map_get_value_iter(Key,[Key-X|_],X) :-!.

map_get_value_iter(Key,[_|Map],X) :-
    map_get_value_iter(Key,Map,X).

map_get_values([],_,[]) :-!.
map_get_values([Key|Keys],Map,[X|Xs]) :-
    map_get_value(Key,Map,X),!,
    map_get_values(Keys,Map,Xs).
    
negate([],[]) :- !.
negate([X|Xs],[-X|Ys]) :-
    negate(Xs,Ys).
    
%%%%%%%%%%%%%%%%%%
%% auxs
pairs(N,Pairs) :-
    findall(p(I,J),(between(1,N,I),between(I,N,J),I < J),Pairs).
    
inv_matrices_lexplus(MatrixList,Cs1-Cs2) :-
    MatrixList = [M|_],
    length(M,N),
    N1 is N-1,
    findall(p(I,J),(between(1,N1,I),J is I+1),Pairs),
    inv_matrices_lexstar(Pairs, MatrixList, Cs1-Cs2).
    
inv_matrices_lexstar(MatrixList,Cs1-Cs2) :-
    MatrixList = [M|_],
    length(M,N),
    findall(p(I,J),(between(1,N,I),between(I,N,J) ,I < J, J - I \= 2),Pairs),
    inv_matrices_lexstar(Pairs, MatrixList, Cs1-Cs2).
    
    
inv_matrices_lexstar([],_,Cs-Cs).
inv_matrices_lexstar([p(I,J)|List],MatrixList,Cs1-Cs3) :-
    maplist(nth1(I),MatrixList,RowsI),
    maplist(nth1(J),MatrixList,RowsJ),
    maplist(shorten(I,J),RowsI,RowsICut),
    maplist(shorten(I,J),RowsJ,RowsJCut),
    interleave_lists(RowsICut,Vec1),
    interleave_lists(RowsJCut,Vec2),
    append(Vec1,Vec1a),
    append(Vec2,Vec2a),
%   append(RowsICut,Vec1),
%   append(RowsJCut,Vec2),
    Cs1 = [int_arrays_lex(Vec1a,Vec2a)|Cs2],!,
    inv_matrices_lexstar(List,MatrixList,Cs2-Cs3).

shorten(_,_,Row, Row) :-
    length(Row,1).

shorten(I,J,Row,Vec) :-
    nth1(J,Row,_,Tmp),
    nth1(I,Tmp,_,Vec).

        
cond_inv_matrices_lexstar(MatrixList,AdjMatrix,Cs1-Cs2) :-
%     writeln('cond_inv_matrices_lexstar'),
    length(AdjMatrix,N),
    findall(p(I,J),(between(1,N,I),between(I,N,J) ,I < J, J - I \= 2),Pairs),
    maplist(to_matrix, MatrixList, MatrixList0),
    cond_inv_matrices_lexstar(Pairs, MatrixList0, AdjMatrix,Cs1-Cs2).
    
to_matrix(M,M) :-
    M = [X|_],
    is_list(X),!.

to_matrix(M,M0):-
    maplist(as_list, M, M0).

as_list(X,[X]).

cond_inv_matrices_lexplus(MatrixList,AdjMatrix,Cs1-Cs2) :-
    MatrixList = [M|_],
    length(M,N),
    N1 is N-1,
    findall(p(I,J),(between(1,N1,I),J is I+1),Pairs),
    maplist(to_matrix, MatrixList, MatrixList0),
    cond_inv_matrices_lexstar(Pairs, MatrixList0, AdjMatrix,Cs1-Cs2).
    
    
cond_inv_matrices_lexstar([],_,_,Cs-Cs).
cond_inv_matrices_lexstar([p(I,J)|List],MatrixList,AdjMatrix,Cs1-Cs3) :-
    maplist(nth1(I),MatrixList,RowsI0),
    maplist(nth1(J),MatrixList,RowsJ0),
    maplist(shorten(I,J), RowsI0, RowsI),
    maplist(shorten(I,J), RowsJ0, RowsJ),
    append(RowsI,Vec1a),
    append(RowsJ,Vec2a),
    nth1(I,AdjMatrix,AdjRowI),
    nth1(J,AdjMatrix,AdjRowJ),
    shorten(I,J,AdjRowI,Vec1b),
    shorten(I,J,AdjRowJ,Vec2b),
    append(Vec1a,Vec1b,Vec1),
    append(Vec2a,Vec2b,Vec2),
    Cs1 = [int_arrays_lex(Vec1,Vec2)|Cs2],
    cond_inv_matrices_lexstar(List,MatrixList,AdjMatrix,Cs2-Cs3).
    
    
bitwise_nand([],[],[],Cs-Cs) :-!.
bitwise_nand([X|Xs],[Y|Ys],[Z|Zs],Cs1-Cs3) :-
    Cs1 = [bool_array_or_reif([-X,-Y],Z)|Cs2],!,
    bitwise_nand(Xs,Ys,Zs,Cs2-Cs3).
    
bitwise_and([],[],[],Cs-Cs) :-!.
bitwise_and([X|Xs],[Y|Ys],[Z|Zs],Cs1-Cs3) :-
    Cs1 = [bool_array_and_reif([X,Y],Z)|Cs2],!,
    bitwise_and(Xs,Ys,Zs,Cs2-Cs3).
    
bitwise_or([],[],[],Cs-Cs) :-!.
bitwise_or([X|Xs],[Y|Ys],[Z|Zs],Cs1-Cs3) :-
    Cs1 = [bool_array_or_reif([X,Y],Z)|Cs2],!,
    bitwise_or(Xs,Ys,Zs,Cs2-Cs3).

bitwise_nor([],[],[],Cs-Cs) :-!.
bitwise_nor([X|Xs],[Y|Ys],[Z|Zs],Cs1-Cs3) :-
    Cs1 = [bool_array_and_reif([-X,-Y],Z)|Cs2],!,
    bitwise_nor(Xs,Ys,Zs,Cs2-Cs3).    

interleave_lists([L|Lists],[Firsts|Rest]) :-
    length(L,K),
    K > 0,!,
    maplist(nth1(1),[L|Lists],Firsts),
    maplist(remove_first,[L|Lists],Lists1),
    interleave_lists(Lists1,Rest).
    
interleave_lists(_,[]) :- !.

interleave([],[],[]) :-!.
interleave([A|As],[B|Bs],[[A,B]|ABs]) :-
    interleave(As,Bs,ABs).

remove_first([_|Xs],Xs) :-!.


int_arrays_eq_reif(As,Bs,T,Cs1-Cs3) :-
    pairs_eq_reifs(As,Bs,Ts,Cs1-Cs2),
    Cs2 = [bool_array_and_reif(Ts,T)|Cs3].
    
pairs_eq_reifs([],[],[],Cs-Cs) :- !. 
pairs_eq_reifs([A|As],[B|Bs],[T|Ts],Cs1-Cs3) :-
    Cs1 = [int_eq_reif(A,B,T)|Cs2],!,
    pairs_eq_reifs(As,Bs,Ts,Cs2-Cs3).