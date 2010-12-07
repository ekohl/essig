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
	;

register:
			^(IDENTIFIER (NUMBER | multiword_register))
		-> register(name={$IDENTIFIER},offset={$NUMBER})
	;

multiword_register:
			^(IDENTIFIER IDENTIFIER+)
	;


map:			^(t=map_type b=NUMBER e=NUMBER)
		-> map(type={$t.st}, begin={$b}, end={$e})
	;

map_type:
			(t=CHUNK | t=REGISTER | t=IO | t=ROM | t=RAM)
		-> template(type={($t.text).toUpperCase()}) "<type>"
	;
