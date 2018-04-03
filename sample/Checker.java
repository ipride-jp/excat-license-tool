/*
 * @(#)Checker.java
 * $Id$
 */

import jp.co.ipride.License;


public class Checker {
    public static void main(String[] args) {
	if (args.length < 2) {
	    System.out.println("usage: Checker [license] [version]\n");
	    System.exit(-1);
	}

	if (License.isValidFile(args[0], args[1])) {
	    System.out.println("valid!");
	    System.exit(0);
	} else {
	    System.out.println("invalid!");
	    System.exit(-1);
	}
    }
}

/* the end of file */
