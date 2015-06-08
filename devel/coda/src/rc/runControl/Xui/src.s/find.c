#include <stdio.h>
#include <XmHTML.h>
#include <XmHTMLP.h>

/* XmHTMLTextScrollToLine */

#include "bm.h"

#include "find.h"

typedef struct {
	XmHTMLObjectTable *object;
	int word;
	int line;	/* for convenience */
	int character;
} FindContext;

static FindContext *FindPattern (XmHTMLObjectTable *formatted, char *str, FindContext *oldContext)
{
	FindContext *context = NULL;

	if (oldContext && oldContext->object)
		formatted = oldContext->object;

	if (NULL != formatted)
	{
		XmHTMLObjectTable *iterator;
		for(iterator = formatted; iterator != NULL; iterator = iterator->next)
		{
			if ( (OBJ_TEXT == iterator->object_type) || (OBJ_PRE_TEXT == iterator->object_type) )
			{
			  int i;
			  int startNum = (oldContext && (iterator==oldContext->object) ? oldContext->word : 0);
			  for (i=startNum; i < iterator->n_words; i++)
			  {
				XmHTMLWord *word = &iterator->words[i];
				{
					char *text;

					/* sanity */
					if(word->len == 0)
						continue;

					text = (oldContext && (iterator==oldContext->object) && (i==oldContext->word) ? &word->word[oldContext->character + 1] : word->word);

					if (text && *text)
					{
						int found;
						int strSize = strlen(str);
						int textSize = strlen(text);
#define MIN(a,b) (a<b ? a : b)
						textSize = MIN(textSize,word->len);	/* because PRE_TEXT has the entire line as text, but uses only the length portion */
						found  = BM(text, str, textSize, strSize);

						if (-1 != found)
						{
							context = (FindContext*)XtMalloc(sizeof(FindContext));
							context->object = iterator;
							context->word = i;
							context->line = word->line;
							if (oldContext)
								found += oldContext->character + 1;
							context->character = found;
							goto getOut;
						}
					}
				}
 			  }
			}
		}
	}
getOut:
	FindSurrenderData((void*)oldContext);
	return context;
}

void *FindString(Widget w, char *str, void *data)
{
	FindContext *context = NULL;
	
	if(w)
	{
		/* TBC: str can't contain spaces if we search by HTMLword */
		context = FindPattern(((XmHTMLWidget)w)->html.formatted, str, (FindContext*)data);
	}
	return (void*)context;
}

int FindLineFromData(void* data)
{
	FindContext *context = (FindContext*) data;
	if (context)
	return context->line;
	else
	return 0;	/* guess */
}

int FindCoordsFromData(void* data, XRectangle *rect /* RETURN */)
{
	FindContext *context = (FindContext*) data;
	if (context && context->object)
	{
		XmHTMLWord *word = &(context->object->words[context->word]);
		rect->x = word->x;
		rect->y = word->y - word->font->height + word->font->xfont->descent;
		rect->width = word->width;
		rect->height = word->height;
		return 1;
	}
	else
		return 0;	
}

void FindSurrenderData(void *data)
{
	XtFree((char*)data);
}
