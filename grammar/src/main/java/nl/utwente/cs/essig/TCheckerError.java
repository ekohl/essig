package nl.utwente.cs.essig;

import org.antlr.runtime.IntStream;

/**
 * A fatal exception that occurred in the checker.
 * 
 * @author Ewoud Kohl van Wijngaarden
 * @see TCheckerException
 */
public class TCheckerError extends RuntimeException {
	/** Generated UID */
	private static final long serialVersionUID = 3909731270588233130L;

	public TCheckerError(String message) {
		super(message);
	}

	public TCheckerError(IntStream input, String message) {
		this(message);
		// FIXME: Do something with the input so you can provide useful error
		// messages
	}
}
