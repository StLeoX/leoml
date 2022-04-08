# leoml

## Intro
StLeoX's OCaml-syntax-like language compiler. Only FrontEnd currently.

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
> let, and, in 
>
> if, then, else 
>
> while, do, done
>
> true, false 
>
> fst, snd 
>
> 
>
> > Todo:
> >
> > rec, fun
> >
> > match, with
> >
> > for,to, do, done



```
program ::= decllist eof

# decline
decllist ::= decl
           | decllist decl
		  
decl ::= let var ( varlist ) = exp ;;
       | exp ;;

varlist ::= var
          | varlist , var

# exp
exp ::= var expblist
      | expb

expblist ::= expb
          | expblist expb

# exp base
expb ::= expa # Expa
       | expb + expb
       | expb - expb
       | expb * expb
       | expb / expb
       | expb < expb
       | expb <= expb
       | expb == expb
       | epxb != epxb
       | expb > expb
       | expb >= expb # End of ExpbBinary
       | ( expb, expb ) #cons # ExpbCons
       | expb; expb # compound # ExpbCompound
       | fst ( expb ) # fst
       | snd ( expb ) # snd # ExpbUnary

# exp atom
expa ::= var
       | constant
       | if exp then exp [else exp]
       | let var = exp [and var = exp]* in exp
       | while exp do exp done
       | (exp)

# constant(literal)
constant ::= intl
           | floatl
           | booll
           | stringl
           | ()

intl ::= [0-9]+
floatl ::= [0-9]*.[0-9]+
booll ::= true | false
stringl ::= ".*"

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

- Support using the '\\' to start a `newline` if you didn't finish current line yet.

- Only digit and float are supported. (No 0x, 0b, 0o numbers).
- No distinct between character and string. (Only string supported, like JS).

- No Pointer! No Pointer! No Pointer! (Everything is a reference?)

Type System:

- `Pair` Type ( dual-element list) supported, that means supporting:
  - construction: ( expb, epxb )
  - first element access: fst ( expb )
  - second elemant access: snd ( expb )

- Not supporting mutable data structure , that means no `refs`, no `arrays`.







### Reflection

1. 其实这个KwTrie冗余，因为已经存在KwMap。这个KwTrie唯一的作用可能是求First，但同样可以使用KwMap完成。
2. 为什么相应的类都需要New函数？因为调用构造函数生成在stack上，而调用New通过new生成在heap上，避免爆栈。
3. 注意区分decl里面的let 和 expa里面的let...in...，两者仅仅是复用关键字let。
4. 从Lexer的角度来看，所谓的Keywords不就是一种特殊的Ident么。
5. 可以看到文法中的decllist、varlist、expblist都是左递归的形式，但我们是有办法处理的，可以使用外部变量而不仅仅是一个栈。

6. ASTNode当中可能的_root字段被用来记录continuation的start位置（主要是利用到token->loc），而可能的\_op字段才是用来记录ASTNode的唯一类型，所以两者并不对应。
