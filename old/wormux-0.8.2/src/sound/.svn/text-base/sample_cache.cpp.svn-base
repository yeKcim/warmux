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

#include <assert.h>
#include "tool/debug.h"
#include "sample_cache.h"

SampleCache::SampleCache( size_t memory_limit )
    : m_memory_limit( memory_limit )
    , m_used_memory( 0 )
{
}

int SampleCache::FindSlot( Mix_Chunk* sample, const std::string & file_name )
{
    if (!sample)
        return -1;

    if ( m_memory_limit > 0 && sample->alen + m_used_memory > m_memory_limit )
        return -1; // refuse to load when cache is full

    m_used_memory += sample->alen;
    CachedChunk chk;

    chk.m_chunk    = sample;
    chk.m_filename = file_name;

    m_cache.push_back( chk );
    int slot = m_cache.size() - 1;
    m_chunks_by_name.insert( std::make_pair( file_name, slot ) );
    m_chunks_by_addr.insert( std::make_pair( chk.m_chunk, slot ) );

    MSG_DEBUG( "jukebox.cache",
               "caching sample '%s' (size %uB), total cache size: %uB",
               chk.m_filename.c_str(), sample->alen, m_used_memory );

    return slot;
}

Mix_Chunk * SampleCache::LoadSound( const std::string & file_name )
{
    // look it up first
    int slot = FindChunkByName( file_name );

    if ( -1 == slot )
    {
        // not found, so try to cache it cache it
        Mix_Chunk *sample = Mix_LoadWAV( file_name.c_str() );
        slot = FindSlot( sample, file_name );
        if ( -1 == slot )
            return sample;
    }

    CachedChunk & chk = m_cache[ slot ];
    chk.m_times_used++;
    chk.m_refcount ++;
    return chk.m_chunk;
}

void SampleCache::FreeChunk( Mix_Chunk * pchk )
{
    int slot = FindChunkByAddr( pchk );

    if ( -1 != slot )
    {
        CachedChunk & chk = m_cache[ slot ];
        chk.m_refcount--;
        assert( chk.m_refcount >= 0 );
        // we don't delete the chunk when refcount == 0, because it can
        // defeat the whole idea of caching (imagine what if game
        // keeps loading and freeing the same sound)
        return;
    }

    // if chunk cannot be found, it's probably because we didn't cache it
    // delegate the work to mixer
    Mix_FreeChunk( pchk );
}

void SampleCache::Clear()
{
    for ( std::vector< CachedChunk >::iterator iter = m_cache.begin();
          iter != m_cache.end(); iter ++ )
    {
        CachedChunk & chk = *iter;

        m_used_memory -= chk.m_chunk->alen;
        if ( chk.m_refcount != 0 )
        {
            MSG_DEBUG( "jukebox.cache",
                       "Freeing sample '%s' with refcount = %d",
                       chk.m_filename.c_str(), chk.m_refcount );
        }

        // print statistics
        MSG_DEBUG( "jukebox.cache", "sample '%s' used %d times",
                   chk.m_filename.c_str(), chk.m_times_used );

        Mix_FreeChunk( chk.m_chunk );

        // reset memory
        chk.m_chunk = NULL;
        chk.m_refcount = 0;
        chk.m_times_used = 0;
        chk.m_filename = "";
    }

    m_cache.clear();

    m_chunks_by_name.clear();
    m_chunks_by_addr.clear();

    assert( 0 == m_used_memory );
}

int SampleCache::FindChunkByName( const std::string & name )
{
    std::map< std::string, int >::iterator it = m_chunks_by_name.find( name );
    if ( m_chunks_by_name.end() == it )
        return -1;

    return it->second;
}

int SampleCache::FindChunkByAddr( Mix_Chunk * pchk )
{
    std::map< Mix_Chunk *, int >::iterator it = m_chunks_by_addr.find( pchk );
    if ( m_chunks_by_addr.end() == it )
        return -1;

    return it->second;
}

void SampleCache::Precache( const std::string & file_name )
{
    Mix_Chunk * precache = LoadSound( file_name );
    FreeChunk( precache ); // this does not remove the sample from cache
}
