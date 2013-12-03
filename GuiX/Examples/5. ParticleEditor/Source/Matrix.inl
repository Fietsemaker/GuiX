namespace guix {
namespace particles {

namespace detail {

enum MatrixDims
{
	M3_DIMS = 3,
	M3_SIZE = 9,
	M4_DIMS = 4,
	M4_SIZE = 16
};

}; // namespace detail

// ===================================================================================
// Identity matrices
// ===================================================================================

template <typename T>
const Matrix3<T> Matrix3<T>::IDENTITY
(
	1, 0, 0,
	0, 1, 0,
	0, 0, 1
);

template <typename T>
const Matrix4<T> Matrix4<T>::IDENTITY
(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
);

// ===================================================================================
// Matrix3
// ===================================================================================

namespace detail {

template <typename T>
void M3Concat(T* out, const T* a, const T* b)
{
	T t[detail::M3_SIZE] =
	{
		a[0]*b[0] + a[1]*b[3] + a[2]*b[6],
		a[0]*b[1] + a[1]*b[4] + a[2]*b[7],
		a[0]*b[2] + a[1]*b[5] + a[2]*b[8],
		a[3]*b[0] + a[4]*b[3] + a[5]*b[6],
		a[3]*b[1] + a[4]*b[4] + a[5]*b[7],
		a[3]*b[2] + a[4]*b[5] + a[5]*b[8],
		a[6]*b[0] + a[7]*b[3] + a[8]*b[6],
		a[6]*b[1] + a[7]*b[4] + a[8]*b[7],
		a[6]*b[2] + a[7]*b[5] + a[8]*b[8]
	};
	for(int i=0; i<detail::M3_SIZE; ++i) out[i] = t[i];
}

template <typename T, typename F>
void M3Function(T* in, F func, int type)
{
	switch(type)
	{
	case Matrix3<T>::POST:
		{
			Matrix3<T> mat;
			func(mat.cell);
			M3Concat(in, in, mat.cell);
		}
		break;

	case Matrix3<T>::PRE:
		{
			Matrix3<T> mat;
			func(mat.cell);
			M3Concat(in, mat.cell, in);
		}
		break;

	case Matrix3<T>::SET:
		{
			func(in);
		}
		break;
	};
}

// ===================================================================================
// Matrix3 functors

template <typename T>
struct M3SetTranslation
{
	const T x, y;
	void operator() (T* out)
	{
		out[6] = x;
		out[7] = y;
	}
};

template <typename T>
struct M3SetScaleXY
{
	const T x, y;
	void operator() (T* out)
	{
		out[0] = x;
		out[4] = y;
	}
};

template <typename T>
struct M3SetScaleXYZ
{
	const T x, y, z;
	void operator() (T* out)
	{
		out[0] = x;
		out[4] = y;
		out[8] = z;
	}
};

template <typename T>
struct M3SetLookAt
{
	const GxVec3<T> direction, up;
	void operator() (T* out)
	{
		const GxVec3<T> z = direction.Normalized();
		const GxVec3<T> x = up.Cross(z).Normalized();
		const GxVec3<T> y = z.Cross(x).Normalized();

		out[0] = x.x, out[1] = x.y, out[2] = x.z;
		out[3] = y.x, out[4] = y.y, out[5] = y.z;
		out[6] = z.x, out[7] = z.y, out[8] = z.z;
	}
};

template <typename T>
struct M3SetRotationX
{
	const T angle;
	void operator() (T* out)
	{
		const T r = angle * (T)(3.14159265359 / 180.0);
		const T s = sin(r);
		const T c = cos(r);

		out[4] =  c, out[5] = s;
		out[7] = -s, out[8] = c;
	}
};

template <typename T>
struct M3SetRotationY
{
	const T angle;
	void operator() (T* out)
	{
		const T r = angle * (T)(3.14159265359 / 180.0);
		const T s = sin(r);
		const T c = cos(r);

		out[0] = c, out[2] = -s;
		out[6] = s, out[8] =  c;
	}
};

template <typename T>
struct M3SetRotationZ
{
	const T angle;
	void operator() (T* out)
	{
		const T r = angle * (T)(3.14159265359 / 180.0);
		const T s = sin(r);
		const T c = cos(r);

		out[0] =  c, out[1] = s;
		out[3] = -s, out[4] = c;
	}
};

template <typename T>
struct M3SetAxisRotation
{
	const T angle;
	const GxVec3<T> axis;
	void operator() (T* out)
	{
		const T r = angle * (T)(3.14159265359 / 180.0);
		const T c = cos(r);
		const T s = sin(r);
		const T t = 1 - c;
		const T x = axis.x;
		const T y = axis.y;
		const T z = axis.z;
		
		out[0] = t*x*x + c,   out[1] = t*x*y + s*z, out[2] = t*x*z - s*y;
		out[3] = t*x*y - s*z, out[4] = t*y*y + c,   out[5] = t*y*z + s*x;
		out[6] = t*x*z + s*y, out[7] = t*y*z - s*x, out[8] = t*z*z + c;
	}
};

}; // namespace detail

// ===================================================================================
// Matrix3 functions with functors

template <typename T>
void Matrix3<T>::SetTranslation(T x, T y, Concat c)
{
	detail::M3SetTranslation<T> func = {x, y};
	detail::M3Function<T>(cell, func, c);
}

template <typename T>
void Matrix3<T>::SetScale(T x, T y, Concat c)
{
	detail::M3SetScaleXY<T> func = {x, y};
	detail::M3Function<T>(cell, func, c);
}

template <typename T>
void Matrix3<T>::SetScale(T x, T y, T z, Concat c)
{
	detail::M3SetScaleXYZ<T> func = {x, y, z};
	detail::M3Function<T>(cell, func, c);
}

template <typename T>
void Matrix3<T>::SetRotation(T angle, Concat c)
{
	detail::M3SetRotationZ<T> func = {angle};
	detail::M3Function<T>(cell, func, c);
}

template <typename T>
void Matrix3<T>::SetLookAt(const GxVec3<T>& direction, const GxVec3<T>& up, Concat c)
{
	detail::M3SetLookAt<T> func = {direction, up};
	detail::M3Function<T>(cell, func, c);
}

template <typename T>
void Matrix3<T>::SetRotationX(T angle, Concat c)
{
	detail::M3SetRotationX<T> func = {angle};
	detail::M3Function<T>(cell, func, c);
}

template <typename T>
void Matrix3<T>::SetRotationY(T angle, Concat c)
{
	detail::M3SetRotationY<T> func = {angle};
	detail::M3Function<T>(cell, func, c);
}

template <typename T>
void Matrix3<T>::SetRotationZ(T angle, Concat c)
{
	detail::M3SetRotationZ<T> func = {angle};
	detail::M3Function<T>(cell, func, c);
}

template <typename T>
void Matrix3<T>::SetAxisRotation(T angle, const GxVec3<T>& axis, Concat c)
{
	detail::M3SetRotationZ<T> func = {angle, axis};
	detail::M3Function<T>(cell, func, c);
}

// ===================================================================================
// Matrix3 functions without functors

template <typename T>
Matrix3<T>::Matrix3()
{
	*this = Matrix3::IDENTITY;
}

template <typename T>
Matrix3<T>::Matrix3(const T* cells9)
{
	for(int i=0; i<detail::M3_SIZE; ++i) 
		cell[i] = cells9[i];
}

template <typename T>
Matrix3<T>::Matrix3(
		T a1, T a2, T a3, 
		T b1, T b2, T b3,
		T c1, T c2, T c3)
{
	cell[0] = a1, cell[1] = a2, cell[2] = a3;
	cell[3] = b1, cell[4] = b2, cell[5] = b3;
	cell[6] = c1, cell[7] = c2, cell[8] = c3;
}

template <typename T>
void Matrix3<T>::Identity()
{
	*this = Matrix3::IDENTITY;
}

template <typename T>
bool Matrix3<T>::Invert()
{
	const float* c = cell;
	T tmp[detail::M3_SIZE];

	tmp[0] = c[4]*c[8] - c[5]*c[7];
	tmp[1] = c[2]*c[7] - c[1]*c[8];
	tmp[2] = c[1]*c[5] - c[2]*c[4];
	tmp[3] = c[5]*c[6] - c[3]*c[8];
	tmp[4] = c[0]*c[8] - c[2]*c[6];
	tmp[5] = c[2]*c[3] - c[0]*c[5];
	tmp[6] = c[3]*c[7] - c[4]*c[6];
	tmp[7] = c[1]*c[6] - c[0]*c[7];
	tmp[8] = c[0]*c[4] - c[1]*c[3];

	const T det = c[0]*tmp[0] + c[1]*tmp[3] + c[2]*tmp[6];

	if(abs(det) > 0)
	{
		const T invDet = 1 / det;
		for(int i=0; i<detail::M3_SIZE; ++i)
			cell[i] = tmp[i] * invDet;
		return true;
	}
	return false;
}

template <typename T>
void Matrix3<T>::Transpose()
{
	T t;
	t = cell[3]; cell[3] = cell[1]; cell[1] = t; // [. 1 2]
	t = cell[6]; cell[6] = cell[2]; cell[2] = t; // [3 . 5]
	t = cell[7]; cell[7] = cell[5]; cell[5] = t; // [6 7 .]
}

template <typename T>
void Matrix3<T>::Orthonormalize()
{
	GxVec3<T> x(cell[0], cell[1], cell[2]);
	GxVec3<T> y(cell[3], cell[4], cell[5]);
	GxVec3<T> z;

	x.Normalize();
	z = x.Cross(y); z.Normalize();
	y = z.Cross(x); y.Normalize();

	cell[0] = x.x, cell[1] = x.y, cell[2] = x.z;
	cell[3] = y.x, cell[4] = y.y, cell[5] = y.z;
	cell[6] = z.x, cell[7] = z.y, cell[8] = z.z;
}

template <typename T>
void Matrix3<T>::Concatenate(const Matrix3<T>& other)
{
	detail::M3Concat<T>(cell, cell, other.cell);
}

template <typename T>
Matrix3<T> Matrix3<T>::Concatenated(const Matrix3<T>& other) const
{
	Matrix3<T> out;
	detail::M3Concat<T>(out.cell, cell, other.cell);
	return out;
}

template <typename T>
GxVec2<T> Matrix3<T>::Transform(const GxVec2<T>& v) const
{
	const float* c = cell;
	return GxVec2<T>(
		c[0] * v.x + c[3] * v.y + c[6],
		c[1] * v.x + c[4] * v.y + c[7]);
}

template <typename T>
GxVec3<T> Matrix3<T>::Transform(const GxVec3<T>& v) const
{
	const float* c = cell;
	return GxVec3<T>(
		c[0] * v.x + c[3] * v.y + c[6] * v.z,
		c[1] * v.x + c[4] * v.y + c[7] * v.z,
		c[2] * v.x + c[5] * v.y + c[8] * v.z);
}

template <typename T>
void Matrix3<T>::Transform(const GxVec2<T>* in, GxVec2<T>* out, int count) const
{
	const float* c = cell;
	for(int i=0; i<count; ++i)
	{
		const GxVec2<T>& v = in[i];
		out[i] = GxVec2<T>(
			c[0] * v.x + c[3] * in[i].y + c[6],
			c[1] * v.x + c[4] * in[i].y + c[7]);
	}
}

template <typename T>
void Matrix3<T>::Transform(const GxVec3<T>* in, GxVec3<T>* out, int count) const
{
	const float* c = cell;
	for(int i=0; i<count; ++i)
	{
		const GxVec3<T>& v = in[i];
		out[i] = GxVec3<T>(
			c[0] * v.x + c[3] * v.y + c[6] * v.z,
			c[1] * v.x + c[4] * v.y + c[7] * v.z,
			c[2] * v.x + c[5] * v.y + c[8] * v.z);
	}
}

template <typename T>
GxVec3<T> Matrix3<T>::GetRow(int index) const
{
	const T* vec = cell + index * detail::M3_DIMS;
	return GxVec3<T>(vec[0], vec[1], vec[2]);
}

template <typename T>
GxVec3<T> Matrix3<T>::GetRotationXYZ() const
{
	T ax, ay, az, c;
	ay = -Asin(cell[2]);
	c  = cos(ay);
	ay = Degrees(ay);

	if(abs(c) > 0.005) // No gimball lock?
	{
		const T i = 1.f / c;
		ax = -Degrees(Atan2(-cell[5] * i, cell[8] * i));
		az = -Degrees(Atan2(-cell[1] * i, cell[0] * i));
	}
	else // Gimball lock has occurred
	{
		ax = 0;
		az = -Degrees(Atan2(cell[3], cell[4]));
	}
	return GxVec3<T>(ax, ay, az);
}

template <typename T>
Matrix3<T>::operator T*()
{
	return cell;
}

template <typename T>
Matrix3<T>::operator const T*() const
{
	return cell;
}

// ===================================================================================
// Matrix4
// ===================================================================================

namespace detail {

template <typename T>
void M4Concat(T* out, const T* a, const T* b)
{
	T tmp[detail::M4_SIZE] =
	{
		a[ 0]*b[0] + a[ 1]*b[4] + a[ 2]*b[ 8] + a[ 3]*b[12],
		a[ 0]*b[1] + a[ 1]*b[5] + a[ 2]*b[ 9] + a[ 3]*b[13],
		a[ 0]*b[2] + a[ 1]*b[6] + a[ 2]*b[10] + a[ 3]*b[14],
		a[ 0]*b[3] + a[ 1]*b[7] + a[ 2]*b[11] + a[ 3]*b[15],
		a[ 4]*b[0] + a[ 5]*b[4] + a[ 6]*b[ 8] + a[ 7]*b[12],
		a[ 4]*b[1] + a[ 5]*b[5] + a[ 6]*b[ 9] + a[ 7]*b[13],
		a[ 4]*b[2] + a[ 5]*b[6] + a[ 6]*b[10] + a[ 7]*b[14],
		a[ 4]*b[3] + a[ 5]*b[7] + a[ 6]*b[11] + a[ 7]*b[15],
		a[ 8]*b[0] + a[ 9]*b[4] + a[10]*b[ 8] + a[11]*b[12],
		a[ 8]*b[1] + a[ 9]*b[5] + a[10]*b[ 9] + a[11]*b[13],
		a[ 8]*b[2] + a[ 9]*b[6] + a[10]*b[10] + a[11]*b[14],
		a[ 8]*b[3] + a[ 9]*b[7] + a[10]*b[11] + a[11]*b[15],
		a[12]*b[0] + a[13]*b[4] + a[14]*b[ 8] + a[15]*b[12],
		a[12]*b[1] + a[13]*b[5] + a[14]*b[ 9] + a[15]*b[13],
		a[12]*b[2] + a[13]*b[6] + a[14]*b[10] + a[15]*b[14],
		a[12]*b[3] + a[13]*b[7] + a[14]*b[11] + a[15]*b[15]
	};
	for(int i=0; i<detail::M4_SIZE; ++i) out[i] = tmp[i];
}

template <typename T, typename F>
void M4Function(T* in, F func, int type)
{
	switch(type)
	{
	case Matrix4<T>::POST:
		{
			Matrix4<T> mat;
			func(mat.cell);
			M4Concat(in, in, mat.cell);
		}
		break;

	case Matrix4<T>::PRE:
		{
			Matrix4<T> mat;
			func(mat.cell);
			M4Concat(in, mat.cell, in);
		}
		break;

	case Matrix4<T>::SET:
		{
			func(in);
		}
		break;
	};
}

// ===================================================================================
// Matrix4 functors

template <typename T>
struct M4SetTranslation
{
	const T x, y, z;
	void operator() (T* out)
	{
		out[12] = x;
		out[13] = y;
		out[14] = z;
	}
};

template <typename T>
struct M4SetScale
{
	const T x, y, z;
	void operator() (T* out)
	{
		out[ 0] = x;
		out[ 5] = y;
		out[10] = z;
	}
};

template <typename T>
struct M4SetLookAt
{
	const GxVec3<T> direction, up;
	void operator() (T* out)
	{
		const GxVec3<T> z = direction.Normalized();
		const GxVec3<T> x = up.Cross(z).Normalized();
		const GxVec3<T> y = z.Cross(x).Normalized();

		out[0] = x.x, out[1] = x.y, out[ 2] = x.z;
		out[4] = y.x, out[5] = y.y, out[ 6] = y.z;
		out[8] = z.x, out[9] = z.y, out[10] = z.z;
	}
};

template <typename T>
struct M4SetAxis
{
	const GxVec3<T> x, y, z;
	void operator() (T* out)
	{
		out[0] = x.x, out[1] = x.y, out[ 2] = x.z;
		out[4] = y.x, out[5] = y.y, out[ 6] = y.z;
		out[8] = z.x, out[9] = z.y, out[10] = z.z;
	}
};

template <typename T>
struct M4SetRotationX
{
	const T angle;
	void operator() (T* out)
	{
		const T r = angle * (T)(3.14159265359 / 180.0);
		const T s = sin(r);
		const T c = cos(r);

		out[5] =  c, out[ 6] = s;
		out[9] = -s, out[10] = c;
	}
};

template <typename T>
struct M4SetRotationY
{
	const T angle;
	void operator() (T* out)
	{
		const T r = angle * (T)(3.14159265359 / 180.0);
		const T s = sin(r);
		const T c = cos(r);

		out[0] = c, out[ 2] = -s;
		out[8] = s, out[10] =  c;
	}
};

template <typename T>
struct M4SetRotationZ
{
	const T angle;
	void operator() (T* out)
	{
		const T r = angle * (T)(3.14159265359 / 180.0);
		const T s = sin(r);
		const T c = cos(r);

		out[0] =  c, out[1] = s;
		out[4] = -s, out[5] = c;
	}
};

template <typename T>
struct M4SetAxisRotation
{
	const T angle;
	const GxVec3<T> axis;
	void operator() (T* out)
	{
		const T r = angle * (T)(3.14159265359 / 180.0);
		const T c = cos(r);
		const T s = sin(r);
		const T t = 1 - c;
		const T x = axis.x;
		const T y = axis.y;
		const T z = axis.z;
		
		out[0] = t*x*x + c,   out[1] = t*x*y + s*z, out[ 2] = t*x*z - s*y;
		out[4] = t*x*y - s*z, out[5] = t*y*y + c,   out[ 6] = t*y*z + s*x;
		out[8] = t*x*z + s*y, out[9] = t*y*z - s*x, out[10] = t*z*z + c;
	}
};

template <typename T>
struct M4SetFrustum
{
	const T left, right, bottom, top, zNear, zFar;
	void operator() (T* out)
	{
		out[ 0] = (2*zNear) / (right-left);
		out[ 5] = (2*zNear) / (top-bottom);

		out[ 8] = (right+left) / (right-left);
		out[ 9] = (top+bottom) / (top-bottom);
		out[10] = (zFar+zNear) / (zNear-zFar);
		out[11] = -1;

		out[14] = -(2*zFar*zNear) / (zFar-zNear);
		out[15] = 0;
	}
};

template <typename T>
struct M4SetOrtho
{
	const T left, right, bottom, top, zNear, zFar;
	void operator() (T* out)
	{
		out[12] = -(right+left) / (right-left);
		out[13] = -(top+bottom) / (top-bottom);
		out[14] = -(zFar+zNear) / (zFar-zNear);

		out[ 0] = 2 / (right-left);
		out[ 5] = 2 / (top-bottom);
		out[10] = -2 / (zFar-zNear);
	}
};

template <typename T>
struct M4SetPerspective
{
	const T fovy, aspect, zNear, zFar;
	void operator() (T* out)
	{
		const T ymax = zNear * tan(fovy * (T)(3.14159265359 / 360.0));
		const T ymin = -ymax;
		const T xmax = ymax * aspect;
		const T xmin = -xmax;
		M4SetFrustum<T> func = {xmin, xmax, ymin, ymax, zNear, zFar};
		func(out);
	}
};

}; // namespace detail

// ===================================================================================
// Matrix4 functions with functors

template <typename T>
void Matrix4<T>::SetTranslation(T x, T y, T z, Concat c)
{
	detail::M4SetTranslation<T> func = {x, y, z};
	detail::M4Function<T>(cell, func, c);
}

template <typename T>
void Matrix4<T>::SetScale(T x, T y, T z, Concat c)
{
	detail::M4SetScale<T> func = {x, y, z};
	detail::M4Function<T>(cell, func, c);
}

template <typename T>
void Matrix4<T>::SetLookAt(const GxVec3<T>& direction, const GxVec3<T>& up, Concat c)
{
	detail::M4SetLookAt<T> func = {direction, up};
	detail::M4Function<T>(cell, func, c);
}

template <typename T>
void Matrix4<T>::SetAxis(const GxVec3<T>& x, const GxVec3<T>& y, const GxVec3<T>& z, Concat c)
{
	detail::M4SetAxis<T> func = {x, y, z};
	detail::M4Function<T>(cell, func, c);
}

template <typename T>
void Matrix4<T>::SetRotationX(T angle, Concat c)
{
	detail::M4SetRotationX<T> func = {angle};
	detail::M4Function<T>(cell, func, c);
}

template <typename T>
void Matrix4<T>::SetRotationY(T angle, Concat c)
{
	detail::M4SetRotationY<T> func = {angle};
	detail::M4Function<T>(cell, func, c);
}

template <typename T>
void Matrix4<T>::SetRotationZ(T angle, Concat c)
{
	detail::M4SetRotationZ<T> func = {angle};
	detail::M4Function<T>(cell, func, c);
}

template <typename T>
void Matrix4<T>::SetAxisRotation(T angle, const GxVec3<T>& axis, Concat c)
{
	detail::M4SetAxisRotation<T> func = {angle, axis};
	detail::M4Function<T>(cell, func, c);
}

template <typename T>
void Matrix4<T>::SetFrustum(T left, T right, T bottom, T top, T zNear, T zFar, Concat c)
{
	detail::M4SetFrustum<T> func = {left, right, bottom, top, zNear, zFar};
	detail::M4Function<T>(cell, func, c);
}

template <typename T>
void Matrix4<T>::SetOrtho(T left, T right, T bottom, T top, T zNear, T zFar, Concat c)
{
	detail::M4SetOrtho<T> func = {left, right, bottom, top, zNear, zFar};
	detail::M4Function<T>(cell, func, c);
}

template <typename T>
void Matrix4<T>::SetPerspective(T fovy, T aspect, T zNear, T zFar, Concat c)
{
	detail::M4SetPerspective<T> func = {fovy, aspect, zNear, zFar};
	detail::M4Function<T>(cell, func, c);
}

// ===================================================================================
// Matrix4 functions without functors

template <typename T>
Matrix4<T>::Matrix4()
{
	*this = Matrix4::IDENTITY;
}

template <typename T>
Matrix4<T>::Matrix4(const T* cells16)
{
	for(int i=0; i<detail::M4_SIZE; ++i) 
		cell[i] = cells16[i];
}

template <typename T>
Matrix4<T>::Matrix4(
		T a1, T a2, T a3, T a4, 
		T b1, T b2, T b3, T b4, 
		T c1, T c2, T c3, T c4,
		T d1, T d2, T d3, T d4)
{
	cell[ 0] = a1, cell[ 1] = a2, cell[ 2] = a3, cell[ 3] = a4;
	cell[ 4] = b1, cell[ 5] = b2, cell[ 6] = b3, cell[ 7] = b4;
	cell[ 8] = c1, cell[ 9] = c2, cell[10] = c3, cell[11] = c4;
	cell[12] = d1, cell[13] = d2, cell[14] = d3, cell[15] = d4;
}

template <typename T>
void Matrix4<T>::Identity()
{
	*this = Matrix4::IDENTITY;
}

template <typename T>
bool Matrix4<T>::Invert()
{
	const float* c = cell;

	const T a0 = c[ 0]*c[ 5] - c[ 1]*c[ 4];
	const T a1 = c[ 0]*c[ 6] - c[ 2]*c[ 4];
	const T a2 = c[ 0]*c[ 7] - c[ 3]*c[ 4];
	const T a3 = c[ 1]*c[ 6] - c[ 2]*c[ 5];
	const T a4 = c[ 1]*c[ 7] - c[ 3]*c[ 5];
	const T a5 = c[ 2]*c[ 7] - c[ 3]*c[ 6];

	const T b0 = c[ 8]*c[13] - c[ 9]*c[12];
	const T b1 = c[ 8]*c[14] - c[10]*c[12];
	const T b2 = c[ 8]*c[15] - c[11]*c[12];
	const T b3 = c[ 9]*c[14] - c[10]*c[13];
	const T b4 = c[ 9]*c[15] - c[11]*c[13];
	const T b5 = c[10]*c[15] - c[11]*c[14];

	const T det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;

	if(abs(det) > 0)
	{
		T out[detail::M4_SIZE];

		out[ 0] = + c[ 5]*b5 - c[ 6]*b4 + c[ 7]*b3;
		out[ 4] = - c[ 4]*b5 + c[ 6]*b2 - c[ 7]*b1;
		out[ 8] = + c[ 4]*b4 - c[ 5]*b2 + c[ 7]*b0;
		out[12] = - c[ 4]*b3 + c[ 5]*b1 - c[ 6]*b0;
		out[ 1] = - c[ 1]*b5 + c[ 2]*b4 - c[ 3]*b3;
		out[ 5] = + c[ 0]*b5 - c[ 2]*b2 + c[ 3]*b1;
		out[ 9] = - c[ 0]*b4 + c[ 1]*b2 - c[ 3]*b0;
		out[13] = + c[ 0]*b3 - c[ 1]*b1 + c[ 2]*b0;
		out[ 2] = + c[13]*a5 - c[14]*a4 + c[15]*a3;
		out[ 6] = - c[12]*a5 + c[14]*a2 - c[15]*a1;
		out[10] = + c[12]*a4 - c[13]*a2 + c[15]*a0;
		out[14] = - c[12]*a3 + c[13]*a1 - c[14]*a0;
		out[ 3] = - c[ 9]*a5 + c[10]*a4 - c[11]*a3;
		out[ 7] = + c[ 8]*a5 - c[10]*a2 + c[11]*a1;
		out[11] = - c[ 8]*a4 + c[ 9]*a2 - c[11]*a0;
		out[15] = + c[ 8]*a3 - c[ 9]*a1 + c[10]*a0;

		const T invDet = 1 / det;
		for(int i=0; i<detail::M4_SIZE; ++i)
			cell[i] = out[i] * invDet;

		return true;
	}
	else
	{
		return false;
	}
}

template <typename T>
void Matrix4<T>::InvertOrtho()
{
	T t;
	t = cell[4]; cell[4] = cell[1]; cell[1] = t; // [. 1 2 .]
	t = cell[8]; cell[8] = cell[2]; cell[2] = t; // [4 . 6 .]
	t = cell[9]; cell[9] = cell[6]; cell[6] = t; // [8 9 . .]
	const T tx = -cell[12];
	const T ty = -cell[13];
	const T tz = -cell[14];
	cell[12] = tx*cell[0] + ty*cell[4] + tz*cell[8];
	cell[13] = tx*cell[1] + ty*cell[5] + tz*cell[9];
	cell[14] = tx*cell[2] + ty*cell[6] + tz*cell[10];
}

template <typename T>
void Matrix4<T>::Transpose()
{
	T t;
	t = cell[ 4]; cell[ 4] = cell[ 1]; cell[ 1] = t; // [ .  1  2  3]
	t = cell[ 8]; cell[ 8] = cell[ 2]; cell[ 2] = t; // [ 4  .  6  7]
	t = cell[ 9]; cell[ 9] = cell[ 6]; cell[ 6] = t; // [ 8  9  . 11]
	t = cell[12]; cell[12] = cell[ 3]; cell[ 3] = t; // [12 13 14  .]
	t = cell[13]; cell[13] = cell[ 7]; cell[ 7] = t;
	t = cell[14]; cell[14] = cell[11]; cell[11] = t;
}

template <typename T>
void Matrix4<T>::Concatenate(const Matrix4<T>& other)
{
	detail::M4Concat<T>(cell, cell, other.cell);
}

template <typename T>
Matrix4<T> Matrix4<T>::Concatenated(const Matrix4<T>& other) const
{
	Matrix4<T> out;
	detail::M4Concat<T>(out.cell, cell, other.cell);
	return out;
}

template <typename T>
GxVec3<T> Matrix4<T>::Transform(const GxVec3<T>& v) const
{
	const float* c = cell;
	return GxVec3<T>(
		c[0] * v.x + c[4] * v.y + c[8] * v.z + c[12],
		c[1] * v.x + c[5] * v.y + c[9] * v.z + c[13],
		c[2] * v.x + c[6] * v.y + c[10]* v.z + c[14]);
}

template <typename T>
GxVec3<T> Matrix4<T>::Transform(T x, T y, T z) const
{
	const float* c = cell;
	return GxVec3<T>(
		c[0] * x + c[4] * y + c[8] * z + c[12],
		c[1] * x + c[5] * y + c[9] * z + c[13],
		c[2] * x + c[6] * y + c[10]* z + c[14]);
}

template <typename T>
GxVec4<T> Matrix4<T>::Transform(const GxVec4<T>& v) const
{
	const float* c = cell;
	return GxVec4<T>(
		c[0] * v.x + c[4] * v.y + c[8] * v.z + c[12] * v.w,
		c[1] * v.x + c[5] * v.y + c[9] * v.z + c[13] * v.w,
		c[2] * v.x + c[6] * v.y + c[10]* v.z + c[14] * v.w,
		c[3] * v.x + c[7] * v.y + c[11]* v.z + c[15] * v.w);
}

template <typename T>
void Matrix4<T>::Transform(const GxVec3<T>* in, GxVec3<T>* out, int count) const
{
	const float* c = cell;
	for(int i=0; i<count; ++i)
	{
		const GxVec3<T>& v = in[i];
		out[i].Set(
			c[0] * v.x + c[4] * v.y + c[8] * v.z + c[12],
			c[1] * v.x + c[5] * v.y + c[9] * v.z + c[13],
			c[2] * v.x + c[6] * v.y + c[10]* v.z + c[14]);
	}
}

template <typename T>
void Matrix4<T>::Transform(const GxVec4<T>* in, GxVec4<T>* out, int count) const
{
	const float* c = cell;
	for(int i=0; i<count; ++i)
	{
		const GxVec3<T>& v = in[i];
		out[i].Set(
			c[0] * v.x + c[4] * v.y + c[8] * v.z + c[12] * v.w,
			c[1] * v.x + c[5] * v.y + c[9] * v.z + c[13] * v.w,
			c[2] * v.x + c[6] * v.y + c[10]* v.z + c[14] * v.w,
			c[3] * v.x + c[7] * v.y + c[11]* v.z + c[15] * v.w);
	}
}

template <typename T>
GxVec4<T> Matrix4<T>::GetRow(int index) const
{
	const T* vec = cell + index * detail::M4_DIMS;
	return GxVec4<T>(vec[0], vec[1], vec[2], vec[3]);
}

template <typename T>
void Matrix4<T>::Orthonormalize()
{
	GxVec3<T> x(cell[0], cell[1], cell[2]);
	GxVec3<T> y(cell[4], cell[5], cell[6]);
	GxVec3<T> z;

	x.Normalize();
	z = x.Cross(y); z.Normalize();
	y = z.Cross(x); y.Normalize();

	cell[0] = x.x, cell[1] = x.y, cell[2 ] = x.z;
	cell[4] = y.x, cell[5] = y.y, cell[6 ] = y.z;
	cell[8] = z.x, cell[9] = z.y, cell[10] = z.z;
}

template <typename T>
Matrix3<T> Matrix4<T>::ToMatrix3() const
{
	Matrix3<T> resMat;
	T* out = resMat.cell;
	out[0] = cell[0], out[1] = cell[1], out[2] = cell[2];
	out[3] = cell[4], out[4] = cell[5], out[5] = cell[6];
	out[6] = cell[8], out[7] = cell[9], out[8] = cell[10];
	return resMat;
}

template <typename T>
GxVec3<T> Matrix4<T>::GetRotationXYZ() const
{
	T ax, ay, az, c;

	ay = -Asin(cell[2]);
	c  = cos(ay);
	ay = Degrees(ay);

	if(abs(c) > 0.005) // No gimball lock?
	{
		const T i = 1.f / c;
		ax = -Degrees(Atan2(-cell[6] * i, cell[10] * i));
		az = -Degrees(Atan2(-cell[1] * i, cell[ 0] * i));
	}
	else // Gimball lock has occurred
	{
		ax = 0;
		az = -Degrees(Atan2(cell[4], cell[5]));
	}
	return GxVec3<T>(ax, ay, az);
}

template <typename T>
Matrix4<T>::operator T*()
{
	return cell;
}

template <typename T>
Matrix4<T>::operator const T*() const
{
	return cell;
}

}; // namespace particles
}; // namespace guix