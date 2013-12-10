/// @file
/// Contains the list class.

#pragma once

#include <GuiX/Config.h>

namespace guix {
namespace core {

// ===================================================================================
// GxList
// ===================================================================================
/** The GxList stores a dynamic array of values.

 The GxList class has functionality similar to vector from the C++ standard template
 library. It is used to store and manipulate a dynamic array of values. The main
 difference compared to the STL vector is that GxList avoids deep copies when it is
 assigned or copy constructed from another GxList object. This is achieved by sharing
 list data and keeping a reference count.

*/
template <typename T>
class GUIX_API GxList
{
public:
	~GxList();

	/// Constructs an empty list.
	GxList();

	/// Constructs an empty list and reserves size for the given number of elements.
	GxList(int reserve);

	/// Constructs a list of count elements, copy constructed from a value.
	GxList(int count, const T& value);

	/// Constructs a list of count elements, copy constructed from an array.
	GxList(int count, const T* source);

	/// Constructs a copy of a list; the data is implicitly shared by reference.
	GxList(const GxList& other);

	/// Sets the content to a copy of a list; the data is implicitly shared by reference.
	void Assign(const GxList& other);

	/// Removes all the elements from the list.
	void Clear();

	/// Resizes the array to count elements.
	/// If count is smaller than the current size, elements at the end of the list are removed.
	/// If count is greater than the current size, copies of value are appended to the list.
	void Resize(int size, const T& value);

	/// Inserts a value at the end of the list.
	void Append(const T& value);

	/// Inserts a value at the beginning of the list.
	void Prepend(const T& value);

	/// Inserts a value at position pos in the list.
	void Insert(int pos, const T& value);

	/// Removes the element at position pos from the list.
	void Erase(int pos);

	/// Removes all elements that are equal to value from the list.
	void EraseValue(const T& value);

	/// Removes the last element of the list.
	void PopBack();

	/// Returns true if the list contains one or more elements that are equal to value.
	bool Contains(const T& value) const;

	const T* Data() const; ///< Returns a const pointer to the array data.
	T* Edit();             ///< Returns a pointer to the array data for editing.
	int Size() const;      ///< Returns the current number of elements in the array.
	int Reserved() const;  ///< Returns for how many elements space is reserved.
	bool Empty() const;    ///< Returns true if the array has a size of zero.

	/// Returns the value at position pos; does not perform an out-of-bounds check.
	T& operator [] (int pos);

	/// Returns the value at position pos; does not perform an out-of-bounds check.
	const T& operator [] (int pos) const;

	GxList& operator << (const T& value);     ///< Calls \c Append().
	GxList& operator = (const GxList& other); ///< Calls \c Assign().

private:
	T* myData;
	int mySize;
};

}; // namespace core
}; // namespace guix

#include <GuiX/List.inl>