:- module(test,[table/2]).

:- use_module(bee('beeCompiler.lib/auxs.Avi/graphSearch'),[]).
:- use_module(pair_invariants,[]).
:- use_module(vertex_invariants,[]).


table(1,N) :-
    gen_lex(N).

table(2,N) :-
    gen_chain(N,[inv_deg, inv_adj]).

table(3,N) :-
    gen_chain(N,[inv_common, inv_adj]).

table(4,N) :-
    gen_chain(N,[inv_deg, inv_common, inv_adj]).

table(5,N) :-
    gen_prod(N,[inv_common, inv_adj]).

table(6,N) :-
    gen_prod(N,[inv_adj, inv_common]).
    
table(7,N) :-
    gen_prod(N,[inv_min_deg, inv_max_deg, inv_adj]).

table(8,N) :-
    gen_prod(N,[inv_adj, inv_min_deg, inv_max_deg]).

table(9,N) :-
    gen_prod(N,[inv_min_deg, inv_max_deg, inv_common, inv_adj]).

table(10,N) :-
    gen_prod(N,[inv_adj, inv_min_deg, inv_max_deg, inv_common]).

table(11,N) :-
    gen_prod(N,[inv_adj, inv_minus_common]).

str(inv_adj, 'adj').
str(inv_min_deg, 'min').
str(inv_max_deg, 'max').
str(inv_common, 'common').
str(inv_minus_common, 'minus_common').
str(inv_deg, 'deg').

str_invs(Invs,Str) :-
    str_invs(Invs,'',Str).
    
    
file_name(N,Invs,SB,Fname) :-
    str_invs(Invs,Str),
    atomic_list_concat(['gen_',N, '_',SB, Str,'.stats'],Fname).

str_invs([],Str,Str) :- !.
str_invs([I|Invs],Prefix,Str) :-
    str(I,Name),
    atomic_list_concat([Prefix, '_', Name],Prefix1),!,
    str_invs(Invs,Prefix1, Str).




require_ds([inv_min_deg|_]) :- !.
require_ds([inv_max_deg|_]) :- !.
require_ds([_|Invs]) :- 
    require_ds(Invs).

gen_chain(N,Invs) :-
    auxAdjacencyMatrix:createAdjacencyMatrix(N,-1,AdjMatrix),
    (require_ds(Invs) ->
	auxAdjacencyMatrix:degrees(AdjMatrix,Ds,Cs-Cs1)
	;
	Ds = [], Cs=Cs1
    ),
    sb_chain(Invs,AdjMatrix,Ds,Cs1-[]),
    file_name(N,Invs,chain,File),
    tell(File),
    write('stats('),
    graphSearch:count_graphs(N,_,[constraints(Cs), adj(AdjMatrix)]),
    writeln(').'),
    told.


gen_prod(N,Invs) :-
    auxAdjacencyMatrix:createAdjacencyMatrix(N,-1,AdjMatrix),
    (require_ds(Invs) ->
	auxAdjacencyMatrix:degrees(AdjMatrix,Ds,Cs-Cs1)
	;
	Ds = [], Cs=Cs1
    ),
    sb_prod(Invs,AdjMatrix,Ds,Cs1-[]),
    file_name(N,Invs,prod,File),
    tell(File),
    write('stats('),
    graphSearch:count_graphs(N,_,[constraints(Cs), adj(AdjMatrix)]),
    writeln(').'),
    told.


gen_lex(N) :-
    atomic_list_concat(['gen_',N,'_','lex','.stats'],File),
    tell(File),
    write('stats('),
    graphSearch:count_graphs(N,_,[sb(lexstar)]),
    writeln(').'),
    told.

    
inv_common(AdjMatrix,_,InvMatrix,Cs1-Cs2) :-
    pair_invariants:inv_common(AdjMatrix,InvMatrix, Cs1-Cs2).

inv_adj(AdjMatrix,_,InvMatrix,Cs1-Cs2) :-
    pair_invariants:inv_adj(AdjMatrix,InvMatrix, Cs1-Cs2).

inv_min_deg(_,Ds,InvMatrix,Cs1-Cs2) :-
    pair_invariants:inv_min_deg(Ds,InvMatrix, Cs1-Cs2).

inv_max_deg(_,Ds,InvMatrix,Cs1-Cs2) :-
    pair_invariants:inv_max_deg(Ds,InvMatrix, Cs1-Cs2).

inv_deg(AdjMatrix,_,Ds,Cs1-Cs2) :-
    auxAdjacencyMatrix:degrees(AdjMatrix,Ds, Cs1-Cs2).

inv_minus_common(AdjMatrix,_,InvMatrix,Cs1-Cs2) :-
    pair_invariants:inv_minus_common(AdjMatrix,InvMatrix, Cs1-Cs2).

sb_chain(Invs,AdjMatrix,Ds,Cs1-Cs3) :-
    compose(Invs,AdjMatrix,Ds,Ms,Cs1-Cs2),
    append(Ms1,[InvAdjMatrix],Ms),
    pair_invariants:cond_inv_matrices_lexstar(Ms1,InvAdjMatrix,Cs2-Cs3).

sb_prod(Invs,AdjMatrix,Ds,Cs1-Cs3) :-
    compose(Invs,AdjMatrix,Ds,Ms,Cs1-Cs2),
    pair_invariants:inv_matrices_lexstar(Ms,Cs2-Cs3).


compose([],_,_,[],Cs-Cs) :- !.
compose([P|Ps],AdjMatrix,Ds,[M|Ms],Cs1-Cs3) :-
    call(P,AdjMatrix,Ds,M,Cs1-Cs2),!,
    compose(Ps,AdjMatrix,Ds,Ms,Cs2-Cs3).
    