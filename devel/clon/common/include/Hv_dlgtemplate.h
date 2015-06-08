#ifndef _DLGTEMPLATEH_
#define _DLGTEMPLATEH_
/*
	struct DLGTEMPLATEEX
	{
		WORD dlgVer;     01 00
		WORD signature;  FF FF
		DWORD helpID;    00 00 00 00 
		DWORD exStyle;   00 00 00 00
		DWORD style;     C0 00 C8 80
		WORD cDlgItems;  05 00
		short x;         00 00
		short y;         00 00
		short cx;        01 01
		short cy;        62 00

		// Everything else in this structure is variable length,
		// and therefore must be determined dynamically

		// sz_Or_Ord menu;			// name or ordinal of a menu resource
		// sz_Or_Ord windowClass;	// name or ordinal of a window class
		// WCHAR title[titleLen];	// title string of the dialog box
		// short pointsize;			// only if DS_SETFONT is set
		// short weight;			// only if DS_SETFONT is set
		// short bItalic;			// only if DS_SETFONT is set
		// WCHAR font[fontLen];		// typeface name, if DS_SETFONT is set
	};
	struct DLGITEMTEMPLATEEX
	{
		DWORD helpID;
		DWORD exStyle;
		DWORD style;
		short x;
		short y;
		short cx;
		short cy;
		DWORD id;

		// Everything else in this structure is variable length,
		// and therefore must be determined dynamically

		// sz_Or_Ord windowClass;	// name or ordinal of a window class
		// sz_Or_Ord title;			// title string or ordinal of a resource
		// WORD extraCount;			// bytes following creation data
	};
*/
typedef struct hvdlgtemplateex  *LPDLGTEMPLATEEX;
typedef struct hvdlgtemplateex
{ 
      // for some reason windows doesn't have a definition
	WORD   dlgVer;    // for an extended dialog box template
	WORD   signature;
	DWORD  helpID; 
    DWORD  exStyle;
	DWORD  style;
	WORD   cDlgItems;
	short  x; 
    short  y;
	short  cx;
	short  cy; 
	/**
    WORD menu;         // name or ordinal of a menu resource
    WORD windowClass;  // name or ordinal of a window class
    WCHAR  *title;          // title string of the dialog box
    short  pointsize;       // only if DS_SETFONT flag is set
    short  weight;          // only if DS_SETFONT flag is set
    short  bItalic;         // only if DS_SETFONT flag is set
    WCHAR  font[fontLen];   // typeface name, if DS_SETFONT is set
	**/
} DLGTEMPLATEEX; 

typedef struct hvdlgitemtemplateex  *LPDLGITEMTEMPLATEEX;

typedef struct hvdlgitemtemplateex

{
	DWORD  helpID;
	DWORD  exStyle;
	DWORD  style; 
    short  x;
	short  y;
	short  cx;
	short  cy;
	DWORD   id; 
	/**
    WORD windowClass; // name or ordinal of a window class
    WORD title;       // title string or ordinal of a resource
    WORD   extraCount;     // bytes of following creation data
	**/
} DLGITEMTEMPLATEEX; 


LPDLGITEMTEMPLATEEX CONTROLEX(LPDLGITEMTEMPLATEEX item, LPCWSTR text, 
					short id, LPCWSTR classname, 
					DWORD style, 
					int x, int y, int cx, int cy, 
					DWORD exstyle);

LPDLGTEMPLATEEX DIALOGEX(LPDLGITEMTEMPLATEEX * item,
			UINT buffsize, int x, int y, int cx, int cy,
			DWORD style, DWORD exstyle, LPCWSTR menu,
			LPCWSTR class, LPCWSTR caption, LPCWSTR font, 
			int height);


LPDLGITEMTEMPLATE CONTROL(LPDLGITEMTEMPLATE item, LPCWSTR text, 
					short id, LPCWSTR classname, 
					DWORD style, 
					int x, int y, int cx, int cy, 
					DWORD exstyle);


LPDLGITEMTEMPLATE CTEXT(LPDLGITEMTEMPLATE item, LPCWSTR text, short id, 
			int x, int y, int cx, int cy, DWORD style, 
			DWORD exstyle);

LPDLGITEMTEMPLATE PUSHBUTTON(LPDLGITEMTEMPLATE item, LPCWSTR text,
			short id, int x, int y, int cx, int cy, 
			DWORD style, DWORD exstyle);

LPDLGITEMTEMPLATE DEFPUSHBUTTON(LPDLGITEMTEMPLATE item, LPCWSTR text,
			short id, int x, int y, int cx, int cy, 
			DWORD style, DWORD exstyle);

LPDLGITEMTEMPLATE ICON(LPDLGITEMTEMPLATE item, LPCWSTR icon, 
					short id, int x, int y, int cx, int cy,
					DWORD style, DWORD exstyle);

LPDLGITEMTEMPLATE LTEXT(LPDLGITEMTEMPLATE item, LPCWSTR text, short id, 
			int x, int y, int cx, int cy, DWORD style, 
			DWORD exstyle);

LPDLGITEMTEMPLATE RTEXT(LPDLGITEMTEMPLATE item, LPCWSTR text, short id, 
			int x, int y, int cx, int cy, DWORD style, 
			DWORD exstyle);

LPDLGTEMPLATE DIALOG(LPDLGITEMTEMPLATE * item,
			UINT buffsize, int x, int y, int cx, int cy,
			DWORD style, DWORD exstyle, LPCWSTR menu,
			LPCWSTR class, LPCWSTR caption, LPCWSTR font, 
			int height);
LPDLGITEMTEMPLATE LISTBOX(LPDLGITEMTEMPLATE item, short id, 
                        int x, int y, int cx, int cy, DWORD style, 
                        DWORD exstyle);
LPDLGITEMTEMPLATE RADIOBUTTON(LPDLGITEMTEMPLATE item, LPCWSTR text,
                        short id, int x, int y, int cx, int cy, 
                        DWORD style, DWORD exstyle);
LPDLGITEMTEMPLATE EDITTEXT(LPDLGITEMTEMPLATE item, short id, 
                        int x, int y, int cx, int cy, DWORD style, 
                        DWORD exstyle,LPCWSTR defaultText);
LPDLGITEMTEMPLATE AUTOCHECKBOX(LPDLGITEMTEMPLATE item, LPCWSTR text,
                        short id, int x, int y, int cx, int cy, 
                        DWORD style, DWORD exstyle);
LPDLGITEMTEMPLATE CHECKBOX(LPDLGITEMTEMPLATE item, LPCWSTR text,
                        short id, int x, int y, int cx, int cy, 
                        DWORD style, DWORD exstyle);
LPDLGITEMTEMPLATE COMBOBOX(LPDLGITEMTEMPLATE item, short id, 
                        int x, int y, int cx, int cy, DWORD style, 
                        DWORD exstyle);
LPDLGITEMTEMPLATE SCROLLBAR(LPDLGITEMTEMPLATE item, short id, 
                        int x, int y, int cx, int cy, DWORD style, 
                        DWORD exstyle);


#endif