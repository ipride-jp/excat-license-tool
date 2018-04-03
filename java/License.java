/*
 * @(#)License.java
 * $Id$
 */

package jp.co.ipride;


public class License {
    static {
	try {
	    System.loadLibrary("License");
	} catch (Throwable t) {
	    throw new ExceptionInInitializerError(t);
	}
    }

    public native static boolean isValidFile(String file, String vers);

    public static void main(String[] args) {
	if (args.length < 2) {
	    System.out.println("usage: License license_file application_version\n");
	    System.out.println("\tlicense_file: name of PEM encoded license file\n");
	    System.out.println("\tapplication_version: name and application version in which the license file contains.\n");
	    System.exit(-1);
	}

	String f = args[0];
	String v = args[1];
	boolean r = isValidFile(f, v);
	System.out.print(f);
	System.out.print(" is");
	System.out.print(r ? "" : " NOT");
	System.out.print(" valid license for ");
	System.out.print(v);
	System.out.println(".");

	System.exit(r ? 0 : -1);
    }
}

/* the end of file */
