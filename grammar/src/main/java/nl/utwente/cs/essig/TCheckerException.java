package nl.utwente.cs.essig;

import org.antlr.runtime.tree.Tree;
import org.antlr.runtime.*;

/**
 * Klasse voor het representeren van een foutmelding bij de ESSIG checker.
 * Gebruikt/ontwikkeld voor eindopdracht VB.
 * @author Danny Bergsma
 * @version 0.1
 */
public class TCheckerException extends RecognitionException {
	/* extra info over fout */
	private String msg;
	
	/**
	 * Maakt een nieuwe TCheckerException met als extra info msg.
	 * @param msg extra info over fout
	 * @require msg != null
	 * @ensure getMessage() == msg
	 */
	public TCheckerException(String msg) {
		this.msg = msg;
	}
	
	/**
	 * Maakt een nieuwe TCheckerException met als extra info msg.
	 * Destilleert uit tree extra info over de plaats (in de source) waar fout is opgetreden.
	 * @param tree de knoop waaruit de extra info over plaats van fout wordt gedestilleerd
	 * @param msg extra info over fout
	 * @require tree != null && msg != null
	 * @ensure getMessage().contains(msg)
	 */
	public TCheckerException(Tree tree, String msg) {
		this.msg = (msg + " ['" + tree.getText() + "' (ln: " + tree.getLine() + ", col: " + tree.getCharPositionInLine() + ")]");
	}
	
	/**
	 * Geeft extra info terug over de fout, mogelijk inclusief info over plaats (in source)
	 * waar fout heeft plaatsgevonden.
	 * @return extra info over fout
	 * @ensure result != null
	 */
	public String getMessage() {
		return msg;
	}
}