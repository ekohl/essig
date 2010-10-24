package nl.utwente.cs.essig;

import static org.junit.Assert.*;

import org.junit.Test;

public class FuncTest {
	@Test
	public final void testConvertReg() {
		String register, expectedResult;

		// Empty register
		register = "";
		expectedResult = "";
		assertEquals(expectedResult, Func.convertReg(register));

		// Register Rd7
		register = "Rd7";
		expectedResult = "GetBit(Rd,7)";
		assertEquals(expectedResult, Func.convertReg(register));
	}

	@Test
	public final void testParseOpcode() {
		String opcode, expectedResult;

		// Empty opcode
		opcode = "";
		expectedResult = "";
		assertEquals(expectedResult, Func.parseOpcode(opcode));

		// Pure binary opcode
		opcode = "0000 0000 0000 0000";
		expectedResult = "";
		assertEquals(expectedResult, Func.parseOpcode(opcode));

		// An opcode with multiple registers
		opcode = "00 0111 rdddd dr rrr";
		expectedResult = "op[7] & op[8] & op[9] & op[10] & op[11];\n"
				+ "op[6] & op[12] & op[13] & op[14] & op[15];\n";
		assertEquals(expectedResult, Func.parseOpcode(opcode));
	}
}
