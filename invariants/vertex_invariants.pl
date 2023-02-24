:- module(vertex_invariants,
    [
	inv_triangles/3,
	inv_dist2/3
    ]).


inv_triangles(AdjMatrix,Counts,Cs1-Cs3) :-
    triangles_reif_map(AdjMatrix,Map,Cs1-Cs2),
    length(AdjMatrix,N),
    length(Counts,N),
    inv_triangles_iter(1,N,AdjMatrix,Map,Counts,Cs2-Cs3).
    

inv_triangles_iter(_,_,_,_,[],Cs-Cs) :-!.
inv_triangles_iter(I,N,AdjMatrix,Map,[TriangleCount|Counts],Cs1-Cs3):-
    findall(t(A,B,C),(between(1,N,A),between(A,N,B), A < B,between(B,N,C), B < C, member(I,[A,B,C])),Keys),
    length(Keys,Max),
    map_get_values(Map,Keys,Vars),
    Cs1 = [new_int(TriangleCount,0,Max),bool_array_sum_eq(Vars,TriangleCount)|Cs2],
    I1 is I+1,!,
    inv_triangles_iter(I1,N,AdjMatrix,Map,Counts,Cs2-Cs3).

    
    
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
    
map_get_values(_,[],[]) :- !.
map_get_values(Map,[K|Keys],[V|Values]) :-
    map_get_value(Map,K,V),!,
    map_get_values(Map,Keys,Values).
    
map_get_value([Key-X|_],Key,X) :- !.
map_get_value([_|Map],Key,X) :-
    map_get_value(Map,Key,X).
    

row_sums([],[],Cs-Cs) :- !.
row_sums([R|Rs],[I|Is],Cs1-Cs3) :-
    length(R,N),
    Cs1 = [new_int(I,0,N), bool_array_sum_eq(R,I)|Cs2],!,
    row_sums(Rs,Is,Cs2-Cs3).
    
inv_dist2(AdjMatrix, Dist2Inv, Cs1-Cs3) :-
    dist2_matrix(AdjMatrix, D2Matrix, Cs1-Cs2),
    row_sums(D2Matrix, Dist2Inv, Cs2-Cs3).
    
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
    
pairs(N,Pairs) :-
    findall(p(I,J),(between(1,N,I),between(I,N,J),I < J),Pairs).

shorten(_,_,Row, Row) :-
    length(Row,1).

shorten(I,J,Row,Vec) :-
    nth1(J,Row,_,Tmp),
    nth1(I,Tmp,_,Vec).

bitwise_and([],[],[],Cs-Cs) :-!.
bitwise_and([X|Xs],[Y|Ys],[Z|Zs],Cs1-Cs3) :-
    Cs1 = [bool_array_and_reif([X,Y],Z)|Cs2],!,
    bitwise_and(Xs,Ys,Zs,Cs2-Cs3).
    