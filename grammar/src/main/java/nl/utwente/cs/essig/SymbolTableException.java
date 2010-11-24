package nl.utwente.cs.essig;

import org.antlr.runtime.tree.CommonTree;

/**
 * Exception class to signal problems with the SymbolTable
 */
public class SymbolTableException extends TCheckerException {
	/** Generated serial UID */
	private static final long serialVersionUID = -4214883471809719605L;

	/**
	 * Constructor
	 * 
	 * @param node
	 *            The node which caused the exception
	 * @param message
	 *            The error message
	 */
	public SymbolTableException(CommonTree node, String message) {
		super(node, message);
	}
}
