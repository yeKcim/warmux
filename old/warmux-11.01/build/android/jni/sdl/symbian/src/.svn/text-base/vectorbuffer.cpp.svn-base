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
    vectorbuffer.cpp
    yet another circle buffer

    Markus Mertama
*/

#include"vectorbuffer.h"



void VectorPanic(TInt aErr, TInt aLine)
    {
    TBuf<64> b;
    b.Format(_L("vector buffer at % d "), aLine);
    User::Panic(b, aErr);
    }

void TNodeBuffer::TNode::Terminator(TNodeBuffer::TNode* aNode)
    {
    Mem::Copy(iSucc, &aNode, sizeof(TNode*));
    }

TInt TNodeBuffer::TNode::Size() const
    {
    return reinterpret_cast<const TUint8*>(iSucc) - Ptr();
    }

const TUint8*  TNodeBuffer::TNode::Ptr() const
    {
    return reinterpret_cast<const TUint8*>(this) + sizeof(TNode);
    }

TNodeBuffer::TNode*  TNodeBuffer::TNode::Empty(TUint8* aBuffer)
    {
    TNode* node = reinterpret_cast<TNode*>(aBuffer);
    node->iSucc = node + 1;
    return node;
    }

 TNodeBuffer::TNode*  TNodeBuffer::TNode::New(TNode* aPred, const TDesC8& aData)
    {
    TNode* node = aPred->Size() == 0 ? aPred : aPred->iSucc;

    
    TUint8* start = reinterpret_cast<TUint8*>(node) + sizeof(TNode);
    node->iSucc = reinterpret_cast<TNode*>(start + aData.Size());
    node->iSucc->iSucc = NULL; //terminator

    __ASSERT_DEBUG(node->Size() == aData.Size(), VECPANIC(KErrCorrupt));

    Mem::Copy(start, aData.Ptr(), aData.Size());
    return node;
    }





