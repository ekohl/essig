package nl.utwente.cs.essig;

import java.util.*;

public class Func {

	public static String convertReg(String value) {

		String var_name = "";
		String number = "";

		for (char tempChar : value.toCharArray()) {
			// Check if byte is a number ASCII value
			if (Character.isDigit(tempChar)) {
				number += tempChar;
			} else {
				var_name += tempChar;
			}
		}

		return !number.equals("") ? "GetBit(" + var_name + "," + number + ")"
				: var_name;
	}
}

	public static String parseOpcode(String opcode) {
		Map<Character, String> arguments = new HashMap<Character, String>();
		opcode = opcode.replace('"', ' ').replace(" ", "");
		char[] opcodeArray = opcode.toCharArray();

		for (int i = 0; i < opcodeArray.length; i++) {
			Character addr = opcodeArray[i];
			// System.out.println(i + ": "+var);
			if (!Character.isDigit(addr)) {
				if (!arguments.containsKey(addr)) {
					arguments.put(addr, "op[" + i + "]");
				} else {
					arguments.put(addr, arguments.get(addr) + " & op[" + i
							+ "]");
				}
			}
		}

		StringBuilder builder = new StringBuilder();
		for (Map.Entry<Character, String> entry : arguments.entrySet()) {
			builder.append(entry.getValue()).append(";\n");
		}

		return builder.toString();
	}


	public static void main(String[] args) {
		// TODO code application logic here
		// System.out.println("Rd7 word dan:" + convertReg("Rd7"));

		String input = "00 0111 rdddd dr rrr";
		// String actual = "";
		System.out.println(parseOpcode(input));
	}
}
