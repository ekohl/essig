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
			OPCODE { Opcode opcode = new Opcode($OPCODE.text);}
			^(PARAMS (p+=param)*)
			^(ARGUMENTS (a+=argument)*)
			^(EXPR (e+=expr)+)
		)
	-> instruction(
		name={$IDENTIFIER},
		params={$p},
		arguments={$a},
		expressions={$e},
		mask={opcode.getMaskString()},
		opcode={opcode.getOpcodeString()},
		opcodeparsed={opcode}
	)
	;

param	: ^(i=word  v=word)
	-> param(name={$i.st},value={$v.comment},comment={$i.st + "=" + $v.comment})
	|	^(CLOCK NUMBER)
	-> template(cycles={$NUMBER.text}) "state->cycles += <cycles>;"
	|	^(SIZE NUMBER)
	-> template(v={$NUMBER.text}) "//size = <v>;"
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

operatorExpr returns [String comment = ""] :
		word  {$comment = $word.comment;}
	-> {$word.st}
	|	^(o=operator w=word e=operatorExpr) {$comment = $w.comment + " " +  $o.st + " " + $e.comment +  " ";}
	-> operatorExpr(operator={$o.st},word={$w.st},expression={$e.st})
	;

condition:	^(EQUALS l=operatorExpr r=word)
	-> condition(left={$l.st},right={$r.st})
	;

word returns [String comment = ""]:
		NUMBER {$comment = $NUMBER.text;}
	-> template (number={$NUMBER}) "<number>"
	|	^(i=IDENTIFIER{String temp_not = "";} (NOT{temp_not = "!";})? (IDENTIFIER | NUMBER)?) {$comment = temp_not + $i.text;}
	-> template (i={Func.convertReg($i.text)},temp_not={temp_not}) "<temp_not>vm_info(state,VM_INFO_REGISTER,<i>,&error)"
	;

operator:	(o=AND | o=OR | o=XOR | o=ADD)
	-> template(operator={$o}) "<operator>";
