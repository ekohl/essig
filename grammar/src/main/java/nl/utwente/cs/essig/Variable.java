package nl.utwente.cs.essig;

/**
 * A variable representation.
 * 
 * @author Ewoud Kohl van Wijngaarden
 */
public class Variable {
	/** Possible variable types */
	public static enum VariableType {
		REGISTER, RAM;
	}

	/** The number. Can be null */
	private final Integer number;

	/** The register name */
	private String name;

	/** The type of variable */
	private final VariableType type;

	/**
	 * Convert a variable into a string that is usable. It splits the numbers
	 * from the name. For example, <code>R5</code> returns <code>R</code> as
	 * name and <code>5</code> as number while <code>Rd</code> simply returns
	 * <code>Rd</code> as name and <code>null</code> as number.
	 * 
	 * @param name
	 *            The register
	 */
	public Variable(String name) {
		StringBuilder varName = new StringBuilder();
		StringBuilder number = new StringBuilder();

		// Walk through the name and split variables and numbers
		for (char tempChar : name.toCharArray()) {
			// Check if byte is a number ASCII value
			if (Character.isDigit(tempChar)) {
				number.append(tempChar);
			} else {
				varName.append(tempChar);
			}
		}

		// Parse the number
		this.number = number.length() > 0 ? Integer.valueOf(number.toString())
				: null;

		// Determine the name and variable type
		this.name = varName.toString();
		switch (this.name.charAt(0)) {
		case 'A':
			this.type = VariableType.RAM;
			break;
		case 'R':
			this.type = VariableType.REGISTER;

			if (this.name.length() > 1
					&& Character.isLowerCase(this.name.charAt(1))) {
				this.name = this.name.substring(1);
			}
			break;
		default:
			this.type = VariableType.REGISTER;
			break;
		}
	}

	public Variable(String name, VariableType type) {
		this.name = name;
		this.type = type;
		this.number = null;	
		
	}

	/**
	 * Returns the variable name
	 * 
	 * @return the variable name
	 */
	public String getName() {
		return this.name;
	}

	/**
	 * Returns the (optional) number
	 * 
	 * @return the (optional) number
	 */
	public Integer getNumber() {
		return this.number;
	}

	/**
	 * Returns the type of variable
	 * 
	 * @return the type of variable
	 */
	public VariableType getType() {
		return this.type;
	}

	@Override
	public String toString() {
		return number == null ? name : name + number;
	}
}
