tree grammar TTree;

options {

    // Default but name it anyway
    //
    language   = Java;

    // Use the vocab from the parser (not the lexer)
    //
    tokenVocab = TParser;

    // Use ANTLR built-in CommonToken for tree nodes
    //
    ASTLabelType = CommonToken;
}

// What package should the generated source exist in?
//
@header {

    package nl.utwente.cs.essig;
}

microcontroller:
	IDENTIFIER { System.out.println("Bla"); }
	parameters
	registers?
	instructions?
	EOF;

parameters:
	PARAMETERS parameter+;

parameter:	RAM NUMBER
	|	GPRS NUMBER
	;

registers:	REGISTERS register+;

register:	IDENTIFIER;

instructions:	instruction+;

instruction:	IDENTIFIER arguments? expr+;

arguments:	IDENTIFIER+;

expr:		assignExpr;

assignExpr:	IDENTIFIER ASSIGN (NOT? IDENTIFIER | NUMBER) (operator (NOT? IDENTIFIER | NUMBER ))*;

operator:	AND | OR | XOR | ADD;
