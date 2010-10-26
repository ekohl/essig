package nl.utwente.cs.essig;

/**
 * A utility class.
 */
public class Func {

	/**
	 * Convert a register into a string that is usable. It splits the numbers
	 * from the name and if there is a number, it returns a GetBit function. For
	 * example, <code>R5</code> returns <code>GetBit(R,5)</code> while
	 * <code>Rd</code> simply returns <code>Rd</code>.
	 *
	 * @param value
	 *            The register
	 * @return A usable string representation of the register
	 */
	public static String convertReg(String value) {
		StringBuilder varName = new StringBuilder();
		StringBuilder number = new StringBuilder();

		for (char tempChar : value.toCharArray()) {
			// Check if byte is a number ASCII value
			if (Character.isDigit(tempChar)) {
				number.append(tempChar);
			} else {
				varName.append(tempChar);
			}
		}

		return number.length() > 0 ? "GetBit(" + varName.toString() + ","
				+ number.toString() + ")" : varName.toString();
	}
}
