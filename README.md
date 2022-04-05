# leoml

## Intro
StLeoX's OCaml-like language compiler. Only FrontEnd currently.

BackEnd: Maybe an Interpreter?



## Usage

``````leoml
leoml [-h|--help]
      [-l|--lexer]
      [-p|--parser]
      [-o <filename>]
      <filename>
``````

If no output file, then print to the stdout.



## Design

### Grammer

> - No type lint.
>
> - No builtin data struct.



> Keywords: 
>
> let, fun
>
> if, then, else,
>
> while, do ,end
>
> true, false
>
> cons, car, cdr, empty



```
program ::= decllist eof

# decline
decllist ::= decl
           | decllist decl
		  
decl ::= let var (varlist) = exp ;;

varlist ::= var
          | varlist var

# exp
exp ::= var explist
      | expb

explist ::= expb
          | explist expb

# exp base
expb ::= ( exp )
       | expa
       | expb + expb
       | expb - expb
       | expb * expb
       | expb / expb
       | expb < expb
       | expb <= expb
       | expb == expb
       | expb > expb
       | expb >= expb
       | cons expb expb # cons
       | car expb # car
       | cdr expb # cdr
       | empty expb # empty
       | expb; expb

# exp atom
expa ::= ( )
       | intl
       | floatl
       | booll
       | NaN
       | var
       | if exp then exp else exp
       | let var = exp
       | while exp do exp end

# literal
intl ::= [0-9]+
floatl ::= [0-9]*.[0-9]+
booll ::= true | false


var ::= [a-zA-Z][a-zA-Z_0-9]*
comment ::= (* ... *)
```



### Precedence

```
1. ( )  left associative

2. . left associative

3. * / left associative

4. + - left associative

5. <= < > >= == left associative

6. -> :: right associative

7. if then else

8. := left associative

9. ; left associative

10. ! ref right associative

```



### Feature

Lexer:

- Support using the '\\' to start a new line if you didn't finish current line yet.

- Only digit and float are supported. (No 0x, 0b, 0o numbers).
- No distinct between character and string. (Only string supported, like JS).
