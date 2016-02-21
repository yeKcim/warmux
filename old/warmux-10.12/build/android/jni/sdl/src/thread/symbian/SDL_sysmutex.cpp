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
    SDL_sysmutex.cpp

    Epoc version by Markus Mertama 
*/


#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id: SDL_sysmutex.c,v 1.1.2.3 2000/06/22 15:25:23 hercules Exp $";
#endif

/* Mutex functions using the Win32 API */

//#include <stdio.h>
//#include <stdlib.h>

#include <e32std.h>

#include "epoc_sdl.h"

#include "SDL_error.h"
#include "SDL_mutex.h"


struct SDL_mutex
    {
    TInt handle;
    };

#define NULLCHECK(s) if(s == NULL){SDL_SetError("Object is NULL"); return -1;}
    
void DeleteMutex(TAny* aMutex)
    {
    SDL_DestroyMutex ((SDL_mutex*) aMutex);
    }

/* Create a mutex */
SDL_mutex *SDL_CreateMutex()
    {
    SDL_mutex* mutex = (SDL_mutex*) User::Alloc(sizeof(SDL_mutex));
    
    if(mutex == NULL)
        {
        SDL_SetError("Couldn't create mutex");
        return NULL;
        }
    
    RMutex rmutex;
    const TInt err = rmutex.CreateLocal();
   
	if(err != KErrNone)
	    {
		SDL_SetError("Couldn't create mutex");
		User::Free(mutex);
		return NULL;
		}
    
    mutex->handle = rmutex.Handle();
    /*audio or something does not clean their mutexes properly*/
    EpocSdlEnv::AppendCleanupItem(TSdlCleanupItem(DeleteMutex, mutex, EFalse));
	return(mutex);
}

/* Free the mutex */
void SDL_DestroyMutex(SDL_mutex *mutex)
    {
	if ( mutex != NULL) 
	    {
	    RMutex rmutex;
	    rmutex.SetHandle(mutex->handle);
	    if(rmutex.IsHeld())
	        {
	        rmutex.Signal();
	        }
	    rmutex.Close();
	    EpocSdlEnv::RemoveCleanupItem(mutex);
	    User::Free(mutex);
	    }
    }

/* Lock the mutex */
int SDL_mutexP(SDL_mutex* mutex)
    {
    NULLCHECK(mutex)
    RMutex rmutex;
    rmutex.SetHandle(mutex->handle);
	rmutex.Wait(); 
	return(0);
    }

/* Unlock the mutex */
int SDL_mutexV(SDL_mutex *mutex)
    {
    NULLCHECK(mutex)
	RMutex rmutex;
    rmutex.SetHandle(mutex->handle);
	rmutex.Signal();
	return(0);
    }

