package nl.utwente.cs.essig;

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;
import org.antlr.stringtemplate.*;

import java.io.*;

public class Main {

	private static boolean makeDot = false;

	static TLexer lexer;

	/** The file suffix which we accept */
	public static final String SUFFIX = ".dmo";

	public static final String TEMPLATE = "templates/c.stg";

	/**
	 * Just a simple test driver for the ASP parser to show how to call it.
	 */

	public static void main(String[] args) {
		try {
			// Create the lexer, which we can keep reusing if we like
			lexer = new TLexer();

			if (args.length > 0) {
				int s = 0;

				if (args[0].startsWith("-dot")) {
					makeDot = true;
					s = 1;
				}
				// Recursively parse each directory, and each file on the
				// command line
				for (int i = s; i < args.length; i++) {
					parse(new File(args[i]));
				}
			} else {
				System.err
						.println("Usage: java -jar essig-0.1-jar-with-dependencies.jar [-dot] <directory | filename"
								+ SUFFIX + ">");
			}
		} catch (Exception ex) {
			System.err.println("ANTLR demo parser threw exception:");
			ex.printStackTrace();
		}
	}

	public static void parse(File source) {

		// Open the supplied file or directory
		//
		try {

			// From here, any exceptions are just thrown back up the chain
			//
			if (source.isDirectory()) {
				System.out.println("Directory: " + source.getAbsolutePath());

				for (String file : source.list()) {
					parse(new File(source, file));
				}
			}

			// Else find out if it ends with the correct suffix and parse it
			//
			else if (source.getName().endsWith(SUFFIX)) {
				parseSource(source.getAbsolutePath());
			}
		} catch (Exception ex) {
			System.err.println("ANTLR demo parser caught error on file open:");
			ex.printStackTrace();
		}
	}

	public static void parseSource(String source) {
		String sourceName = source.substring(0, source.length()
				- SUFFIX.length());

		// Parse an ANTLR demo file
		//
		try {
			// First create a file stream using the provided file/path
			// and tell the lexer that that is the character source.
			// You can also use text that you have already read of course
			// by using the string stream.
			lexer.setCharStream(new ANTLRFileStream(source, "UTF8"));

			// Using the lexer as the token source, we create a token
			// stream to be consumed by the parser
			CommonTokenStream tokens = new CommonTokenStream(lexer);

			System.out.println("file: " + source);

			// Lex the tokens. Note that this is only to provide feedback
			lex(tokens);

			// Pick up the generic tree
			Tree t = parseTokens(tokens);

			// Run checker
			//checkTree(t);
			System.out.println("Checker currently disabled");
			// Now walk it with the tree walker, which generates the C file
			try {
				StringTemplate output = generateCode(t);
				FileWriter outputStream = new FileWriter(sourceName + ".c");
				outputStream.write(output.toString());
				outputStream.close();
			} catch (Exception w) {
				System.out.println("AST walk caused exception.");
				w.printStackTrace();
			}

			// Optionally make a dot file
			if (makeDot && tokens.size() < 4096) {
				dot(sourceName, t);
			}
		} catch (FileNotFoundException ex) {
			// The file we tried to parse does not exist
			System.err.println("\n  !!The file " + source
					+ " does not exist!!\n");
		} catch (TCheckerError ex) {
			// Something went wrong in the parser, report this
			System.err.println("  !!" + ex.getMessage() + "!!");
		} catch (Exception ex) {
			// Something went wrong in the parser, report this
			System.err.println("Exception caught:\n\n");
			ex.printStackTrace();
		}
	}

	/**
	 * Lex the token stream. This is not needed, but provides some timing.
	 *
	 * @param tokens
	 *            The tokens to lex
	 */
	private static void lex(CommonTokenStream tokens) {
		System.out.println("    Lexer Start");
		long start = System.currentTimeMillis();
		tokens.LT(1);
		long lexerStop = System.currentTimeMillis();
		System.out.println("      lexed in " + (lexerStop - start) + "ms.");
	}

	/**
	 * Parse the tokens into a tree.
	 *
	 * @param tokens
	 *            The tokens to parse
	 * @return The tree generated from the tokens
	 * @throws RecognitionException
	 *             In case the parser couldn't construct a tree
	 */
	private static Tree parseTokens(CommonTokenStream tokens)
			throws RecognitionException {
		TParser parser = new TParser(tokens);
		System.out.println("    Parser Start");
		long pStart = System.currentTimeMillis();
		TParser.microcontroller_return psrReturn = parser.microcontroller();
		long stop = System.currentTimeMillis();
		System.out.println("      Parsed in " + (stop - pStart) + "ms.");

		if (parser.getNumberOfSyntaxErrors() != 0) {
			throw new RecognitionException();
		}

		// Pick up the generic tree
		return (Tree) psrReturn.getTree();
	}

	/**
	 * Checks the tree for inconsistencies and throws an exception in case
	 * something is wrong.
	 *
	 * @param tree
	 *            The tree to check
	 * @throws RecognitionException
	 *             In case the tree has an inconsistency
	 */
	private static void checkTree(Tree tree) throws RecognitionException, TCheckerError {
		TChecker checker = new TChecker(new CommonTreeNodeStream(tree));
		System.out.println("    Checker Start\n");
		long pStart = System.currentTimeMillis();
		checker.microcontroller();
		long stop = System.currentTimeMillis();
		System.out.println("      Checking finished in " + (stop - pStart)
				+ "ms.");

		if (checker.getNumberOfSyntaxErrors() != 0) {
			throw new TCheckerError("Checker finished with " + checker.getNumberOfSyntaxErrors() + " errors");
		}
	}

	/**
	 * Generates a {@link StringTemplate} from the given <code>tree</code>
	 *
	 * @param tree
	 *            The tree to generate code from
	 * @return A {@link StringTemplate} containing the generated code
	 * @throws IOException
	 *             If template could not be opened
	 * @throws RecognitionException
	 *             If the tree could not be walked
	 */
	private static StringTemplate generateCode(Tree tree) throws IOException,
			RecognitionException {
		TTree walker = new TTree(new CommonTreeNodeStream(tree));

		// Load Stringtemplate
		InputStream template = getInputStreamForFilename(TEMPLATE);
		Reader groupFileR = new InputStreamReader(template);
		StringTemplateGroup templates = new StringTemplateGroup(groupFileR);
		template.close();
		groupFileR.close();

		walker.setTemplateLib(templates);

		System.out.println("    AST Walk Start\n");
		long pStart = System.currentTimeMillis();
		TTree.microcontroller_return mr = walker.microcontroller();
		long stop = System.currentTimeMillis();
		System.out.println("      AST Walked in " + (stop - pStart) + "ms.");

		return (StringTemplate) mr.getTemplate();
	}

	/**
	 * Generate a tree from dot and save it.
	 *
	 * @param sourceName
	 *            The source filename without any suffix
	 * @param tree
	 *            The tree to generate from
	 * @throws IOException
	 *             If an I/O error occurs
	 * @throws InterruptedException
	 *             If the thread is interrupted while waiting.
	 */
	private static void dot(String sourceName, Tree tree) throws IOException,
			InterruptedException {
		// Use the ANLTR built in dot generator
		DOTTreeGenerator gen = new DOTTreeGenerator();

		String outputName = sourceName + ".dot";

		System.out.println("    Producing AST dot (graphviz) file");

		// Create a string template and write it to a file
		StringTemplate st = gen.toDOT(tree, new CommonTreeAdaptor());
		FileWriter outputStream = new FileWriter(outputName);
		outputStream.write(st.toString());
		outputStream.close();

		// Invoke dot to generate a .png file
		System.out.println("    Producing png graphic for tree");
		long pStart = System.currentTimeMillis();
		Process proc = Runtime.getRuntime().exec(
				"dot -Tpng -o" + sourceName + ".png " + outputName);
		proc.waitFor();
		long stop = System.currentTimeMillis();
		System.out.println("      PNG graphic produced in " + (stop - pStart)
				+ "ms.");
	}

	/**
	 * This method returns an InputStream for the given filename.
	 *
	 * @param name the filename
	 * @return the InputStream
	 */
	public static InputStream getInputStreamForFilename( String name )
	{
		InputStream result = Main.class.getResourceAsStream( "/" + name );
		if( result == null )
		{
			try
			{
				result = new FileInputStream( name );
			}
			catch( FileNotFoundException e ) {}
		}
		return result;
	}
}
