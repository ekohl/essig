package nl.utwente.cs.essig;

import java.util.*;

class Func { 

public static boolean isDigit(char digit) {
        return (digit <= 57 && digit >= 48);
}

public static String convertReg(String value) {

	String var_name = "";
	String number = "";
	String output = "";

	char tempChar;
	for (int i=0; i < value.length(); i++) {
		tempChar = value.charAt(i);
		
		//Check if byte is a number ASCII value
		if (isDigit(tempChar)) {
			number += tempChar;
		} else {
                    var_name+=tempChar;
                }
	}
	
	if (!number.equals("")) 
	{
		output = "GetBit("+ var_name + ","+number+")";
	} else { output = var_name; };

	return output;
}

public static String parseOpcode(String opcode) {
        HashMap<String,String> arguments = new HashMap<String,String>();
	opcode = opcode.replace('"',' ');        
	opcode = opcode.replace(" ","");
        char var = 0;
        String addr = "";
        for (int i = 0; i < opcode.length(); i++)
        {
            
            var = opcode.charAt(i);
            addr = var + "";
            //System.out.println(i + ": "+var);
            if (!isDigit(var)){
                if (!arguments.containsKey(addr)) {
                    arguments.put(addr, "op["+i+"]");
                } else {
                    arguments.put(addr, arguments.get(addr) + " & op["+i+"]");
                }
            }
        }

	ArrayList aList = new ArrayList(arguments.entrySet());
        String output = "";
        for (int i = 0; i < arguments.size(); i++)
        {
             output += aList.get(i).toString() + ";\n";
        }
        
        return output;
	
        
    }

public static void main(String[] args) {
        // TODO code application logic here
        //System.out.println("Rd7 word dan:" + convertReg("Rd7"));

        String input = "00 0111 rdddd dr rrr";
        //String actual = "";
        System.out.println(parseOpcode(input));
    }

}
