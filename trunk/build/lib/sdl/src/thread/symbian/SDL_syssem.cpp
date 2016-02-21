/*
    SDL - Simple DirectMedia Layer

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


*/

/*
    SDL_syssem.cpp

    Epoc version by Markus Mertama  
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id: SDL_syssem.c,v 1.1.2.4 2000/06/22 15:24:48 hercules Exp $";
#endif

/* Semaphore functions using the Win32 API */

//#include <stdio.h>
//#include <stdlib.h>
#include <e32std.h>

#include "SDL_error.h"
#include "SDL_thread.h"


#define SDL_MUTEX_TIMEOUT -2

struct SDL_semaphore
 {
 TInt handle;
 TInt count;
 };

/* Create a semaphore */
SDL_sem *SDL_CreateSemaphore(Uint32 initial_value)
    {
   SDL_semaphore* sem = (SDL_semaphore*) User::Alloc(sizeof(SDL_semaphore)); 
   
   if(sem == NULL)
	 {
	 SDL_SetError("Couldn't create semaphore");
	 return NULL;
	 }
   
   RSemaphore s;
   const TInt err = s.CreateLocal(initial_value);

   if(err != KErrNone)
       {
       User::Free(sem);
       SDL_SetError("Couldn't create semaphore");
       return NULL;
       }
      
    sem->handle = s.Handle();
	sem->count = initial_value;
	return(sem);
    }

/* Free the semaphore */
void SDL_DestroySemaphore(SDL_sem *sem)
    {
	if ( sem != NULL) 
	    {
	    RSemaphore sema;
	    sema.SetHandle(sem->handle);
	    sema.Signal(sem->count);
	    sema.Close();
	    User::Free(sem);
	    }
    }

#define NULLCHECK(s) if(s == NULL){SDL_SetError("Object is NULL"); return -1;}
  
int SDL_SemWaitTimeout(SDL_sem *sem, Uint32 timeout)
    {
    NULLCHECK(sem)

    RSemaphore sema;
    sema.SetHandle(sem->handle);
    
    --sem->count;
    
	if (timeout == SDL_MUTEX_MAXWAIT)
	    {
	    sema.Wait();
	    return 0;
	    } 
	
    
	const TInt err = sema.Wait(timeout * 1000);
	switch(err)
	    {
	    case KErrNone:
	        return 0;
	    case KErrGeneral:
	    case KErrArgument:
	        return -1;
	    default:
	        return SDL_MUTEX_TIMEOUT;
	    }
    }

int SDL_SemTryWait(SDL_sem *sem)
    {
    return SDL_SemWaitTimeout(sem, 0);
    }

int SDL_SemWait(SDL_sem *sem) 
    {
	return SDL_SemWaitTimeout(sem, SDL_MUTEX_MAXWAIT);
    }

/* Returns the current count of the semaphore */
Uint32 SDL_SemValue(SDL_sem *sem)
    {
    NULLCHECK(sem)
	return (Uint32) sem->count;
    }

int SDL_SemPost(SDL_sem *sem)
    {
    NULLCHECK(sem)
	sem->count++;
    RSemaphore sema;
    sema.SetHandle(sem->handle);
	sema.Signal();
	return 0;
    }

