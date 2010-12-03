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
	//private SymbolTable<CommonTree> symbolTable = new SymbolTable<CommonTree>();
}

microcontroller: ^(
			IDENTIFIER
			^(PARAMETERS (p+=parameter)+)
			^(REGISTERS (r+=register)*)
			^(MAPS map+)
			^(INSTRUCTIONS (i+=instruction)*)
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
	|       ^(IDENTIFIER multiword_register {}) -> register(name={$IDENTIFIER},offset={"123"})
	;

//register_type : 
//	NUMBER 	-> template (number={$NUMBER}) "<number>"
//	| multiword_register -> template(operator={"123"}) "<operator>" 
//	;

 multiword_register : 	^(IDENTIFIER IDENTIFIER+);

map:		^((CHUNK | REGISTERS | IO | RAM) NUMBER NUMBER)
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
	-> argument(name={new Variable($IDENTIFIER.text).getName()},signed={$SIGNED})
	;

expr	:	assignExpr
	-> {$assignExpr.st}
	|	ifExpr
	-> {$ifExpr.st}
	| HALT
	-> halt()
	;

assignExpr:	^(
			ASSIGN { Variable var = new Variable("A"); }
			(
				CONSTANT? IDENTIFIER (LPAREN (op=operatorExpr) RPAREN)?
					{ 
						var = new Variable($IDENTIFIER.text + ($op.st!=null?$op.st:"")); if ($CONSTANT!=null) var.setConstant();
						if (var.getName().equals("R")) { var.setConstant(false); var.setResult(); }
						if ($op.st!=null) {var.setConstant(false); var.setResult(false);}
					
					}
				| RAM op2=operatorExpr
					{ var = new Variable($op2.st.toString(),Variable.VariableType.RAM); }
			)
			o=operatorExpr
		)
	-> assignExpr(var={var},type={var.getType()},value={$o.st},comment={var + " = " + $o.comment}, is_result={var.isResult()},constant={var.getConstant()})
	|	^(MULTI_REG i1=IDENTIFIER o1=operatorExpr i2=IDENTIFIER o2=operatorExpr o3=operatorExpr)
	-> multiRegisterAssignExpr(low={$o1.st},high={$o2.st},value={$o3.st})
	;


ifExpr	:	^(IF condition (i+=expr)+ (ELSE (e+=expr)+)?)
	-> ifExpr(condition={$condition.st},ifExpr={$i},elseExpr={$e})
	;

operatorExpr returns [String comment = ""] :
		word  {$comment = $word.comment;}
	-> {$word.st}
	|	^(o=operator w=word e=operatorExpr) {$comment = $w.comment + " " +  $o.st + " " + $e.comment +  " ";}
	-> operatorExpr(operator={$o.st},word={$w.st},expression={$e.st})
	;

condition:	^(c=comparison l=operatorExpr r=word)
	-> condition(left={$l.st},comparison={$c.st},right={$r.st})
	;

word returns [String comment = ""]:
		NUMBER {$comment = $NUMBER.text;}
	-> template (number={$NUMBER}) "<number>"
	|	^( v=IDENTIFIER NOT? CONSTANT? (IDENTIFIER|NUMBER)? )
		{
			Variable var;
			if ($NUMBER == null ) {
				var = new Variable($v.text);
			} else {
				var = new Variable($v.text + $NUMBER.text,Variable.VariableType.REGISTER);
			}
			if ($CONSTANT!=null) var.setConstant();
			
			$comment = (($NOT != null) ? $NOT.text : "") + $v.text;
			
		}
	// FIXME: Also handle $i
	-> wordVariable (variable={var.getName()},bit={var.getNumber()},type={var.getType()},not={$NOT != null},constant={var.getConstant()})
	|	^(RAM operatorExpr)
		{ $comment = $RAM + "(" + $operatorExpr.comment + ")"; }
	-> wordVariable(variable={$operatorExpr.st}, type={"RAM"})
	|	^(MULTI_REG IDENTIFIER o1=operatorExpr IDENTIFIER o2=operatorExpr)
	-> multiRegister(r1={$o1.st},r2={$o2.st})
	;

comparison:		(c=EQUALS | c=LT | c=LTE | c=GT | c=GTE)
	-> template(c={$c}) "<c>"
	;

operator:      (o=AND | o=OR | o=XOR | o=ADD | o=MINUS | o=MULT | o=SHIFT)
	-> template(operator={$o}) "<operator>"
	;
