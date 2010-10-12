tree grammar TTree;

options {

    // Default but name it anyway
    //
    language   = Java;

    // Use the vocab from the parser (not the lexer)
    //
    tokenVocab = TParser;

    // Use ANTLR built-in CommonTree for tree nodes
    //
    ASTLabelType = CommonTree;

    // Output a template
    //
    output = template;
}

// What package should the generated source exist in?
//
@header {

    package nl.utwente.cs.essig;
}

microcontroller:
	^(
		n=IDENTIFIER
		^(PARAMETERS (p+=parameter)+)
		^(REGISTERS (r+=register)*)
		^(INSTRUCTIONS (i+=instruction)*)
	)
	-> microcontroller(name={$n},parameters={$p},registers={$r},instructions={$i})
	;

parameter:	k=RAM n=NUMBER -> parameter(key={$k},value={$n})
	|	k=GPRS n=NUMBER -> parameter(key={$k},value={$n})
	;

register:	name=IDENTIFIER -> register(name={$name});

instruction:	^(
			f=IDENTIFIER
			(a+=IDENTIFIER)*
			(e+=expr)+
		)
		-> instruction(name={$f},arguments={$a},expressions={$e})
	;


expr	:	a=assignExpr -> {$a.st}
	|	i=ifExpr -> {$i.st}
	;

assignExpr:	^(ASSIGN var=IDENTIFIER value+=word (value+=operator value+=word)*)
	->	assign(var={$var},value={$value})
	;

operator:	AND | OR | XOR | ADD;

ifExpr:		^(IF condition expr+ (ELSE expr+)?);
condition:	word EQUALS word;
word	:	n=NOT? i=IDENTIFIER
	|	NUMBER
	;
