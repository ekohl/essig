grammar essig;

options {
	output = AST;
}

tokens {
	MICROCONTROLLER =	'uc';
	LBRACK		=	'{';
	RBRACK		=	'}';
	ASSIGN		=	'=';
	REGISTER	=	'register';
	LINE_SEPERATOR	=	';';
	
	// Logical operators
	NOT		=	'!';
	AND		=	'&';
	OR		=	'|';
}

/*
 * Parser
 */
microcontroller:	IDENTIFIER LBRACK! registers? instructions? RBRACK! EOF!;

registers:		(register LINE_SEPERATOR!)+;
register:		REGISTER IDENTIFIER ;

instructions:		instruction+;
instruction:		IDENTIFIER LBRACK! expr+ RBRACK!;

expr:			assignExpr LINE_SEPERATOR!;
assignExpr:		IDENTIFIER ASSIGN NOT? IDENTIFIER (operator NOT? IDENTIFIER)*;

operator:		AND | OR;

/*
 * Lexer
 */
IDENTIFIER		:	LETTER (LETTER | DIGIT)*;

WHITESPACE		:	( '\t' | ' ' | '\r' | '\n'| '\u000C' )+ 	{ $channel = HIDDEN; } ;

ML_COMMENT		:	 '/*' ( options {greedy=false;} : . )* '*/';

fragment DIGIT		:	'0'..'9';

fragment LETTER		:	('a'..'z'|'A'..'Z');
