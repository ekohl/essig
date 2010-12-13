tree grammar THeader;

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

microcontroller:
			^(
				IDENTIFIER
				^(PARAMETERS (p+=parameter)+)
				^(REGISTERS (r+=register)+)
				^(MAPS (m+=map+))
				^(INSTRUCTIONS .+)
			)
		-> header(parameters={$p},registers={$r},maps={$m})
	;

parameter:
			CLOCK
	|		OP_SIZE
		-> opcode_size(bits={$OP_SIZE})
	|		^(ENDIANNESS .)
	;

register:
			^(IDENTIFIER NUMBER)
		-> register(name={$IDENTIFIER},offset={$NUMBER})
	;

map:			^(t=MAP_TYPE b=NUMBER e=NUMBER)
		-> map(type={($t.text).toUpperCase()}, begin={$b}, end={$e})
	;
