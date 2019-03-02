**STIL PARSER**

A program to parse simplified STIL 1998 grammar. STIL is an IEEE standard aimed at promoting transportability of test vectors.
At the end of parsing it creates a flattened vector with signal names and values of the signals at relative time events.
The generated vector can be directly used in a simulation environment using VPI.
This program uses BOOST SPIRIT X3 library to generate the parser.It's a work in progress and only subset of grammar is supported.
