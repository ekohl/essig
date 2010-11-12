package nl.utwente.cs.essig;

import static org.junit.Assert.*;

import org.antlr.runtime.tree.CommonTree;
import org.junit.Test;

public class SymbolTableTest {
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

		// declare a different node under the same name
		String name2 = "bla2";
		CommonTree node2 = new CommonTree();
		symbolTable.declare(name2, node2);
		assertSame(symbolTable.getDeclaration(name2, node2), node2);

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