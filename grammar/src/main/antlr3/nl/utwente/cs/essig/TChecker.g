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
		^(p=(RAM | CLOCK | OP_SIZE) NUMBER) {
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
		^((CHUNK | REGISTERS | IO | RAM) NUMBER NUMBER)
	;

instruction:
		^(
			IDENTIFIER
			{ symbolTable.openScope(); }
			^(
				PARAMS
				(opcodes+=OPCODE)+
				(^(CLOCK NUMBER))?
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
			^(EXPR expr {
				// First expression declares R
				symbolTable.declare("R", $EXPR);
			} expr* )
			{ symbolTable.closeScope(); }
		)
	;

argument:		
		SIGNED? IDENTIFIER {
			symbolTable.declare(new Variable($IDENTIFIER.text).getName(), $IDENTIFIER);
		}
	;

expr:
		assignExpr | ifExpr | HALT
	;

assignExpr:
		^(ASSIGN (CONSTANT? IDENTIFIER (LPAREN (operatorExpr) RPAREN)? | (RAM operatorExpr)) operatorExpr)
	|	^(MULTI_REG IDENTIFIER operatorExpr IDENTIFIER operatorExpr operatorExpr)
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
		NUMBER
	|	^(id=IDENTIFIER NOT? CONSTANT? (IDENTIFIER|n=NUMBER)? )
		{
			Variable var;
			if ($n == null ) {
				var = new Variable($id.text);
			} else {
				var = new Variable($id.text + $n.text,Variable.VariableType.REGISTER);
			}
			symbolTable.getDeclaration(var.getName(), $id);
		}
	|	^(RAM operatorExpr)
	|	multi_register
	;

multi_register:
	^(MULTI_REG multi_identifier operatorExpr IDENTIFIER? operatorExpr)
	;

multi_identifier : IDENTIFIER | RAM;

comparison:
		EQUALS | LT | LTE | GT | GTE
	;

operator:
		AND | OR | XOR | ADD | MINUS | MULT | SHIFT
	;
