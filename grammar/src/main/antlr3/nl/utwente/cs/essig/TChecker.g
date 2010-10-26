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
}

// What package should the generated source exist in?
//
@header {
    package nl.utwente.cs.essig;
}

@members {
	List<String> params = new ArrayList<String>();
}

microcontroller:
		^(IDENTIFIER
			^(PARAMETERS parameter*)
			^(REGISTERS register*)
			^(INSTRUCTIONS instruction*)
		)
	;

parameter
@init {
	String paramName = ((CommonTree) (input.LT(1))).getText();
      }
	:	^((RAM | GPRS | SIZE | CLOCK) NUMBER)
{
	if (!params.contains(paramName))
		params.add(paramName);
	else
		System.out.println("Param " + paramName + " defined more than once");
}
	;

register:	IDENTIFIER
	;

instruction:	^(IDENTIFIER ^(OP_CODE OPCODE) ^(PARAMS param*) ^(ARGUMENTS argument*) ^(EXPR expr+))
	;

param	:	^((SIZE | CLOCK) NUMBER)
//	|	^(OP_CODE OPCODE)
	;

argument:	IDENTIFIER
	;

expr	:	assignExpr | ifExpr;
assignExpr:	^(ASSIGN IDENTIFIER operatorExpr);
ifExpr:		^(IF condition expr+ (ELSE expr+)?);

operatorExpr:	word
	|	^(operator word operatorExpr)
	;

condition:	^(EQUALS operatorExpr word)
	;

word	:	NUMBER
	|	^(IDENTIFIER NOT? (IDENTIFIER | NUMBER)?)
	;

operator:	AND | OR | XOR | ADD;
