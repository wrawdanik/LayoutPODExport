/*
-----------------------------------------------------------------------------

 Copyright (c) 2009 Walter Rawdanik	

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.
-----------------------------------------------------------------------------
*/


#ifndef MATRIX3_H
#define MATRIX3_H


#include "Vector3.h"
#include <assert.h>

namespace MiniEngine
{
	

	class Matrix3
	{
	public:

		float f[9];	/*!< Array of float */
		inline float* operator [] ( const size_t column )
		{
			return &f[column*3];
		}

		inline const float* operator [] (const size_t column) const 
		{ 
			return &f[column*3]; 
		} //const


		inline Matrix3() {}

		inline explicit	Matrix3(const float* pMat)
		{
			float *ptr = f;
			for(int i=0;i<9;i++)
			{
				(*ptr++)=(*pMat++);
			}
		}

		Matrix3(float m0,float m1,float m2,
			float m3,float m4,float m5,
			float m6,float m7,float m8)
		{
			f[0]=m0;f[1]=m1;f[2]=m2;
			f[3]=m3;f[4]=m4;f[5]=m5;
			f[6]=m6;f[7]=m7;f[8]=m8;
		}

		inline Matrix3 (const Matrix3& rhs)
		{
			memcpy(f,rhs.f,9*sizeof(float));
		}

		inline float& operator()(const size_t r, const size_t c)
		{
			return f[c*3+r];
		}

		const float& operator()(const size_t r, const size_t c) const
		{
			return f[c*3+r];
		}

		Matrix3 operator*(const Matrix3& rhs) const;

		
		Matrix3 operator+(const Matrix3& rhs) const;

		Matrix3 operator-(const Matrix3& rhs) const;

		Matrix3& operator+=(const Matrix3& rhs);

		Matrix3& operator-=(const Matrix3& rhs);

		Matrix3& operator*=(const Matrix3& rhs);

		Matrix3& operator*(const float rhs);

		Matrix3& operator*=(const float rhs);

		Matrix3	operator- () const;

		Vector3 operator*(const Vector3& rhs) const
		{

					return Vector3(rhs.x*f[0]+rhs.y*f[3]+rhs.z*f[6],
								   rhs.x*f[1]+rhs.y*f[4]+rhs.z*f[7],
								   rhs.x*f[2]+rhs.y*f[5]+rhs.z*f[8]);
		}




		void fromAngleAxis (const float fRadians,const Vector3& rkAxis);

		void transform(Vector3 &vec) const;

		float determinant() const;

		Matrix3 inverse() const;
		Matrix3 transpose() const;

		void setIdentity()
		{
			f[0] = 1.0f;f[1] = 0;f[2] = 0;
			f[3] = 0;f[4] = 1.0f;f[5] = 0;
			f[6] = 0;f[7] = 0;f[8] = 1.0f;
		}

		static Matrix3 createRotationX(const float angle)
		{
			float fSine;
			float fCosine;
			MathUtils::SinCos(angle,fSine,fCosine);

			return Matrix3(1.0f,0.0f,0.0f, 0.0f, fCosine, fSine,0.0f, -fSine,fCosine);

		}
		static Matrix3 createRotationY(const float angle)
		{
			float fSine;
			float fCosine;
			MathUtils::SinCos(angle,fSine,fCosine);

			return Matrix3(fCosine,0.0f, -fSine,0.0f,1.0f,0.0f, fSine,0.0f, fCosine);
		}

		static Matrix3 createRotationZ(const float angle)
		{
			float fSine;
			float fCosine;
			MathUtils::SinCos(angle,fSine,fCosine);
			return Matrix3(fCosine,fSine,0.0f, -fSine, fCosine,0.0f, 0.0f, 0.0f,1.0f);

		}

		static Matrix3 createScale(const float fx,const float fy,const float fz)
		{
			return Matrix3(fx,0.f,0.f,0.0f,fy,0.0f,0.0f,0.0f,fz);
		}

		static Matrix3  createTranslation(const float tx, const float ty)
		{
			return Matrix3(1.0f,0.0f,tx,0.0f,1.0f,ty,0.0f,0.0f,1);
		}

		static const Matrix3 Zero;
		static const Matrix3 Identity;

	};
}

#endif