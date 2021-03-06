parser grammar TParser;

options {

    // Default language but name it anyway
    //
    language  = Java;

    // Produce an AST
    //
    output    = AST;

    // Use a superclass to implement all helper
    // methods, instance variables and overrides
    // of ANTLR default methods, such as error
    // handling.
    //
    superClass = AbstractTParser;

    // Use the vocabulary generated by the accompanying
    // lexer.
    //
    tokenVocab = TLexer;
}

// Some imaginary tokens for tree rewrites
//
tokens {
	PARAMS;
	ARGUMENTS;
	EXPR;
	MULTI_REG;
}

// What package should the generated source exist in?
//
@header {
    package nl.utwente.cs.essig;
}

@members {
    private int gprs = 0;
    private int gprs_offset = 0;
}

// Parser
//
microcontroller:	IDENTIFIER^ LBRACK! parameters registers maps instructions RBRACK! EOF!
	;

parameters:		PARAMETERS^ LBRACK! (parameter LINE_SEPERATOR!)+ RBRACK!
	;

parameter:		GPRS amount=NUMBER {
				gprs = Integer.parseInt($amount.text);
			}
			(ADD offset=NUMBER {
				gprs_offset = Integer.parseInt($offset.text);
			})?
		->
	|		OP_SIZE NUMBER
		-> OP_SIZE[$NUMBER.text]
	|		CLOCK NUMBER
		-> CLOCK[$NUMBER.text]
	|		ENDIANNESS^ (BIG | LITTLE)
	;

registers:		REGISTERS^ LBRACK! (register LINE_SEPERATOR!)+ RBRACK!
		{
			// Hack in the general purpose registers
			for(int i=0; i < gprs; i++) {
				// -> IDENTIFIER^ NUMBER[Ri]
				Object reg = adaptor.create(IDENTIFIER, "R" + i);
				adaptor.becomeRoot($REGISTERS.tree, reg);
				adaptor.addChild(reg, adaptor.create(NUMBER, Integer.toString(gprs_offset + i)));
			}
		}
	;

register:		IDENTIFIER^ ASSIGN! NUMBER
	;

maps:			MAPS^ LBRACK! (map LINE_SEPERATOR!)+ RBRACK!
	;

map:			MAP_TYPE^ LPAREN! NUMBER ARG_SEPERATOR! NUMBER RPAREN!
	;

instructions:		INSTRUCTIONS^ LBRACK! instruction+ RBRACK!
	;

instruction:		IDENTIFIER params arguments? LBRACK expr+ RBRACK
		-> ^(IDENTIFIER params ^(ARGUMENTS arguments?) ^(EXPR expr+))
	;

params	:		OPCODE (ARG_SEPERATOR OPCODE)* (ARG_SEPERATOR CLOCK ASSIGN NUMBER)?
		-> ^(PARAMS OPCODE+ (CLOCK[$NUMBER.text])?)
	;

arguments:		argument (ARG_SEPERATOR! argument)*
	;

argument:		SIGNED? identifier
	;

expr	:		assignExpr LINE_SEPERATOR!
	|		ifExpr
	|		HALT LINE_SEPERATOR!
	;

assignExpr:		variable ASSIGN^ operatorExpr
	;

ifExpr:			IF^ condition LBRACK! expr+ RBRACK! (ELSE LBRACK! expr+ RBRACK!)?
	;

operatorExpr:		word (operator^ operatorExpr)?
	;

condition:		word comparison^ word
	|		LPAREN! operatorExpr RPAREN! comparison^ word
	;

word 	:		variable (BIT^ (NUMBER|constant))?
	|		NUMBER
	|		NOT^ word
	;

constant:		CONSTANT IDENTIFIER
		-> CONSTANT[$IDENTIFIER.text]
	;

identifier:
			IDENTIFIER
		{
			String t = $IDENTIFIER.text;
			if('R' == (t.charAt(0))
					&& t.length() > 1
					&& Character.isLowerCase(t.charAt(1))) {
				t = t.substring(1);
			}
		}
		-> IDENTIFIER[t]
	;

variable:		constant
	|		identifier
	|		MAP_TYPE^ LPAREN! operatorExpr RPAREN!
	|		multi_register
	;

multi_register: 	multi_identifier LBRACE operatorExpr INTERVAL operatorExpr RBRACE
				-> ^(MULTI_REG multi_identifier operatorExpr operatorExpr)
	;

multi_identifier:	IDENTIFIER | MAP_TYPE
	;

comparison:		EQUALS | LT | LTE | GT | GTE
	;

operator:		AND | OR | XOR | ADD | MINUS | MULT | SHIFT
	;
