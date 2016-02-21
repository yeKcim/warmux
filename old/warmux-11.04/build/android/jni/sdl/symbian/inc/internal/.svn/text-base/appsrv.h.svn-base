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
    appsrv.h
    Symbian OS services for SDL

    Markus Mertama
*/

#ifndef SDLAPPSRV_H
#define SDLAPPSRV_H

#include<e32base.h>
#include "dsa.h"

class MSDLObserver;

NONSHARABLE_CLASS(CSdlAppServ) : public CActive
    {
    public:
        enum
            {
            EAppSrvNoop = CDsa::ELastDsaRequest,
            EAppSrvWindowWidth,
            EAppSrvWindowHeight,
            EAppSrvWindowDisplayMode,
            EAppSrvWindowPointerCursorMode,
            EAppSrvDsaStatus,
        //    EAppSrvStopThread,
            EAppSrvWaitDsa,
            EVideoUpdate,
            };
        CSdlAppServ();
        void ConstructL();
        ~CSdlAppServ();
        TInt Request(TInt aService);
        TInt RequestValue(TInt aService);
        void Init(); 
        void PanicMain(TInt aReason);
        void PanicMain(const TDesC& aInfo, TInt aReason);
        void SetObserver(MSDLObserver* aObserver);
        TInt ObserverEvent(TInt aEvent, TInt aParam);
        void SetParam(TInt aParam);
        void HandleObserverValue(TInt aService, TInt aReturnValue, TBool aMainThread);
        MSDLObserver* Observer();
    private:
        void RunL();
        void DoCancel();
    private:
        const TThreadId iMainId;
        RThread iAppThread;
        TInt iService;
        TInt iReturnValue;
        RSemaphore iSema;
        MSDLObserver* iObserver;
        TRequestStatus* iStatusPtr;
    };
    
#endif    

