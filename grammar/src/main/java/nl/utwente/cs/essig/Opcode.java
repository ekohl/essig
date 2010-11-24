package nl.utwente.cs.essig;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * An opcode representation.
 */
public class Opcode {
	/** Map from argument name to a list of indices in the opcode */
	private final Map<Character, List<Integer>> arguments;

	/** The bit-mask */
	private int mask;

	/** The bit-mask */
	private int opcode;

	/**
	 * Constructor that parses an opcode.
	 *
	 * @param opcode
	 *            The opcode
	 */
	public Opcode(String opcode) {
		this.arguments = new HashMap<Character, List<Integer>>();
		this.mask = 0;

		opcode = opcode.replace('"', ' ').replace(" ", "");
		char[] opcodeArray = opcode.toCharArray();
		int opcodeSize = opcodeArray.length - 1;

		// Build the map from address to list of indices
		for (int i = 0; i < opcodeArray.length; i++) {
			Character addr = opcodeArray[i];
			if (!Character.isDigit(addr)) {
				if (!this.arguments.containsKey(addr)) {
					this.arguments.put(addr, new ArrayList<Integer>());
				}
				this.arguments.get(addr).add(opcodeSize - i);
			} else {
				this.mask |= 1 << (opcodeSize - i);
				this.opcode |= Character.digit(addr, 2) << (opcodeSize - i);
			}
		}
	}

	/**
	 * Returns the opcode in a map structure. Each key is a variable name and
	 * the value is a list of indices where you can find values for that
	 * variable in the opcode.
	 *
	 * @return the opcode in a map structure
	 */
	public Map<Character, List<Integer>> getArguments() {
		return Collections.unmodifiableMap(this.arguments);
	}

	/**
	 * Produces a string representation of the opcode which should be valid C
	 * code. The output is a line for each address that is in the original
	 * opcode. Each line is composed of an initial assignment and for each
	 * position in the address an AddBit function.
	 *
	 * <pre>
	 * int ADDR = 0; AddBit(&ADDR,opcode,INDEX); \n
	 * </pre>
	 */
	@Override
	public String toString() {
		// Build the map into a string
		StringBuilder builder = new StringBuilder();
		for (Map.Entry<Character, List<Integer>> entry : arguments.entrySet()) {
			Character addr = entry.getKey();

			// Build a string that looks like:
			// int ADDR = 0; AddBit(&ADDR,opcode,INDEX); \n
			builder.append("long long ").append(addr).append(" = 0;\nint ").append(addr)
				.append("_bits = ").append(entry.getValue().size()).append(";\n");
			for (Integer index : entry.getValue()) {
				builder.append("AddBit(&").append(addr).append(",opcode,")
						.append(index).append(");\n");
			}
			builder.append("\n");
		}

		return builder.toString();
	}

	/**
	 * Returns the bit-mask that should result into the opcode. A matching
	 * opcode is found if for a given opcode the following is true:
	 *
	 * <pre>
	 * opcode &amp; o.getMask() == o.getMask()
	 * </pre>
	 *
	 * @return The bit-mask for this opcode
	 */
	public int getMask() {
		return this.mask;
	}

	/**
	 * Returns the mask as a binary string
	 *
	 * @return the mask as a binary string
	 * @see #getMask()
	 */
	public String getMaskString() {
		return Integer.toBinaryString(getMask());
	}

	/**
	 * Returns the opcode. A matching opcode is found if for a given opcode the
	 * following is true:
	 *
	 * <pre>
	 * opcode &amp; o.getMask() == o.getMask()
	 * </pre>
	 *
	 * @return The opcode
	 */
	public int getOpcode() {
		return this.opcode;
	}

	/**
	 * Returns the opcode as a binary string
	 *
	 * @return the opcode as a binary string
	 * @see #getOpcode()
	 */
	public String getOpcodeString() {
		return Integer.toBinaryString(getOpcode());
	}

	/**
	 * Returns whether the given <code>opcode</code> matches this opcode.
	 *
	 * @param opcode
	 *            The opcode to match
	 * @return whether the given <code>opcode</code> matches this opcode.
	 */
	public boolean matches(int opcode) {
		return (opcode & this.getMask()) == this.getOpcode();
	}
}
