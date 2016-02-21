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

//  INCLUDES
#include <aknapp.h>
#include <aknappui.h>
#include <eikdoc.h>
#include <sdlepocapi.h>
#include <bautils.h>
#include <eikstart.h>
#include <badesca.h>
#include <bautils.h>
#include <apgcli.h>
#include <sdlmain.h>
#include <eikedwin.h>
#include <eiklabel.h>
#include <aknglobalmsgquery.h>
#include <apgwgnam.h> 

#include "hwablitter.h"


#include "blitter.h"
#include "fpsview.h"
#include "virtualcursor.h"
#include "zoomer.h"

//#include <sdlexe.mbg>

_LIT(KGlobalPath, "\\data\\sdl\\");
_LIT(KDefaultHWABlitter, "sdlhwablitter.dll");




//  FORWARD DECLARATIONS
class CApaDocument;


//const TUid KSDLUID = { 0xF01F605E };

LOCAL_C void MakeCCmdLineL(const TDesC8& aParam, CDesC8Array& aArray)
    { 
    
    const TChar dq('\"');

    TLex8 lex(aParam);
    TBool in = EFalse;

    lex.SkipSpaceAndMark();

    while(!lex.Eos())
        {
        TPtrC8 ptr;
        if(in)
            {
            const TPtrC8 rem = lex.RemainderFromMark();
            const TInt pos = rem.Locate(dq);
            if(pos > 0)
                {
                lex.Inc(pos);
                ptr.Set(lex.MarkedToken());
                lex.SkipAndMark(1);
                }
            else
                {
                ptr.Set(rem);
                }
            in = EFalse;
            }
        else
            {
            ptr.Set(lex.NextToken());
            const TInt pos = ptr.Locate(dq);
            if(pos == 0)
                {
                lex.UnGetToMark();
                lex.SkipAndMark(1);
                in = ETrue;
                continue; // back to in brace
                }
            else
                lex.SkipSpaceAndMark();
            }
        
        aArray.AppendL(ptr);

        }
    }  

void GetPrivatePath(RFs& aFs, TDes& aName)
    {
    RFs fs;
    TChar drive;
    TFileName name = RProcess().FileName();
    drive = name[0];
    drive.UpperCase();
    if(drive == 'Z')
        drive = 'C';
    
    aName.Append(drive);
    aName.Append(':');

    aFs.PrivatePath(name);
   
    aName.Append(name);
    }


NONSHARABLE_CLASS(TSdlClass)
	{
	public:
		TSdlClass();
		void SetMain(const TMainFunc& aFunc, MSDLMainObs* aObs);
		TInt SdlFlags() const;
		const TMainFunc& Main() const;
		void SendEvent(TInt aEvent, TInt aParam, CSDL* aSDL);
		TInt AppFlags() const; 
		void AppFlags(TInt aFlags); 
		void SetFlags(TInt aSdlFlags, TInt aExeFlags);
	private:
		TMainFunc iFunc;
		TInt iSdlFlags;
		TInt iExeFlags;
		MSDLMainObs* iObs;
	};
	
	
void TSdlClass::AppFlags(TInt aFlags)
	{
	iExeFlags |= aFlags;
	}
	
void TSdlClass::SendEvent(TInt aEvent, TInt aParam, CSDL* aSDL)
	{
	if(iObs != NULL)
		iObs->SDLMainEvent(aEvent, aParam, aSDL);
	}
	
TInt TSdlClass::AppFlags() const
	{
	return iExeFlags;
	}
	
void TSdlClass::SetFlags(TInt aSdlFlags, TInt aExeFlags)
    {
    iSdlFlags = aSdlFlags;
    iExeFlags = aExeFlags;
    }

void TSdlClass::SetMain(const TMainFunc& aFunc, MSDLMainObs* aObs)
	{	
	iFunc = aFunc;
	iObs = aObs;
	}
	
const TMainFunc& TSdlClass::Main() const
	{
	return iFunc;
	}
	
 
 TInt TSdlClass::SdlFlags() const
 	{
 	return iSdlFlags;
 	}
 	

 	
TSdlClass::TSdlClass()
	{
	Mem::FillZ(this, sizeof(this));
	}
 
TSdlClass gSDLClass;    
    
	     
////////////////////////////////////////////////////////////////    

NONSHARABLE_CLASS(CSDLApplication) : public CAknApplication
    {
    public:
    	CSDLApplication();
    private:
        CApaDocument* CreateDocumentL(); 
        TFileName ResourceFileName() const;
        TUid AppDllUid() const; 
      	void FindMeL();
     	TUid iUid;
    };
    
NONSHARABLE_CLASS(CSDLDocument)  : public CEikDocument
    {
    public:
        CSDLDocument(CEikApplication& aApp);
     private:
     	CEikAppUi* CreateAppUiL();
     
     };
     
 ////////////////////////////////////////////////////////////////////
 
     
NONSHARABLE_CLASS(MExitWait)
 	{
 	public:
 		virtual void DoExit(TInt aErr) = 0;
 	};   
 
///////////////////////////////////////////////////////////////////////// 
 	
NONSHARABLE_CLASS(CExitWait) : public CActive
 	{
 	public:
 		CExitWait(MExitWait& aWait);
 		~CExitWait();
 	private:
 		void RunL();
 		void DoCancel();
 	private:
 		MExitWait& iWait;
 		TRequestStatus* iStatusPtr;
 	};
 	
//////////////////////////////////////////////////////////////////////// 

NONSHARABLE_CLASS(MSDLDraw)
    {
    public:
        virtual void SdlDraw() = 0;
    };
 	
NONSHARABLE_CLASS(CSDLWin) : public CCoeControl
	{
	public:
	    CSDLWin(MSDLDraw& aSdlDraw);
		void ConstructL(const TRect& aRect, TBool aBackup);
		RWindow& GetWindow() const;
		void SetNoDraw();
		~CSDLWin();
	private:
		void Draw(const TRect& aRect) const;
	private:
	    MSDLDraw& iSdlDraw;
		TBool iNoDraw;
	}; 	
	

////////////////////////////////////////////////////////////////////////////	
     
NONSHARABLE_CLASS(CSDLAppUi) : public CAknAppUi, public MExitWait, public MSDLObserver, public MSDLDraw
	{
	public:
		~CSDLAppUi();
		static void FlagsFromFileL(TInt& aSdlFlags, TInt& aExeFlags);
   	private: // New functions
 		void ConstructL(); 
 		void HandleCommandL(TInt aCommand);
 		void HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination);
 		void HandleResourceChangeL(TInt aType);
        
   		void DoExit(TInt aErr);
   	
   		TInt SdlEvent(TInt aEvent, TInt aParam);
    	TInt SdlThreadEvent(TInt aEvent, TInt aParam);
    
    	void StartL();
    	static TBool StartL(TAny* aThis);
    	
    	TBool ParamEditorL(TDes& aCheat);
    	
    	TBool ProcessCommandParametersL(CApaCommandLine &aCommandLine);
    	
    	void PrepareToExit();
    	void HandleConsoleWindowL();
    	void HandleConsoleWindow();
    	void HandleForegroundEventL(TBool aForeground);
    	
    	static TBool IdleRequestL(TAny* aThis);
    	
    	TBool HandleKeyL(const TWsEvent& aEvent);
    	
    	TBool CreateHwaBlitterL(RWindow& aWindow, TInt aFlags);
    	
    
    	void SdlDraw();
    	
    	static void ParseFlags(const TDesC8& aString, TInt& aSdlFlags, TInt& aExeFlags);
    	static void FlagsFromFileL(const TDesC& aFile, TInt& aSdlFlags, TInt& aExeFlags);
    	static void MakeCCmdLineL(const TDesC8& Cmd, CDesC8Array& aArray);
    	static TBool FindFileL(const TDesC& aFile, TDes& aName, const TDesC& aSearchPath = KNullDesC);
	private:
		CExitWait* iWait;
		CSDLWin* iSDLWin;
		CSDL* iSdl;
		CIdle* iStarter;
		TBool iExitRequest;
		CDesC8Array* iParams;
		TInt iResOffset;
		CIdle* iIdle;
		TInt iStdOut;
		TVirtualCursor iCursor;
		CFbsBitmap*	iCBmp;
		CFbsBitmap*	iAlpha;
		CFbsBitmap* iCBmpMove;
		CBlitter* iZoomer;
		CFpsView* iFpsView;
		RBuf      iMbmFile;
		RLibrary  iHwaLib;
	//	TTime iLastPress;
	//	CSDL::TOrientationMode iOrientation;
	};
	
////////////////////////////////////////////////////////////////////////////////////////7



CApaDocument* CSDLApplication::CreateDocumentL()
	{
	return new (ELeave) CSDLDocument(*this);
	}
	
TUid CSDLApplication::AppDllUid() const
	{
	return iUid;
	}
	
	
CSDLApplication::CSDLApplication()
	{
	TRAPD(err, FindMeL());
	ASSERT(err == KErrNone);
	}	
	
void CSDLApplication::FindMeL()
	{
	RApaLsSession apa;
	User::LeaveIfError(apa.Connect());
	CleanupClosePushL(apa);
	User::LeaveIfError(apa.GetAllApps());
	TFileName name = RProcess().FileName();
	TApaAppInfo info;
	while(apa.GetNextApp(info) == KErrNone)
		{
		if(info.iFullName.CompareF(name) == 0)
			{
			iUid = info.iUid;
			break;
			}
		}
	CleanupStack::PopAndDestroy();
	}
	
TFileName CSDLApplication::ResourceFileName() const
	{
	return KNullDesC();
	}
	
///////////////////////////////////////////////////////////////////////////////////////////

CExitWait::CExitWait(MExitWait& aWait) : CActive(CActive::EPriorityStandard), iWait(aWait)
	{
	CActiveScheduler::Add(this);
	SetActive();
	iStatusPtr = &iStatus;
	}
	
CExitWait::~CExitWait()
	{
	Cancel();
	}
 
void CExitWait::RunL()
	{
	if(iStatusPtr != NULL )
		iWait.DoExit(iStatus.Int());
	}
	
void CExitWait::DoCancel()
	{
	if(iStatusPtr != NULL )
		User::RequestComplete(iStatusPtr , KErrCancel);
	}
	

//////////////////////////////////////////////////////////////////////////////////////////////

CSDLDocument::CSDLDocument(CEikApplication& aApp) : CEikDocument(aApp)
	{}
    
CEikAppUi* CSDLDocument::CreateAppUiL()
	{
	TInt sdlFlags = gSDLClass.SdlFlags();
	TInt exeFlags = gSDLClass.AppFlags();
	CSDLAppUi::FlagsFromFileL(sdlFlags, exeFlags);
	gSDLClass.SetFlags(sdlFlags, exeFlags);
	return new (ELeave) CSDLAppUi;
	}
	
///////////////////////////////////////////////////////////////////////////	
	
CSDLWin::CSDLWin(MSDLDraw& aSdlDraw) : iSdlDraw(aSdlDraw)
    {}

void CSDLWin:: ConstructL(const TRect& aRect, TBool aBackup)	
	{
    if(aBackup)
        CreateBackedUpWindowL(iEikonEnv->RootWin());
    else
        CreateWindowL();
    Window().EnableModifierChangedEvents(0xFFFFFFFF, EEventControlOnlyWithKeyboardFocus);
	SetRect(aRect);
	ActivateL();
	}
	
	
RWindow& CSDLWin::GetWindow() const
	{
	return Window();
	}
	

void CSDLWin::Draw(const TRect& /*aRect*/) const
	{
	if(!iNoDraw)
		{
		iSdlDraw.SdlDraw();
		}
	}	
	
/*
void CSDLWin::SetCanvasL(TInt aHandle)
    {
    if(iBmp == NULL)
        iBmp = new CWsBitmap(iEikonEnv->WsSession());
    else
        iBmp->Reset();
    User::LeaveIfError(iBmp->Duplicate(aHandle));
    }
*/
void CSDLWin::SetNoDraw()
	{
	iNoDraw = ETrue;
	}

CSDLWin::~CSDLWin()
    {
//     delete iBmp;
    }

/////////////////////////////////////////////////////////////////////////			
	
CSDLAppUi::~CSDLAppUi()
	{
	if(iIdle)
		iIdle->Cancel();
	delete iIdle;
	if(iStarter != NULL)
		iStarter->Cancel();
	if(iSdl != NULL)
	    iSdl->SetBlitter(NULL);
	
	if(iWait != NULL)
	    iWait->Cancel();

	delete iStarter;
	

	
	delete iWait;
	

    
    delete iSdl;
	
	delete iZoomer;
	
	delete iParams;
	delete iCBmp;
	delete iAlpha;
	delete iCBmpMove;
	delete iFpsView;

    delete iSDLWin;
    
	
	iMbmFile.Close();
	iHwaLib.Close();
	}


NONSHARABLE_STRUCT(SFlag)
    {
    TPtrC8 iName;
    TInt iValue;
    };

void CSDLAppUi::ParseFlags(const TDesC8& aString, TInt& aSdlFlags, TInt& aExeFlags)
    {
    
    const SFlag sdlFlags[] = {
            {_L8("EnableFocusStop"), CSDL::EEnableFocusStop },  
            {_L8("DrawModeDSB"), CSDL::EDrawModeDSB },    
            {_L8("AllowImageResize"), CSDL::EAllowImageResize},
            {_L8("DrawModeDSBDoubleBuffer"), CSDL::EDrawModeDSBDoubleBuffer},       
            {_L8("DrawModeDSBIncrementalUpdate"), CSDL::EDrawModeDSBIncrementalUpdate},
            {_L8("AllowImageResizeKeepRatio"),  CSDL::EAllowImageResizeKeepRatio},     
            {_L8("DrawModeGdi"),  CSDL::EDrawModeGdi}, 
       //     {_L8("DrawModeGdiGc"),  CSDL::EDrawModeGdiGc},  
            {_L8("DrawModeDSBAsync"), CSDL::EDrawModeDSBAsync},              
            {_L8("EOwnThread"),  CSDL::EOwnThread},                    
            {_L8("MainThread"), CSDL::EMainThread},                   
            {_L8("ImageResizeZoomOut"), CSDL::EImageResizeZoomOut},            
            {_L8("AutoOrientation"), CSDL::EAutoOrientation},             
            {_L8("DisableVolumeKeys"), CSDL::EDisableVolumeKeys}
    };
    
    
    const SFlag exeFlags[] = {
            {_L8("ParamQuery"), SDLEnv::EParamQuery}, 
            {_L8("AllowConsoleView"), SDLEnv::EAllowConsoleView}, 
            {_L8("VirtualMouse"), SDLEnv::EVirtualMouse}, 
            {_L8("ParamQueryDialog"), SDLEnv::EParamQueryDialog},
            {_L8("FastZoomBlitter"), SDLEnv::EFastZoomBlitter},
            {_L8("EnableVirtualMouseMoveEvents"), SDLEnv::EEnableVirtualMouseMoveEvents},
            {_L8("HWABlitter"),SDLEnv::EHWABlitter},
            {_L8("HWABlitterNoScale"), SDLEnv::EHWABlitterNoScale},
            {_L8("HWABlitterRatioScale"), SDLEnv::EHWABlitterRatioScale},
            {_L8("NoHWABlitter"), SDLEnv::ENoHWABlitter},
            {_L8("ViewFPS"), SDLEnv::EViewFPS}
    };

    
            
    TLex8 lx(aString);
    
    
    while(!lx.Eos())
        {
        const TPtrC8 token = lx.NextToken();
        if(0 == token.CompareF(_L8("reset")))
            {
            aSdlFlags = 0;
            aExeFlags = 0;
            }
        
        for(TInt i = 0; i < sizeof(sdlFlags) / sizeof(SFlag); i++)
            {
            if(0 == token.CompareF(sdlFlags[i].iName))
                {
                aSdlFlags |= sdlFlags[i].iValue;
                }
            }
        
        for(TInt i = 0; i < sizeof(exeFlags) / sizeof(SFlag); i++)
            {
            if(0 == token.CompareF(exeFlags[i].iName))
                {
                aExeFlags |= exeFlags[i].iValue;
                }            
            }
        }
    }
	
void CSDLAppUi::FlagsFromFileL(TInt& aSdlFlags, TInt& aExeFlags) 
     {
     _LIT(flagsfile, "sdl_flags.txt");
     TFileName name;
     if(FindFileL(flagsfile, name) || FindFileL(flagsfile, name, KGlobalPath))
         FlagsFromFileL(name, aSdlFlags, aExeFlags);
     }

TBool CSDLAppUi::FindFileL(const TDesC& aFile, TDes& aName, const TDesC& aSearchPath)
     {
     RFs& fs = CEikonEnv::Static()->FsSession();
     if(aSearchPath.Length() == 0)
         {
         GetPrivatePath(fs, aName);
         }
     else
         {
         aName.Insert(0, aSearchPath);
         aName.Insert(0, _L("C:"));
         }
     aName.Append(aFile);
     TEntry entry;
     TBool found = ETrue;
     if(fs.Entry(aName, entry) != KErrNone)
         {
         found = EFalse;
         TDriveList drivelist;
         User::LeaveIfError(fs.DriveList(drivelist)); 
         for(TInt driveNumber = EDriveA;
             driveNumber <= EDriveZ && !found;
             driveNumber++) 
             { 
             if (drivelist[driveNumber]) 
                 {
                 if(fs.Entry(aName, entry)   != KErrNone)
                     {
                     TChar driveLetter; 
                     User::LeaveIfError(
                             fs.DriveToChar(driveNumber,driveLetter));
                     if(driveLetter != aName[0])
                         {
                         aName[0] = driveLetter;
                         if(fs.Entry(aName, entry) == KErrNone)
                             {
                             found = ETrue;
                             }
                         }
                     }
                 }
             }
         }
    return found;
    }
    
 void CSDLAppUi::FlagsFromFileL(const TDesC& aName, TInt& aSdlFlags, TInt& aExeFlags) 
     { 
     RFile file;
     User::LeaveIfError(file.Open(CEikonEnv::Static()->FsSession(), aName, EFileRead));
     
     CleanupClosePushL(file);
   
     RBuf8 buf;
     TInt size; 
     User::LeaveIfError(file.Size(size));
     buf.CreateL(size); 
     CleanupClosePushL(buf);
     
     file.Read(buf);
     
     ParseFlags(buf, aSdlFlags, aExeFlags);

     CleanupStack::PopAndDestroy(2); //f
     }  
		
void CSDLAppUi::ConstructL()
 	{
 	BaseConstructL(ENoAppResourceFile | EAknEnableSkin);
 	SetFullScreenApp(ETrue);
 	
 	iIdle = CIdle::NewL(CActive::EPriorityIdle);
 	
 	iSDLWin = new (ELeave) CSDLWin(*this);
 	
        			
    iStarter = CIdle::NewL(CActive::EPriorityIdle);   
    iStarter->Start(TCallBack(StartL, this));
    
    
 	}
 	
void CSDLAppUi::SdlDraw()
    {
    if(iSdl != NULL)
        iSdl->RedrawRequest();
    }

TBool CSDLAppUi::StartL(TAny* aThis)
	{
	static_cast<CSDLAppUi*>(aThis)->StartL();
	return EFalse;
	}
	
	
void CSDLAppUi::PrepareToExit()
	{
	CAknAppUiBase::PrepareToExit(); //aknappu::PrepareToExit crashes
	iCoeEnv->DeleteResourceFile(iResOffset);
	}

void CSDLAppUi::MakeCCmdLineL(const TDesC8& aCmd, CDesC8Array& aArray)
    {
    const TInt apos = aCmd.LocateReverse('|');
    
    if(apos >= 0 && aCmd[0] == '|')
        {
        const TInt len = apos - 1;
        if(len > 0)
            {
            TInt sdlFlags = gSDLClass.SdlFlags();
            TInt exeFlags = gSDLClass.AppFlags();
            ParseFlags(aCmd.Mid(1, apos - 1), sdlFlags, exeFlags);
            gSDLClass.SetFlags(sdlFlags, exeFlags);
            }
        if(apos > 0)
            ::MakeCCmdLineL(aCmd.Mid(apos + 1), aArray);
        }
    else
        {
        ::MakeCCmdLineL(aCmd, aArray);
        }
    }

TBool CSDLAppUi::ProcessCommandParametersL(CApaCommandLine &aCommandLine)
	{
	const TPtrC8 cmdLine = aCommandLine.TailEnd();
	iParams = new (ELeave) CDesC8ArrayFlat(8);
	
	MakeCCmdLineL(cmdLine, *iParams);
	    
	return EFalse;
	}
 	
 
 TBool CSDLAppUi::ParamEditorL(TDes& aCheat)
	{
#if 0
	 CAknTextQueryDialog* query = CAknTextQueryDialog::NewL(aCheat);
	CleanupStack::PushL(query);
	query->SetPromptL(_L("Enter parameters"));
	CleanupStack::Pop();
	return query->ExecuteLD(R_PARAMEDITOR);
#else
	return false;
#endif
	}
 
 TBool CSDLAppUi::CreateHwaBlitterL(RWindow& aWindow, TInt aFlags)
     {
	 iZoomer = CHWABlitter::NewL(aWindow, aFlags);
	 return ETrue;
     }
 	
 void CSDLAppUi::StartL()	
 	{ 		
 	if(gSDLClass.AppFlags() & SDLEnv::EParamQuery)
 		{
 	     _LIT(paramFile, "sdl_param.txt");
 	     TFileName name;
 	     const TBool found = FindFileL(paramFile, name);
         TBuf8<256> cmd;
 	     if(found)
 	         {
 	         RFile file;
 	         User::LeaveIfError(file.Open(iEikonEnv->FsSession(), name ,EFileRead));
 	         file.Read(cmd);
 	         file.Close();	
 	         MakeCCmdLineL(cmd, *iParams);
 	         }
 		if(!found || gSDLClass.AppFlags() & (SDLEnv::EParamQueryDialog ^ SDLEnv::EParamQuery))
 			{
 			TBuf<256> buffer;
 			if(ParamEditorL(buffer))
 				{
 				cmd.Copy(buffer);
 				MakeCCmdLineL(cmd, *iParams);
 				}	
 			}
 		}
 	
 	iSDLWin->ConstructL(ApplicationRect(), gSDLClass.SdlFlags() & CSDL::EDrawModeGdi);   
 	
 	iSdl = CSDL::NewL(gSDLClass.SdlFlags());
 	        
    iSdl->SetObserver(this);
    
    gSDLClass.SendEvent(MSDLMainObs::ESDLCreated, 0, iSdl);
    
    iSdl->DisableKeyBlocking(*this);
    iSdl->SetContainerWindowL(
                    iSDLWin->GetWindow(), 
                    iEikonEnv->WsSession(),
                    *iEikonEnv->ScreenDevice());
    iSdl->AppendOverlay(iCursor, 0);
    
    if(!(gSDLClass.AppFlags() & SDLEnv::ENoHWABlitter))
        {
        RWindow& win = iSDLWin->GetWindow();
        TInt flags = 0;
        if((gSDLClass.AppFlags() & SDLEnv::EHWABlitterNoScale) == SDLEnv::EHWABlitterNoScale)
            flags = CHWABlitter::EPreventScale;
        if((gSDLClass.AppFlags() & SDLEnv::EHWABlitterRatioScale) == SDLEnv::EHWABlitterRatioScale)
            flags = CHWABlitter::EPreserveRatio;
        
        if(CreateHwaBlitterL(win, flags))
            {
            iSdl->SetBlitter(iZoomer);
            }
       // ASSERT(!gSDLClass.AppFlags() & SDLEnv::EFastZoomBlitter);
        }
    
    if(gSDLClass.AppFlags() & SDLEnv::EHWABlitter && iZoomer == NULL)
        {
        User::Leave(KErrNotFound);
        }
    
    if(iZoomer == NULL && gSDLClass.AppFlags() & SDLEnv::EFastZoomBlitter)
        {
        iZoomer = CZoomer::NewL();
        iSdl->SetBlitter(iZoomer);
        }
 	        
 	iWait = new (ELeave) CExitWait(*this);
 	iSdl->CallMainL(gSDLClass.Main(), &iWait->iStatus, iParams, CSDL::ENoParamFlags, 0xA000);
 	}
 	

 
void CSDLAppUi::HandleCommandL(TInt aCommand)
	{
	switch(aCommand)
		{
		case EAknSoftkeyBack:
 		case EAknSoftkeyExit:
		case EAknCmdExit:
		case EEikCmdExit:
			gSDLClass.AppFlags(SDLEnv::EAllowConsoleView); 
		    if(iWait == NULL || !iWait->IsActive() || iSdl == NULL)
		    	{
		    	Exit();
		    	}	
			  else if(!iExitRequest)
			  	{
			  	TWsEvent event;
			  	event.SetType(EEventSwitchOff),
				event.SetTimeNow();
			  	iSdl->AppendWsEvent(event);
			  //	User::After(1000000);
			  	iExitRequest = ETrue; //trick how SDL can be closed!
			  //	iSdl->Suspend();
			  	} 
			break;
		}
	}
	

	
TBool CSDLAppUi::HandleKeyL(const TWsEvent& aEvent)
	{
	const TInt type = aEvent.Type();
	if(!(type == EEventKey || type == EEventKeyUp || type == EEventKeyDown))
 			{
 			return ETrue;
 			}
 	const TKeyEvent& key = *aEvent.Key();
	if((key.iScanCode == EStdKeyYes) && (gSDLClass.AppFlags() & SDLEnv::EVirtualMouse))
 		{
 		if(type == EEventKeyUp)
 			{
 			iCursor.ToggleOn();
 			SdlDraw();	
 			}
 		return EFalse;
		}
	if(iCursor.IsOn())
		{
		switch(key.iScanCode)
			{
			case EStdKeyUpArrow:
				iCursor.Move(0, -1);
				break;
			case EStdKeyDownArrow:
				iCursor.Move(0, 1);
				break;
			case EStdKeyLeftArrow:
				iCursor.Move(-1, 0);
				break;
			case EStdKeyRightArrow:
				iCursor.Move(1, 0);
				break; 
			case EStdKeyDevice3:
				if(type == EEventKeyUp && iSdl != NULL)
					{
					TWsEvent event;
					TPointerEvent::TType type = iCursor.IsMove() ? 
					    TPointerEvent::EButton1Up : TPointerEvent::EButton1Down;
					                   
					iCursor.MakeEvent(type, event, iSDLWin->Position());
					iSdl->AppendWsEvent(event);
					if((gSDLClass.AppFlags() & SDLEnv::EEnableVirtualMouseMoveEvents) == SDLEnv::EEnableVirtualMouseMoveEvents)
					    {
					    iCursor.ToggleMove();
					    CFbsBitmap* bmp = iCursor.IsMove()? iCBmpMove : iCBmp;
					    iCursor.Set(iSDLWin->Rect(), bmp, iAlpha);
	                    SdlDraw();  
					    }
					else
					    {
					    iCursor.MakeEvent(TPointerEvent::EButton1Up, event, iSDLWin->Position());
					    iSdl->AppendWsEvent(event);
					    }
					}
				return EFalse;
			default:
				return ETrue;
			}
		if(iCursor.IsMove())
		    {
		    TWsEvent event;
		    iCursor.MakeEvent(TPointerEvent::EMove, event, iSDLWin->Position());
		    iSdl->AppendWsEvent(event);
		    }
		SdlDraw();	
		return EFalse;
		}
	return ETrue;
	}
 		
 void CSDLAppUi::HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination)
 	{
 	if(iSdl && iWait && HandleKeyL(aEvent))
 	    {
 		iSdl->AppendWsEvent(aEvent);
 	    }
 	CAknAppUi::HandleWsEventL(aEvent, aDestination);
 	}
 	
 void CSDLAppUi::HandleResourceChangeL(TInt aType)
 	{
    CAknAppUi::HandleResourceChangeL(aType);
    if(iSdl != NULL && aType == KEikDynamicLayoutVariantSwitch)
        {  	
        iSDLWin->SetRect(ApplicationRect());
      	iSdl->SetContainerWindowL(
      				iSDLWin->GetWindow(),
        			iEikonEnv->WsSession(),
        			*iEikonEnv->ScreenDevice());
      	iCursor.Set(iSDLWin->Rect(), iCBmp, iAlpha);
        }
 	}
 	
 	
void CSDLAppUi::DoExit(TInt/*Err*/)
   	{
   	iExitRequest = ETrue;
   	CExitWait* w = iWait;
   	if(iWait != NULL)
   	    iWait->Cancel();
   	iWait = NULL;
   	Exit();
   	delete w;
   	}

    
 TInt CSDLAppUi::SdlThreadEvent(TInt aEvent, TInt /*aParam*/)    
	{
	return KErrNone; //MSDLObserver::EParameterNone;	
	}
	    
TInt CSDLAppUi::SdlEvent(TInt aEvent, TInt /*aParam*/)    
	{
	switch(aEvent)
		{
		case MSDLObserver::EEventResume:
		    iSdl->RedrawRequest();
			break;
		case MSDLObserver::EEventSuspend:
		    if(iZoomer != NULL)
		        iZoomer->Release();
			//if(iExitRequest)
			//	return MSDLObserver::ESuspendNoSuspend;
			break;
		case MSDLObserver::EEventWindowReserved:
			break;
		case MSDLObserver::EEventWindowNotAvailable:
			{
			TRAP_IGNORE(HandleConsoleWindowL());
			}
			break;
		case MSDLObserver::EEventScreenSizeChanged:
     		break;
		case MSDLObserver::EEventKeyMapInit:
			break;
		case MSDLObserver::EEventMainExit:
			if(iStdOut != 0)
				{
				gSDLClass.AppFlags(SDLEnv::EAllowConsoleView); 
				iEikonEnv->WsSession().SetWindowGroupOrdinalPosition(iStdOut, 0);
				}
			break;
		}
	return KErrNone; //MSDLObserver::EParameterNone;
	}
	
void CSDLAppUi::HandleForegroundEventL(TBool aForeground)
	{
	CAknAppUi::HandleForegroundEventL(aForeground);	
	if(!aForeground)
		HandleConsoleWindow();
	}
	
void CSDLAppUi::HandleConsoleWindow()
	{
	if(!iIdle->IsActive())
		iIdle->Start(TCallBack(IdleRequestL, this));
	}
	
TBool CSDLAppUi::IdleRequestL(TAny* aThis)
	{
	static_cast<CSDLAppUi*>(aThis)->HandleConsoleWindowL();
	return EFalse;
	}

void CSDLAppUi::HandleConsoleWindowL()
	{
	if(gSDLClass.AppFlags() & SDLEnv::EAllowConsoleView)
		{
		return;
		}
	RWsSession& ses = iEikonEnv->WsSession();
	const TInt focus = ses.GetFocusWindowGroup();
	CApaWindowGroupName* name = CApaWindowGroupName::NewLC(ses, focus);
	const TPtrC caption = name->Caption();
	if(0 == caption.CompareF(_L("STDOUT")))
		{
		iStdOut = focus;
		ses.SetWindowGroupOrdinalPosition(iEikonEnv->RootWin().Identifier(), 0);
		}
	CleanupStack::PopAndDestroy(); //name
	}
	
    	
	
	
////////////////////////////////////////////////////////////////////////


CApaApplication* NewApplication()
    {
    return new CSDLApplication();
    }

	
EXPORT_C TInt SDLEnv::SetMain(const TMainFunc& aFunc, TInt aSdlFlags, MSDLMainObs* aObs, TInt aSdlExeFlags)
	{
	gSDLClass.SetMain(aFunc, aObs);
	gSDLClass.SetFlags(aSdlFlags, aSdlExeFlags);
	return EikStart::RunApplication(NewApplication);
	}	
	
//////////////////////////////////////////////////////////////////////

TInt SDLUiPrint(const TDesC8& /*aInfo*/)
    {
    return KErrNotFound;
    }    



