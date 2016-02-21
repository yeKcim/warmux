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
    SDL_systhread.cpp
    Epoc thread management routines for SDL

    Epoc version by Markus Mertama  
*/

#include "epoc_sdl.h"

//#include <stdlib.h>
//#include <stdio.h>



extern "C" {
#undef NULL
#include "SDL_error.h"
#include "SDL_thread.h"
#include "SDL_systhread.h"
#include "SDL_thread_c.h"
    }

#include <e32std.h>
#include "epoc_sdl.h"


int RunThread(TAny* data)
    {
    CTrapCleanup* cleanup = CTrapCleanup::New();
    EpocSdlEnv::AppendThread(RThread().Id());
    TRAPD(err, SDL_RunThread(data));
    EpocSdlEnv::CleanupItems();
    delete cleanup;
    EpocSdlEnv::RemoveThread(RThread().Id());
    return(err);
    }


TInt NewThread(const TDesC& aName, TAny* aPtr1, TAny* aPtr2)
    {
    return ((RThread*)(aPtr1))->Create(aName,
            RunThread,
            EpocSdlEnv::StackSize(),
            NULL,
            aPtr2);
    }


int CreateUnique(TInt (*aFunc)(const TDesC& aName, TAny*, TAny*), TAny* aPtr1, TAny* aPtr2)
    {
    TBuf<32> name;
    TInt status = KErrNone;
    static TInt ObjectCount = 0;
    do
        {
        name.Format(_L("SDL_%x"), ObjectCount++);
        status = aFunc(name, aPtr1, aPtr2);
        }
        while(status == KErrAlreadyExists);
    return status;
    }


int SDL_SYS_CreateThread(SDL_Thread *thread, void *args)
    {
    RThread rthread;
    const TInt status = CreateUnique(NewThread, &rthread, args);
    if (status != KErrNone) 
        {
        thread->handle = 0;
				SDL_SetError("Thread create error");
				return(-1);
        }
		rthread.Resume();
    thread->handle = rthread.Handle();
		return(0);
    }

void SDL_SYS_SetupThread()
    {
    }

Uint32 SDL_ThreadID(void)
    {
    RThread current;
    const TThreadId id = current.Id();
	return id;
    }

void SDL_SYS_WaitThread(SDL_Thread *thread)
    {
    SDL_TRACE1("Close thread", thread);
    RThread t;
    const TInt err = t.Open(thread->threadid);
    if(err == KErrNone && t.ExitType() == EExitPending)
        {
        TRequestStatus status;
        t.Logon(status);
        User::WaitForRequest(status);
        }
    t.Close();
    SDL_TRACE1("Closed thread", thread);
    }

/* WARNING: This function is really a last resort.
 * Threads should be signaled and then exit by themselves.
 * TerminateThread() doesn't perform stack and DLL cleanup.
 */
void SDL_SYS_KillThread(SDL_Thread *thread)
    {
    RThread rthread;
    rthread.SetHandle(thread->handle);
    rthread.Kill(KErrDied);
    rthread.Close();
    }

