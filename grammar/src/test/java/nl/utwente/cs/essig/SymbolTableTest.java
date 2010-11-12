package nl.utwente.cs.essig;

import static org.junit.Assert.*;

import org.antlr.runtime.tree.CommonTree;
import org.junit.Test;

public class SymbolTableTest {
	/**
	 * Test the symbol table by simulating the following code:
	 * 
	 * <pre>
	 * {
	 *   bla = node
	 *   bla == node
	 *   bla = node // Should fail
	 * 	 {
	 * 	   bla = node2
	 *     bla == node2
	 *     bla3 = node3
	 *     bla3 == node3
	 *   }
	 *   bla == node
	 *   bla3 == null // bla3 should no longer exist
	 * }
	 * </pre>
	 * 
	 * @throws SymbolTableException
	 */
	@Test
	public final void testSymbolTable() throws SymbolTableException {
		SymbolTable<CommonTree> symbolTable = new SymbolTable<CommonTree>();

		// Declare name in the first scope
		symbolTable.openScope();
		assertEquals(symbolTable.getCurrentLevel(), 0);

		String name = "bla";
		CommonTree node = new CommonTree();
		symbolTable.declare(name, node);
		assertSame(symbolTable.getDeclaration(name, node), node);

		// Expect that we cannot redeclare it
		try {
			symbolTable.declare(name, node);
			fail("No exception thrown while redeclaring the same name");
		} catch (SymbolTableException e) {
			// We expect an exception here
		}

		// Open a new scope
		symbolTable.openScope();
		assertEquals(symbolTable.getCurrentLevel(), 1);

		// redeclare name with a different node
		CommonTree node2 = new CommonTree();
		symbolTable.declare(name, node2);
		assertSame(symbolTable.getDeclaration(name, node2), node2);
		assertNotSame(symbolTable.getDeclaration(name, node), node);

		String name3 = "bla3";
		CommonTree node3 = new CommonTree();
		symbolTable.declare(name3, node3);
		assertSame(symbolTable.getDeclaration(name3, node3), node3);

		// Close the second scope and expect to get the first node back
		symbolTable.closeScope();
		assertEquals(symbolTable.getCurrentLevel(), 0);
		assertSame(symbolTable.getDeclaration(name, node), node);

		// Expect that name3 is no longer defined
		try {
			symbolTable.getDeclaration(name3, node3);
			fail("No exception thrown for expected undeclared node");
		} catch (SymbolTableException e) {
			// We expect an exception here
		}
	}
}