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
grammar specific information.

## How to update the LEMON LALR(1) parser generator in a repository of the Lemon Server.

The LEMON LALR(1) parser generator is still being under development, so it is 
needed to update it time-by-time as a core dependency. The Fossil is a great 
DVCS, but a poor external (additional) dependency, so it was decided to store 
LEMON LALR(1) parser generator in the primary repository of the Lemon Server 
and update it manually.

So, to update LEMON LALR(1) parser generator for a Lemon Server, please, follow 
the steps:

1. Let's start from a getting of Sqlite source code from a Fossil repository. 
Assumes that Fossil utilities already present. Create folder and obtain Sqlite's 
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

1. All tests must be passed! To check it:

    ```Shell
    cd %LEMON_SERVER%
    cmake -DWITHTESTS=1 .
    make clean all test
    ```

1. Push changes to a separate branch and create a pull request.