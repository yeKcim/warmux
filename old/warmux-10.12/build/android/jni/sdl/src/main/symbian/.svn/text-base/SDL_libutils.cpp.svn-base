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

#include<e32std.h>

#include "SDL_loadso.h"


void* SDL_LoadObject(const char *sofile)
	{
	RLibrary* lib = new RLibrary();
	if(lib == NULL)
		return NULL;
	TFileName name;
	name.Copy(TPtrC8((const TUint8*)sofile));
	if(KErrNone == lib->Load(name))
		return lib;
	delete lib;
	return NULL;
	}

	
void*  SDL_LoadFunction(void *handle, const char *name)
	{
	TLex8 v((const TUint8*)(name));
	TInt ord;
	
	if(KErrNone != v.Val(ord))
		return NULL;
	
	const RLibrary* lib = reinterpret_cast<RLibrary*>(handle);
	TLibraryFunction f = lib->Lookup(ord);
	return (void*)(f); 
/*
	int i = 0;
	TPtrC8 functionName((const TUint8*)(name));
	for(;;)
		{
		const TInt ord = _Ordinals[i].iOrd;
		if(ord < 0)
			return NULL;
		if(functionName.Compare(
		TPtrC8((const TUint8*)(_Ordinals[i].iName))) == 0)
			{
			const RLibrary* lib = reinterpret_cast<RLibrary*>(handle);
			TLibraryFunction f = lib->Lookup(ord);
			return (void*)(f); 
			}
		++i;
		}*/
	}
	
void   SDL_UnloadObject(void *handle)	
	{
	RLibrary* lib = reinterpret_cast<RLibrary*>(handle);
	lib->Close();
	delete lib; 
	}

