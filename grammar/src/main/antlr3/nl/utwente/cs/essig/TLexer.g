lexer grammar TLexer;

options {

   language=Java;  // Default

   // Tell ANTLR to make the generated lexer class extend the
   // the named class, which is where any supporting code and
   // variables will be placed.
   //
   superClass = AbstractTLexer;
}

// What package should the generated source exist in?
//
@header {

    package nl.utwente.cs.essig;
}

// Lexer
//
REGISTERS	:	'registers';
PARAMETERS	:	'parameters';
INSTRUCTIONS	:	'instructions';
MAPS		:	'maps';
IF		:	'if';
ELSE		:	'else';
GPRS		:	'gprs';
CLOCK		:	'clock';
OP_SIZE		:	'opcode-size';
ENDIANNESS	:	'endianness';

MAP_TYPE		:	CHUNK | IO | RAM | ROM | REGISTER | PRINT;
fragment CHUNK		:	'chunk';
fragment IO		:	'io';
fragment RAM		:	'ram';
fragment ROM		:	'rom';
fragment REGISTER	:	'register';
fragment PRINT		:	'print';

HALT		:       'HALT';
CONSTANT	:	'$';

BIG		:	'big';
LITTLE		:	'little';

IDENTIFIER		:	LETTER (LETTER | DIGIT)* ;

NUMBER			:	(NUM | HEX_NUM);

fragment NUM		:	MINUS? DIGIT+;

fragment HEX_NUM	:	'0' 'x' (DIGIT | HEX_LETTER)+;

WHITESPACE		:	( '\t' | ' ' | '\r' | '\n'| '\u000C' )+ 	{ $channel = HIDDEN; } ;

COMMENT
			:	'//' ~('\n'|'\r')* '\r'? '\n' {skip();}
			|	'/*' ( options {greedy=false;} : . )* '*/' {skip();}
			;

OPCODE			:	QUOTE (DIGIT | LETTER | ' ')+ QUOTE;

fragment DIGIT		:	'0'..'9';

fragment LETTER		:	('a'..'z'|'A'..'Z');

fragment HEX_LETTER	:	('a'..'f'|'A'..'F');

LBRACK		:	'{';
RBRACK		:	'}';
LBRACE		:	'[';
RBRACE		:	']';
LPAREN		:	'(';
RPAREN		:	')';
ASSIGN		:	'=';
LINE_SEPERATOR	:	';';
COLON		:	':';
ARG_SEPERATOR	:	',';
BIT		:	'.';
INTERVAL	:	'..';
EQUALS		:	'==';
QUOTE		:	'"';


// Logical operators
NOT		:	'!';
SIGNED		:	'~';
AND		:	'&';
OR		:	'|';
XOR		:	'^';
ADD		:	'+';
MINUS		:	'-';
MULT		:	'*';
GT		:	'>';
GTE		:	'>=';
LT		:	'<';
LTE		:	'=<';
SHIFT		: 	'<<' | '>>';
