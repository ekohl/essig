package nl.utwente.cs.essig;

/** Exception class to signal problems with the SymbolTable */
public class SymbolTableException extends Exception {
    public static final long serialVersionUID = 24362462L; // for Serializable
    public SymbolTableException(String msg) { super(msg); }
}