package nl.utwente.cs.essig;

import org.antlr.runtime.IntStream;
import org.antlr.runtime.RecognitionException;

/**
 * A non-fatal exception that occurred in the checker.
 * 
 * @author Ewoud Kohl van Wijngaarden
 * @see TCheckerError
 */
public class TCheckerException extends RecognitionException {
	/** Generated UID */
	private static final long serialVersionUID = 8893205774955639424L;

	/** A message for the user */
	private final String message;

	public TCheckerException(IntStream input, String message) {
		super(input);
		this.message = message;
	}

	@Override
	public String getMessage() {
		return this.message;
	}
}