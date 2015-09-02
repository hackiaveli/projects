
(* Name: Andrej Ivanovic

   UID: 604016344

   Others With Whom I Discussed Things:

   Other Resources I Consulted:
   http://www.cs.cornell.edu/courses/cs3110/2011sp/recitations/rec05.htm
   
*)

(* Problem 1a
   doubleAllPos : int list -> int list *)

let doubleAllPos l =
   List.fold_right (fun x y -> if x>=0 then x+x::y else x::y ) l [] ;;

(* Problem 1b
   unzip : ('a * 'b) list -> 'a list * 'b list *)

let unzip l =
   List.fold_right (fun (a,b) (c,d)->(a::c, b::d) ) l ([],[])  
 

(*  unzip [(1,2);(3,4)] ;; *)


(* Problem 1c
   encode : 'a list -> (int * 'a) list *)

let encode l =
   List.fold_right ( fun x sofar -> match sofar with
                        (num,symbol)::rest -> if(x=symbol) then (num+1,symbol)::rest
                                                else (1,x)::(num,symbol)::rest
                        | []-> [(1,x)]
                     

                     ) l []

(*encode ["a";"b";"b";"a";"a"];;*)

(* Problem 1d
   intOfDigits : int list -> int *)

let intOfDigits l =
   List.fold_left (fun x digit -> (10*x+digit)) 0 l


(* Problem 2a
   map2 : ('a -> 'b -> 'c) -> 'a list -> 'b list -> 'c list *)

let rec map2 f l1 l2 = 
   match l1 with 
      []->[]
      |a::rest -> match l2 with
                  b::rest2 -> (f a b)::map2 f rest rest2
                  |[]->[]



(* Problem 2b
   zip : 'a list * 'b list -> ('a * 'b) list *)
             
let zip (l1,l2) =
   map2 (fun x y -> (x,y)) l1 l2

(* Problem 2c
   foldn : (int -> 'a -> 'a) -> int -> 'a -> 'a *)
let rec foldn f n b = 
   match n with 
     0 -> b
   |_-> f n (foldn f (n-1) b )
     
     
(* Problem 2d
   clone : 'a * int -> 'a list *)

let clone (e,n) = 
   foldn (fun x d -> e::d) n []


(* Problem 3a
   empty1: unit -> ('a * 'b) list
   put1: 'a -> 'b -> ('a * 'b) list -> ('a * 'b) list
   get1: 'a -> ('a * 'b) list -> 'b option
*)  



let empty1 () : ('a*'b) list =
   []

let rec put1 (a:'a) (b:'b) (l:('a * 'b) list): ('a * 'b) list  =
   [(a,b)]@l


let rec get1 a l =
   match l with
      []-> None
      |(x,y)::rest->if(x=a) then Some y else (get1 a rest)

(* Problem 3b
   empty2: unit -> ('a,'b) dict2
   put2: 'a -> 'b -> ('a,'b) dict2 -> ('a,'b) dict2
   get2: 'a -> ('a,'b) dict2 -> 'b option
*)  
 
type ('a,'b) dict2 = Empty | Entry of 'a * 'b * ('a,'b) dict2 

let empty2 () : ('a,'b) dict2=
  Empty

let put2 (a:'a) (b:'b) (dict:('a,'b) dict2) : ('a,'b) dict2 =
         Entry(a,b,dict)
   

let rec get2 (a:'a) (dict:('a,'b) dict2) : ('b option)  = 
   match dict with
    Empty-> None 
    |Entry(x,y,z) -> if(x=a) then Some y else get2 a z




(* Problem 3c
   empty3: unit -> ('a,'b) dict3
   put3: 'a -> 'b -> ('a,'b) dict3 -> ('a,'b) dict3
   get3: 'a -> ('a,'b) dict3 -> 'b option
*)  

type ('a,'b) dict3 = ('a -> 'b option)


let empty3 (): ('a,'b) dict3 =
   fun x -> None 


let put3 (x:'a) (y:'b) (dict: ('a,'b) dict3) = 
      fun addA ->
      match addA with
      _ when addA=x -> Some y
     | _ -> dict addA


let get3 (x:'a) (dict: ('a,'b) dict3) = 
   dict x


