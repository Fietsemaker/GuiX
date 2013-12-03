#pragma once

#include <GuiX/Common.h>

namespace guix {
namespace particles {

// ===================================================================================
// Matrix
// ===================================================================================
/** The Matrix base class contains an enum of concatenation types.

 A number of matrix operations have a concatenation parameter. This parameter affects
 how the operation is applied to the matrix that is performing it. The following types
 are available:

 - POST, the operation is performed on an identity matrix m. The target matrix is then
   computed by post-concatenation with m: <c>target = target * m</c>. The resulting 
   transformation is the old transformation followed by the new transformation.

 - PRE, the operation is performed on an identity matrix m. The target matrix is then
   computed by pre-concatenation with m: <c>target = m * target</c>. The resulting
   transformation is the new transformation followed by the old transformation.

 - SET, the operation is performed directly on the target matrix. This overwrites the
   cells of the target matrix, so it might give unexpected results when used on a
   matrix that already contain transformations.

 The default concatenation type is POST.

 @see Matrix3, Matrix4
*/
class Matrix
{
public:
	/// Enumeration of matrix concatenation types, see Matrix class description.
	enum Concat
	{
		POST, ///< Post-concatenation
		PRE,  ///< Pre-concatenation
		SET,  ///< No concatenation
	};
};

// ===================================================================================
// Matrix3
// ===================================================================================
/** The Matrix3 class represents a transformation matrix of 3 by 3 cells.

 The Matrix3 class can be used to represent two types of transformations:

 - a 2D transformation matrix with translation.
 - a 3D transformation matrix without translation.

 Internally there is no difference between the two, since they are both stored as a
 3 by 3 grid of cells that represent an abstract transformation. Therefore, whether a 
 given matrix is a 2D or 3D matrix depends on how it is used. Some of the operations
 such as \c SetTranslation() and \c SetAxisRotation() are only meaningful on either
 a 2D or 3D matrix, while others such as \c Identity() are meaningful on both.

 @see Matrix, Matrix4
*/
template <typename T>
class Matrix3 : public Matrix
{
public:
	static const Matrix3 IDENTITY; ///< Identity matrix.

	/// Enumeration of several important cell indices.
	enum Indices 
	{
		_SX = 0, ///< This cell contains the x-scale.
		_SY = 4, ///< This cell contains the y-scale.
		_SZ = 8, ///< This cell contains the z-scale.
		_TX = 6, ///< This cell contains the x-translation .
		_TY = 7, ///< This cell contains the y-translation.
	};

	/// Constructs an identity matrix.
	Matrix3();
	
	/// Constructs a matrix by copying an array of 9 values.
	Matrix3(const T* cells9);

	/// Constructs a matrix by copying 9 values.
	Matrix3(
		T a1, T a2, T a3,
		T b1, T b2, T b3,
		T c1, T c2, T c3);
	
	// ===================================================================================
	// 2D matrix functions

	/// Translates a 2D coordinate system by x and y amount.
	void SetTranslation(T x, T y, Concat c = POST);

	/// Scales a 2D coordinate system by x and y amount.
	void SetScale(T x, T y, Concat c = POST);

	/// Rotates a 2D coordinate system by angle degrees.
	void SetRotation(T angle, Concat c = POST);

	// ===================================================================================
	// 3D matrix functions

	/// Scales a 3D coordinate system by x, y and z amount.
	void SetScale(T x, T y, T z, Concat c = POST);

	/// Rotates a 3D coordinate system towards a direction, such that:
	/// - the z-axis is pointing towards direction.
	/// - the x-axis is perpendicular to both the z-axis and up.
	/// - the y-axis is perpendicular to both the x- ans z-axis and points in the same direction as up.
	void SetLookAt(const GxVec3<T>& direction, const GxVec3<T>& up, Concat c = POST);

	/// Rotates a 3D coordinate system around the x-axis by angle degrees.
	void SetRotationX(T angle, Concat c = POST);

	/// Rotates a 3D coordinate system around the y-axis by angle degrees.
	void SetRotationY(T angle, Concat c = POST);

	/// Rotates a 3D coordinate system around the z-axis by angle degrees.
	void SetRotationZ(T angle, Concat c = POST);

	/// Rotates a 3D coordinate system around an arbitrary axis by angle degrees.
	void SetAxisRotation(T angle, const GxVec3<T>& axis, Concat c = POST);

	// ===================================================================================
	// General matrix functions

	/// Sets the matrix to an identity matrix.
	void Identity();

	/// Inverts the matrix. If matrix is not invertible, the function returns false and the matrix remains unchanged.
	bool Invert();

	/// Transposes the matrix. This reflects all cells over the matrix's main diagonal.
	void Transpose();

	/// Orthonormalizes the matrix. This makes every axis unit length and perpendicular to the other axes.
	void Orthonormalize();

	/// Concatenates the matrix with another matrix, and stores the result.
	/// The function uses post concatenation: <c>this = this * other</c>.
	void Concatenate(const Matrix3<T>& other);

	/// Returns the result of concatenating this matrix with another matrix.
	/// The function uses post concatenation: <c>result = this * other</c>.
	Matrix3<T> Concatenated(const Matrix3<T>& other) const;

	/// Transforms a 2D vector and returns the resulting vector.
	GxVec2<T> Transform(const GxVec2<T>& in) const;

	/// Transforms a 3D vector and returns the resulting vector.
	GxVec3<T> Transform(const GxVec3<T>& in) const;

	/// Transforms an array of 2D vectors.
	/// @param in    : an array of vectors to be transformed.
	/// @param out   : an array to which the resulting vectors will be written.
	/// @param count : the number of vectors to convert.
	void Transform(const GxVec2<T>* in, GxVec2<T>* out, int count) const;

	/// Transforms an array of 3D vectors.
	/// @param in    : an array of vectors to be transformed.
	/// @param out   : an array to which the resulting vectors will be written.
	/// @param count : the number of vectors to convert.
	void Transform(const GxVec3<T>* in, GxVec3<T>* out, int count) const;

	/// Returns a row of the matrix as a 3D vector.
	/// note: 0 = x-axis, 1 = y-axis, 2 = z-axis/translation.
	GxVec3<T> GetRow(int index) const;

	/// Converts a 3D rotation matrix to a set of rotation angles around the xyz axes.
	GxVec3<T> GetRotationXYZ() const;

	operator T*();             ///< Access the cells as an array of values.
	operator const T*() const; ///< Access the cells as an array of values.

	// Public members
	T cell[9];
};

// Common type definitions
typedef Matrix3<float> Matrix3f;

// ===================================================================================
// Matrix4
// ===================================================================================
/** The Matrix4 class represents a transformation matrix of 4 by 4 cells.

 The Matrix4 represents a 3D transformation matrix with translation. Besides common
 3D transformation functions such as rotation and translation, the Matrix4 class
 also has several functions for setting up projection matrices for a camera frustum.

 @see Matrix, Matrix3
*/
template <typename T>
class Matrix4 : public Matrix
{
public:
	static const Matrix4 IDENTITY; ///< Identity matrix.

	/// Enumeration of several important cell indices.
	enum Indices
	{
		_SX = 0,  ///< This cell contains the x-scale.
		_SY = 5,  ///< This cell contains the y-scale.
		_SZ = 10, ///< This cell contains the z-scale.
		_SW = 15, ///< This cell contains the w-scale.
		_TX = 12, ///< This cell contains the x-translation.
		_TY = 13, ///< This cell contains the y-translation.
		_TZ = 14, ///< This cell contains the z-translation.
	};

	/// Constructs an identity matrix.
	Matrix4();

	/// Constructs a matrix by copying an array of 16 values.
	Matrix4(const T* cells16);

	/// Constructs a matrix by copying 16 values.
	Matrix4(
		T a1, T a2, T a3, T a4,
		T b1, T b2, T b3, T b4,
		T c1, T c2, T c3, T c4,
		T d1, T d2, T d3, T d4);

	// ===================================================================================
	// 3D matrix functions

	/// Translates a 3D coordinate system by x, y and z amount.
	void SetTranslation(T x, T y, T z, Concat c = POST);

	/// Scales a 3D coordinate system by x, y and z amount.
	void SetScale(T x, T y, T z, Concat c = POST);

	/// Rotates a 3D coordinate system towards direction.
	/// - the z-axis is pointing towards direction.
	/// - the x-axis is perpendicular to both the z-axis and up.
	/// - the y-axis is perpendicular to both the x- ans z-axis and points in the same direction as up.
	void SetLookAt(const GxVec3<T>& direction, const GxVec3<T>& up, Concat c = POST);

	/// Set the x-, y- and z-axis of the matrix to the corresponding vector parameters.
	void SetAxis(const GxVec3<T>& x, const GxVec3<T>& y, const GxVec3<T>& z, Concat c = POST);

	/// Rotates a 3D coordinate system around the x-axis by angle degrees.
	void SetRotationX(T angle, Concat c = POST);

	/// Rotates a 3D coordinate system around the y-axis by angle degrees.
	void SetRotationY(T angle, Concat c = POST);

	/// Rotates a 3D coordinate system around the z-axis by angle degrees.
	void SetRotationZ(T angle, Concat c = POST);

	/// Rotates a 3D coordinate system around an arbitrary axis by angle degrees.
	void SetAxisRotation(T angle, const GxVec3<T>& axis, Concat c = POST);

	// ===================================================================================
	// Projection matrix functions

	/// Sets up a perspective projection matrix based on six clipping plane coordinates.
	void SetFrustum(T left, T right, T bottom, T top, T zNear, T zFar, Concat c = POST);

	/// Sets up a orthogonal projection matrix based on six clipping plane coordinates.
	void SetOrtho(T left, T right, T bottom, T top, T zNear, T zFar, Concat c = POST);

	/// Sets up a perspective projection matrix based on vertical field-of-view in degrees, 
	/// horizontal to vertical aspect ratio and near/far clipping plane coordinates.
	void SetPerspective(T fov, T aspect, T zNear, T zFar, Concat c = POST);

	// ===================================================================================
	// General matrix functions

	/// Sets the matrix to an identity matrix.
	void Identity();

	/// Inverts the matrix. If matrix is not invertible, the function returns false and the matrix remains unchanged.
	bool Invert();
	
	/// Inverts the matrix by means of transposing and negating the translation. 
	/// Faster than Invert() but only works on orthogonal matrices (a combination of rotations and/or translations).
	void InvertOrtho();

	/// Orthonormalizes the matrix. This makes every axis unit length and perpendicular to the others.
	void Orthonormalize();

	/// Transposes the matrix. This reflects all cells over the matrix's main diagonal.
	void Transpose();

	/// Concatenates the matrix with another matrix, and stores the result.
	/// The function uses post concatenation: <c>this = this * other</c>.
	void Concatenate(const Matrix4<T>& other);

	/// Returns the result of concatenating this matrix with another matrix.
	/// The function uses post concatenation: <c>result = this * other</c>.
	Matrix4<T> Concatenated(const Matrix4<T>& other) const;

	/// Transforms a 3D vector and returns the resulting vector; the w-component is treated as 1.
	GxVec3<T> Transform(const GxVec3<T>& in) const;

	/// Transforms a 3D vector and returns the resulting vector; the w-component is treated as 1.
	GxVec3<T> Transform(T x, T y, T z) const;

	/// Transforms a 4D vector and returns the resulting vector.
	GxVec4<T> Transform(const GxVec4<T>& in) const;

	/// Transforms an array of 3D vectors; the w-component is treated as 1.
	/// @param in    : an array of vectors to be transformed.
	/// @param out   : an array to which the resulting vectors will be written.
	/// @param count : the number of vectors to convert.
	void Transform(const GxVec3<T>* in, GxVec3<T>* out, int count) const;

	/// Transforms an array of 3D vectors.
	/// @param in    : an array of vectors to be transformed.
	/// @param out   : an array to which the resulting vectors will be written.
	/// @param count : the number of vectors to convert.
	void Transform(const GxVec4<T>* in, GxVec4<T>* out, int count) const;

	/// Returns a row of the matrix as a 4D vector.
	/// note: 0 = x-axis, 1 = y-axis, 2 = z-axis, 3 = translation.
	GxVec4<T> GetRow(int index) const;

	/// Converts a 3D rotation matrix to a set of rotation angles around the xyz axes.
	GxVec3<T> GetRotationXYZ() const;

	/// Returns the rotation/scaling part of the matrix as a 3x3 matrix; translation cells are lost.
	Matrix3<T> ToMatrix3() const;

	operator T*();             ///< Access the cells as an array of values.
	operator const T*() const; ///< Access the cells as an array of values.

	// Public members
	T cell[16];
};

// Common type definitions
typedef Matrix4<float> Matrix4f;

}; // namespace particles
}; // namespace guix

#include "Matrix.inl"