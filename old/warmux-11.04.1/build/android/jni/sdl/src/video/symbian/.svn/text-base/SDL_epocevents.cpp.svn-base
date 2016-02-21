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

Markus Mertama

*/

#include "epoc_sdl.h"

#include <stdio.h>
#undef NULL
extern "C" {
#include "SDL_error.h"
#include "SDL_video.h"
#include "SDL_keysym.h"
#include "SDL_keyboard.h"
#include "SDL_events_c.h"
#include "SDL_timer.h"
} /* extern "C" */

#include "SDL_epocvideo.h"
#include "SDL_epocevents_c.h"

#include "sdlepocapi.h"
#include "sdlenvdata.h"

#include <eikenv.h>

#include<bautils.h>


extern "C"
	{
	static SDL_keysym *TranslateKey(_THIS, int scancode, unsigned int icode, SDL_keysym *keysym);
	void SDL_PauseAudio(int);
	int SDL_PrivateResize(int, int);
	}

//extern "C" {
/* The translation tables from a console scancode to a SDL keysym */
static SDLKey keymap[MAX_SCANCODE];
static SDL_keysym *TranslateKey(_THIS, int scancode, unsigned int icode, SDL_keysym *keysym);
void DisableKeyBlocking(_THIS);

//} /* extern "C" */

SDLKey* KeyMap()
	{
	return keymap;
	}
	
TBool isCursorVisible = EFalse;

void ResetKeyMap()
	{
	unsigned int i;

	/* Initialize the key translation table */
	for ( i=0; i<SDL_TABLESIZE(keymap); ++i )
		keymap[i] = SDLK_UNKNOWN;


	/* Numbers */
	for ( i = 0; i<32; ++i ){
		keymap[' ' + i] = (SDLKey)(SDLK_SPACE+i);
	}
	/* e.g. Alphabet keys */
	for ( i = 0; i<32; ++i ){
		keymap['A' + i] = (SDLKey)(SDLK_a+i);
	}

	keymap[EStdKeySpace]        = SDLK_SPACE;
	keymap[EStdKeyBackspace]    = SDLK_BACKSPACE;
	keymap[EStdKeyTab]          = SDLK_TAB;
	keymap[EStdKeyEnter]        = SDLK_RETURN;
	keymap[EStdKeyEscape]       = SDLK_ESCAPE;
   	keymap[EStdKeySpace]        = SDLK_SPACE;
   	keymap[EStdKeyPause]        = SDLK_PAUSE;
   	keymap[EStdKeyHome]         = SDLK_HOME;
   	keymap[EStdKeyEnd]          = SDLK_END;
   	keymap[EStdKeyPageUp]       = SDLK_PAGEUP;
   	keymap[EStdKeyPageDown]     = SDLK_PAGEDOWN;
	keymap[EStdKeyDelete]       = SDLK_DELETE;
	keymap[EStdKeyUpArrow]      = SDLK_UP;
	keymap[EStdKeyDownArrow]    = SDLK_DOWN;
	keymap[EStdKeyLeftArrow]    = SDLK_LEFT;
	keymap[EStdKeyRightArrow]   = SDLK_RIGHT;
	keymap[EStdKeyCapsLock]     = SDLK_CAPSLOCK;
	keymap[EStdKeyLeftShift]    = SDLK_LSHIFT;
	keymap[EStdKeyRightShift]   = SDLK_RSHIFT;
	keymap[EStdKeyLeftAlt]      = SDLK_LALT;
	keymap[EStdKeyRightAlt]     = SDLK_RALT;
	keymap[EStdKeyLeftCtrl]     = SDLK_LCTRL;
	keymap[EStdKeyRightCtrl]    = SDLK_RCTRL;
	keymap[EStdKeyLeftFunc]     = SDLK_LMETA;
	keymap[EStdKeyRightFunc]    = SDLK_RMETA;
	keymap[EStdKeyInsert]       = SDLK_INSERT;
	keymap[EStdKeyComma]        = SDLK_PERIOD;//SDLK_COMMA;
	keymap[EStdKeyFullStop]     = SDLK_PERIOD;
	keymap[EStdKeyForwardSlash] = SDLK_SLASH;
	keymap[EStdKeyBackSlash]    = SDLK_BACKSLASH;
	keymap[EStdKeySemiColon]    = SDLK_SEMICOLON;
	keymap[EStdKeySingleQuote]  = SDLK_QUOTE;
	keymap[EStdKeyHash]         = SDLK_HASH;
	keymap[EStdKeySquareBracketLeft]    = SDLK_LEFTBRACKET;
	keymap[EStdKeySquareBracketRight]   = SDLK_RIGHTBRACKET;
	keymap[EStdKeyMinus]        = SDLK_MINUS;
	keymap[EStdKeyEquals]       = SDLK_EQUALS;

   	keymap[EStdKeyF1]          = SDLK_F1;  
   	keymap[EStdKeyF2]          = SDLK_F2;  
   	keymap[EStdKeyF3]          = SDLK_F3;  
   	keymap[EStdKeyF4]          = SDLK_F4; 
   	keymap[EStdKeyF5]          = SDLK_F5;  
   	keymap[EStdKeyF6]          = SDLK_F6;  
   	keymap[EStdKeyF7]          = SDLK_F7;  
   	keymap[EStdKeyF8]          = SDLK_F8;  

   	keymap[EStdKeyF9]          = SDLK_F9;  
   	keymap[EStdKeyF10]         = SDLK_F10; 
   	keymap[EStdKeyF11]         = SDLK_F11; 
   	keymap[EStdKeyF12]         = SDLK_F12; 


   	keymap[EStdKeyXXX]         = SDLK_RETURN;	/* "fire" key */

   	keymap[EStdKeyDevice3]     = SDLK_RETURN;	/* "fire" key */
   	keymap[EStdKeyNkpAsterisk] = SDLK_ASTERISK; 
   	keymap[EStdKeyYes]         = SDLK_HOME;		/* "call" key */
   	keymap[EStdKeyNo]		   = SDLK_END;		/* "end call" key */
   	keymap[EStdKeyDevice0]     = SDLK_SPACE;	/* right menu key */
   	keymap[EStdKeyDevice1]     = SDLK_ESCAPE;	/* left menu key */
   	keymap[EStdKeyDevice2]     = SDLK_POWER;	/* power key */

    keymap[EStdKeyMenu]        = SDLK_MENU;   	// menu key
    keymap[EStdKeyDevice6]     = SDLK_LEFT;     // Rocker (joystick) left
    keymap[EStdKeyDevice7]     = SDLK_RIGHT;    // Rocker (joystick) right
    keymap[EStdKeyDevice8]     = SDLK_UP;       // Rocker (joystick) up
    keymap[EStdKeyDevice9]     = SDLK_DOWN;     // Rocker (joystick) down
    keymap[EStdKeyLeftFunc]     = SDLK_LALT;    //chr?
	keymap[EStdKeyRightFunc]    = SDLK_RALT;
    keymap[EStdKeyDeviceA]      = SDLK_RETURN;	/* "fire" key */
    
    
    


    ///////////////////////////////////////////////////////////
    /*
    RFs fs;
    if(KErrNone == fs.Connect())
        {
        RArray<TInt> array;
        TRAPD(err, ReadL(fs, array));
        if(err == KErrNone && array.Count() > 0)
            {
            
            SDLKey temp[MAX_SCANCODE];
            Mem::Copy(temp, keymap, MAX_SCANCODE * sizeof(SDLKey));

            for(TInt k = 0; k < array.Count(); k+= 2)
                {
                const TInt oldval = array[k]; 
                const TInt newval = array[k + 1]; 
                if(oldval >=  0 && oldval < MAX_SCANCODE && newval >=  0 && newval < MAX_SCANCODE)
                    {
                    keymap[oldval] = temp[newval];
                    }
                }
            }
        array.Close();
        }

    fs.Close();*/
    ///////////////////////////////////////////////////////////

    
	keymap[EStdKeyNumLock] = SDLK_NUMLOCK;
	keymap[EStdKeyScrollLock] = SDLK_SCROLLOCK;
	
	keymap[EStdKeyNkpForwardSlash] = SDLK_KP_DIVIDE;
	keymap[EStdKeyNkpAsterisk] = SDLK_KP_MULTIPLY;
	keymap[EStdKeyNkpMinus] = SDLK_KP_MINUS;
	keymap[EStdKeyNkpPlus] = SDLK_KP_PLUS;
	keymap[EStdKeyNkpEnter] = SDLK_KP_ENTER;
	keymap[EStdKeyNkp1] = SDLK_KP1;
	keymap[EStdKeyNkp2] = SDLK_KP2;
	keymap[EStdKeyNkp3] = SDLK_KP3;
	keymap[EStdKeyNkp4] = SDLK_KP4;
	keymap[EStdKeyNkp5] = SDLK_KP5;
	keymap[EStdKeyNkp6] = SDLK_KP6;
	keymap[EStdKeyNkp7] = SDLK_KP7;
	keymap[EStdKeyNkp8] = SDLK_KP8;
	keymap[EStdKeyNkp9] = SDLK_KP9;
	keymap[EStdKeyNkp0] = SDLK_KP0;
	keymap[EStdKeyNkpFullStop] = SDLK_KP_PERIOD;
    /*
    keymap[EStdKeyMenu] = SDLK_MENU; should be, but not yet
    keymap[EStdKeyBacklightOn] =
    keymap[EStdKeyBacklightOff] =
    keymap[EStdKeyBacklightToggle] =
    keymap[EStdKeyIncContrast] =
    keymap[EStdKeyDecContrast] =
    keymap[EStdKeySliderDown] =
    keymap[EStdKeySliderUp] =
    keymap[EStdKeyDictaphonePlay] =
    keymap[EStdKeyDictaphoneStop] =
    keymap[EStdKeyDictaphoneRecord] =
    keymap[EStdKeyHelp] =
    keymap[EStdKeyOff] =
    keymap[EStdKeyDial] =
    keymap[EStdKeyIncVolume] =
    keymap[EStdKeyDecVolume] =
    keymap[EStdKeyDevice0] =
    keymap[EStdKeyDevice1] =
    keymap[EStdKeyDevice2] =
    keymap[EStdKeyDevice3] =
    keymap[EStdKeyDevice4] =
    keymap[EStdKeyDevice5] =
    keymap[EStdKeyDevice6] =
    keymap[EStdKeyDevice7] =
    keymap[EStdKeyDevice8] =
    keymap[EStdKeyDevice9] =
    keymap[EStdKeyDeviceA] =
    keymap[EStdKeyDeviceB] =
    keymap[EStdKeyDeviceC] =
    keymap[EStdKeyDeviceD] =
    keymap[EStdKeyDeviceE] =
    keymap[EStdKeyDeviceF] =
    keymap[EStdKeyApplication0] =
    keymap[EStdKeyApplication1] =
    keymap[EStdKeyApplication2] =
    keymap[EStdKeyApplication3] =
    keymap[EStdKeyApplication4] =
    keymap[EStdKeyApplication5] =
    keymap[EStdKeyApplication6] =
    keymap[EStdKeyApplication7] =
    keymap[EStdKeyApplication8] =
    keymap[EStdKeyApplication9] =
    keymap[EStdKeyApplicationA] =
    keymap[EStdKeyApplicationB] =
    keymap[EStdKeyApplicationC] =
    keymap[EStdKeyApplicationD] =
    keymap[EStdKeyApplicationE] =
    keymap[EStdKeyApplicationF] =
    keymap[EStdKeyYes] =
    keymap[EStdKeyNo] =
    keymap[EStdKeyIncBrightness] =
    keymap[EStdKeyDecBrightness] = 
    keymap[EStdKeyCaseOpen] =
    keymap[EStdKeyCaseClose] =  */
    


}

LOCAL_C TInt PointerEvent(_THIS, const TWsEvent& aWsEvent)
    {
    
    const TPointerEvent* pointerEvent = aWsEvent.Pointer();
    const TPoint mousePos = EpocSdlEnv::WindowCoordinates(pointerEvent->iPosition - Private->iScreenPos);

    TInt posted = 0;
    
    if(mousePos.iX >= 0 && mousePos.iY >= 0 && mousePos.iX < _this->screen->w && mousePos.iY < _this->screen->h)
        {
        posted = SDL_PrivateMouseMotion(0, 0, mousePos.iX, mousePos.iY); /* Absolute position on screen */
        }
    
    switch (pointerEvent->iType)
        {
        case TPointerEvent::EButton1Down:
            posted += SDL_PrivateMouseButton(SDL_PRESSED, SDL_BUTTON_LEFT, 0, 0);
            break;
        case TPointerEvent::EButton1Up:
            posted += SDL_PrivateMouseButton(SDL_RELEASED, SDL_BUTTON_LEFT, 0, 0);
            break;
        case TPointerEvent::EButton2Down:
            posted += SDL_PrivateMouseButton(SDL_PRESSED, SDL_BUTTON_RIGHT, 0, 0);
            break;
        case TPointerEvent::EButton2Up:
            posted += SDL_PrivateMouseButton(SDL_RELEASED, SDL_BUTTON_RIGHT, 0, 0);
            break;
        case TPointerEvent::EButton3Down:
            posted += SDL_PrivateMouseButton(SDL_PRESSED, SDL_BUTTON_MIDDLE, 0, 0);
            break;
        case TPointerEvent::EButton3Up:
            posted += SDL_PrivateMouseButton(SDL_RELEASED, SDL_BUTTON_MIDDLE, 0, 0);
            break;
        default:;
        } // switch
    return posted;
    }

LOCAL_C TInt KeyEvent(_THIS, const TWsEvent& aWsEvent)
    {
    if (!SDL_TranslateUNICODE)
        return 0;

    SDL_keysym keysym;
    TKeyEvent *event = aWsEvent.Key();
    TranslateKey(_this, event->iScanCode, event->iCode, &keysym);
    return SDL_PrivateKeyboard(SDL_PRESSED, &keysym);
    }

LOCAL_C TInt KeyDownEvent(_THIS, const TWsEvent& aWsEvent)
    {
    SDL_keysym keysym;
    TKeyEvent *event = aWsEvent.Key();
    if (SDL_TranslateUNICODE)
        return 0;
    
    TranslateKey(_this, event->iScanCode, event->iCode, &keysym);
    return SDL_PrivateKeyboard(SDL_PRESSED, &keysym);
    }

LOCAL_C TInt KeyUpEvent(_THIS, const TWsEvent& aWsEvent)
    {
    SDL_keysym keysym;
    TKeyEvent *event = aWsEvent.Key();
    TranslateKey(_this, event->iScanCode, event->iCode, &keysym);
    return SDL_PrivateKeyboard(SDL_RELEASED, &keysym);
    }

LOCAL_C TInt FocusGainedEvent(_THIS)
    {
    EpocSdlEnv::EnableDraw();
    return SDL_PrivateAppActive(1, SDL_APPINPUTFOCUS|SDL_APPMOUSEFOCUS);
    }

LOCAL_C TInt FocusLostEvent(_THIS)
    {
    EpocSdlEnv::DisableDraw();
    return SDL_PrivateAppActive(0, SDL_APPINPUTFOCUS|SDL_APPMOUSEFOCUS);
    }

LOCAL_C TInt ModifiersChangedEvent(const TWsEvent& aWsEvent)
    {
    TModifiersChangedEvent* modEvent = aWsEvent.ModifiersChanged();
    TUint modstate = KMOD_NONE;
    if (modEvent->iModifiers & EModifierLeftShift)
        modstate |= KMOD_LSHIFT;
    if (modEvent->iModifiers & EModifierRightShift)
        modstate |= KMOD_RSHIFT;
    if (modEvent->iModifiers & EModifierLeftCtrl)
        modstate |= KMOD_LCTRL;
    if (modEvent->iModifiers & EModifierRightCtrl)
        modstate |= KMOD_RCTRL;
    if (modEvent->iModifiers & EModifierLeftAlt)
        modstate |= KMOD_LALT;
    if (modEvent->iModifiers & EModifierRightAlt)
        modstate |= KMOD_RALT;
    if (modEvent->iModifiers & EModifierLeftFunc)
        modstate |= KMOD_LMETA;
    if (modEvent->iModifiers & EModifierRightFunc)
        modstate |= KMOD_RMETA;
    if (modEvent->iModifiers & EModifierCapsLock)
        modstate |= KMOD_CAPS;
    SDL_SetModState(STATIC_CAST(SDLMod,modstate));
    return 0;
    }


LOCAL_C TInt PointerBufferReadyEvent(_THIS)
    {
    // Own threaded is not supported - it is fair easy to do, but needs some work so I would implemented
    // it only upon request :-) 
    RWindow* win = EnvUtils::IsOwnThreaded() ? NULL : EpocSdlEnv::Window();
    TInt posted = 0;
    if(win != NULL)
        {
        TPoint points[KPointerBufferSize];
        TPckg<TPoint[KPointerBufferSize]> buf(points);
        User::LeaveIfError(win->RetrievePointerMoveBuffer(buf));
        const TInt count = buf.Length() / sizeof(TPoint);
        
        for(TInt i = 0; i < count; i++)
            {
            const TPoint point = points[i] - Private->iScreenPos; 
            const TPoint mousePos = EpocSdlEnv::WindowCoordinates(point);
            if(mousePos.iX >= 0 && mousePos.iY >= 0 && mousePos.iX < _this->screen->w && mousePos.iY < _this->screen->h)
                {
                posted += SDL_PrivateMouseMotion(0, 0, mousePos.iX, mousePos.iY); /* Absolute position on screen */
                }
            }
        }
    return posted;
    }

LOCAL_C TInt ScreenDeviceChanged()
    {
    static TInt w = -1;
    static TInt h = -1;
    const TSize screenSize = EpocSdlEnv::WindowSize();
    if(w != screenSize.iWidth || h != screenSize.iHeight)
        {
        SDL_PrivateResize(screenSize.iWidth, screenSize.iHeight);
      //  EpocSdlEnv::WaitDeviceChange();          
        w = screenSize.iWidth;
        h = screenSize.iHeight;
        }
    return 0;
    }

LOCAL_C TInt InternalEvent(const TWsEvent& aWsEvent)
    {
    const TInt* load = reinterpret_cast<const TInt*>(aWsEvent.EventData());
    EpocSdlEnv::ObserverEvent(load[0], load[1]);
    return 0;
    }

int HandleWsEvent(_THIS, const TWsEvent& aWsEvent)
{
    switch (aWsEvent.Type())
		{    
		case EEventSwitchOff:
			SDL_PrivateQuit();
			return 0;
    	case EEventPointerBufferReady:
    	    return PointerBufferReadyEvent(_this);
        case EEventPointer: /* Mouse pointer events */
            return PointerEvent(_this, aWsEvent);
        case EEventKey: /* Key events */
            return KeyEvent(_this, aWsEvent);
        case EEventKeyDown: /* Key events */
            return KeyDownEvent(_this, aWsEvent);
        case EEventKeyUp: /* Key events */
            return KeyUpEvent(_this, aWsEvent);
        case EEventFocusGained: /* SDL window got focus */
            return FocusGainedEvent(_this);	   
        case EEventFocusLost: /* SDL window lost focus */
        	return FocusLostEvent(_this);
        case EEventModifiersChanged: 
            return ModifiersChangedEvent(aWsEvent);         
    	case EEventScreenDeviceChanged:
    	    return ScreenDeviceChanged();
    	case ESDLWsEvent:
    	    return InternalEvent(aWsEvent);
        default:            
            return 0;
		} 
    }

extern "C" {

void PumpEvents(_THIS)
    {
    MEventQueue& events = EpocSdlEnv::EventQueue();
    while(events.HasData() /*&& !EpocSdlEnv::IsSuspend()*/)
        {
        events.Lock();
       
       //there have to be a copy, so we can release
       //lock immediately. HandleWsEvent may cause
       //deadlock otherwise.
        
        const TWsEvent event = events.Shift();
		events.Unlock();
		HandleWsEvent(_this, event);
	    }
    }



void InitOSKeymap(_THIS)
	{
	ResetKeyMap();
	EpocSdlEnv::ObserverEvent(MSDLObserver::EEventKeyMapInit ,0);
	}

static SDL_keysym* TranslateKey(_THIS, int scancode, unsigned int icode, SDL_keysym* keysym)
{
//    char debug[256];
    //SDL_TRACE1("SDL: TranslateKey, scancode=%d", scancode); //!!

	/* Set the keysym information */ 

	keysym->scancode = scancode;

    if ((scancode >= MAX_SCANCODE) && 
        ((scancode - ENonCharacterKeyBase + 0x0081) >= MAX_SCANCODE)) {
        SDL_SetError("Too big scancode");
        keysym->scancode = SDLK_UNKNOWN;
	    keysym->mod = KMOD_NONE; 
        return keysym;
    }

	keysym->mod = SDL_GetModState();

    /* Handle function keys: F1, F2, F3 ... */
    if (keysym->mod & KMOD_META) {
        if (scancode >= 'A' && scancode < ('A' + 24)) { /* first 32 alphabet keys */
            switch(scancode) {
                case 'Q': scancode = EStdKeyF1; break;
                case 'W': scancode = EStdKeyF2; break;
                case 'E': scancode = EStdKeyF3; break;
                case 'R': scancode = EStdKeyF4; break;
                case 'T': scancode = EStdKeyF5; break;
                case 'Y': scancode = EStdKeyF6; break;
                case 'U': scancode = EStdKeyF7; break;
                case 'I': scancode = EStdKeyF8; break;
                case 'A': scancode = EStdKeyF9; break;
                case 'S': scancode = EStdKeyF10; break;
                case 'D': scancode = EStdKeyF11; break;
                case 'F': scancode = EStdKeyF12; break;
            }
            keysym->sym = keymap[scancode];
        }
    }

    if (scancode >= ENonCharacterKeyBase) {
        // Non character keys
	    keysym->sym = keymap[scancode - 
            ENonCharacterKeyBase + 0x0081]; // !!hard coded
    } else {
	    keysym->sym = keymap[scancode];
    }


	/* If UNICODE is on, get the UNICODE value for the key */
	keysym->unicode = 0;

#if 1 // !!TODO:unicode

	if ( SDL_TranslateUNICODE && icode != 0)
		{
		if (scancode < 0x80)
			{
			/* Populate the unicode field with the ASCII value */
			keysym->unicode = icode;
			}
		}

#endif

    //!!
    //sprintf(debug, "SDL: TranslateKey: keysym->scancode=%d, keysym->sym=%d, keysym->mod=%d",
    //    keysym->scancode, keysym->sym, keysym->mod);
    //SDL_TRACE(debug); //!!

	return(keysym);
}

} /* extern "C" */



