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

microcontroller: ^(
			IDENTIFIER
			^(PARAMETERS (p+=parameter)+)
			^(REGISTERS (r+=register)*)
			^(INSTRUCTIONS (i+=instruction)*)
		)
	-> microcontroller(name={$IDENTIFIER},parameters={$p},registers={$r},instructions={$i})
	;

parameter:	^(RAM NUMBER)
	-> ram(ram={$NUMBER})
	|	^(GPRS NUMBER)
	-> gprs(registers={$NUMBER})
	|	^(SIZE NUMBER)
	|	^(CLOCK NUMBER)
	;

register:	IDENTIFIER -> register(name={$IDENTIFIER});

instruction:	^(
			IDENTIFIER
			^(PARAMS (p+=param)*)
			^(ARGUMENTS (a+=argument)*)
			^(EXPR (e+=expr)+)
		)
	-> instruction(name={$IDENTIFIER},arguments={$a},expressions={$e})
	;

param	:	^(SIZE NUMBER)
	|	^(CLOCK NUMBER)
	|	^(OP_CODE OPCODE)
	;

argument:	IDENTIFIER
	-> template(name={$IDENTIFIER}) "<name>"
	;

expr	:	assignExpr
	-> {$assignExpr.st}
	|	ifExpr
	-> {$ifExpr.st}
	;

assignExpr:	^(ASSIGN IDENTIFIER operatorExpr)
	-> assignExpr(var={$IDENTIFIER},value={$operatorExpr.st})
	;


ifExpr	:	^(IF condition (i+=expr)+ (ELSE (e+=expr)+)?)
	-> ifExpr(condition={$condition.st},ifExpr={$i},elseExpr={$e})
	;

operatorExpr:	word
	-> {$word.st}
	|	^(o=operator w=word e=operatorExpr)
	-> operatorExpr(operator={$o.st},word={$w.st},expression={$e.st})
	;

condition:	^(EQUALS l=operatorExpr r=word)
	-> condition(left={$l.st},right={$r.st})
	;

word	:	NUMBER
	-> template(number={$NUMBER}) "<number>"
	|	^(i=IDENTIFIER NOT? (IDENTIFIER | NUMBER)?)
	-> template(i={$i}) "<i>"
	;

operator:	(o=AND | o=OR | o=XOR | o=ADD)
	-> template(operator={$o}) "<operator>";
