package nl.utwente.cs.essig;

import static org.junit.Assert.*;

import org.junit.Test;

/**
 * @author ekohl
 * 
 */
public class VariableTest {
	/**
	 * Test a simple register (I)
	 */
	@Test
	public final void testRegister() {
		Variable variable = new Variable("I");
		assertEquals(variable.getName(), "I");
		assertNull(variable.getNumber());
		assertEquals(variable.getType(), Variable.VariableType.REGISTER);
	}

	/**
	 * Test a constant (general purpose) register (R1)
	 */
	@Test
	public final void testConstantRegister() {
		Variable variable = new Variable("R1");
		assertEquals(variable.getName(), "R");
		assertEquals(variable.getNumber(), new Integer(1));
		assertEquals(variable.getType(), Variable.VariableType.REGISTER);
		assertEquals(variable.toString(), "R1");
	}

	/**
	 * Test a variable (general purpose) register (Rd)
	 */
	@Test
	public final void testVariableRegister() {
		Variable variable = new Variable("Rd");
		assertEquals(variable.getName(), "d");
		assertNull(variable.getNumber());
		assertEquals(variable.getType(), Variable.VariableType.REGISTER);
		assertEquals(variable.toString(), "d");
	}

	/**
	 * Test a RAM variable (A)
	 */
	@Test
	public final void testRAM() {
		Variable variable = new Variable("A");
		assertEquals(variable.getName(), "A");
		assertNull(variable.getNumber());
		assertEquals(variable.getType(), Variable.VariableType.RAM);
	}
}