//-----------------------------------------------------------------------------
// Copyright (c) 1991,1992 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
// Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
//-----------------------------------------------------------------------------
// 
// Description:
//	 X Window Reactor Class for I/O
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: X_Reactor.i,v $
//   Revision 1.1.1.1  1996/10/10 20:04:47  chen
//   simple ACE version 3.1
//
//
INLINE 
X_Reactor::X_Reactor (XtAppContext app_context)
{
    app_context_ = app_context;
    if (this->open())
	::perror("X Reactor::open");
}

INLINE X_Reactor::X_Reactor (XtAppContext app_context, int size, int rs)
{
    app_context_ = app_context;
    if (this->open(size, rs))
	::perror("X Reactor::open");
}

INLINE int
X_Reactor::open (int size, int rs)
{
    max_size_ = size;
    event_handlers_ = (Event_Handler **)new Event_Handler *[size];
    max_handle_ = 0;
    xinput_id_ = (XtInputId *)new XtInputId[size];
    mask_ = (Reactor_Mask *)new Reactor_Mask[size];

    for (int i = 0; i < size; i++){
	event_handlers_[i] = 0;
	xinput_id_ [i] = 0;
    	mask_[i] = 0;
    }
    return 0;
}

INLINE void
X_Reactor::close (void)
{
    if (max_size_){
        for (int i = 0; i < max_handle_; i++){
	    this->detach (i, Event_Handler::RWE_MASK);
	    if (xinput_id_ [i])
            	XtRemoveInput (xinput_id_ [i]);
        }
        delete [] event_handlers_;    
    	delete [] xinput_id_;
    	delete [] mask_;
    	max_handle_ = 0;
    	max_size_ = 0;
    }
}

INLINE int
X_Reactor::attach (HANDLE handle,
	           Event_Handler *handler,
		   Reactor_Mask mask)
{
    if (handle < 0 || handle >= max_size_)
	return -1;

    event_handlers_[handle] = handler;
    mask_[handle] = mask;
    xinput_id_ [handle] = XtAppAddInput (app_context_, handle, 
					 (XtPointer)mask, 
		   			 &X_Reactor::inputCallback,
		   			 (XtPointer)this);
    if (max_handle_ < handle + 1)
	max_handle_ = handle + 1;

    return 0;
}

INLINE int
X_Reactor::detach (HANDLE handle, Reactor_Mask mask)
{
    if (handle < 0 || handle >= max_size_ || event_handlers_ [handle] == 0)
	return -1;

    Event_Handler *eh = (Event_Handler *)event_handlers_[handle];
    XtInputId id = xinput_id_ [handle];

    eh->handle_close (handle, mask);
    XtRemoveInput (id);
    event_handlers_[handle] = 0;
    xinput_id_ [handle] = 0;
    mask_[handle] = 0;
    return 0;
}

INLINE void
X_Reactor::notify_handle (HANDLE handle)
{
    int status;
    Event_Handler *eh = event_handlers_[handle];

    if (mask_[handle] & Event_Handler::READ_MASK){
	status = eh->handle_input(handle);    
	if (status < 0)
	    detach (handle, mask_[handle]);
    }
    if (mask_[handle] & Event_Handler::WRITE_MASK){
	status = eh->handle_output(handle);    
	if (status < 0)
	    detach (handle, mask_[handle]);
    }
    if (mask_[handle] & Event_Handler::EXCEPT_MASK){
	status = eh->handle_exception(handle);    
	if (status < 0)
	    detach (handle, mask_[handle]);
    }
}

INLINE int
X_Reactor::register_handler (Event_Handler *handler, Reactor_Mask mask)
{
    return attach (handler->get_handle (), handler, mask);
}

INLINE int
X_Reactor::remove_handler (Event_Handler *handler, Reactor_Mask mask)
{
    return detach (handler->get_handle(), mask);
}

INLINE 
X_Reactor::~X_Reactor (void)
{
    close();
}
