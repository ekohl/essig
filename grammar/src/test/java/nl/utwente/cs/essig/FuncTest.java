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

		// Simple (numberless) register
		register = "Rd";
		expectedResult = "Rd";
		assertEquals(expectedResult, Func.convertReg(register));

		// Register Rd7
		register = "Rd7";
		expectedResult = "GetBit(Rd,7)";
		assertEquals(expectedResult, Func.convertReg(register));
	}
}
