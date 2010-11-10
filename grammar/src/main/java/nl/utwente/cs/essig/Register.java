package nl.utwente.cs.essig;

/**
 * A register representation.
 */
public class Register {
	/** The number. Can be null */
	private Integer number;

	/** The register name */
	private String name;

	private VariableType type;

	public enum VariableType {
		REGISTER, RAM;
	}

	/**
	 * Convert a register into a string that is usable. It splits the
	 * numbers from the name. For example, <code>R5</code> returns
	 * <code>R</code> as name and <code>5</code> as number while
	 * <code>Rd</code> simply returns <code>Rd</code> as name and
	 * <code>null</code> as number.
	 *
	 * @param name
	 *            The register
	 */
	public Register(String name) {
		StringBuilder varName = new StringBuilder();
		StringBuilder number = new StringBuilder();

		for (char tempChar : name.toCharArray()) {
			// Check if byte is a number ASCII value
			if (Character.isDigit(tempChar)) {
				number.append(tempChar);
			} else {
				varName.append(tempChar);
			}
		}

		try {
			this.number = Integer.valueOf(number.toString());
		} catch(NumberFormatException nfe) {
			this.number = null;
		}

		this.name = varName.toString();

		switch(this.name.charAt(0)) {
			case 'A':
				this.type = VariableType.RAM;
				break;
			case 'R':
				this.type = VariableType.REGISTER;

				try {
					if(this.name.length() > 1 && Character.isLowerCase(this.name.charAt(1))) {
						this.name = this.name.substring(1);
					}
				} catch(IndexOutOfBoundsException ioe) {
					System.out.println(name);
					System.exit(1);
				}
				break;
			default:
				this.type = VariableType.REGISTER;
				break;
		}
	}

	public VariableType getType() {
		return this.type;
	}

	public Integer getNumber() {
		return this.number;
	}

	public String getName() {
		return this.name;
	}

	@Override
	public String toString() {
		return number == null ? name : name + number;
	}
}
