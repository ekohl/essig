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
	-> instruction(name={$IDENTIFIER},params={$p},arguments={$a},expressions={$e})
	;

param	: ^(i=word  v=word) -> param(name={$i.st},value={$v.comment},comment={$i.st + "=" + $v.comment})
	| ^(OP_CODE v2=word) -> template(v={$v2.st}) "<v>"
	;

argument:	IDENTIFIER
	-> template(name={$IDENTIFIER}) "<name>"
	;

expr	:	assignExpr
	-> {$assignExpr.st}
	|	ifExpr
	-> {$ifExpr.st}
	;

assignExpr:	^(ASSIGN IDENTIFIER o=operatorExpr)
	-> assignExpr(var={$IDENTIFIER},value={$o.st},comment={$IDENTIFIER + " = " + $o.comment}) 
	;


ifExpr	:	^(IF condition (i+=expr)+ (ELSE (e+=expr)+)?)
	-> ifExpr(condition={$condition.st},ifExpr={$i},elseExpr={$e})
	;

operatorExpr returns [String comment = ""] :	word  {$comment = $word.comment;}
	-> {$word.st}
	|	^(o=operator w=word e=operatorExpr) {$comment = $w.comment + " " +  $o.st + " " + $e.comment +  " ";}
	-> operatorExpr(operator={$o.st},word={$w.st},expression={$e.st})
	;

condition:	^(EQUALS l=operatorExpr r=word)
	-> condition(left={$l.st},right={$r.st})
	;

word returns [String comment = ""]:	NUMBER {$comment = $NUMBER.text;}
			-> template (number={$NUMBER}) "<number>" 
	|	^(i=IDENTIFIER NOT? (IDENTIFIER | NUMBER)?) {$comment = $i.text;}
			-> template (i={Func.convertReg($i.text)}) "<i>"
	|	OPCODE {$comment=$OPCODE.text; } -> template (v={new Opcode($OPCODE.text)}) "<v>"
	|	(i=CLOCK | i=SIZE) 
			-> template(v={$i}) "<v>"
	;

//ident : IDENTIFIER {String temp=;}  -> template (i={temp}) "<i>";

operator:	(o=AND | o=OR | o=XOR | o=ADD) 
	-> template(operator={$o}) "<operator>";
