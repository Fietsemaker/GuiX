#include <string.h>
#include <new>

//#define GX_LIST_DEBUG

namespace guix {
namespace core {

// ===================================================================================
// Reference management functions
// ===================================================================================

namespace list {

#ifdef GX_LIST_DEBUG
	#define GX_LIST_TAG(ref,a,b,c,d) (ref->tag = (d<<24)|(c<<16)|(b<<8)|(a))
#else
	#define GX_LIST_TAG(ref,a,b,c,d)
#endif

struct Ref
{
#ifdef GX_LIST_DEBUG
	uint tag;
#endif
	size_t reserved;
	size_t count;
};

// ===================================================================================
/// Inline functions

inline Ref& ToRef(void* data)
{
	return reinterpret_cast<Ref*>(data)[-1];
}

template <typename T>
inline T* NewRef(size_t size)
{
	Ref* ref = static_cast<Ref*>(malloc(sizeof(Ref) + sizeof(T)*size));
	ref->reserved = size;
	ref->count = 1;
	GX_LIST_TAG(ref, 'G','x','L','n');
	return reinterpret_cast<T*>(ref + 1);
}

template <typename T>
inline T* ReallocRef(Ref& src, size_t size)
{
	Ref* ref = static_cast<Ref*>(realloc(&src, sizeof(Ref) + sizeof(T)*size));
	ref->reserved = size;
	GX_LIST_TAG(ref, 'G','x','L','r');
	return reinterpret_cast<T*>(ref + 1);
}

// ===================================================================================
/// Utility functions

template <typename T>
void Detach(T*& data, size_t size)
{
	Ref& ref = ToRef(data);
	if(ref.count > 1)
	{
		--ref.count;
		T* mem = NewRef<T>(size);
		for(size_t i=0; i<size; ++i)
			new (mem + i) T(data[i]);
		data = mem;
	}
}

template <typename T>
void Grow(T*& data, size_t size, size_t inc)
{
	size_t n = size + inc;
	Ref& ref = ToRef(data);
	if(ref.count > 1)
	{
		--ref.count;
		T* mem = NewRef<T>(n);
		for(size_t i=0; i<size; ++i)
			new (mem + i) T(data[i]);
		data = mem;
	}
	else if(ref.reserved < n)
	{
		n = GxMax(n, ref.reserved << 1);
		data = ReallocRef<T>(ref, n);
	}
}

template <typename T>
void AddRef(T* data)
{
	++ToRef(data).count;
}

template <typename T>
void Release(T* data, size_t size)
{
	Ref& ref = ToRef(data);
	if(--ref.count == 0)
	{
		for(size_t i=0; i<size; ++i)
			data[i].~T();
		free(&ref);
	}
}

#ifdef GX_LIST_TAG
	#undef GX_LIST_TAG
#endif

}; // namespace list

// ===================================================================================
// Constructors and destructor
// ===================================================================================

template <typename T>
GxList<T>::~GxList()
{
	list::Release(myData, mySize);
}

template <typename T>
GxList<T>::GxList()
	:mySize(0)
{
	myData = list::NewRef<T>(0);
}

template <typename T>
GxList<T>::GxList(int reserve)
	:mySize(0)
{
	myData = list::NewRef<T>(GxMax(0, reserve));
}

template <typename T>
GxList<T>::GxList(int count, const T& value)
	:mySize(0)
{
	if(count > 0)
	{
		myData = list::NewRef<T>(count);
		for(int i=0; i<count; ++i)
			new (myData + i) T(value);

		mySize = count;
	}
	else
	{
		myData = list::NewRef<T>(0);
	}
}

template <typename T>
GxList<T>::GxList(int count, const T* source)
	:mySize(0)
{
	if(count > 0)
	{
		myData = list::NewRef<T>(count);
		for(int i=0; i<count; ++i)
			new (myData + i) T(source[i]);

		mySize = count;
	}
	else
	{
		myData = list::NewRef<T>(0);
	}
}

template <typename T>
GxList<T>::GxList(const GxList& other)
	:myData(other.myData)
	,mySize(other.mySize)
{
	list::AddRef(myData);
}

template <typename T>
void GxList<T>::Assign(const GxList& other)
{
	if(myData == other.myData) return;

	list::Release(myData, mySize);
	myData = other.myData;
	mySize = other.mySize;
	list::AddRef(myData);
}

template <typename T>
void GxList<T>::Clear()
{
	list::Detach(myData, mySize);
	for(int i=0; i<mySize; ++i)
		myData[i].~T();

	mySize = 0;
}

template <typename T>
void GxList<T>::Resize(int size, const T& value)
{
	if(size > mySize)
	{
		list::Grow(myData, mySize, size - mySize);
		for(int i=mySize; i<size; ++i)
			new (myData + i) T(value);
	}
	else
	{
		size = GxMax(0, size);
		list::Detach(myData, mySize);
		for(int i=size; i<mySize; ++i)
			myData[i].~T();
	}
	mySize = size;
}

template <typename T>
void GxList<T>::Append(const T& value)
{
	list::Grow(myData, mySize, 1);
	new (myData + mySize) T(value);
	++mySize;
}

template <typename T>
void GxList<T>::Prepend(const T& value)
{
	list::Grow(myData, mySize, 1);
	memmove(myData + 1, myData, sizeof(T) * mySize);
	new (myData) T(value);
	++mySize;
}

template <typename T>
void GxList<T>::Insert(int pos, const T& value)
{
	if(pos >= mySize)
		Append(value);
	else if(pos <= 0)
		Prepend(value);
	else
	{
		list::Grow(myData, mySize, 1);
		memmove(myData + pos + 1, myData + pos, sizeof(T) * (mySize - pos));
		new (myData + pos) T(value);
		++mySize;
	}
}

template <typename T>
void GxList<T>::Erase(int pos)
{
	if(pos >= 0 && pos < mySize)
	{
		list::Detach(myData, mySize);
		myData[pos].~T();
		memmove(myData + pos, myData + pos + 1, sizeof(T) * (mySize - pos - 1)); 
		--mySize;
	}
}

template <typename T>
void GxList<T>::EraseValue(const T& value)
{
	for(int i=mySize-1; i>=0; --i)
		if(myData[i] == value)
			Erase(i);
}

template <typename T>
void GxList<T>::PopBack()
{
	if(mySize > 0)
	{
		list::Detach(myData, mySize);
		myData[--mySize].~T();
	}
}

template <typename T>
bool GxList<T>::Contains(const T& value) const
{
	for(int i=mySize-1; i>=0; --i)
		if(myData[i] == value)
			return true;

	return false;
}

template <typename T>
const T* GxList<T>::Data() const
{
	return myData;
}

template <typename T>
T* GxList<T>::Edit()
{
	list::Detach(myData, mySize);
	return myData;
}

template <typename T>
int GxList<T>::Size() const
{
	return mySize;
}

template <typename T>
int GxList<T>::Reserved() const
{
	return list::ToRef(myData).reserved;
}

template <typename T>
bool GxList<T>::Empty() const
{
	return (mySize == 0);
}

template <typename T>
T& GxList<T>::operator [] (int pos)
{
	list::Detach(myData, mySize);
	return myData[pos];
}

template <typename T>
const T& GxList<T>::operator [] (int pos) const
{
	return myData[pos];
}

template <typename T>
GxList<T>& GxList<T>::operator << (const T& value)
{
	Append(value);
	return *this;
}

template <typename T>
GxList<T>& GxList<T>::operator = (const GxList<T>& other)
{
	Assign(other);
	return *this;
}

}; // namespace core
}; // namespace guix