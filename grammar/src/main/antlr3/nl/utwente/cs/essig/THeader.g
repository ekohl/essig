tree grammar THeader;

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
				IDENTIFIER
				^(PARAMETERS (p+=parameter)+)
				^(REGISTERS (r+=register)+)
				^(MAPS (m+=map+))
				^(INSTRUCTIONS instruction+)
			)
		-> header(parameters={$p},registers={$r},maps={$m})
	;

parameter:
			^(RAM NUMBER)
	|		^(CLOCK NUMBER)
	|		^(OP_SIZE NUMBER)
		-> opcode_size(bits={$NUMBER})
	;

register:
			^(IDENTIFIER (NUMBER | multiword_register))
		-> register(name={$IDENTIFIER},offset={$NUMBER})
	;

multiword_register:
			^(IDENTIFIER IDENTIFIER+)
	;


map:			^((t=CHUNK | t=REGISTER | t=IO | t=RAM | t=ROM) b=NUMBER e=NUMBER)
		-> map(type={($t.text).toUpperCase()}, begin={$b}, end={$e})
	;

instruction:
			^(
				IDENTIFIER
				^(PARAMS
					opcode+
					(^(CLOCK NUMBER))?
				)
				^(ARGUMENTS argument*)
				^(EXPR expr+)
			)
	;

opcode:
			OPCODE
	;

argument:
			SIGNED? IDENTIFIER
	;

expr:
			assignExpr
	|		ifExpr
	|		HALT
	;

assignExpr:
			^(
				ASSIGN
				(
					CONSTANT? IDENTIFIER
					| RAM operatorExpr
				)
				operatorExpr
			)
	|	^(MULTI_REG i1=IDENTIFIER o1=operatorExpr i2=IDENTIFIER o2=operatorExpr o3=operatorExpr)
	;


ifExpr:
			^(IF condition expr+ (ELSE expr+)?)
	;

operatorExpr:
			word
	|		^(operator word operatorExpr)
	;

condition:
			^(comparison operatorExpr word)
	;

word:
			NUMBER
	|		^(IDENTIFIER NOT? CONSTANT? (IDENTIFIER|NUMBER)?)
	|		^(RAM operatorExpr)
	|		^(MULTI_REG (IDENTIFIER|RAM) operatorExpr IDENTIFIER? operatorExpr)
	;

comparison:
			EQUALS | LT | LTE | GT | GTE
	;

operator:
			(AND | OR | XOR | ADD | MINUS | MULT | SHIFT)
	;
