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
		expectedResult = "int d = 0; AddBit(&d,opcode,9); AddBit(&d,opcode,8); AddBit(&d,opcode,7); AddBit(&d,opcode,6); AddBit(&d,opcode,5); \nint r = 0; AddBit(&r,opcode,10); AddBit(&r,opcode,4); AddBit(&r,opcode,3); AddBit(&r,opcode,2); AddBit(&r,opcode,1); \n";
		//System.out.println("\\"+Func.parseOpcode(opcode)+"\\");
                assertEquals(expectedResult, Func.parseOpcode(opcode));
	}
}
