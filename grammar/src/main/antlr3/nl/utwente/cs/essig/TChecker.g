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
    import java.util.Map;
    import java.util.HashMap;
}

@members {	
	/**
	 * Toont/verwerkt de exception e.
	 * @param tokenNames te gebruiken beschrijvingen van tokens (meestal niet nodig)
	 * @param e de te tonen/verwerken exception
	 * @require e != null
	 */
	@Override
	public void displayRecognitionError(String[] tokenNames, RecognitionException e) {
		if (e instanceof TCheckerException) {
			// error printen en aantal fouten verhogen, verder niets:
			emitErrorMessage("[ESSIG] error: " + e.getMessage());
			state.syntaxErrors++;
		} else
			super.displayRecognitionError(tokenNames, e);  // standaard ANTLR error handler
	}
	
	/* Probeert node toe te voegen aan symbol table. Als id voor symbol table wordt de tekst van
     * parameter id gebruikt. Wanneer toevoegen niet lukt (id is bijvoorbeeld al gedeclareerd op huidige level),
	 * wordt een error geprint.
	 * @param id tekst van deze Tree wordt gebruikt als id voor symbol table
	 * @param node de aan symbol table toe te voegen node
	 * @require id != null && node != null
	 */
	private void declare(Tree id, CommonTree node) {
		try {
			symbolTable.enter(id.getText(), new CommonTreeEntry(node));
		} catch (SymbolTableException e) {
			displayRecognitionError(null, new TCheckerException(id, "Scope error: " + e.getMessage()));
		}
	}

	private SymbolTable<CommonTreeEntry> symbolTable = new SymbolTable<CommonTreeEntry>();
	private List<String> params = new ArrayList<String>();
}

microcontroller:
	^(IDENTIFIER ^(PARAMETERS parameter*) ^(REGISTERS {
														symbolTable.openScope();
														try {
															symbolTable.enter("R", new CommonTreeEntry(null));
															symbolTable.enter("stack", new CommonTreeEntry(null));
														} catch (SymbolTableException e) {
															displayRecognitionError(null, new TCheckerException(null, "Can't add R (result): " + e.getMessage()));
														}
		                                              }
													  register*)
																 ^(INSTRUCTIONS instruction*) {
																								symbolTable.closeScope();
																							  }
	)
	;

parameter:	
	^(p=(RAM | GPRS | SIZE | CLOCK) NUMBER) {
												if(params.contains($p.text))
													displayRecognitionError(null, new TCheckerException($p, "Duplicate parameter"));
												else
													params.add($p.text);
	                                        }
	;

register:
	id=IDENTIFIER {
					declare($id, $id);
	              }
	;

instruction:	^(id=IDENTIFIER {
									declare($id, $id);
	                            }
                                op=OPCODE { 
											List<Character> opCodeArgs = new ArrayList<Character>();
											String opCodeText = $op.getText();
											for (int i=0; i<opCodeText.length(); i++) {
												char c = opCodeText.charAt(i);
												if (Character.isLetter(c) && !opCodeArgs.contains(c))
													opCodeArgs.add(c);
											}
											params.clear();
											symbolTable.openScope();
		                                  }
	                                      ^(PARAMS param*) ^(ARGUMENTS argument*) {
																					for (Character c: opCodeArgs) {
																						CommonTreeEntry entry = symbolTable.retrieve(c + "");
																						if (entry == null || entry.getLevel() != 1)
																							entry = symbolTable.retrieve("R" + c);
																						if (entry == null || entry.getLevel() != 1)
																							displayRecognitionError(null, new TCheckerException($op, "Opcode mask element " + c + " not specified as instruction argument"));
																					}
																				  }
																					^(EXPR expr+)) {
																									symbolTable.closeScope();
																								   }
	;

param:	
	^(p=(SIZE | CLOCK) NUMBER) {
								if(params.contains($p.text))
									displayRecognitionError(null, new TCheckerException($p, "Duplicate parameter"));
								else
									params.add($p.text);
	                           }
	;

argument:		
	id=IDENTIFIER {
					declare($id, $id);
	              }
	;

expr	:	assignExpr | ifExpr;
assignExpr:	^(ASSIGN word operatorExpr);
ifExpr:		^(IF condition expr+ (ELSE expr+)?);

operatorExpr:	word
	|	^(operator word operatorExpr)
	;

condition:	^(EQUALS operatorExpr word)
	;

word:	
	NUMBER
	|	^(id=IDENTIFIER {
							String idText = $id.getText(), nonIndexed = "";
							int i=0;
							boolean finished = false;
							while (!finished && i<idText.length()) {
								char c = idText.charAt(i);
								if (Character.isLetter(c) && c != 'h' && c != 'l') {
									nonIndexed += c;
									i++;
								} else
									finished = true;
							}
							if (symbolTable.retrieve(nonIndexed) == null)
								displayRecognitionError(null, new TCheckerException($id, "Identifier " + nonIndexed + " not declared"));
					    }
    	NOT? (IDENTIFIER | NUMBER)?)
	;

operator:	AND | OR | XOR | ADD;
