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
    import java.util.Map;
    import java.util.HashMap;
}

@members {
	List<String> params = new ArrayList<String>();
	Map<String, Integer> registers = new HashMap<String, Integer>();
}

microcontroller:
		^(IDENTIFIER
			^(PARAMETERS parameter*)
			^(REGISTERS register*)
			^(INSTRUCTIONS instruction*)
		)
	;

parameter:	^(p=(RAM | GPRS | SIZE | CLOCK) NUMBER)
{
	if(params.contains($p.text)) {
		throw new TCheckerException(input, "Found duplicate parameter: " + $p.text);
	}
	params.add($p.text);
}
	;

register:	IDENTIFIER
{
	if(registers.containsKey($IDENTIFIER.text)) {
		throw new TCheckerException(input, "Found duplicate register: "+$IDENTIFIER.text);
	}
	registers.put($IDENTIFIER.text, registers.size());
}
	;

instruction:	^(IDENTIFIER OPCODE ^(PARAMS param*) ^(ARGUMENTS argument*) ^(EXPR expr+))
	;

param	:	^((SIZE | CLOCK) NUMBER)
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
