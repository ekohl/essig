package nl.utwente.cs.essig;

import java.util.HashMap;
import java.util.Map;
import java.util.Stack;

import org.antlr.runtime.tree.CommonTree;

/**
 * Symbol table with support for scopes.
 * <p>
 * Note that this implementation is not thread safe.
 *
 * @author Ewoud Kohl van Wijngaarden
 */
public class SymbolTable<Entry extends CommonTree> {
	/**
	 * Symbol table. Each entry is a scope. In each entry the key is the
	 * variable name and the value is the type of that variable.
	 */
	private final Stack<Map<String, Entry>> table;

	/**
	 * Constructor. Constructs an empty SymbolTable. For convenience an empty
	 * scope will be opened.
	 *
	 * @ensure this.currentLevel() == -1
	 */
	public SymbolTable() {
		table = new Stack<Map<String, Entry>>();
	}

	/**
	 * Closes the current scope.
	 *
	 * @return The size of the closed scope
	 *
	 * @require getCurrentLevel() &gt;= 0
	 * @ensure this.currentLevel() == old.currentLevel()-1
	 */
	public void closeScope() {
		table.pop();
	}

	/**
	 * Declares the <code>node</code> with the variable name <code>var</code>.
	 *
	 * @param var
	 *            The name of the variable to set
	 * @param node
	 *            The node of the variable that is being declared
	 * @throws SymbolTableException
	 *             In case <code>var</code> is already defined on this scope
	 *
	 * @return The type of the declared variable
	 *
	 * @require var != null
	 */
	public void declare(String var, Entry node) throws SymbolTableException {
		if (table.peek().put(var, node) != null)
			throw new SymbolTableException(node, "var " + var
					+ " already defined in current scope.");
	}

	/**
	 * Returns the current scope level.
	 *
	 * The first scope opened has level 0, and for each nested scope the number
	 * is incremented by 1. If no scope has been defined the level is -1.
	 *
	 * @return The current level
	 *
	 * @ensure result &gt;= -1
	 */
	public int getCurrentLevel() {
		return table.size() - 1;
	}

	/**
	 * Returns the declaration of <code>var</code>.
	 *
	 * @param var
	 *            The variable to get the declaration of
	 * @param node
	 *            The node that declared the variable. Only used in case
	 *            it's undefined.
	 *
	 * @return The declaration of <code>var</code>
	 *
	 * @ensure result != null
	 */
	public Entry getDeclaration(String var, Entry node) throws SymbolTableException {
		// Search the stack from top to bottom
		for (int i = table.size() - 1; i >= 0; i--) {
			Entry type = table.get(i).get(var);
			if (type != null)
				return type;
		}

		// Apparently it's undeclared which is not good
		throw new SymbolTableException(node, "var " + var + " is undeclared");
	}

	/**
	 * Opens a new scope.
	 *
	 * @ensure this.currentLevel() == old.currentLevel()+1
	 */
	public void openScope() {
		table.push(new HashMap<String, Entry>());
	}

	/**
	 * Returns a textual representation of this Symbol Table
	 */
	@Override
	public String toString() {
		StringBuffer sb = new StringBuffer("Symbol table for current level:\n");
		// FIXME: Print all scopes
		for (Map.Entry<String, Entry> var : table.peek().entrySet()) {
			sb.append(var.getKey() + " " + var.getValue() + "\n");
		}
		return sb.toString();
	}
}
