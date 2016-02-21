/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * Simple class that caches the sounds played - part of jukebox
 *****************************************************************************/

#ifndef SAMPLE_CACHE_H
#define SAMPLE_CACHE_H

#include <vector>
#include <map>
#include "include/base.h"

#include <SDL.h>
#include <SDL_mixer.h>

class SampleCache
{
    struct CachedChunk
    {
        Mix_Chunk *     m_chunk;
        int             m_refcount;
        int             m_times_used;
        std::string     m_filename;

        CachedChunk()
         : m_chunk( NULL ), m_refcount( 0 ), m_times_used( 0 ) {};
    };

    std::vector< CachedChunk >  m_cache;
    size_t                      m_memory_limit;
    size_t                      m_used_memory;

    // two maps for fast indexing
    std::map< std::string, int > m_chunks_by_name;
    std::map< Mix_Chunk *, int > m_chunks_by_addr;

    int           FindSlot        ( Mix_Chunk* sample, const std::string & fn );
    int           FindChunkByName ( const std::string & name );
    int           FindChunkByAddr ( Mix_Chunk * pchk );

public:

    void          Clear           ();
    void          Precache        ( const std::string & file_name );
    Mix_Chunk *   LoadSound       ( const std::string & file_name );
    void          FreeChunk       ( Mix_Chunk * pchk );


                  SampleCache     ( size_t memory_limit = 0 );
    virtual       ~SampleCache    () { Clear(); }
};

#endif

