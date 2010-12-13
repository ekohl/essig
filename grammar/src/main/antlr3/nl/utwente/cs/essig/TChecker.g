tree grammar TChecker;

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

	// Use AbstractTChecker to get custom error reporting
	//
	superClass = AbstractTChecker;
}

// What package should the generated source exist in?
//
@header {
	package nl.utwente.cs.essig;

	import java.util.HashSet;
	import java.util.Set;
}

@members {	
	private SymbolTable<CommonTree> symbolTable = new SymbolTable<CommonTree>();
	private Set<String> params = new HashSet<String>();
}

microcontroller:
		^(IDENTIFIER
			{ symbolTable.openScope(); }
			^(PARAMETERS parameter+)
			^(REGISTERS register+)
			^(MAPS map+)
			^(INSTRUCTIONS instruction+)
			{ symbolTable.closeScope(); }
		)
	;

parameter:	
		(p=(CLOCK | OP_SIZE) | ^(p=ENDIANNESS .)) {
			if(!params.add($p.text)) {
				throw new TCheckerException($p, "Duplicate parameter " + $p.text);
			}
		}
	;

register:
		// FIXME Check NUMBER
		^(IDENTIFIER (NUMBER | multiword_register)) {
			symbolTable.declare($IDENTIFIER.text, $IDENTIFIER);
		}
	;

multiword_register:
		^(IDENTIFIER IDENTIFIER+)
	;

map:
		// FIXME Check if maps aren't duplicated
		^(MAP_TYPE NUMBER NUMBER)
	;

instruction:
		^(
			IDENTIFIER
			{ symbolTable.openScope(); }
			^(
				PARAMS
				(opcodes+=OPCODE)+
				CLOCK?
			)
			^(ARGUMENTS argument*)
			{
				// Verify opcodes with the defined arguments
				for(Object opcode : $opcodes) {
					for (Character c : new Opcode(((CommonTree)opcode).getText()).getArguments().keySet()) {
						symbolTable.getDeclaration(c.toString(), (CommonTree) opcode);
					}
				}
			}
			^(EXPR { symbolTable.declare("R", $EXPR); } expr+ )
			{ symbolTable.closeScope(); }
		)
	;

argument:		
		SIGNED? IDENTIFIER {
			symbolTable.declare($IDENTIFIER.text, $IDENTIFIER);
		}
	;

expr:
		assignExpr | ifExpr | HALT
	;

assignExpr:
		^(ASSIGN variable operatorExpr)
	;

ifExpr:
		^(IF condition expr+ (ELSE expr+)?)
	;

operatorExpr:
		word
	|	^(operator word operatorExpr)
	;

condition:
		^(comparison operatorExpr word)
	;

word:
		variable
	|	NUMBER
	|	^(NOT word)
	|	^(BIT variable (NUMBER|CONSTANT))
	;

variable:
		v=(CONSTANT|IDENTIFIER)
		{ symbolTable.getDeclaration($v.text, $v); }
	|	^(MAP_TYPE operatorExpr)
	|	multi_register
	;

multi_register:
		^(MULTI_REG multi_identifier operatorExpr operatorExpr)
	;

multi_identifier:
		IDENTIFIER
	|	MAP_TYPE
	;

comparison:
		EQUALS | LT | LTE | GT | GTE
	;

operator:
		AND | OR | XOR | ADD | MINUS | MULT | SHIFT
	;
