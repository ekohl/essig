package nl.utwente.cs.essig;

import org.antlr.runtime.*;
import org.antlr.runtime.tree.TreeNodeStream;
import org.antlr.runtime.tree.TreeParser;

/**
 * The super class of the generated checker. It is extended by the generated
 * code because of the superClass option in the .g file.
 * 
 * This class contains any helper functions used within the parser grammar
 * itself, as well as any overrides of the standard ANTLR Java runtime methods,
 * such as an implementation of a custom error reporting method, symbol table
 * populating methods and so on.
 * 
 * @author Jim Idle - Temporal Wave LLC - jimi@temporal-wave.com
 * @author Ewoud Kohl van Wijngaarden
 */
public abstract class AbstractTChecker extends TreeParser {
	/**
	 * Create a new checker instance, pre-supplying the input token stream.
	 * 
	 * @param input
	 *            The stream of nodes that will be pulled from the parser
	 */
	protected AbstractTChecker(TreeNodeStream input) {
		super(input);
	}

	/**
	 * Create a new checker instance, pre-supplying the input token stream and
	 * the shared state.
	 * 
	 * This is only used when a grammar is imported into another grammar, but we
	 * must supply this constructor to satisfy the super class contract.
	 * 
	 * @param input
	 *            The stream of nodes that will be pulled from the parser
	 * @param state
	 *            The shared state object created by an interconnectd grammar
	 */
	protected AbstractTChecker(TreeNodeStream input, RecognizerSharedState state) {
		super(input, state);
	}

	@Override
	public String getErrorMessage(RecognitionException e, String[] tokenNames) {
		if (e instanceof TCheckerException) {
			return e.getMessage();
		}
		return super.getErrorMessage(e, tokenNames);
	}
}
