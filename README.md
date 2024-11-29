# BATASM
A programming language that combines Batch, Assembly and Python together.

> [!WARNING]
> This is no-where near done. Help through issues and/or pull requests would be extremely helpful.

## Concept:
Mixing Batch and Assembly with a few Python things together into a single programming languge anyone can use while also being coded in CPP (C++).
See more of what I want to do with this in the [guide.batasm file](/examples/guide.batasm).

## Features:
- Interpreter (like Python)
- Compiler (coming soon)

- Printing (so I can Hello, World!)
- Running commands in the default shell
- Opening links easily

## Download:
- Download from the [releases page](https://github.com/james-beans/BATASM/releases)
- or
- the [/dist/ folder](/dist/)

> [!WARNING]
> Most syntax commands need quotation marks to use (like printing). This breaks the CLI and makes it end the command early when using the syntax in the -C terminal run. You can avoid this by using `\`'s before the main syntax command quotes or just use different types of quotes (like ` `` ` backticks or `''` single quotes instead of `""` double qoutes). 

## CLI Commands:
- Interpret script - `batasm <script.batasm>`
- Run command only (from terminal) - `batasm -C "<command>"`

## Accepted Syntax Commands:
- `// <comment here>`
- `PRINT "<text to print here>"`
- `OPEN "<link to open>"`
- `RUN "<command runnable in default terminal>"`

## For examples see [the examples folder](/examples).
