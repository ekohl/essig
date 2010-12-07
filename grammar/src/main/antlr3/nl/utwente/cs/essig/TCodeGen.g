tree grammar TCodeGen;

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

@members {
	private String defaultClock;
	private HashMap<String,String> registers = new HashMap<String,String>(); 
}

microcontroller: ^(
			IDENTIFIER
			^(PARAMETERS (p+=parameter)+)
			^(REGISTERS (r+=register)+)
			^(MAPS map+)
			^(INSTRUCTIONS (i+=instruction)+)
		)
	-> microcontroller(name={$IDENTIFIER},parameters={$p},registers={$r},instructions={$i})
	;

parameter:	^(RAM NUMBER)
	-> ram(ram={$NUMBER})
	|	^(CLOCK NUMBER)
		{ defaultClock = $NUMBER.text; }
	|	^(OP_SIZE NUMBER)
	;

register:	^(IDENTIFIER NUMBER) -> register(name={$IDENTIFIER},offset={$NUMBER})
	|       ^(IDENTIFIER multiword_register) -> register(name={$IDENTIFIER},offset={"123"})
	;

multiword_register: 	^(IDENTIFIER IDENTIFIER+)
	;

map:		^(map_type NUMBER NUMBER)
	;

map_type returns [String comment]:
			(t=CHUNK | t=REGISTER | t=IO | t=ROM | t=RAM)
				{ $comment = $t.text; }
		-> template(type={($t.text).toUpperCase()}) "<type>"
	;

instruction:	^(
			IDENTIFIER
			^(PARAMS
				(ops+=opcode)+
				(^(CLOCK NUMBER))?
			)
			^(ARGUMENTS (a+=argument)*)
			^(EXPR (e+=expr)+)
		)
	-> instruction(
		name={$IDENTIFIER},
		opcodes={$ops},
		clock={$NUMBER != null ? $NUMBER.text : defaultClock},
		arguments={$a},
		expressions={$e}
	)
	;

opcode:
		OPCODE
		{ Opcode opcode = new Opcode($OPCODE.text); }
	-> opcode(mask={opcode.getMaskString()},opcode={opcode.getOpcodeString()},parsed={opcode})
	;

argument:	SIGNED? IDENTIFIER
	-> argument(name={$IDENTIFIER},signed={$SIGNED})
	;

expr	:	assignExpr
	-> {$assignExpr.st}
	|	ifExpr
	-> {$ifExpr.st}
	| HALT
	-> halt()
	;

assignExpr:	^(ASSIGN i=IDENTIFIER o=operatorExpr)
	-> assignExpr(var={$i},type={"REGISTER"},value={$o.st},comment={$i + " = " + $o.comment})

	|	^(ASSIGN CONSTANT o=operatorExpr)
	-> assignConstant(var={$CONSTANT.text},value={$o.st},comment={$o.comment})

	|	^(ASSIGN ^(m=map_type o1=operatorExpr) o2=operatorExpr)
	-> assignExpr(var={$o1.st},type={$m.st},value={$o2.st},comment={$m.st + "(" + $o1.comment + ") = " + $o.comment})

	|	^(ASSIGN ^(MULTI_REG t=multi_identifier o1=operatorExpr o2=operatorExpr) o3=operatorExpr)
	-> multiRegisterAssignExpr(type={$t.st},low={$o1.st},high={$o2.st},value={$o3.st})
	;

ifExpr	:	^(IF condition (i+=expr)+ (ELSE (e+=expr)+)?)
	-> ifExpr(condition={$condition.st},ifExpr={$i},elseExpr={$e})
	;

operatorExpr returns [String comment]:
		word
			{$comment = $word.comment;}
	-> {$word.st}
	|	^(o=operator w=word e=operatorExpr)
			{$comment = $w.comment + " " +  $o.st + " " + $e.comment +  " ";}
	-> operatorExpr(operator={$o.st},word={$w.st},expression={$e.st})
	;

condition:	^(c=comparison l=operatorExpr r=word)
	-> condition(left={$l.st},comparison={$c.st},right={$r.st})
	;

word returns [String comment]:
		NUMBER {$comment = $NUMBER.text;}
	-> template (number={$NUMBER}) "<number>"
	|	variable {$comment = $variable.comment; }
	-> {$variable.st}
	|	^(NOT w=word) { $comment = $NOT.text + $w.comment; }
	-> not(value={$w.st})
	|	^(BIT variable (b=NUMBER|b=CONSTANT))
		{ $comment = $variable.comment + $BIT.text + $b.text; }
	-> bit(var={$variable.st}, bit={$b.text})
	;

variable returns [String comment]:
		CONSTANT
		{ $comment = $CONSTANT.text; }
	-> template(constant={$CONSTANT.text}) "<constant>"
	|	v=IDENTIFIER
		{ $comment = $IDENTIFIER.text; }
	-> wordVariable(variable={$IDENTIFIER}, type={"REGISTER"})
	|	^(map_type operatorExpr)
			{ $comment = $map_type.comment + "(" + $operatorExpr.comment + ")"; }
	-> wordVariable(variable={$operatorExpr.st}, type={$map_type.st})
	|	multi_register
	-> {$multi_register.st}
	;

multi_register:
		^(MULTI_REG multi_identifier o1=operatorExpr o2=operatorExpr)
	-> multiRegister(r1={$o1.st},r2={$o2.st},type={$multi_identifier.st})
	;

multi_identifier : 
		IDENTIFIER -> template(var={"REGISTER"}) "<var>"
	|	map_type -> {$map_type.st}
	;

comparison:		(c=EQUALS | c=LT | c=LTE | c=GT | c=GTE)
	-> template(c={$c}) "<c>"
	;

operator:      (o=AND | o=OR | o=XOR | o=ADD | o=MINUS | o=MULT | o=SHIFT)
	-> template(operator={$o}) "<operator>"
	;
