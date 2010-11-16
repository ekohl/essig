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
			OPCODE { Opcode opcode = new Opcode($OPCODE.text);}
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
		opcodeparsed={opcode}
	)
	;

param	: ^(i=word  v=word)
	-> param(name={$i.st},value={$v.comment},comment={$i.st + "=" + $v.comment})
	|	^(CLOCK NUMBER)
	-> template(cycles={$NUMBER.text}) "state->cycles += <cycles>;"
	|	^(SIZE NUMBER)
	-> template(v={$NUMBER.text}) "// FIXME size = <v>;"
	;

argument:  {int signed=0;} (SIGNED{signed=0;})? IDENTIFIER
		-> template(name={$IDENTIFIER},signed={($SIGNED!=null)}) "<if(signed)> <name> = (int) vm_convert_to_signed(<name>,<name>_bits); <endif>"
	;

expr	:	assignExpr
	-> {$assignExpr.st}
	|	ifExpr
	-> {$ifExpr.st}
	;

assignExpr:	^(ASSIGN {Variable var = new Variable("A");} (IDENTIFIER{var = new Variable($IDENTIFIER.text);}  | RAM op2=operatorExpr {var = new Variable(""+$op2.st,Variable.VariableType.RAM);} ) o=operatorExpr)
	-> assignExpr(var={var},type={var.getType()},value={$o.st},comment={" = " + $o.comment}, is_result={var.getName().equals("R")})
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
	|	^( v=IDENTIFIER NOT? (IDENTIFIER|NUMBER)? )
		{
			Variable var = new Variable($v.text);
			if ($NUMBER!=null) var = new Variable($NUMBER.text,Variable.VariableType.REGISTER);			
			$comment = (($NOT != null) ? $NOT.text : "") + $v.text;
			
		}
	// FIXME: Also handle $i
	-> wordVariable (variable={var.getName()},bit={var.getNumber()},type={var.getType()},not={$NOT != null})
	|		RAM operatorExpr
	;

operator:      (o=AND | o=OR | o=XOR | o=ADD | o=MINUS | o=MULT)
	-> template(operator={$o}) "<operator>"
	;
