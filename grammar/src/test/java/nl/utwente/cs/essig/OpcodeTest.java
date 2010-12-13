package nl.utwente.cs.essig;

import static org.junit.Assert.*;

import org.junit.Test;

public class OpcodeTest {
	/**
	 * Test an empty opcode.
	 */
	@Test
	public final void testEmpty() {
		String opcode = "";
		String expectedString = "";
		String expectedMask = "0000 0000 0000 0000";
		String exampleOpcode = "0000 0000 0000 0000";

		test(opcode, expectedString, expectedMask, exampleOpcode);
	}

	/**
	 * Test binary opcodes
	 */
	@Test
	public final void testBinary() {
		// A full binary is always the same, thus the mask is always the full
		// length. The opcode is equal to the input.
		String mask = Integer.toBinaryString(0xFFFF);
		for (int i = 0; i < 0xFFFF; i++) {
			String opcode = toOpcode(i, 16);
			test(opcode, "", mask, opcode, opcode);
		}
	}

	/**
	 * Test an opcode with multiple registers (<code>00 0111 rdddd dr rrr</code>
	 * ).
	 */
	@Test
	public final void testComplex() {
		String opcode = "00 0111 rdddd dr rrr";
		String expectedString = "d_bits += 5;\n" +
				"AddBit(&d,opcode,8);\n" +
				"AddBit(&d,opcode,7);\n" +
				"AddBit(&d,opcode,6);\n" +
				"AddBit(&d,opcode,5);\n" +
				"AddBit(&d,opcode,4);\n" +
				"\n" +
				"r_bits += 5;\n" +
				"AddBit(&r,opcode,9);\n" +
				"AddBit(&r,opcode,3);\n" +
				"AddBit(&r,opcode,2);\n" +
				"AddBit(&r,opcode,1);\n" +
				"AddBit(&r,opcode,0);\n" +
				"\n";
		String expectedMask = "11 1111 00000 00 000";
		String expectedOpcode = "00 0111 00000 00 000";
		String exampleOpcodes[] = { "00 0111 00000 00 000",
				"00 0111 00000 00 001", "00 0111 11111 11 111" };

		test(opcode, expectedString, expectedMask, expectedOpcode,
				exampleOpcodes);
	}

	private final void test(String opcode, String expectedString,
			String expectedMask, String expectedOpcode,
			String... exampleOpcodes) {
		Opcode op = new Opcode(opcode);
		assertEquals("Testing toString", expectedString, op.toString());
		assertEquals("Testing getMask", parseOpcode(expectedMask), op.getMask());
		assertEquals("Testing getOpcode", parseOpcode(expectedOpcode), op
				.getOpcode());

		for (String exampleOpcode : exampleOpcodes) {
			String text = "Testing example opcode " + exampleOpcode;
			assertTrue(text, op.matches(parseOpcode(exampleOpcode)));
		}
	}

	private int parseOpcode(String opcode) {
		return Integer.parseInt(opcode.replace(" ", ""), 2);
	}

	private String toOpcode(int opcode, int bits) {
		String binary = Integer.toBinaryString(opcode);
		return binary.length() >= bits ? binary : String.format("%0"
				+ (bits - binary.length()) + "d%s", 0, binary);
	}
}
