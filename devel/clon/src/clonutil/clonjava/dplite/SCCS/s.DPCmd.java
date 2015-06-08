h20195
s 00001/00001/00410
d D 1.2 99/11/23 11:28:55 wolin 3 1
c Removed unneeded printout
e
s 00000/00000/00000
d R 1.2 99/11/23 11:12:48 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 jar/dplite/DPCmd.java
e
s 00411/00000/00000
d D 1.1 99/11/23 11:12:47 wolin 1 0
c date and time created 99/11/23 11:12:47 by wolin
e
u
U
f e 0
t
T
I 1
// Source file: C:/home/classes/org/jlab/coda/codaIIi/dplite/DPCmd.java

//package org.jlab.coda.codaIIi.dplite;

import java.awt.*;
import java.lang.*;
import java.io.*;
import java.net.*;
import java.util.*;
import java.sql.*;

/**
* This package implements a very simple version of the dplite library which I
* wrote for use with the CODA 2.x run control simply ported to Java.
*
* It relies on JDBC for access to the coda 2.x configuration database. With
* this in mind one has to create a connection to the database. The dpCmd
* object can be created with either a URL to a Jdbc database or using the
* default database from the codaManager package.
*
* Errors are handled by the dpliteException.
* @author Graham Heyes
* @version %I%, %G%
*/
public final class DPCmd {
    /**
	* This is the current (last) socket that TCP communication took place on
	* Mostly used for debugging communication problems.
	*/
	private Socket s;
    /**
	* The current input data stream.
	*/
	private InputStream DPin;
	/**
	* Ten second timeout
	*/
	private int timeout = 10000;
	/**
	* number of times to execute the dpAsk before giving up...
	*/
	private int commandRetries = 2;
    /**
	* The current output data stream.
	*/
	private OutputStream DPout;
    /**
	* The JDBC URL pointing to the CODA 2.x configuration
	* database.
	*/
	private String url;
    /**
	* A local connection to JDBC.
	*/
	public Connection con;
    /**
	* A local statement got SQL queries.
	*/
	public Statement stmt;
    /**
	* The CODA 2.x reply from the command.
	*/
	private String strReply;
    /**
	* Current TCP port.
	*/
	private static Integer port;
    /**
	* Current targer host.
	*/
	private static String host;
    /**
	* Hashtable of known hosts.
	* @SBGen Collection of String
	*/
	private Hashtable hosts = new Hashtable();
    /**
	* Hash table of ports (hashed by name of CODA 2.x target).
	* @SBGen Collection of Integer
	*/
	private Hashtable ports = new Hashtable();
    /**
	* Hash table of open sockets (hashed by name of CODA 2.x target).
	* @SBGen Collection of Socket
	*/
	private Hashtable sockets = new Hashtable();
    
    /**
	* Generate a dplite connection using an existing JDBC database connection
	* and statement.
	* @param connection
	* @param statement
	*/
	public DPCmd(Connection connection,Statement statement) {
		con = connection;
		stmt = statement;
    }
	
	public void SetTimeout(int to)
	{
		if ((to > 0) && (to < 1000000)) {
			timeout = to;
		} else {
			System.out.println("DP timeout of " + to + " millisceconds is invalid 0 < to < 100000");
		}
	}
	
	
	public void SetRetries(int ret)
	{
		commandRetries = ret;
	}
	
	public int GetTimeout()
	{
		return timeout;
	}
	
	public int GetRetries()
	{
		return commandRetries;
	}
	
    /**
	* Create a dplite connection using the JDBC URL taken from the argument URL and the driver from Driver.
	* an example of URLs are
	*
	* jdbc:msql://mizar.cebaf.gov:8101/ghexp  with driver com.imaginary.sql.msql.MsqlDriver
	*
	* and
	*
	* jdbc:odbc:rcdb with driver sun.jdbc.odbc.JdbcOdbcDriver
	* @param theURL URL for the database, i.e. jdbc:msql://mizar.cebaf.gov:8101/ghexp
	* @param databaseDriver jdbc driver suitable for the URL above, i.e. com.imaginary.sql.msql.MsqlDriver
	* @exception DpliteException <<Invalid>> <<Invalid>>
	* @exception org.jlab.coda.codaIIi.dplite.DpliteException <<Invalid>> <<Invalid>>
	* @roseuid 36D5A088037B
	*/
	public DPCmd(String theURL, String databaseDriver)
		throws DpliteException {
		
		// I don't know what this is doing here but
		// it looks cool.
		url = theURL;
		try {
D 3
			System.out.println("Loading jdbc driver : " + databaseDriver);
E 3
I 3
		    //  System.out.println("Loading jdbc driver : " + databaseDriver);
E 3
			
			Class.forName(databaseDriver);
			
			// borg is a dummy username since msql isn't using usernames...
			// ... and resistance is futile...
			con = DriverManager.getConnection(url, "borg", "");
			stmt = con.createStatement();
		}
		catch( Exception e ) {
			throw new DpliteException(e.getMessage());
		}
		
		
    }
    
    /**
	* close the dplite connection
	* @exception DpliteException <<Invalid>> <<Invalid>>
	* @exception org.jlab.coda.codaIIi.dplite.DpliteException <<Invalid>> <<Invalid>>
	* @return
	* @roseuid 36D5A0880399
	*/
	public int dpClose()
		throws DpliteException {
		//  throw new DpliteException(e.getMessage());
		//}
		return 0;
		
    }
    
    /**
	* Send the command in the second argument to the object named in the first
	* argument and return the CODA2.x status as a String.
	* @param server
	* @param command The command sent to the server.
	* @exception DpliteException <<Invalid>>
	* @exception org.jlab.coda.codaIIi.dplite.DpliteException <<Invalid>> <<Invalid>>
	* @return
	* @roseuid 36D5A08803A3
	*/
	public String dpAsk(String server, String command)
		throws DpliteException {
		/**
		* We try the command a couple of times before giving up. This allows for the
		* case where we try an old host/port and the server has been down and come
		* back again since we last used them.
		*/
		int retries = commandRetries;
		
		while (retries > 0) {
			
			/* look for the hostname in the hash table.
			*/
			host = (String) hosts.get(server);
			
			if (host == null) {
				// host isn't in hash table so get host and port from database and add to
				// hash table
				try {
					ResultSet rs = stmt.executeQuery("SELECT host,port from process WHERE name='"
														 + server + "'" );
					rs.next();
					host = rs.getString("host");
					port = new Integer(rs.getInt("port"));
					
					hosts.put(server,host);
					ports.put(server,(Integer) port);
					
				}
				catch( Exception e ) {
					throw new DpliteException(e.getMessage());
				}
			}
			else {
				// if getting the host worked then the port must be in the DB too.
				port = (Integer) ports.get(server);
			}
			
			try {
				// Open connection
				
				// assume that the socket is in the hash table
				s = (Socket) sockets.get(server);
				if (s == null)  {
				    // if not create a new socket
					s = new Socket(host, port.intValue());
					
					// disable some annoying features of TCP
				    s.setSoLinger(false,0);
				    s.setTcpNoDelay(false);
				    
					// Get Streams
					DPin = s.getInputStream();
					DPout = s.getOutputStream();
					
					// Read Connection Info
					
					//  If the server hangs we will hang here so we need a timeout.
					s.setSoTimeout(timeout);
					
					// unfortunately DP is a pretty dumb protocol a connect returns the
					// string "Connection accepted" in plain text with a line feed at the
					// end. The only way that I could get this message off the socket was
					// to read char by char and look for the "\n"
					
					int i;            // index to a char buffer
					StringBuffer str = new StringBuffer(); // buffer being indexed
					char c;			  // one char at a time...
					
					try {
						c = 0;
						for(i=0; c!='\n'; i++) {
							c = (char)DPin.read();
							
							if (c > 0) {
								str.append(c);
							} else {
								// we get here if read returns a -1, i.e. s is closed at the other end
								throw new IOException("connect DPin socket unexpectedly closed");
							}
						}
					} catch (InterruptedIOException e) {
						// get here if there was a timeout
						System.out.println("DPask connect timeout");
						throw new IOException("connect timeout on DPin socket");
					} catch (Exception e) {
						// get here if anything else went wrong...
						System.out.println("Another exception " + e.getMessage());
						throw new IOException("connect exception on DPin socket");
					}
					
					sockets.put(server,s);
					
				}
				else {
					
					// Convert s into I/O Streams
					
					DPin = s.getInputStream();
					DPout = s.getOutputStream();
					
				}
				
				// Send string formed by "el" followed by our command.
				// the "e" tells the server that this is a command
				String cmd = "el";
				
				sendDP(cmd.concat(command));
				
				// Read back the reply
				strReply = recvDP();
				// Anything starting with x is an exception
				if (strReply.startsWith("x")) {
					throw new DpliteException(
						strReply.substring(2)
					);
				}
				// Anything else is the result of the dpAsk
				return strReply.substring(2);
				
			} catch(Exception e) {
				// Prepare to go around the retry loop again
				retries --;
				System.out.println("Got an Exception " + retries + " retries left");
				try {
					// close the current socket
					s.close();
				} catch(Exception e1) {
					// ignore the case where s is already closed...
				}
				// Force the next retry to read the host and the port
				// from the database rather than the hash table...
				hosts.remove(server);
				ports.remove(server);
				sockets.remove(server);
				
				// If we run out of retries tell the world.
				if (retries == 0)
					throw new DpliteException(e.getMessage());
				
			}
			System.out.println("Retry #" + retries);
		}
		return("never see this message, keeps compiler happy!!");
		
    }
    
    /**
	* @param str
	* @exception IOException <<Invalid>>
	* @exception java.io.IOException <<Invalid>> <<Invalid>>
	* @roseuid 36D5A0890015
	*/
	private void sendDP(String str)
		throws IOException {
		// This code gets around byte ordering problems since
		// the DP protocol expects a 4 byte length followed by the data.
		// (The data consists of a two byte code followed by a clear text string
		// and is not affected by byte ordering).
		int i,j,k;
		
		j = k = str.length();
		
		byte buf[] = new byte[j+4];
		
		buf[0] = (byte) ((k >>> 24) & 0xFF);
		buf[1] = (byte) ((k >>> 16) & 0xFF);
		buf[2] = (byte) ((k >>>  8) & 0xFF);
		buf[3] = (byte) ((k >>>  0) & 0xFF);
		
		for(i=0; i<j; i++) {
			buf[4+i] = (byte)str.charAt(i);
		}
		
		s.setSoTimeout(timeout);
		
		try {
			DPout.write(buf);
		} catch (InterruptedIOException e) {
			System.out.println("DPask write timeout");
			throw new IOException("write timeout on DPin socket");
		}
	}
	
	/**
	* @exception IOException <<Invalid>>
	* @exception java.io.IOException <<Invalid>>
	* @return
	* @roseuid 36D5A0890065
	*/
	private String recvDP() throws IOException {
		
		StringBuffer str = new StringBuffer();
		
		int i,j,k=0;
		int ch1 = -1;
		int ch2 = -1;
		int ch3 = -1;
		int ch4 = -1;
		
		s.setSoTimeout(timeout);
		// this looks odd but it gets around byte ordering problems see previous method
		try {
			ch1 = DPin.read();
			ch2 = DPin.read();
			ch3 = DPin.read();
			ch4 = DPin.read();
			// assemble the message length
			j = (ch1 << 24) + (ch2 << 16) + (ch3 << 8) + (ch4 << 0);
			
			if ((ch1 == -1) || (ch2 == -1) || (ch3 == -1) || (ch4 == -1))  {
				throw new IOException("read on closed socket");
			}
			// get the message
			for(i=0; i<j; i++) {
				char c = (char)DPin.read();
				str.append(c);
			}
		} catch (InterruptedIOException e) {
			throw new IOException("read timeout on DPin socket");
		}
		return str.toString();
		
	}
}
E 1
