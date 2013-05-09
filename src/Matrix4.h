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


#ifndef MATRIX4_H
#define MATRIX4_H

#include "Vector3.h"
#include "Vector4.h"
#include "Vector2.h"
#include <assert.h>

namespace MiniEngine
{
	class Quat;
	class Matrix3;
#define MAT00 0
#define MAT01 1
#define MAT02 2
#define MAT03 3
#define MAT10 4
#define MAT11 5
#define MAT12 6
#define MAT13 7
#define MAT20 8
#define MAT21 9
#define MAT22 10
#define MAT23 11
#define MAT30 12
#define MAT31 13
#define MAT32 14
#define MAT33 15

	class Matrix4
	{
	public:



		float f[16];

		inline float* operator [] ( const int Row )
		{
			return &f[Row<<2];
		}
		inline const float* operator [] (const size_t Row) const 
		{ 
			return &f[Row<<2]; 
		} //const

		inline Matrix4() {}

		inline Matrix4(float m0,float m1,float m2,float m3,
			float m4,float m5,float m6,float m7,
			float m8,float m9,float m10,float m11,
			float m12,float m13,float m14,float m15)
		{
			f[0]=m0;f[1]=m1;f[2]=m2;f[3]=m3;
			f[4]=m4;f[5]=m5;f[6]=m6;f[7]=m7;
			f[8]=m8;f[9]=m9;f[10]=m10;f[11]=m11;
			f[12]=m12;f[13]=m13;f[14]=m14;f[15]=m15;
		}
		explicit Matrix4(const float* mat)
		{
			float* ptr = f;
			for(int i=0;i<16;i++)
			{
				(*ptr++)=(*mat++);
			}
		}

		inline float* ptr()  
		{
			return &f[0];
		}

		inline float& operator()(const int r, const int c)
		{
			return f[c*4+r];
		}

		const float& operator()(const int r, const int c) const
		{
			return f[c*4+r];
		}

		inline Vector2 operator * ( const Vector2 &v ) const
		{
			return Vector2(((f[MAT00]* v.x) + (f[MAT10]* v.y) +f[MAT30]),
					((f[MAT01]* v.x) + (f[MAT11]* v.y) +f[MAT31]));
		}


		inline Vector3 operator * ( const Vector3 &vec ) const
		{
			//return Vector3::One;
			
			return Vector3(f[0] * vec.x  +  f[4] * vec.y  +  f[8] * vec.z  +  f[12],
						   f[1] * vec.x  +  f[5] * vec.y  +  f[9] * vec.z  +  f[13],
						   f[2] * vec.x  +  f[6] * vec.y  +  f[10]* vec.z  +  f[14]);
						   
		}
		
		inline bool isTranslationOnly() const
		{
			return (f[0]==1 && f[1]==0 && f[2]==0 && f[4]==0 && f[5]==1 && f[6]==0 && f[8]==0 && f[9]==0 && f[10]==1);
		}




		inline Vector4 operator * ( const Vector4 &vec ) const
		{
			return Vector4(f[0] * vec.x  +  f[4] * vec.y  +  f[8] * vec.z   +  f[12] * vec.w,
						   f[1] * vec.x  +  f[5] * vec.y  +  f[9] * vec.z   +  f[13] * vec.w,			
						   f[2] * vec.x  +  f[6] * vec.y  +  f[10] * vec.z  +  f[14] * vec.w,
						   f[3] * vec.x  +  f[7] * vec.y  +  f[11] * vec.z  +  f[15] * vec.w);	
		}


		Matrix4 operator*(const Matrix4& rhs) const;
		Matrix4 operator+(const Matrix4& rhs) const;
		Matrix4 operator-(const Matrix4& rhs) const;
		Matrix4& operator+=(const Matrix4& rhs);
		Matrix4& operator-=(const Matrix4& rhs);

        void operator = ( const Matrix3& mat3 );

		Matrix4& operator*=(const Matrix4& rhs);
		
		Matrix4 operator*(const float rhs) const;
		Matrix4& operator*(const float rhs);
		Matrix4& operator*=(const float rhs);

		Matrix4  inverse() const;

		static Matrix4 createRotationX(const float angle)
		{
			float fSine;
			float fCosine;
			MathUtils::SinCos(angle,fSine,fCosine);

			return Matrix4(1.0f,0.0f,0.0f,0.0f,0.0f,fCosine,fSine,0.0f,0.0f,-fSine,fCosine,0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		static Matrix4 createRotationY(const float angle)
		{
			float fSine;
			float fCosine;
			MathUtils::SinCos(angle,fSine,fCosine);

			return Matrix4(fCosine,0.0f,-fSine,0.0f,0.0f,1.0f,0.0f,0.0f,fSine,0.0f,fCosine,0.0f,0.0f,0.0f,0.0f,1.0f);
		}

		static Matrix4 createRotationZ(const float angle)
		{
			float fSine;
			float fCosine;
			MathUtils::SinCos(angle,fSine,fCosine);

			return Matrix4(fCosine,fSine,0.0f,0.0f,-fSine,fCosine,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f);

		}


		static Matrix4 createScale(const float fx,const float fy,const float fz)
		{
			return Matrix4(fx,0,0,0,
						   0,fy,0,0,
						   0,0,fz,0,
						   0,0,0,1);
		}

		static Matrix4  createTranslation(const float tx, const float ty, const float tz)
		{
			return Matrix4(1,0,0,0,
					    0,1,0,0,
				        0,0,1,0,
				        tx,ty,tz,1);
		}


		static void multiply(float *matrices, const float *matrix, size_t count);

		static Matrix4  createPerspectiveFov(float fov, float aspect, float nearClip, float farClip, float rotationDegrees=0);

		static Matrix4 createLookAt(const Vector3 &position,const Vector3 &at,const Vector3 &up);

		void makeTransform(const Vector3& position, const Vector3& scale, const Quat& orientation);
		static void makeTransform(float *matrixArray4,const Vector3& position, const Vector3& scale, const Quat& orientation);
	
		static const Matrix4 Zero;
		static const Matrix4 Identity;

	};

		inline Vector4 operator * (const Vector4& vec, const Matrix4& mat)
		{

			return Vector4(mat.f[0] * vec.x + mat.f[4] * vec.y + mat.f[8] * vec.z + mat.f[12] * vec.w,
						   mat.f[1] * vec.x + mat.f[5] * vec.y + mat.f[9] * vec.z + mat.f[13] * vec.w,			
						   mat.f[2] * vec.x + mat.f[6] * vec.y + mat.f[10] * vec.z + mat.f[14] * vec.w,
						   mat.f[3] * vec.x + mat.f[7] * vec.y + mat.f[11] * vec.z + mat.f[15] * vec.w);	
		}
}

#endif