package nl.utwente.cs.essig;

import java.util.*;

/**
 * Klasse die een symbol table (identification table) implementeert.
 * Gebruikt/ontwikkeld bij VB.
 * @author Danny Bergsma
 * @version 0.1
 */
public class SymbolTable<Entry extends IdEntry> {
	/* Error berichten */
	private static final String WRONG_SCOPE_LEVEL = "Wrong scope level",
	                            ALREADY_DECLARED = "Already declared on current level";
	
	/* huidige (scope)level */
	private int currentLevel;
	/* een mapping van Strings (identifiers) naar een Stack van Entry's (de diverse declaraties van die identifier) */
	private Map<String, Stack<Entry>> mapDecl;
	
    /** 
     * Maakt een nieuwe symbol table aan.
     * @ensure currentLevel() == -1 
     */
    public SymbolTable() { 
    	currentLevel = -1;
    	mapDecl = new HashMap<String, Stack<Entry>>();
    }

    /** 
     * Opent een nieuwe scope. 
     * @ensure this.currentLevel() == old.currentLevel()+1 
     */
    public void openScope()  {
    	this.currentLevel++;
    }

    /** 
     * Sluit de huidige scope. Alle declaraties uit de huidige scope worden verwijderd.
     * @require old.currentLevel() > -1
     * @ensure this.currentLevel() == old.currentLevel()-1 
     */
    public void closeScope() {
    	Set<String> ids = mapDecl.keySet();
        for (String id: ids) { // ga alle ids na (optimalisatie mogelijk: sla per scope alle ids op)
        	Stack<Entry> stack = mapDecl.get(id); // stack van id      	
        	if (stack.size() > 0 && stack.peek().getLevel() == currentLevel) // id is gedeclareerd in huidige scope
        		stack.pop(); // pop bovenste declaratie van stack van id
        }
        currentLevel--;
    }

    /** 
	 * Geeft het huidige scope level terug.
	 * @return huidige scope level
     */
    public int currentLevel() {
    	return this.currentLevel;        
    }    

    /** 
	 * Voegt een id, samen met z'n Entry toe aan deze symbol table, gebruikmaken van huidige scope level.
	 * Het level van Entry wordt op currentLevel() gezet.
	 * @param id toe te voegen id
	 * @param entry toe te voegen entry; leven van deze entry wordt op currentLevel() gezet
     * @require String != null && String != "" && entry != null
     * @ensure  this.retrieve(id).getLevel() == currentLevel()
     * @throws  SymbolTableException wanneer huidige scope level niet geldig is of wanneer id al gedeclareerd
	 *                               is op huidige level 
     */
    public void enter(String id, Entry entry) throws SymbolTableException { 
        if (this.currentLevel >= 0) {
        	Stack<Entry> stack = mapDecl.get(id); // vraag stack op van id (mogelijk null)
        	if (stack == null) { // id nog niet eerder gedeclareerd 
        		stack = new Stack<Entry>();
        	    mapDecl.put(id, stack);
        	} else
        		if (stack.size() > 0 && stack.peek().getLevel() == currentLevel) // check of al niet gedeclareerd op dit level
        			throw new SymbolTableException(ALREADY_DECLARED);
        	entry.setLevel(currentLevel);
        	stack.push(entry);
        } else
        	throw new SymbolTableException(WRONG_SCOPE_LEVEL);
    }

    /** 
	 * Geeft de Entry met hoogste level terug behorend bij id. Oftewel, geeft Entry terug die als laatst
	 * voor id is toegevoegd aan de symbol table.
	 * Kan ook null zijn, wanneer id niet gedeclareerd is op het huidige of lagere niveaus.
	 * @param id de entry met hoogste level behorend bij deze id moet teruggeven worden
	 * @return entry met hoogste level behorend bij id, kan ook null zijn
     * @require id != null
     */
    public Entry retrieve(String id) {
    	Stack<Entry> stack = mapDecl.get(id);
		return ((stack != null && stack.size() > 0) ? stack.peek() : null);
    }    
}