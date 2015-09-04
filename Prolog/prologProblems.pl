%%NAME: Andrej Ivanovic
%%ID: 604016344
%% no outside resources or people besides the homework that was the ocaml version of problem 3


%% [2,3],[]

%% append([2,2],G,L)
%% L=[2,2]|[L2]-> L=[2,2]|[3,3]|[].
%% duplist([3],G)
%% L2=[3,3]|[]
%% duplist([],G).
%% G=[]. yes

duplist([],[]).
duplist([H|T],L):- append([H,H],G,L),duplist(T,G).

%%% end problem 1 %%%
isNotEqual(A,B):-A\=B.

put(K,V,[],[[K,V]]).
put(K,V,[[K,_]|T],[[K,V]|T]).
put(K,V,[[H1,H2]|T],[[H1,H2]|D2]):-isNotEqual(H1,K),put(K,V,T,D2).

get(K,[[K,V]|_],V).
get(K,[[_,_]|T],V):-get(K,T,V).


%%% end problem 2 %%%

%% test(X,V):- (
%% 				X#>=V ->
%% 				true
%% 				;
%% 				false
%% 			).
makeBT(boolval(true)).
makeBF(boolval(false)).
checkGreater(intval(A),intval(B),V):-(A#>=B -> makeBT(V);makeBF(V)).
eval(intconst(I),_,intval(I)).
%% eval(intval(I),_,intval(I)).
eval(boolconst(B),_,boolval(B)).
eval(var(K),Env,V):-get(K,Env,V).
%% ,eval(X,[],V). also change get to get(K,Env,X)
eval(geq(E1,E2),Env,V):- eval(E1,Env,V1),eval(E2,Env,V2),checkGreater(V1,V2,V).
eval(if(X, E2, E3),Env,V):-
						  ( 
						  eval(X,Env,boolval(A)),A=true ->
						  eval(E2,Env,V)
						  ;
						  eval(E3,Env,V)).
eval(function(X, E),Env,funval(X,E,Env)).
%%double check might have to change Env passed in the original function 
%% to a different name 
eval(funcall(E1,E2),Env,V):- eval(E1,Env,funval(X,E,Env)),eval(E2,Env,E2V),put(X,E2V,Env,Env1),eval(E,Env1,V).

%%% end problem 3 %%%


isNotEmpty(Hand):-Hand\==[].
isNotEmptyHand(Hand):-Hand\==none.

putOnStack(Block,[],Block).
putOnStack(Block,Stack,Result) :- Stack\=[],append(Block,Stack,Result).

move(world([H|T],Second,Third,none),pickup(stack1),world(T,Second,Third,H)) :- isNotEmpty(H).
move(world(First,[H|T],Third,none),pickup(stack2),world(First,T,Third,H)) :- isNotEmpty(H).
move(world(First,Second,[H|T],none),pickup(stack3),world(First,Second,T,H)) :- isNotEmpty(H).
move(world(First,Second,Third,Hand),putdown(stack1),world(NewStack,Second,Third,none)) :- isNotEmptyHand(Hand),putOnStack([Hand],First,NewStack).
move(world(First,Second,Third,Hand),putdown(stack2),world(First,NewStack,Third,none)) :- isNotEmptyHand(Hand),putOnStack([Hand],Second,NewStack).
move(world(First,Second,Third,Hand),putdown(stack3),world(First,Second,NewStack,none)) :- isNotEmptyHand(Hand),putOnStack([Hand],Third,NewStack).

blocksworld(World, [], World).
blocksworld(Start, [Action|T], Goal) :-
move(Start, Action, Next), blocksworld(Next, T, Goal).

%%% end problem 4 %%%



