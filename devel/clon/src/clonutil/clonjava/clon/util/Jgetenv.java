/*
* Author: Vardan Gyurjyan
* Jefferson Lab DAQ group
* Date: Apr 18, 2002
* Time: 12:36:41 PM
*/

package clonjava.clon.util;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Hashtable;

public class Jgetenv {

    private Process p;
    private BufferedReader br;
    private Hashtable m;
    private String l;
    private String re;
    private String rn;
    private String notFound = "Environmental variable is not defined";

    public Jgetenv() {
        try {
            updateInfo();
        } catch (JgetenvException a) { System.err.println(a);}
    }

    public void updateInfo () throws JgetenvException {
        try {
            p  = Runtime.getRuntime().exec("printenv");
            br = new BufferedReader( new InputStreamReader(p.getInputStream()));
            m = new Hashtable();
            while((l = br.readLine()) != null) {
                re = (l.substring(l.indexOf('=')+1));
                rn = l.substring(0,l.indexOf('='));
                m.put(rn,re);
            }
        } catch (java.io.IOException e) {
            throw ( new JgetenvException(e.toString()));
        } finally {
            try {
                br.close();
                p.destroy();
            } catch (java.io.IOException e) {
            }
        }
    }

    public String echo( String env) throws JgetenvException {
        if(m.containsKey(env)) return (String)m.get(env);
        else  {
            throw ( new JgetenvException(notFound));
        }
    }

    public static void main (String[] args) {
        Jgetenv myJgetenv = new Jgetenv();
        System.out.println(args[0]);
        try {
            System.out.println(myJgetenv.echo(args[0]));
        } catch (JgetenvException a) { System.err.println(a); }
    }
}
