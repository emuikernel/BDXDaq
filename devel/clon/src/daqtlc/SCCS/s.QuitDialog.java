h50203
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 daqtlc/QuitDialog.java
e
s 00134/00000/00000
d D 1.1 00/07/31 14:48:42 gurjyan 1 0
c date and time created 00/07/31 14:48:42 by gurjyan
e
u
U
f e 0
t
T
I 1
import java.awt.*;
import java.awt.event.*;

 public class QuitDialog extends Dialog
 {

	public QuitDialog(Frame parent, boolean modal)
	{
		super(parent, modal);

		//{{INIT_CONTROLS
		setLayout(null);
		setBackground(java.awt.Color.green);
		setFont(new Font("Serif", Font.BOLD, 14));
		setSize(337,135);
		setVisible(false);
		yesButton.setLabel(" Yes ");
		add(yesButton);
		yesButton.setBackground(java.awt.Color.lightGray);
		yesButton.setFont(new Font("Dialog", Font.BOLD, 12));
		yesButton.setBounds(72,80,79,22);
		noButton.setLabel("  No  ");
		add(noButton);
		noButton.setBackground(java.awt.Color.lightGray);
		noButton.setFont(new Font("Dialog", Font.BOLD, 12));
		noButton.setBounds(185,80,79,22);
		label1.setText("Quit Daqtlc?");
		label1.setAlignment(java.awt.Label.CENTER);
		add(label1);
		label1.setFont(new Font("Serif", Font.BOLD, 14));
		label1.setBounds(78,33,180,23);
		setTitle("Exit Program?");
		//}}

		//{{REGISTER_LISTENERS
		SymWindow aSymWindow = new SymWindow();
		this.addWindowListener(aSymWindow);
		SymAction lSymAction = new SymAction();
		noButton.addActionListener(lSymAction);
		yesButton.addActionListener(lSymAction);
		//}}
	}

	public void addNotify()
	{
	    // Record the size of the window prior to calling parents addNotify.
	    Dimension d = getSize();
	    
		super.addNotify();

		if (fComponentsAdjusted)
			return;

		// Adjust components according to the insets
		setSize(insets().left + insets().right + d.width, insets().top + insets().bottom + d.height);
		Component components[] = getComponents();
		for (int i = 0; i < components.length; i++)
		{
			Point p = components[i].getLocation();
			p.translate(insets().left, insets().top);
			components[i].setLocation(p);
		}
		fComponentsAdjusted = true;
	}

	public QuitDialog(Frame parent, String title, boolean modal)
	{
		this(parent, modal);
		setTitle(title);
	}

    /**
     * Shows or hides the component depending on the boolean flag b.
     * @param b  if true, show the component; otherwise, hide the component.
     * @see java.awt.Component#isVisible
     */
    public void setVisible(boolean b)
	{
		if(b)
		{
			Rectangle bounds = getParent().getBounds();
			Rectangle abounds = getBounds();
	
			setLocation(bounds.x + (bounds.width - abounds.width)/ 2,
				 bounds.y + (bounds.height - abounds.height)/2);
		}
		super.setVisible(b);
	}

    // Used for addNotify check.
	boolean fComponentsAdjusted = false;

	//{{DECLARE_CONTROLS
	java.awt.Button yesButton = new java.awt.Button();
	java.awt.Button noButton = new java.awt.Button();
	java.awt.Label label1 = new java.awt.Label();
	//}}

    class SymWindow extends java.awt.event.WindowAdapter
	{
		public void windowClosing(java.awt.event.WindowEvent event)
		{
			Object object = event.getSource();
			if (object == QuitDialog.this)
				QuitDialog_WindowClosing(event);
		}
	}
    void QuitDialog_WindowClosing(java.awt.event.WindowEvent event)
	{
           dispose();
	}
  class SymAction implements java.awt.event.ActionListener
	{
		public void actionPerformed(java.awt.event.ActionEvent event)
		{
			Object object = event.getSource();
			if (object == noButton)
				noButton_Clicked(event);
			else if (object == yesButton)
				yesButton_Clicked(event);
		}
	}

   	void yesButton_Clicked(java.awt.event.ActionEvent event)
	{
		Toolkit.getDefaultToolkit().getSystemEventQueue().postEvent(new WindowEvent((java.awt.Window)getParent(), WindowEvent.WINDOW_CLOSING));
	}

	void noButton_Clicked(java.awt.event.ActionEvent event)
	{
	    dispose();
	}

}
E 1
