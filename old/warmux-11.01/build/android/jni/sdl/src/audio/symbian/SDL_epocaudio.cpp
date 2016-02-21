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
    SDL_epocaudio.cpp
    Epoc based SDL audio driver implementation
    
    Markus Mertama
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id: SDL_epocaudio.c,v 0.0.0.0 2001/06/19 17:19:56 hercules Exp $";
#endif


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include "epoc_sdl.h"

#include <e32hal.h>

#include <remconinterfaceselector.h>
#include <remconcoreapitarget.h> 
#include <remconcoreapitargetobserver.h>


extern "C" {
#include "SDL_audio.h"
#include "SDL_error.h"
#include "SDL_audiomem.h"
#include "SDL_audio_c.h"
#include "SDL_timer.h"
#include "SDL_audiodev_c.h"
#include "SDL_sysaudio.h"
}


#include "streamplayer.h"

#ifdef DEBUG_AUDIO
#define TRACE(x) trace(x, 0)
#define TRACE1(x, y) trace(x, y)
FILE* f;
void trace(char* aa, int d)
    {
    if(f == NULL)
        {
        f = fopen("e:\\audio_db.txt", "w");
        }
    fprintf(f, aa, d);
    fflush(f);
    }
#else
#define TRACE(x)
#define TRACE1(x, y)
#endif

/* Audio driver functions */

static int OpenAudio(SDL_AudioDevice *aThisdevice, SDL_AudioSpec *aSpec);
static void WaitAudio(SDL_AudioDevice *aThisdevice);
static void PlayAudio(SDL_AudioDevice *aThisdevice);
static Uint8* GetAudioBuf(SDL_AudioDevice *aThisdevice);
static void CloseAudio(SDL_AudioDevice *aThisdevice);
static void ThreadInit(SDL_AudioDevice *aThisdevice);

static int AudioAvailable();
static SDL_AudioDevice* AudioCreateDevice(int aDevindex);
static void AudioDeleteDevice(SDL_AudioDevice *aDevice);

    
#ifdef __WINS__
//#define DODUMP
#endif

#ifdef DODUMP
NONSHARABLE_CLASS(TDump)
    {
    public:
    TInt Open();
    void Close();
    void Dump(const TDesC8& aDes);
    private:
        RFile iFile;
        RFs iFs; 
    };
    
TInt TDump::Open()
    {
    TInt err = iFs.Connect();
    if(err == KErrNone)
        {
#ifdef __WINS__
_LIT(target, "C:\\sdlau.raw");
#else
_LIT(target, "E:\\sdlau.raw");
#endif 
        err = iFile.Replace(iFs, target, EFileWrite);
        }
    return err;
    }
void TDump::Close()
    {
    iFile.Close();
    iFs.Close();
    }
void TDump::Dump(const TDesC8& aDes)
    {
    iFile.Write(aDes);
    }
#endif

const TInt KMaxVolumeId = 0x12345;

LOCAL_C void ReadValueL(TInt aId, TInt& aValue)
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    RFile file;
    User::LeaveIfError(file.Open(fs, _L("sdl.dat"), EFileRead));
    
    CleanupClosePushL(file);
    TPckgBuf<TInt> count;
    file.Read(count);
    for(TInt i = 0 ; i < count(); i++)
            {
            TPckgBuf<TInt> tidb;
            file.Read(tidb);
            TPckgBuf<TInt> valueb;
            file.Read(valueb);
            TInt tid = tidb();
            TInt tvalue = valueb();
            if(aId == tid)
                {
                aValue = tvalue;
                break;
                }
            }
    CleanupStack::PopAndDestroy(2);
    }

LOCAL_C void StoreValueL(TInt aId, TInt aValue)
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    RFile file;
    const TInt err = file.Open(fs, _L("sdl.dat"), EFileRead);
    RArray<TInt> values;
    CleanupClosePushL(values);
    TBool found = EFalse;
    if(err == KErrNone)
        {
        CleanupClosePushL(file);
        TPckgBuf<TInt> count;
        file.Read(count);
        for(TInt i = 0 ; i < count(); i++)
            {
            TPckgBuf<TInt> tidb;
            file.Read(tidb);
            TPckgBuf<TInt> valueb;
            file.Read(valueb);
            TInt tid = tidb();
            TInt tvalue = valueb();
            if(aId == tid)
                {
                tvalue = aValue;
                found = ETrue;
                }
            User::LeaveIfError(values.Append(tid));
            User::LeaveIfError(values.Append(tvalue));
            }
        CleanupStack::PopAndDestroy();
        }
    if(!found)
        {
        User::LeaveIfError(values.Append(aId));
        User::LeaveIfError(values.Append(aValue));
        }
    User::LeaveIfError(file.Replace(fs, _L("sdl.dat"), EFileWrite));
    CleanupClosePushL(file);
    TPckgC<TInt> size(values.Count() / 2);
    file.Write(size);
    for(TInt i = 0; i < values.Count(); i++)
        {
        TPckgC<TInt> value(values[i]);
        file.Write(value);
        }
    CleanupStack::PopAndDestroy(3);
    }

NONSHARABLE_CLASS(MVolumeChange)
    {
public:
    virtual void ChangeVolume(TInt aSteps) = 0;
    };
    
NONSHARABLE_CLASS(CVolumeKeys) : public CBase, public MRemConCoreApiTargetObserver
    {
public:
    CVolumeKeys(MVolumeChange& aChange);
    ~CVolumeKeys();
    void CaptureL();
private:
    void MrccatoCommand(TRemConCoreApiOperationId aOperationId, TRemConCoreApiButtonAction aButtonAct);
private:
    MVolumeChange& iVol;
    CRemConInterfaceSelector* iSelector;
    };

CVolumeKeys::CVolumeKeys(MVolumeChange& aVol) : iVol(aVol)
    {
    }

void CVolumeKeys::CaptureL()
    {
    iSelector = CRemConInterfaceSelector::NewL();
    CRemConCoreApiTarget* target = CRemConCoreApiTarget::NewL(*iSelector, *this );
    iSelector->OpenTargetL();
    }



CVolumeKeys::~CVolumeKeys()
    {
    delete iSelector;
    }

void CVolumeKeys::MrccatoCommand(TRemConCoreApiOperationId aOperationId, TRemConCoreApiButtonAction aButtonAct)
    {
    if(ERemConCoreApiButtonClick != aButtonAct)
        return;
    switch(aOperationId)
        {
        case ERemConCoreApiVolumeDown:
            iVol.ChangeVolume(-1);
            break;
        case ERemConCoreApiVolumeUp:
            iVol.ChangeVolume(+1);
            break;
        }
    }

NONSHARABLE_CLASS(CWait) : public CActive
    {
    public:
        void Wait(TBool aLongWait);
        static CWait* NewL();
        ~CWait();
    private:
        CWait();
        void RunL();
        void ConstructL();
        void DoCancel();
    private:
        RTimer iTimer;
    };


CWait* CWait::NewL()
    {
    CWait* wait = new (ELeave) CWait();
    CleanupStack::PushL(wait);
    wait->ConstructL();
    CleanupStack::Pop();
    return wait;
    }

void CWait::ConstructL()
    {
    User::LeaveIfError(iTimer.CreateLocal());
    }

void CWait::DoCancel()
    {
    TRACE("wait cancel\n");
    iTimer.Cancel();
    CActiveScheduler::Stop();
    }
     
CWait::~CWait()
    {
    iTimer.Close();
    }

void CWait::Wait(TBool aLongWait)
    {
    SetActive();
    const TInt waitTime = aLongWait ? 3000000 : 500000; 
    iTimer.After(iStatus, waitTime);
    CActiveScheduler::Start();
    }
    
CWait::CWait() : CActive(CActive::EPriorityLow) 
    {
    CActiveScheduler::Add(this);
    }

void CWait::RunL()
    {
    TRACE("wait elapsed\n");
    CActiveScheduler::Stop();
    }


NONSHARABLE_CLASS(CEpocAudio) : public CBase, public MStreamObs, public MStreamProvider, public MVolumeChange
    {
    public:
        static void* NewL(TInt BufferSize, TInt aFill);
        inline static CEpocAudio& Current(SDL_AudioDevice* thisdevice);
        static void Free(SDL_AudioDevice* thisdevice);
        void Wait();
        void Play();
        void ThreadInitL(TAny* aDevice);
        void Open(TInt iRate, TInt iChannels, TUint32 aType, TInt aBytes);
        ~CEpocAudio();
        TUint8* Buffer();
        void SetPause(TBool aPause);
        TBool IsPaused() const;
        void ChangeVolume(TInt aVolume);
    #ifdef DODUMP
        void Dump(const TDesC8& aBuf) {iDump.Dump(aBuf);}
    #endif
    private:
        CEpocAudio(TInt aBufferSize);
        void Complete(TInt aState, TInt aError);
        TPtrC8 Data();
        void ConstructL(TInt aFill);
    private:
        TInt iBufferSize;
        CStreamPlayer* iPlayer;
        TInt iBufferRate;
        TInt iRate;
        TInt iChannels;
        TUint32 iType;
        TThreadId iTid;
     //   TBool iPaused;
        TUint8* iBuffer;
      //  TTime iStart;
       // TInt iTune;
        CWait* iWait;
      //  TBool iPending;
        CVolumeKeys* iKeys;
    #ifdef DODUMP
        TDump iDump;
    #endif
    };


inline CEpocAudio& CEpocAudio::Current(SDL_AudioDevice* thisdevice)
    {
    return *static_cast<CEpocAudio*>((void*)thisdevice->hidden);
    }
    
    
void CEpocAudio::Free(SDL_AudioDevice* thisdevice)
    {
    CEpocAudio* ea = static_cast<CEpocAudio*>((void*)thisdevice->hidden);
    if(ea)
        {
        ASSERT(ea->iTid == RThread().Id());
        delete ea;
        thisdevice->hidden = NULL;  
   
        CActiveScheduler* as =  CActiveScheduler::Current();
        ASSERT(as->StackDepth() == 0);      
        delete as;
        CActiveScheduler::Install(NULL);
        }
    ASSERT(thisdevice->hidden == NULL);
    }
    
CEpocAudio::CEpocAudio(TInt aBufferSize) : iBufferSize(aBufferSize)/*, iPaused(ETrue)*/ 
    {
    }

void* CEpocAudio::NewL(TInt aBufferSize, TInt aFill)
    {
    CEpocAudio* eAudioLib = new (ELeave) CEpocAudio(aBufferSize);
    CleanupStack::PushL(eAudioLib);
    eAudioLib->ConstructL(aFill);
    CleanupStack::Pop();
    return eAudioLib;
    }
    
void CEpocAudio::ConstructL(TInt aFill)
    {
  /*  for(TInt i = 0; i < KBufferCount; i++)
        {
        iBuffers[i] = (TUint8*) User::AllocL(iBufferSize);
        memset(iBuffers[i], aFill,iBufferSize);
        }
    iFreeBuffer = 0;
    iReservedBuffer = 0;*/
    
    iBuffer = (TUint8*) User::AllocL(iBufferSize);
    memset(iBuffer, aFill,iBufferSize);
    
    }

TBool CEpocAudio::IsPaused() const
    {
    return !iPlayer->Playing();
    }

void CEpocAudio::SetPause(TBool aPause)
    {
    if(iPlayer != NULL && aPause && iPlayer->Playing())
        iPlayer->Stop();
    if(iPlayer != NULL && !aPause && !iPlayer->Playing())
        iPlayer->Start();
    }

void CEpocAudio::ChangeVolume(TInt aStepDelta)
    {
    TInt volume = iPlayer->Volume(); 
    volume += (iPlayer->MaxVolume() / 10) * aStepDelta;
    iPlayer->SetVolume(volume);
    ASSERT(iPlayer->MaxVolume() == 256);
    EpocSdlEnv::ObserverEvent(MSDLObserver::EEventVolumeChange , volume);
    }

void CEpocAudio::ThreadInitL(TAny* aDevice)
    {
    iTid = RThread().Id(); 
    CActiveScheduler* as =  new (ELeave) CActiveScheduler();
    CActiveScheduler::Install(as);
    
    EpocSdlEnv::AppendCleanupItem(TSdlCleanupItem((TSdlCleanupOperation)CloseAudio, aDevice));
   
    iWait = CWait::NewL();
    
    TInt vol = TInt(TReal(iPlayer->MaxVolume()) * 0.5); //Initial mastervolume
    
    TRAP_IGNORE(ReadValueL(KMaxVolumeId, vol));
    
    iPlayer = new (ELeave) CStreamPlayer(*this, *this);
    iPlayer->ConstructL();  
    iPlayer->OpenStream(iRate, iChannels, iType);
    iPlayer->SetVolume(vol);
    
    if(!EpocSdlEnv::Flags(CSDL::EDisableVolumeKeys))
        {
        iKeys = new CVolumeKeys(*this);
        iKeys->CaptureL();
        }
    
    #ifdef DODUMP
    User::LeaveIfError(iDump.Open());
    #endif
    }
    
    
    
TUint8* CEpocAudio::Buffer()
    {
    /*swap buffers*/
   /* TUint8* tmp = iReadBuffer;
    iReadBuffer = iWriteBuffer;
    iWriteBuffer = tmp;
    iPending = ETrue; */
    
   /* TUint8* buf = iBuffers[iFreeBuffer];
    
    ++iFreeBuffer;
    if(iFreeBuffer >= KBufferCount)
        iFreeBuffer = 0;
    
    return buf;*/
    return iBuffer;
    }
    
CEpocAudio::~CEpocAudio()
    {
    if(iPlayer != NULL)
        {
        TRAP_IGNORE(StoreValueL(KMaxVolumeId, iPlayer->Volume()));
        }
    delete iKeys;
    if(iWait != NULL)
        iWait->Cancel();
    delete iWait; 
    if(iPlayer != NULL)
        iPlayer->Close();
    delete iPlayer;
  //  for(TInt i = 0; i < KBufferCount; i++)
  //      User::Free(iBuffers[i]);
    User::Free(iBuffer);
    }
    
void CEpocAudio::Complete(TInt aState, TInt aError)
    {
    if(aError == KErrNone)
        {
        if(aState == MStreamObs::EPlay) 
            {
        /*    ++iReservedBuffer;
            if(iReservedBuffer >= KBufferCount)
                iReservedBuffer = 0; */
            if(iWait->IsActive())
                iWait->Cancel();
            }
        }
    else
        {
        TRACE1("Complete %d", aState);
        TRACE1(", Error %d\n", aError);
        if(iPlayer->Closed())
            return;
        switch(aError)
            {
            case KErrUnderflow:
            case KErrInUse:
                iPlayer->Start();
                break;
            case KErrAbort:
                iPlayer->Open();
            }
        }
    }
    
    
    
void CEpocAudio::Play()
    {
    SetPause(EFalse);
    //iPaused = EFalse;
    }

void CEpocAudio::Wait()
    {
    const TBool playing = iPlayer->Playing();
    TRACE1("do wait, paused: %d\n", playing);
    /*if(IsPaused())
        {
        User::After(100000);
        }
    else
        {*/
   // if(!playing || iFreeBuffer == iReservedBuffer)
        iWait->Wait(playing); 
        //}   
    }
    
void CEpocAudio::Open(TInt aRate, TInt aChannels, TUint32 aType, TInt aBytes)   
    {
    iRate = aRate;
    iChannels = aChannels;
    iType = aType;
    iBufferRate = iRate * iChannels * aBytes; //1/x
    }

TPtrC8 CEpocAudio::Data()
    {  
    TRACE("read data\n");
    /*
      if(iWait->IsActive())
        {
        iWait->Cancel();
        }
    */
 //   if(iPaused || !iPending)
 //       return KNullDesC8();
    
  //  iPending = EFalse;
  /*  for(int i =0 ; i < iBufferSize; i++)
        {
        if(iBuffer[i] != 0)
            {
            RDebug::Print(_L("data!"));
            break;
            }
        }
    */
    TPtrC8 data(/*iReadBuffer*/iBuffer, iBufferSize);
      
#ifdef DODUMP
    iDump.Dump(data);
#endif
    
    return data;
    }
 

/* Audio driver bootstrap functions */

AudioBootStrap EPOCAudio_bootstrap = {
    "epoc\0\0\0",
    "EPOC streaming audio\0\0\0",
    AudioAvailable,
    AudioCreateDevice
};


static SDL_AudioDevice *AudioCreateDevice(int /*aDevindex*/)
{
    SDL_AudioDevice *thisdevice;

    /* Initialize all variables that we clean on shutdown */
    thisdevice = (SDL_AudioDevice *)malloc(sizeof(SDL_AudioDevice));
    if ( thisdevice ) {
        memset(thisdevice, 0, (sizeof *thisdevice));
        thisdevice->hidden = NULL; 
    }
    if ( (thisdevice == NULL) ) {
        SDL_OutOfMemory();
        if ( thisdevice ) {
            free(thisdevice);
        }
        return(0);
    }


    /* Set the function pointers */
    thisdevice->OpenAudio = OpenAudio;
    thisdevice->WaitAudio = WaitAudio;
    thisdevice->PlayAudio = PlayAudio;
    thisdevice->GetAudioBuf = GetAudioBuf;
    thisdevice->CloseAudio = CloseAudio;
    thisdevice->ThreadInit = ThreadInit;
    thisdevice->free = AudioDeleteDevice;

    return thisdevice;
}


static void AudioDeleteDevice(SDL_AudioDevice *aDevice)
    {
    //free(device->hidden);
    free(aDevice);
    }

static int AudioAvailable()
    {
    return(1); // Audio stream modules should be always there!
    }


static int OpenAudio(SDL_AudioDevice *aThisdevice, SDL_AudioSpec *aSpec)
    {
    SDL_TRACE("SDL:EPOC_OpenAudio");

    SDL_CalculateAudioSpec(aSpec);
    
    TUint32 type = KMMFFourCCCodePCM16;
    TInt bytes = 2;
    
    switch(aSpec->format)
        {
        case AUDIO_U16LSB: 
            type = KMMFFourCCCodePCMU16; 
            break;
        case AUDIO_S16LSB: 
            type = KMMFFourCCCodePCM16; 
            break;
        case AUDIO_U16MSB: 
            type = KMMFFourCCCodePCMU16B; 
            break;
        case AUDIO_S16MSB: 
            type = KMMFFourCCCodePCM16B; 
            break; 
            //8 bit not supported!
        case AUDIO_U8: 
        case AUDIO_S8:
        default:
            aSpec->format = AUDIO_S16LSB;
        };
    

    
    if(aSpec->channels > 2)
        aSpec->channels = 2;
    
    aSpec->freq = CStreamPlayer::ClosestSupportedRate(aSpec->freq);
    

    /* Allocate mixing buffer */
    const TInt buflen = aSpec->size;
    
    TRAPD(err, aThisdevice->hidden = static_cast<SDL_PrivateAudioData*>(CEpocAudio::NewL(buflen, aSpec->silence)));
    if(err != KErrNone)
        return -1;

    CEpocAudio::Current(aThisdevice).Open(aSpec->freq, aSpec->channels, type, bytes);
    
    CEpocAudio::Current(aThisdevice).SetPause(ETrue);
    

    aThisdevice->enabled = 0; /* enable only after audio engine has been initialized!*/

    /* We're ready to rock and roll. :-) */
    return(0);
}


static void CloseAudio(SDL_AudioDevice* aThisdevice)
    {
#ifdef DEBUG_AUDIO
    TRACE("Close audio\n");
#endif

    CEpocAudio::Free(aThisdevice);
    }


static void ThreadInit(SDL_AudioDevice *aThisdevice)
    {
    TRACE("SDL:EPOC_ThreadInit\n");
    CEpocAudio::Current(aThisdevice).ThreadInitL(aThisdevice);
    RThread().SetPriority(EPriorityRealTime);
    aThisdevice->enabled = 1;
    }

/* This function waits until it is possible to write a full sound buffer */
static void WaitAudio(SDL_AudioDevice* aThisdevice)
    {
#ifdef DEBUG_AUDIO
      TInt tics = User::TickCount();
#endif

    CEpocAudio::Current(aThisdevice).Wait();

#ifdef DEBUG_AUDIO
    TInt ntics =  User::TickCount() - tics;
    TRACE1("audio waited %d\n", ntics);
    TRACE1("audio at %d\n", tics);
#endif
    }


 
static void PlayAudio(SDL_AudioDevice* aThisdevice)
    {
    const TInt status = SDL_GetAudioStatus();
    if(status == SDL_AUDIO_PAUSED)
        {
        CEpocAudio::Current(aThisdevice).SetPause(ETrue);
        SDL_Delay(500); //hold on the busy loop
        }
    else
        CEpocAudio::Current(aThisdevice).Play();
    
#ifdef DEBUG_AUDIO
    if(status  == SDL_AUDIO_PAUSED)
        TRACE("audio paused\n");
    else
        TRACE("audio play\n");
#endif

    

    }

static Uint8* GetAudioBuf(SDL_AudioDevice* aThisdevice)
    {
    return CEpocAudio::Current(aThisdevice).Buffer();
    }

#if ADUMP
void sos_adump(SDL_AudioDevice* thisdevice, void* data, int len)
    {
#ifdef DODUMP
    const TPtrC8 buf((TUint8*)data, len);
    CEpocAudio::Current(thisdevice).Dump(buf);
#endif
    }
#endif 




