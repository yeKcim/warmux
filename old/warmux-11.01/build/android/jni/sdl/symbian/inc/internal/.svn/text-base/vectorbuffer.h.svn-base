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

#ifndef __VECTORBUFFER_H__
#define __VECTORBUFFER_H__

#include<e32std.h>
#define VECPANIC(x) VectorPanic(x, __LINE__)
void VectorPanic(TInt, TInt);


//Internal

NONSHARABLE_CLASS(TNodeBuffer)
    {
    public:
    protected:
        NONSHARABLE_CLASS(TNode)
            {
            public:
                static  TNode* Empty(TUint8* iBuffer);
                static  TNode* New(TNode* aPrev,  const TDesC8& aData);
                const TUint8* Ptr() const;
                TInt Size() const;
                inline TNode* Succ();
                static void SetSucc(TNode*& aNode);
                void Terminator(TNode* aNode);
            private:
                TNode* iSucc;
            };
    };

inline TNodeBuffer::TNode* TNodeBuffer::TNode::Succ()
    {
    return iSucc;
    }


//Vector buffer for variable length data

template <TInt C>
NONSHARABLE_CLASS(TVectorBuffer) : public TNodeBuffer
    {
    public:
        TVectorBuffer();
        TInt Append(const TDesC8& aData);
        TPtrC8 Shift();
        TPtrC8 operator[](TInt aIndex) const;
        TInt Size() const;
    private:
        TInt GetRoom(TInt aSize) const;
        TInt Unreserved() const;
    private:
        TNode* iTop;
        TNode* iBottom;
        TInt iSize;
        TUint8 iBuffer[C];
    };

template <TInt C>
TVectorBuffer<C>::TVectorBuffer() : iSize(0)
    {
    Mem::FillZ(iBuffer, C);
    iTop = TNode::Empty(iBuffer); //these points to buffer
    iBottom = TNode::Empty(iBuffer);
    }

template<TInt C >
TInt TVectorBuffer<C>::Unreserved() const
    {
    __ASSERT_DEBUG(iBottom < iBottom->Succ(), VECPANIC(KErrCorrupt));
    const TInt bytesbetween =
        reinterpret_cast<const TUint8*>(iBottom->Succ()) -
        reinterpret_cast<const TUint8*>(iTop);
    const TInt topsize = sizeof(TNode);
    if(bytesbetween > 0)            //bytesbetween is room between bottom and top 
        {                           //therefore free room is subracted from free space 
                    
        const TInt room = C - bytesbetween - topsize; 
        return room;
        }
    if(bytesbetween == 0)           
        {
        
        if(Size() > 0)              
            return 0;               
        else
            return C - topsize;
        }
    const TInt room = -bytesbetween - topsize; //free is space between pointers
    return room;                     
    }

template <TInt C>
TInt TVectorBuffer<C>::GetRoom(TInt aSize) const
    {
    const TInt bytesnew = sizeof(TNode) + aSize;
    const TInt room = Unreserved() - bytesnew;
    return room;
    }

template <TInt C>
TInt TVectorBuffer<C>::Append(const TDesC8& aData) 
    {
    const TInt len = aData.Length();
    if(GetRoom(len) < 0)
        {
        return KErrOverflow;
        }
    if(iBottom->Succ()->Ptr() - iBuffer > (C - (len + TInt(sizeof(TNode)))))
        {
     
        TNode* p = TNode::Empty(iBuffer);
        iBottom->Terminator(p);
      	iBottom = p;
      	return Append(aData);
        } 
    
         
    iBottom = TNode::New(iBottom, aData);

    iSize += len;
    return KErrNone;
    }

template <TInt C>
TPtrC8 TVectorBuffer<C>::Shift()
    {
    __ASSERT_ALWAYS(iTop->Succ() != NULL, VECPANIC(KErrUnderflow)); //can never pass-by bottom
    TNode* node = iTop;
    iTop = iTop->Succ();
    if(iTop > node)
        {
        iSize -= node->Size();
        return TPtrC8(node->Ptr(), node->Size());
        }
    else
        {
        return Shift(); //this happens when buffer is terminated, and data lies in next 
        }
    }

template <TInt C>
TInt TVectorBuffer<C>::Size() const
    {
    return iSize;
    }

template <TInt C>
TPtrC8 TVectorBuffer<C>::operator[](TInt aIndex) const
    {
    TInt index = 0;
    TNode* t = iTop->Size() > 0 ? iTop : iTop->Succ(); //eliminate terminator
    while(index < aIndex)
        {
        TNode* nt = t->Succ();
        if(nt < t)
            {
            nt = nt->Succ();
            }
        t = nt;
        if(t->Size() > 0)
        	index++;
        __ASSERT_ALWAYS(t->Succ() != NULL, VECPANIC(KErrUnderflow)); //can never pass-by bottom
        }
    return t->Ptr();
    }

//Vector buffer for fixed length data

template <class T, TInt C>
NONSHARABLE_CLASS(TVector) : public TVectorBuffer<C * sizeof(T)>
    {
    public:
        TVector();
        TInt Append(const T& aData);
        const T& Shift();
        TInt Size() const;
        const T& operator[](TInt aIndex) const;
    };

template <class T, TInt C>
TVector<T, C>::TVector() : TVectorBuffer<C * sizeof(T)>()
    {
    }

template <class T, TInt C>
TInt TVector<T, C>::Append(const T& aData)
    {
    const TPckgC<T> data(aData);
    return TVectorBuffer<C * sizeof(T)>::Append(data);
    }

template <class T, TInt C>
const T& TVector<T, C>::Shift()
    {
    const TPtrC8 ptr = TVectorBuffer<C * sizeof(T)>::Shift();
    return *(reinterpret_cast<const T*>(ptr.Ptr()));
    }


template <class T, TInt C>
TInt TVector<T, C>::Size() const
    {
    return TVectorBuffer<C * sizeof(T)>::Size() / sizeof(T);
    }

template <class T, TInt C>
const T& TVector<T, C>::operator[](TInt aIndex) const
    {
    const TPtrC8 ptr = TVectorBuffer<C * sizeof(T)>::operator[](aIndex);
    return *(reinterpret_cast<const T*>(ptr.Ptr()));
    }

#endif


