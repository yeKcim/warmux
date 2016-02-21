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

#ifndef STREAMPLAYER_H
#define STREAMPLAYER_H

#include<MdaAudioOutputStream.h>

const TInt KSilenceBuffer = 256;

class MStreamObs
    {
    public:
    	enum 
    	{
    	EInit, 
    	EPlay,
    	EWrite,
    	EClose,	
    	};
        virtual void Complete(TInt aState, TInt aError) = 0;
    };

class MStreamProvider
	{
	public:
		virtual TPtrC8 Data() = 0;	
	};

NONSHARABLE_CLASS(CStreamPlayer) : public CBase, public MMdaAudioOutputStreamCallback
	{
	public:
		CStreamPlayer(MStreamProvider& aProvider, MStreamObs& aObs);
		~CStreamPlayer();
		void ConstructL();
		
		static TInt ClosestSupportedRate(TInt aRate);
		
		TInt OpenStream(TInt aRate, TInt aChannels, TUint32 aType = KMMFFourCCCodePCM16);
		
		void SetVolume(TInt aNew);
		TInt Volume() const;
		TInt MaxVolume() const;
		
		void Stop();
		void Start();
		void Open();
		void Close();
		
		TBool Playing() const;
		TBool Closed() const;
		TBool Active() const;
		
	private:

		void MaoscOpenComplete(TInt aError) ;
		void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
		void MaoscPlayComplete(TInt aError);
	
	private:
		void Request();
		void SetCapsL();

	private:
		MStreamProvider& iProvider;
		MStreamObs& iObs;	
		TInt iVolume;
	
		CMdaAudioOutputStream* iStream;
	
		TInt iRate;
		TInt iChannels;
		TUint32 iType;
		
		enum 
			{
				ENone = 0,
				EInited = 0x1,
				EStarted = 0x2,
				EStopped = 0x4,
				EVolumeChange = 0x8,
				EDied		  = 0x10,
			};
		
		TInt iState;
		TBuf8<KSilenceBuffer> iSilence;
		TPtrC8 iPtr;
	
	};


#endif

