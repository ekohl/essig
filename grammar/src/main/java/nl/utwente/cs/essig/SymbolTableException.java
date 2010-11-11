package nl.utwente.cs.essig;

import org.antlr.runtime.tree.CommonTree;

/**
 * Exception class to signal problems with the SymbolTable
 */
public class SymbolTableException extends TCheckerException {
	public SymbolTableException(CommonTree node, String msg) {
		super(node, msg);
	}
}
