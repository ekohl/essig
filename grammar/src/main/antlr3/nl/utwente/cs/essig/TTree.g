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

parameter:	^(RAM ram=NUMBER) -> ram(ram={$ram})
	|	^(GPRS n=NUMBER) -> gprs(registers={$n})
	|	^(SIZE NUMBER)
	|	^(CLOCK NUMBER)
	;

register:	name=IDENTIFIER -> register(name={$name});

instruction:	^(
			f=IDENTIFIER
			(p+=param)*
			(a+=argument)+
			(e+=expr)+
		)
		-> instruction(name={$f},arguments={$a},expressions={$e})
	;

param	:	^(SIZE NUMBER)
	|	^(CLOCK NUMBER)
	|	^(OP_CODE OPCODE)
	;

argument:	IDENTIFIER;

expr	:	a=assignExpr -> {$a.st}
	|	i=ifExpr -> {$i.st}
	;

assignExpr:	^(ASSIGN var=IDENTIFIER value=operatorExpr)
		-> assign(var={$var},value={$value.st})
	;


ifExpr:		^(IF condition expr+ (ELSE expr+)?);

operatorExpr:	w=word
		-> {$w.st}
	|	^(o=operator w=word e=operatorExpr)
		-> operator(operator={$o.st},word={$w.st},expression={$e.st})
	;

condition:	^(EQUALS word (operator word)? word);
word	:	NUMBER
	|	^(IDENTIFIER NOT? (IDENTIFIER | NUMBER)?)
	;

operator:	AND | OR | XOR | ADD;
