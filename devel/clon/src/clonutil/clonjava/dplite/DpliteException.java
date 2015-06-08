// Source file: C:/home/classes/org/jlab/coda/codaIIi/dplite/DpliteException.java

//package org.jlab.coda.codaIIi.dplite;

import java.io.IOException;


/**
 * Signals that an error occurred while attempting to use a socket.
 * @version 1.1.0
 * @version 1.1, 11/23/99
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
