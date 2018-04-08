# Note about a LEMON LALR(1) parser generator.

A Lemon Server use a [LEMON LALR(1) parser 
generator](https://www.sqlite.org/src/doc/trunk/doc/lemon.html) for a 
[HTTP](https://tools.ietf.org/html/rfc7230) protocol parsing.

A source code of the Lemon Parser stores in a Sqlite repository and consists of 
two files:

* ``lemon.c`` --- the LEMON LALR(1) parser generator;
* ``lempar.c`` --- a driver template for the LEMON parser generator.

The LEMON LALR(1) parser generator (as a compiled executable file ``lemon.c``) 
takes a grammar file (here ``http11.y``) and fills a driver template by a 
grammar specific information. Noticed that generator works well, but there some 
questions to the driver.

The first one is how to use a ``Parse_ENGINEALWAYSONSTACK`` definition properly. 
In general case it is needed to call ``ParseHTTP11Init(void *yypParser)`` 
before a parsing but it is not so clear how put ``yypParser`` in a stack (what 
is a size of ``yypParser``).

The second one is asserts in the driver. In a Fossil Sqlite's repository, on 
2017-12-27 18:19:06 a developer ``drh`` has added a line 
[``tool/lempar.c:514``](https://www.sqlite.org/src/info/1b22b42e59793af1):

```diff
+ assert( i>=0 && i+YYNTOKEN<=sizeof(yy_lookahead)/sizeof(yy_lookahead[0]) );
```

(SHA3-256: 1b22b42e59793af19c69a2e5f6822883cc2687d4a0d9b9280bbff885276c6baa)

Next commits say that this line was 
[split](https://www.sqlite.org/src/info/a6c3115483d597fc) into:

```diff
+ assert( i>=0 );
+ assert( i+YYNTOKEN<=(int)sizeof(yy_lookahead)/sizeof(yy_lookahead[0]) );
```

(SHA3-256: a6c3115483d597fc77ab19fdcfd1d3437cad7e467081ad8c5315fb98c115eed9)

Tests of the Lemon Server say that this assert brokes a pasing process.

## How to update the LEMON LALR(1) parser generator in a repository of the Lemon Server.

The LEMON LALR(1) parser generator is still being under development, so it is 
needed to update it time-by-time as a core dependency. The Fossil is a great 
DVCS, but a poor external (additional) dependency, so it was decided to store 
LEMON LALR(1) parser generator in the primary repository of the Lemon Server 
and update it manually with a respect of the two problems (questions) described 
above.

So, to update LEMON LALR(1) parser generator for a Lemon Server, please, follow 
the steps:

1. Let's start from a getting of Sqlite source code from a Fossil repository. 
Assumes that Fossil utilites already present. Create folder and obtain Sqlite's 
source code:

    ```Shell
    mkdir ./sqlite
    cd ./sqlite
    fossil clone https://www.sqlite.org/src sqlite.fossil
    fossil open sqlite.fossil
    fossil update trunk
    ```

1. Save commit info from an output of the ``fossil update trunk`` to a 
``%LEMON_SERVER%/3rdParty/lemon/lemon.hash`` (``%LEMON_SERVER%`` is a path to 
Lemon Server local Git repository). Example:

    ```
    checkout:     602fbd8149b53d8f0e9a223cc1aec912e7df03fc 2018-04-06 19:12:55 UTC
    tags:         trunk
    comment:      Enhance LEMON to show precendence of symbols and all rules in the report that is generated in parallel to the
                  parser. (user: drh)
    ```

1. Copy two files:

    ```Shell
    cp ./tool/lemon.c %LEMON_SERVER%/3rdParty/lemon/lemon.c
    cp ./tool/lempar.c %LEMON_SERVER%/src/lemonHttp/lempar.c
    ```

1. Patch the ``%LEMON_SERVER%/src/lemonHttp/lempar.c`` to return a size of the 
parser's structure:

    ```diff
    --- a/src/lemonHttp/lempar.c
    +++ b/src/lemonHttp/lempar.c
    @@ -359,10 +359,6 @@
       if( pParser ) ParseInit(pParser);
       return pParser;
     }
    +#else
    +size_t ParseSize() {
    +    return sizeof(yyParser);
    +}
     #endif /* Parse_ENGINEALWAYSONSTACK */
    ```

1. Patch the same file to disable the assert:

    ```diff
    --- a/src/lemonHttp/lempar.c
    +++ b/src/lemonHttp/lempar.c
    @@ -516,7 +516,7 @@
       do{
         i = yy_shift_ofst[stateno];
         assert( i>=0 );
    -    assert( i+YYNTOKEN<=(int)sizeof(yy_lookahead)/sizeof(yy_lookahead[0]) );
    +    /*assert( i+YYNTOKEN<=(int)sizeof(yy_lookahead)/sizeof(yy_lookahead[0]) );*/
         assert( iLookAhead!=YYNOCODE );
         assert( iLookAhead < YYNTOKEN );
         i += iLookAhead;
    ```

1. All tests must be passed! To check it:

    ```Shell
    cd %LEMON_SERVER%
    cmake -DWITHTESTS=1 .
    make clean all test
    ```

1. Push changes to a separate branch and create a pull request.