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
	private String opcode = "";
	private String mask = "";
	private String opcode_withoutsymbols = "";

	/**
	 * Constructor that parses an opcode.
	 * 
	 * @param opcode
	 *            The opcode
	 */
	public Opcode(String opcode) {
		this.arguments = new HashMap<Character, List<Integer>>();
		opcode = opcode.replace('"', ' ').replace(" ", "");
		
		this.opcode = opcode;

		char[] opcodeArray = opcode.toCharArray();
		int opcodeSize = opcodeArray.length - 1;

		// Build the map from address to list of indices
		for (int i = 0; i < opcodeArray.length; i++) {
			Character addr = opcodeArray[i];
			if (!Character.isDigit(addr)) {
				if (!arguments.containsKey(addr)) {
					arguments.put(addr, new ArrayList<Integer>());
				}
				arguments.get(addr).add(opcodeSize - i);
				this.mask += "0";
				this.opcode_withoutsymbols += "0";
			} else {
				this.mask += "1";
				this.opcode_withoutsymbols += "" + addr;
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
			builder.append("int ").append(addr).append(" = 0; ");
			for (Integer index : entry.getValue()) {
				builder.append("AddBit(&").append(addr).append(",opcode,")
						.append(index).append("); ");
			}
			builder.append("\n");
		}

		return builder.toString();
	}

	public String getMask() {
		return this.mask;
	}

	public String getOpcode() {
		return this.opcode_withoutsymbols;
	}	
}
