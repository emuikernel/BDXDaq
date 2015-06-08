h30631
s 00000/00000/00000
d R 1.2 99/11/23 11:12:58 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 jar/dplite/DpliteException.java
e
s 00037/00000/00000
d D 1.1 99/11/23 11:12:57 wolin 1 0
c date and time created 99/11/23 11:12:57 by wolin
e
u
U
f e 0
t
T
I 1
// Source file: C:/home/classes/org/jlab/coda/codaIIi/dplite/DpliteException.java

//package org.jlab.coda.codaIIi.dplite;

import java.io.IOException;


/**
 * Signals that an error occurred while attempting to use a socket.
 * @version 1.1.0
 * @version %I%, %G%
 * @author Darryl Collins
 * @author Graham Heyes
 */
public class DpliteException extends IOException {
    
    /**
	 * Constructs a new DpliteException with the specified detail 
	 * message.
	 * A detail message is a String that gives a specific 
	 * description of this error.
	 * @param msg the detail message
	 * @roseuid 36D567EF011C
	 */
	public DpliteException(String msg) {
	super(msg);
    }
    
    /**
     * Constructs a new DpliteException with no detail message.
     * A detail message is a String that gives a specific 
     * description of this error.
     * @roseuid 36D567EF0131
     */
    public DpliteException() {
    }
}
E 1
