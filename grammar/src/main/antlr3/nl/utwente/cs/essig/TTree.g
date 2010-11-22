tree grammar TTree;

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
	private int gprs;

	private List addGPRS(List l) {
		for(int i=0; i<this.gprs; i++) {
			StringTemplate attr = new StringTemplate();
			attr.setAttribute("name", "R" + i);
			l.add(i, attr);
		}
		return l;
	}
}

microcontroller: ^(
			IDENTIFIER
			^(PARAMETERS (p+=parameter)+)
			^(REGISTERS (r+=register)*)
			^(INSTRUCTIONS (i+=instruction)*)
		)
	-> microcontroller(name={$IDENTIFIER},parameters={$p},registers={addGPRS($r)},instructions={$i})
	;

parameter:	^(RAM NUMBER)
	-> ram(ram={$NUMBER})
	|	^(GPRS NUMBER) { gprs = Integer.parseInt($NUMBER.text); }
	|	^(SIZE NUMBER)
	-> template(size={$NUMBER.text}) "// FIXME: size = <size>;"
	|	^(CLOCK NUMBER)
	-> template(clock={$NUMBER.text}) "// FIXME: clock = <clock>;"
	;

register:	IDENTIFIER -> register(name={$IDENTIFIER});

instruction:	^(
			IDENTIFIER
			op=OPCODE { Opcode opcode = new Opcode($op.text);}
			(op2=OPCODE { Opcode opcode2 = new Opcode($op2.text);})?
			^(PARAMS (p+=param)*)
			^(ARGUMENTS (a+=argument)*)
			^(EXPR (e+=expr)+)
		)
	-> instruction(
		name={$IDENTIFIER},
		params={$p},
		arguments={$a},
		expressions={$e},
		mask={opcode.getMaskString()},
		opcode={opcode.getOpcodeString()},
		next_is_arg={($op2!=null ? "true" : "false")},
		opcodeparsed={opcode}
	)
	;

param	: ^(i=word  v=word)
	-> param(name={$i.st},value={$v.comment},comment={$i.st + "=" + $v.comment})
	|	^(CLOCK NUMBER)
	-> cycles(cycles={$NUMBER.text})
	|	^(SIZE NUMBER)
	-> template(v={$NUMBER.text}) "// FIXME size = <v>;"
	;

argument:	SIGNED? IDENTIFIER
	-> argument(name={$IDENTIFIER},signed={$SIGNED})
	;

expr	:	assignExpr
	-> {$assignExpr.st}
	|	ifExpr
	-> {$ifExpr.st}
	| HALT -> halt()
	;

assignExpr:	^(
			ASSIGN { Variable var = new Variable("A"); }
			(
				IDENTIFIER
					{ var = new Variable($IDENTIFIER.text); }
				| RAM op2=operatorExpr
					{ var = new Variable($op2.st.toString(),Variable.VariableType.RAM); }
			)
			o=operatorExpr
		)
	-> assignExpr(var={var},type={var.getType()},value={$o.st},comment={var + " = " + $o.comment}, is_result={var.getName().equals("R")})
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

condition:	^(EQUALS l=operatorExpr r=word)
	-> condition(left={$l.st},right={$r.st})
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
	;

operator:      (o=AND | o=OR | o=XOR | o=ADD | o=MINUS | o=MULT)
	-> template(operator={$o}) "<operator>"
	;
