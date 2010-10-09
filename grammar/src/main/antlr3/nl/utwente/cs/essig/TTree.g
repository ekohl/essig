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
    ASTLabelType = CommonTree;
}

// What package should the generated source exist in?
//
@header {

    package nl.utwente.cs.essig;
}

microcontroller:
	^(
		name=IDENTIFIER { System.out.println("Name: "+$name.text); }
		parameters
		registers?
		instructions?
	)
	EOF
	;

parameters:	^(PARAMETERS parameter+);

parameter:	RAM n=NUMBER { System.out.println("Ram: "+$n.text); }
	|	GPRS n=NUMBER { System.out.println("Registers: "+$n.text); }
	;

registers:	^(REGISTERS register+);

register:	r=IDENTIFIER { System.out.println("Register: "+$r.text); };

instructions:	instruction+;

instruction:	^(
			f=IDENTIFIER { System.out.print($f.text+"("); }
			(a=IDENTIFIER { System.out.print($a.text+", "); } )*
			{ System.out.println(")"); }
			expr+
		);

expr:		assignExpr;

assignExpr:	^(IDENTIFIER ASSIGN (NOT? IDENTIFIER | NUMBER) (operator (NOT? IDENTIFIER | NUMBER ))*);

operator:	AND | OR | XOR | ADD;
