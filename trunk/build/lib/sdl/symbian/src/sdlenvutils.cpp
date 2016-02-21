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

#include"sdlenvdata.h"
#include <w32std.h>
#include <aknappui.h>
#include <aknapp.h>

extern EpocSdlEnvData* gEpocEnv;    


NONSHARABLE_CLASS(CCurrentAppUi) : public CAknAppUi
	{
	public:
		static CCurrentAppUi* Cast(CEikAppUi* aUi);
		void DisableKeyBlocking();
		TInt SetAppOrientation(CSDL::TAppOrientation aOrientation);
		CSDL::TAppOrientation AppOrientation();
	};
	
	
CCurrentAppUi* CCurrentAppUi::Cast(CEikAppUi* aUi)
	{
	return static_cast<CCurrentAppUi*>(aUi);	
	}
	
void CCurrentAppUi::DisableKeyBlocking()	
	{
	SetKeyBlockMode(ENoKeyBlock);	
	}

TInt CCurrentAppUi::SetAppOrientation(CSDL::TAppOrientation aOrientation)	
	{
	CAknAppUiBase::TAppUiOrientation prevDir = Orientation();
	CAknAppUiBase::TAppUiOrientation dir = CAknAppUiBase::EAppUiOrientationUnspecified;
	switch(aOrientation)
	    {
	    case CSDL::EDefaultAppOrientation: 
	        break;
	    case CSDL::ELandscapeAppOrientation:
	        dir = CAknAppUiBase::EAppUiOrientationLandscape;
	        break;
	    case CSDL::EPortraitAppOrientation:
	        dir = CAknAppUiBase::EAppUiOrientationPortrait;
	        break;
	    }
	TInt err = KErrAlreadyExists;
	if(prevDir != dir)
	    {
	    TRAP(err, SetOrientationL(dir));
	    }
	return err;	
	}

CSDL::TAppOrientation CCurrentAppUi::AppOrientation()   
    {
    switch(Orientation())
        {
        case CAknAppUiBase::EAppUiOrientationAutomatic:
        case CAknAppUiBase::EAppUiOrientationUnspecified: return CSDL::EDefaultAppOrientation;
        case CAknAppUiBase::EAppUiOrientationLandscape: return CSDL::ELandscapeAppOrientation;
        case CAknAppUiBase::EAppUiOrientationPortrait: return CSDL::EPortraitAppOrientation;
        }
    return CSDL::EDefaultAppOrientation;
    }
    
TInt EnvUtils::RunSingleThread()
	{
	if(!EnvUtils::IsOwnThreaded())
    	{
    	
        /*TInt signals = 0;
        while(gEpocEnv->iEpocEnvFlags & EpocSdlEnvData::ESuspend)
            {
            CActiveScheduler::Current()->WaitForAnyRequest();
            TInt err;
            if(!CActiveScheduler::RunIfReady(err, CActive::EPriorityLow))
                {
                ++signals;
                }
            };
    		    
        while(signals)
            {
            RThread().RequestSignal();
            --signals;
            }
        */
    	
    	
    	
        const TInt count = RThread().RequestCount();
        if(count > 0)
            {
            const TInt filter = gEpocEnv->iEpocEnvFlags & EpocSdlEnvData::ESuspend ? 
                        CActive::EPriorityLow : CActive::EPriorityIdle; 
            TInt err;
    		if(CActiveScheduler::RunIfReady(err, filter))
    		    {
    		    CActiveScheduler::Current()->WaitForAnyRequest();
    		    return 1;
                }
            }
    	}
	return 0;
	}

void EnvUtils::DisableKeyBlocking()
	{        
	if(gEpocEnv->iAppUi != NULL)
		return CCurrentAppUi::Cast(gEpocEnv->iAppUi)->DisableKeyBlocking();
	}


TInt EnvUtils::SetAppOrientation(CSDL::TAppOrientation aOrientation)
	{
  if(gEpocEnv->iAppUi != NULL)
  	return CCurrentAppUi::Cast(gEpocEnv->iAppUi)->SetAppOrientation(aOrientation);
 	return KErrNotFound;
	}
	
TBool EnvUtils::Rendezvous(RThread& aThread, TRequestStatus& aStatus)
	{
	if(gEpocEnv->iId != TThreadId(0) &&
    		 	aThread.Open(gEpocEnv->iId) &&
    		  	aThread.ExitType() == EExitPending)
    			{
    			aThread.Rendezvous(aStatus);
    			return ETrue;
    			}
    return EFalse;
	}

	
CSDL::TAppOrientation EnvUtils::AppOrientation()
    {
    if(gEpocEnv->iAppUi != NULL)
        return CCurrentAppUi::Cast(gEpocEnv->iAppUi)->AppOrientation();
    return CSDL::EDefaultAppOrientation;
    }

TInt Panic(TInt aErr, TInt aLine)
	{
	TBuf<64> b;
	b.Format(_L("line: %d"), aLine);
	User::Panic(b, aErr);
	return 0;	
	}


