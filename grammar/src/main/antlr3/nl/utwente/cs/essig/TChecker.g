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
}

@members {	
	private SymbolTable<CommonTree> symbolTable = new SymbolTable<CommonTree>();
	private List<String> params = new ArrayList<String>();
}

microcontroller:
		^(IDENTIFIER
			{ symbolTable.openScope(); }
			^(PARAMETERS parameter*)
			^(REGISTERS register*)
			^(INSTRUCTIONS instruction*)
			{ symbolTable.closeScope(); }
		)
	;

parameter:	
		^(p=(RAM | GPRS | SIZE | CLOCK) NUMBER) {
			if(params.contains($p.text)) {
				throw new TCheckerException($p, "Duplicate parameter " + $p.text);
			}
			params.add($p.text);

			int number;
			try {
				number = Integer.parseInt($NUMBER.text);
			} catch(Exception nfe) {
				throw new TCheckerException($NUMBER, nfe.getMessage());
			}

			if($p.text.equals("gprs")) { // FIXME: Ugly check
				for(int i = 0; i < number; i++) {
					symbolTable.declare("R" + i, $p); // FIXME: Correct node
				}
			}
		}
	;

register:
		IDENTIFIER {
			symbolTable.declare($IDENTIFIER.text, $IDENTIFIER);
		}
	;

instruction:
		^(
			IDENTIFIER
			op=OPCODE {
				Opcode opcode = new Opcode($op.text);
				symbolTable.openScope();
				params.clear();
			}
			(op2=OPCODE {
				Opcode opcode2 = new Opcode($op2.text);
				//System.out.println(opcode2.toString());
				//symbolTable.openScope();
				//params.clear();
			})?
			^(PARAMS param*)
			^(ARGUMENTS argument*)
			{
				for (Character c : opcode.getArguments().keySet()) {
					symbolTable.getDeclaration(c + "", $op);
				}
			}
			^(EXPR expr {
				symbolTable.declare("R", $EXPR);
			} expr* )
			{ symbolTable.closeScope(); }
		)
	;

param:	
		^(p=(SIZE | CLOCK) NUMBER) {
			if(params.contains($p.text)) {
				throw new TCheckerException($p, "Duplicate parameter " + $p.text);
			}
			params.add($p.text);
		}
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
		^(ASSIGN (IDENTIFIER | (RAM operatorExpr)) operatorExpr)
	;
ifExpr:
		^(IF condition expr+ (ELSE expr+)?)
	;

operatorExpr:
		word
	|	^(operator word operatorExpr)
	;

condition:
		^(EQUALS operatorExpr word)
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
	;

operator:	AND | OR | XOR | ADD | MINUS | MULT;
