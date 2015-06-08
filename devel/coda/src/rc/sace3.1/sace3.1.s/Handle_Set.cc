/* Wrapper for the fd_set abstraction. */

#include "Handle_Set.h"

/* Hack for Read Hat Linux 6.0   DJA  */
/*Sergey: do not need it anymore ???
#ifdef Linux
#define fds_bits __fds_bits
#endif
*/

const char Handle_Set::nbits_[256] = 
{
  0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
};

/* Counts the number of bits enabled in N.  Uses a table lookup to
   speed up the count. */

inline int
Handle_Set::count_bits (unsigned long n) const
{
  return (Handle_Set::nbits_[n & 0xff] + Handle_Set::nbits_[(n >> 8) & 0xff] +
	  Handle_Set::nbits_[(n >> 16) & 0xff] + Handle_Set::nbits_[n >> 24]);
}

void
Handle_Set_Iterator::operator++ (void)
{
  this->val_ = (this->val_ >> 1) & Handle_Set::MSB_MASK;
  this->num_++;

  if (this->val_ == 0) 
    {
      for (this->index_++; 
	   this->index_ < Handle_Set::NUM_WORDS && fds_.mask_.fds_bits[this->index_] == 0; 
	   this->index_++)
	;

      if (this->index_ == Handle_Set::NUM_WORDS)
	{
	  this->num_ = fds_.max_handle_ + 1;
	  return;
	}
      else
	{
	  this->val_ = fds_.mask_.fds_bits[this->index_];
	  this->num_ = this->index_ * Handle_Set::WORD_SIZE;
	}
    }

  for (; BIT_DISABLED (this->val_); this->num_++)
    this->val_ = (this->val_ >> 1) & Handle_Set::MSB_MASK;
}

Handle_Set_Iterator::Handle_Set_Iterator (Handle_Set &f)
				 : fds_ (f), 
				   index_ (0), 
				   num_ (f.size_ == 0 ? f.max_handle_ + 1 : 0)
{
  for (; fds_.mask_.fds_bits[this->index_] == 0; this->index_++)
    this->num_ += Handle_Set::WORD_SIZE;

  for (this->val_ = this->fds_.mask_.fds_bits[this->index_];
       (BIT_DISABLED (this->val_)) && this->num_ < Handle_Set::MAX_SIZE;
       this->num_++)
    this->val_ = (this->val_ >> 1) & Handle_Set::MSB_MASK;
}

/* Synchronize the underlying FD_SET with the MAX_FD and the SIZE. */

void
Handle_Set::sync (int max)
{
  this->size_ = 0;
  int i;

  for (i = max / Handle_Set::WORD_SIZE; i >= 0; i--)
    this->size_ += count_bits (this->mask_.fds_bits[i]);

  this->set_max (max);
}

/* Resets the MAX_FD after a clear of the original MAX_FD. */

void
Handle_Set::set_max (int current_max)
{
  int i;

  if (this->size_ == 0)
    this->max_handle_ = -1;
  else
    {
      for (i = current_max / Handle_Set::WORD_SIZE;
	   this->mask_.fds_bits[i] == 0; 
	   i--)
	;

      this->max_handle_ = i * Handle_Set::WORD_SIZE;
      for (fd_mask val = this->mask_.fds_bits[i]; 
	   (val & ~1) != 0;
	   val = (val >> 1) & Handle_Set::MSB_MASK)
	this->max_handle_++;
    }
}

/* Debugging method that prints out the underlying mask. */

int
Handle_Set::pr_mask (void) 
{
  int i;

  fprintf(stdout, "[ ");
  for (i = 0; i < this->max_handle_ + 1; i++) 
    if (this->is_set (i))
      fprintf(stdout," %d ", i);

  fprintf(stdout, " ]");
  return this->size_;
}

#if defined (DEBUG)
void
test1 (int count)
{
  int duplicates = 0;
  int sets	 = 0;
  int clears	 = 0;
  int i;

  Handle_Set fd_set;

  srandom (time (0L));

  for (i = 0; i < count; i++)
    {
      i = int (random () % FD_SETSIZE);

      if (i & 1)
	{
	  if (fd_set.is_set (i))
	    duplicates++;

	  fd_set.set_bit (i);
	  sets++;
	}
      else
	{
	  if (fd_set.is_set (i))
	    duplicates--;

	  fd_set.clr_bit (i);
	  clears++;
	}
    }

  printf ("count = %d, set_size = %d, duplicates = %d\n",
	  count, fd_set.num_set (), (sets - clears) == duplicates);
}

void
test2 (void)
{
  Handle_Set fd_set;
  int	 i;

  fd_set.set_bit (0);
  fd_set.set_bit (1);
  fd_set.set_bit (32);
  fd_set.set_bit (63);
  fd_set.set_bit (64);
  fd_set.set_bit (65);
  fd_set.set_bit (122);
  fd_set.set_bit (129);
  fd_set.set_bit (245);
  fd_set.set_bit (255);

  for (Handle_Set_Iter fi (fd_set); (i = fi ()) != -1; fi++)
    printf ("i = %d\n", i);
}

int
main (int argc, char *argv[])
{
  int count = atoi (argv[1]);
  /*  test1 (count); */
  test2 ();
}
#endif /* DEBUG */
