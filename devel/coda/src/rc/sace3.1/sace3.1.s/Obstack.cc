#include "Obstack.h"

class Obchunk *
Obstack::new_chunk (void)
{
  class Obchunk *temp = (class Obchunk *) new char[sizeof (class Obchunk) + this->size];

  temp->next = 0;
  temp->end  = temp->contents + this->size;
  temp->cur  = temp->contents;

  return temp;
}

Obstack::Obstack (int sz): size (sz), head (0)
{
  this->head = this->new_chunk ();
  this->curr = this->head;
}

Obstack::~Obstack (void)
{
  class Obchunk *temp = this->head;

  while (temp != 0)
    {
      class Obchunk *next = temp->next;
      temp->next  = 0;
      delete [] temp;
      temp = next;
    }
}

char *
Obstack::copy (const char* s, size_t len)
{
  char *result;

  assert (this->size >= len + 1);

  /* Check whether we need to grow our chunk... */

  if (this->curr->cur + len + 1 >= this->curr->end)
    {
      /* Check whether we can just reuse previously allocated memory */

      if (this->curr->next == 0)
	{
	  this->curr->next = this->new_chunk ();
	  this->curr       = this->curr->next;
	}
      else
	{
	  this->curr = this->curr->next;
	  this->curr->cur = this->curr->contents;
	}
    }

  result = this->curr->cur;
  ::memcpy (result, s, len);
  result[len] = '\0';
  this->curr->cur += (len + 1);
  return result;
}

void 
Obstack::release (void)
{
  this->curr      = this->head;
  this->curr->cur = this->curr->contents;
}


