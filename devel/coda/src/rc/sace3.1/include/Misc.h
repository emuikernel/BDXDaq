/* Miscellaneous C functions from $WRAPPER_ROOT/libsrc/Misc/Misc.C */

#if !defined (ACE_MISC_H)
#define ACE_MISC_H

int ace_daemonize (void);
// Become a daemon process.

ssize_t ace_recv_n (HANDLE handle, void *buf, size_t len, int flags);
// Receive <len> bytes into <buf> from <handle> (uses the <recv> system call).

ssize_t ace_recv_n (HANDLE handle, void *buf, size_t len);
// Receive <len> bytes into <buf> from <handle> (uses the <read> system call).

ssize_t ace_send_n (HANDLE handle, const void *buf, size_t len, int flags);
// Receive <len> bytes into <buf> from <handle> (uses the <send> system call).

ssize_t ace_send_n (HANDLE handle, const void *buf, size_t len);
// Receive <len> bytes into <buf> from <handle> (uses the <write> system call).

int ace_set_fl (HANDLE handle, int flags);
// Set flags associated with <handle>.
int ace_clr_fl (HANDLE handle, int flags);
// Clear flags associated with <handle>.

int ace_set_handle_limit (int new_limit = -1);
// Reset the limit on the number of open handles.
#endif  /* ACE_MISC_H */
