package nl.utwente.cs.essig;

import static org.junit.Assert.*;

import org.junit.Test;

public class OpcodeTest {
	/**
	 * Test an empty opcode.
	 */
	@Test
	public final void testEmpty() {
		Opcode emptyOpcode = new Opcode("");
		String expectedResult = "";
		assertEquals(expectedResult, emptyOpcode.toString());
	}

	/**
	 * Test a binary opcode (<code>0000 0000 0000 0000</code>).
	 */
	@Test
	public final void testBinary() {
		Opcode binaryOpcode = new Opcode("0000 0000 0000 0000");
		String expectedResult = "";
		assertEquals(expectedResult, binaryOpcode.toString());
	}

	/**
	 * Test an opcode with multiple registers (<code>00 0111 rdddd dr rrr</code>
	 * ).
	 */
	@Test
	public final void testComplex() {
		Opcode complexOpcode = new Opcode("00 0111 rdddd dr rrr");
		String expectedResult = "int d = 0; AddBit(&d,opcode,8); AddBit(&d,opcode,7); AddBit(&d,opcode,6); AddBit(&d,opcode,5); AddBit(&d,opcode,4); \nint r = 0; AddBit(&r,opcode,9); AddBit(&r,opcode,3); AddBit(&r,opcode,2); AddBit(&r,opcode,1); AddBit(&r,opcode,0); \n";
		assertEquals(expectedResult, complexOpcode.toString());
	}
}