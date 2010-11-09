package nl.utwente.cs.essig;

import org.antlr.runtime.tree.*;

/**
 * Klasse voor het representeren van de entry die wordt toegevoegd aan de symbol
 * table wanneer een identifier wordt gedeclareerd. De symbol table geeft deze entry
 * terug als je hem later opvraagt.
 * De entry bevat simpelweg de AST node die de declaratie representeert.
 * Wordt gebruikt in de contextual analyser.
 * Gebruikt/ontwikkeld bij VB.
 * @author Danny Bergsma
 * @version 0.1
 */
public class CommonTreeEntry extends IdEntry {
	/* De AST node behorend bij de declaratie.
	 * @invariant node != null
	 */
	private CommonTree node;
	
	/**
	 * Construeert een nieuwe CommonTreeEntry, waarbij de opgegeven node als bijbehorende
	 * AST node wordt gebruikt. Normaal gezien zal de node parameter de declaratie AST node
	 * bevatten.
	 * @param node de vast te leggen AST node
	 * @require node != null
	 * @ensure getNode() == node
	 */
	public CommonTreeEntry(CommonTree node) {
		this.node = node;
	}
	
	/**
	 * Geeft de node terug die bij deze CommonTreeEntry hoort. Normaal gezien zal dit de declaratie
	 * AST node zijn.
	 * @return AST node behorend bij deze entry
	 * @ensure result != null
	 */
	public CommonTree getNode() {
		return node;
	}
}