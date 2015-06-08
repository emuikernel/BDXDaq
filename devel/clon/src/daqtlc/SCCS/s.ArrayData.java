h07833
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 daqtlc/ArrayData.java
e
s 00101/00000/00000
d D 1.1 00/07/31 14:48:41 gurjyan 1 0
c date and time created 00/07/31 14:48:41 by gurjyan
e
u
U
f e 0
t
T
I 1
  
import java.lang.*; 
import java.util.*; 
import jclass.chart.*; 
import java.util.Vector; 
 
public class ArrayData implements Chartable { 
 
// The data - first row is x, all the rest are y 
public double rawData[][] = { 
   
{15718.0,  15723.0,  15724.0,  15722.0,  15730.0,  15735.0,  15736.0,  15741.0,  15741.0,  15747.0,  15747.0,  15747.0,  15747.0}, 
{3713.0,  3713.0,  3713.0, 3713.0,  3713.0,  3713.0,  3713.0,  3713.0,  3713.0,  3713.0,  3713.0,  3713.0,  3713.0}}; 
 
// Holds the data 
Vector data = new Vector(); 
 
// Default constructor.  Puts default rawData into data vector. 
public ArrayData() { 
    //makeData(rawData); 
} 
 
// Typical constructor.  Puts supplied data into the data vector. 
public ArrayData(double data[][],int numRows, String[] columns) { 
  //    System.out.println("Start Graph...."); 
    makeData(data,numRows,columns); 
} 
 
// Create the data vector given an 2D array of doubles 
private void makeData(double array[][],int numRows,String columns[]) { 
    for (int i = 0; i < columns.length; i++) { 
        Vector row = new Vector(); 
	//        System.out.println(array[i].length); 
        for (int j = 0; j < numRows; j++) { 
             
            row.addElement(new Double(array[i][j])); 
        } 
        data.addElement(row); 
    } 
} 
 
// Overridden from Chartable 
public Object getDataItem(int row, int column) 
 { 
        Object rval = null; 
        try { 
                rval = ((Vector)data.elementAt(row)).elementAt(column); 
        } 
        catch (Exception e) { 
        } 
        return rval; 
} 
// Overridden from Chartable 
public Vector getRow(int row) 
 { 
        Vector rval = null; 
        try { 
                rval = (Vector)data.elementAt(row); 
        } 
        catch (Exception e) { 
        } 
        return rval; 
} 
 
// Overridden from Chartable 
public int getDataInterpretation() 
 { 
        return ARRAY; 
} 
 
// Overridden from Chartable 
public int getNumRows() 
 { 
        if (data == null) return 0; 
        return data.size(); 
} 
 
// Overridden from Chartable 
public String[] getPointLabels() { 
        return null; 
} 
 
// Overridden from Chartable 
public String getSeriesName(int row) { 
        return null; 
} 
 
// Overridden from Chartable 
public String getSeriesLabel(int row) { 
        return getSeriesName(row); 
} 
 
// Overridden from Chartable 
public String getName() { 
        return null;

	//new String(""); 
} 

 
}
E 1
