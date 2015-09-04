(* Name: Andrej Ivanovic

   UID: 604016344

   Others With Whom I Discussed Things:

   Other Resources I Consulted:
   http://caml.inria.fr/pub/docs/oreilly-book/html/book-ora017.html
   *this was to figure out how to declare an exception, i learned it must be a capital letter
   
*)

(* EXCEPTIONS *)

(* This is a marker for places in the code that you have to fill in.
   Your completed assignment should never raise this exception. *)
exception ImplementMe of string

(* This exception is thrown when a type error occurs during evaluation
   (e.g., attempting to invoke something that's not a function).
*)
exception DynamicTypeError

(* This exception is thrown when pattern matching 
fails during evaluation. *)  
exception MatchFailure  


(*ADDED exception to be thrown in the helper function when we could not find a pattern that matches the arg in that invocation*)
exception HelperDidNotFindMatch

(* EVALUATION *)

(* See if a value matches a given pattern.  If there is a match, 
return an environment for any name bindings in the pattern. 
    If there is not
   a match, raise the MatchFailure exception.
*)
let rec patMatch (pat:mopat) (value:movalue) : moenv =
  match (pat, value) with 
      (* an integer pattern matches an integer only when 
      they are the same constant;
   no variables are declared in the pattern so the 
  returned environment is empty *)

(*note match x with 5 would get mapped to like this 5 would be the pat and value is the x*)
      (IntPat(i), IntVal(j)) when i=j -> Env.empty_env()
    | (BoolPat(i),BoolVal(j)) when i=j -> Env.empty_env()
    | (WildcardPat,_)-> Env.empty_env() 
    | (VarPat(i),_)-> (Env.add_binding i value (Env.empty_env())) 
    | (NilPat,ListVal(NilVal))-> ((Env.empty_env()))    
    (* the pattern is first::rest and the value is the actual list values [10;20;30] and this has to map to 10 to first and the rest is the rest of the values *)     
    | (ConsPat(elemOne,listRest), ListVal(ConsVal(val1,valAllRest)))->(
      (Env.combine_envs (patMatch elemOne val1) (patMatch listRest (ListVal(valAllRest))))
    ) 

    | _ -> raise MatchFailure
  
    
(* Evaluate an expression in the given environment and return the
   associated value.  Raise a MatchFailure if pattern matching fails.
   Raise a DynamicTypeError if any other kind of error occurs (e.g.,
   trying to add a boolean to an integer) which prevents evaluation
   from continuing.
*)

(* let rec matchPat = 
      Nil -> raise MatchFailure
    | 

                
*)


(*rough draft algo for helper to go thru all pattersn
* base case is no more patterns to look at it means match fail raise excp
* look at first pattern in list and check against the pattern against our value
   **might need second helper to do this cant do in body of func?

* recurse if we fail so catch the fail and go on the rest of the list otherwise
return the env our helper returns and return the two return values together
(bodyToRun,environmenttorunin)

*)


let findPatMatch (whatToMatch,valToMatch) =
  try (patMatch whatToMatch valToMatch) 
with MatchFailure -> raise HelperDidNotFindMatch 


let rec help_w_match (passingArg) =
  match passingArg with 
    ([],valToCompare)-> raise MatchFailure 
 | ((matchHere,ifMatchExecThis)::otherCases,valToCompare )->
    (
    try ( let envReturned= (findPatMatch (matchHere,valToCompare))
          in (ifMatchExecThis,envReturned)
        )  
  with HelperDidNotFindMatch -> (  help_w_match (otherCases,valToCompare)))


let rec evalExpr (e:moexpr) (env:moenv) : movalue =
  match e with
      (* an integer constant evaluates to itself *)
      IntConst(i) -> IntVal(i)
    | Nil -> ListVal(NilVal)
    | BoolConst(b)->BoolVal(b)
    | Var(vari)-> ( try (Env.lookup vari env) 
                    with Env.NotBound -> raise DynamicTypeError)
    | BinOp(one,op,two) -> (
              let expr1 = (evalExpr one env) in (
              let expr2 = (evalExpr two env) in (
          match op with
            
            Plus->(match expr1 with
                        IntVal(x)->( match expr2 with
                                    IntVal(y)->IntVal(x+y)
                                  |_->raise DynamicTypeError
                                )
                       | _ ->raise DynamicTypeError
                      )
            | Minus ->(match expr1 with
                        IntVal(x)->( match expr2 with
                                    IntVal(y)->IntVal(x-y)
                                  |_->raise DynamicTypeError
                                )
                       | _ ->raise DynamicTypeError
                      )
             | Times ->(match expr1 with
                        IntVal(x)->( match expr2 with
                                    IntVal(y)->IntVal(x*y)
                                  |_->raise DynamicTypeError
                                )
                       | _ ->raise DynamicTypeError
                      )
            | Eq -> (match expr1 with
                        IntVal(x)->( match expr2 with
                                    IntVal(y)->(if (x=y) then BoolVal(true) else BoolVal(false))
                                  |_->raise DynamicTypeError
                                )
                       | _ ->raise DynamicTypeError
                      )
            |Gt -> (match expr1 with
                        IntVal(x)->( match expr2 with
                                    IntVal(y)->(if (x>y) then BoolVal(true) else BoolVal(false))
                                  |_->raise DynamicTypeError
                                )
                       | _ ->raise DynamicTypeError
                      )
            |Cons ->(match expr2 with
                  ListVal(y)->(ListVal(ConsVal(expr1,y )))
                  |ListVal(NilVal)->(ListVal(ConsVal(expr1,NilVal)))
                  |_->raise DynamicTypeError
                    )


            | _ -> raise DynamicTypeError


          ) ) )

    | Negate(n)->( match evalExpr n env with
                  IntVal(i)-> IntVal(-i)
                  |_->raise DynamicTypeError)

    | If(x,y,z)-> ( match evalExpr x env with
                      BoolVal(true)-> evalExpr y env 
                    |BoolVal(false)-> evalExpr z env 
                    |_ -> raise DynamicTypeError ) 

    |Function(thePat,theExp)-> (FunctionVal(None,thePat,theExp,env))
    
    |Match(expr,allPats) -> (

                (*helper goes thru all possible matches and gives me the correct
                 tuple of (body to run, env to run in) *)
      let (toExec,returnEnv) = (help_w_match (allPats,(evalExpr expr env))) in 
          (
                
                        (evalExpr toExec (Env.combine_envs env returnEnv))
                      
          )
      )

    (* algo idea for func call hard case

      this is for the funcs with name rec case

                                motivating case from piazza (great case to make sure binding is correct)
                                
                                let rec f f(arg) = match f(arg) with
                                                    0->0
                                                    |_-> f(arg)+1

                            this maps to name -> f
                          the f arg maps to -> patToMatch
                          the    0->0
                                  |_-> f(arg)+1    to -> body 
                          and originalEnv is the env

                          now i have to pattern match the patToMatch with callWith to
                          match my passed in argument to the Farg and this gives me a new environment

                          i have to also add to environmnet the functions name matched with itself(func)

                          then i combine those two envs with the functions original env and evaluate
                          the right order should be

                          functionname,function in the original function env then
                          shadowed by (functarg,passed in arg) all added into the above combined env

                          then evaluate the body with this final new env
    easy case 
  this part is for anon funcs
    rough draft idea 
                          function x->x*x this is skeleton of func
                        
                          this maps to name -> none
                          the x maps to -> patToMatch
                          the x*x to -> body 
                          and originalEnv

                          so now i just must match the argument i called the function with to x using patmatch
                          and get this new env back 
                          then i must combine the originalEnv with this new binding of the variable to the argument i called the func with
                          and it must be done in the order that originalenv goes first because bindings from the second env overshadow the first env

                          **tested and it works** 

                        *)





    |FunctionCall(functionWhole,argPassed)-> ( 
                          let func = (evalExpr functionWhole env) in
                          let callWith = (evalExpr argPassed env) in
                          match func with
                          FunctionVal(aliasName,patToMatch,body,funcEnv)->
                         ( 
                          
                          match aliasName with 
                          Some(funcName ) ->(
                        
                          
                              let addFuncEnv = (Env.add_binding funcName func funcEnv) in (
                              let bindArgToPassedArgEnv= (Env.combine_envs addFuncEnv (patMatch patToMatch callWith)) in (
                              
                              (evalExpr body bindArgToPassedArgEnv) )

                                )  )


                          |None->

                           ( let envFinal = (Env.combine_envs funcEnv (patMatch patToMatch callWith)) in
                          (evalExpr body envFinal) )
                        

                      )
                       |_->raise DynamicTypeError  

                 )

    | _ -> raise DynamicTypeError

(* Evaluate a declaration in the given environment.  Evaluation
   returns the name of the variable declared (if any) by the
   declaration along with the value of the declaration's expression.
*)
let rec evalDecl (d:modecl) (env:moenv) : moresult =
  match d with
      Expr(e) -> (None, evalExpr e env)
   | Let(x,c) -> (Some x, evalExpr c env)
    (*rec has the name of the func, the pattern is the arg and the exp is the body or expression*)
   | LetRec(nm,pt,exp) -> (Some(nm),FunctionVal(Some(nm),pt,exp,env))

   |_-> raise DynamicTypeError


   (*DONE*)



