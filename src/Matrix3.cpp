
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

#include "Matrix3.h"
#include <math.h>

namespace MiniEngine
{
 
    const Matrix3 Matrix3::Zero(0,0,0,0,0,0,0,0,0);
    const Matrix3 Matrix3::Identity(1,0,0,0,1,0,0,0,1);


	Matrix3	Matrix3::operator*(const Matrix3& rhs) const
	{
		Matrix3 out;

		out.f[0] =	f[0]*rhs.f[0]+f[1]*rhs.f[3]+f[2]*rhs.f[6];
		out.f[1] =	f[0]*rhs.f[1]+f[1]*rhs.f[4]+f[2]*rhs.f[7];
		out.f[2] =	f[0]*rhs.f[2]+f[1]*rhs.f[5]+f[2]*rhs.f[8];

		out.f[3] =	f[3]*rhs.f[0]+f[4]*rhs.f[3]+f[5]*rhs.f[6];
		out.f[4] =	f[3]*rhs.f[1]+f[4]*rhs.f[4]+f[5]*rhs.f[7];
		out.f[5] =	f[3]*rhs.f[2]+f[4]*rhs.f[5]+f[5]*rhs.f[8];

		out.f[6] =	f[6]*rhs.f[0]+f[7]*rhs.f[3]+f[8]*rhs.f[6];
		out.f[7] =	f[6]*rhs.f[1]+f[7]*rhs.f[4]+f[8]*rhs.f[7];
		out.f[8] =	f[6]*rhs.f[2]+f[7]*rhs.f[5]+f[8]*rhs.f[8];
		return out;
	}



	Matrix3& Matrix3::operator*=(const Matrix3& rhs)
	{
		Matrix3 out;

		out.f[0] =	f[0]*rhs.f[0]+f[1]*rhs.f[3]+f[2]*rhs.f[6];
		out.f[1] =	f[0]*rhs.f[1]+f[1]*rhs.f[4]+f[2]*rhs.f[7];
		out.f[2] =	f[0]*rhs.f[2]+f[1]*rhs.f[5]+f[2]*rhs.f[8];

		out.f[3] =	f[3]*rhs.f[0]+f[4]*rhs.f[3]+f[5]*rhs.f[6];
		out.f[4] =	f[3]*rhs.f[1]+f[4]*rhs.f[4]+f[5]*rhs.f[7];
		out.f[5] =	f[3]*rhs.f[2]+f[4]*rhs.f[5]+f[5]*rhs.f[8];

		out.f[6] =	f[6]*rhs.f[0]+f[7]*rhs.f[3]+f[8]*rhs.f[6];
		out.f[7] =	f[6]*rhs.f[1]+f[7]*rhs.f[4]+f[8]*rhs.f[7];
		out.f[8] =	f[6]*rhs.f[2]+f[7]*rhs.f[5]+f[8]*rhs.f[8];
		*this = out;
		return *this;
	}

		Matrix3 Matrix3::operator+(const Matrix3& rhs) const
		{
			Matrix3 out;
			float const *lptr = f, *rptr = rhs.f;
			float *outptr = out.f;
			for(int i=0;i<9;i++)
			{
				(*outptr++) = (*lptr++) + (*rptr++);
			}
			return out;
		}

		Matrix3 Matrix3::operator-(const Matrix3& rhs) const
		{
			Matrix3 out;
			float const *lptr = f, *rptr = rhs.f;
			float *outptr = out.f;
			for(int i=0;i<9;i++)
			{
				(*outptr++) = (*lptr++) - (*rptr++);
			}
			return out;
		}

		Matrix3& Matrix3::operator+=(const Matrix3& rhs)
		{
			float *lptr = f;
			float const *rptr = rhs.f;
			for(int i=0;i<9;i++)
			{
				(*lptr++) += (*rptr++);
			}
			return *this;
		}

		Matrix3& Matrix3::operator-=(const Matrix3& rhs)
		{
			float *lptr = f;
			float const *rptr = rhs.f;
			for(int i=0;i<9;i++)
			{
				(*lptr++) -= (*rptr++);
			}
			return *this;
		}


		Matrix3& Matrix3::operator*(const float rhs)
		{
			for (int i=0; i<9; ++i)
			{
				f[i]*=rhs;
			}
			return *this;
		}

		Matrix3& Matrix3::operator*=(const float rhs)
		{
			for (int i=0; i<9; ++i)
			{
				f[i]*=rhs;
			}
			return *this;
		}

		Matrix3	Matrix3::operator- () const
		{
			Matrix3 negMat;

			for(int i=0;i<9;i++)
				negMat.f[i]=-f[i];
			return negMat;
		}

		void Matrix3::fromAngleAxis (const float fRadians,const Vector3& rkAxis)
		{
			float fCos = MathUtils::Cos(fRadians);
			float fSin = MathUtils::Sin(-fRadians);
			float fOneMinusCos = 1.0f-fCos;
			float fX2 = rkAxis.x*rkAxis.x;
			float fY2 = rkAxis.y*rkAxis.y;
			float fZ2 = rkAxis.z*rkAxis.z;
			float fXYM = rkAxis.x*rkAxis.y*fOneMinusCos;
			float fXZM = rkAxis.x*rkAxis.z*fOneMinusCos;
			float fYZM = rkAxis.y*rkAxis.z*fOneMinusCos;
			float fXSin = rkAxis.x*fSin;
			float fYSin = rkAxis.y*fSin;
			float fZSin = rkAxis.z*fSin;

			f[0] = fX2*fOneMinusCos+fCos;
			f[1] = fXYM-fZSin;
			f[2] = fXZM+fYSin;
			f[3] = fXYM+fZSin;
			f[4] = fY2*fOneMinusCos+fCos;
			f[5] = fYZM-fXSin;
			f[6] = fXZM-fYSin;
			f[7] = fYZM+fXSin;
			f[8] = fZ2*fOneMinusCos+fCos;
		}



		Matrix3 Matrix3::inverse() const
		{
			Matrix3 out;

			float recDet = determinant();
			assert(recDet!=0);
			recDet = 1.0f/recDet;

			out.f[0] = f[4]*f[8] - f[5]*f[7];
			out.f[1] = f[2]*f[7] - f[1]*f[8];
			out.f[2] = f[1]*f[5] - f[2]*f[4];

			out.f[3] = f[5]*f[6] - f[3]*f[8];
			out.f[4] = f[0]*f[8] - f[2]*f[6];
			out.f[5] = f[2]*f[3] - f[0]*f[5];

			out.f[6] = f[3]*f[7] - f[4]*f[6];
			out.f[7] = f[1]*f[6] - f[0]*f[7];
			out.f[8] = f[0]*f[4] - f[1]*f[3];

			out *= recDet;
			return out;
		}

		Matrix3 Matrix3::transpose() const
		{
			Matrix3 out;
			out.f[0] = f[0];	out.f[3] = f[1];	out.f[6] = f[2];
			out.f[1] = f[3];	out.f[4] = f[4];	out.f[7] = f[5];
			out.f[2] = f[6];	out.f[5] = f[7];	out.f[8] = f[8];
			return out;
		}

		inline float Matrix3::determinant() const
		{
			return f[0]*(f[4]*f[8]-f[7]*f[5])
				- (f[3]*(f[1]*f[8]-f[7]*f[2]))
				+ (f[6]*(f[1]*f[5]-f[4]*f[2]));
		}



}
