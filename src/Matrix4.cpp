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

#include "Matrix4.h"
#include <math.h>
#include "Vector3.h"
#include "Matrix3.h"
#include "Quat.h"
namespace MiniEngine
{
    const Matrix4 Matrix4::Zero(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    const Matrix4 Matrix4::Identity(1, 0, 0, 0,0, 1, 0, 0,0, 0, 1, 0,0, 0, 0, 1 );




		Matrix4 Matrix4::operator+(const Matrix4& rhs) const
		{
			Matrix4 out;
			float const *lptr = f, *rptr = rhs.f;
			float *outptr = out.f;
			for(int i=0;i<16;i++)
			{
				(*outptr++) = (*lptr++) + (*rptr++);
			}
			return out;
		}	

		Matrix4 Matrix4::operator-(const Matrix4& rhs) const
		{
			Matrix4 out;
			for(int i=0;i<16;i++)
			{
				out.f[i] = f[i]-rhs.f[i];
			}
			return out;
		}

		Matrix4& Matrix4::operator+=(const Matrix4& rhs)
		{
			float *lptr = f;
			float const *rptr = rhs.f;
			for(int i=0;i<16;i++)
			{
				(*lptr++) += (*rptr++);
			}
			return *this;
		}

		Matrix4& Matrix4::operator-=(const Matrix4& rhs)
		{
			float *lptr = f;
			float const *rptr = rhs.f;
			for(int i=0;i<16;i++)
			{
				(*lptr++) -= (*rptr++);
			}
			return *this;
		}

//	012
//	345
//	678



//	0	1	2	3	
//	4	5	6	7	
//	8   9   10  11  
//	12  13  14  15  
/*
			out.f[0] =	f[0]*rhs.f[0]  +  f[1]*rhs.f[3]  +  f[2]*rhs.f[6];
			out.f[1] =	f[0]*rhs.f[1]  +  f[1]*rhs.f[4]  +  f[2]*rhs.f[7];
			out.f[2] =	f[0]*rhs.f[2]  +  f[1]*rhs.f[5]  +  f[2]*rhs.f[8];

			out.f[3] =	f[3]*rhs.f[0]+f[4]*rhs.f[3]+f[5]*rhs.f[6];
			out.f[4] =	f[3]*rhs.f[1]+f[4]*rhs.f[4]+f[5]*rhs.f[7];
			out.f[5] =	f[3]*rhs.f[2]+f[4]*rhs.f[5]+f[5]*rhs.f[8];

			out.f[6] =	f[6]*rhs.f[0]+f[7]*rhs.f[3]+f[8]*rhs.f[6];
			out.f[7] =	f[6]*rhs.f[1]+f[7]*rhs.f[4]+f[8]*rhs.f[7];
			out.f[8] =	f[6]*rhs.f[2]+f[7]*rhs.f[5]+f[8]*rhs.f[8];
*/
		Matrix4 Matrix4::operator*(const Matrix4& rhs) const
		{
			Matrix4 result;

			result.f[0] =	f[0]*rhs.f[0]  +  f[4]*rhs.f[1]  +  f[8]*rhs.f[2]  +  f[12]*rhs.f[3];
			result.f[1] =	f[1]*rhs.f[0]  +  f[5]*rhs.f[1]  +  f[9]*rhs.f[2]  +  f[13]*rhs.f[3];
			result.f[2] =	f[2]*rhs.f[0]  +  f[6]*rhs.f[1]  +  f[10]*rhs.f[2] +  f[14]*rhs.f[3];
			result.f[3] =	f[3]*rhs.f[0]  +  f[7]*rhs.f[1]  +  f[11]*rhs.f[2] +  f[15]*rhs.f[3];

			result.f[4] =	f[0]*rhs.f[4]  +  f[4]*rhs.f[5]  +  f[8]*rhs.f[6]  +  f[12]*rhs.f[7];
			result.f[5] =	f[1]*rhs.f[4]  +  f[5]*rhs.f[5]  +  f[9]*rhs.f[6]  +  f[13]*rhs.f[7];
			result.f[6] =	f[2]*rhs.f[4]  +  f[6]*rhs.f[5]  +  f[10]*rhs.f[6] +  f[14]*rhs.f[7];
			result.f[7] =	f[3]*rhs.f[4]  +  f[7]*rhs.f[5]  +  f[11]*rhs.f[6] +  f[15]*rhs.f[7];

			result.f[8] =	f[0]*rhs.f[8]  +  f[4]*rhs.f[9]  +  f[8]*rhs.f[10] +  f[12]*rhs.f[11];
			result.f[9] =	f[1]*rhs.f[8]  +  f[5]*rhs.f[9]  +  f[9]*rhs.f[10] +  f[13]*rhs.f[11];
			result.f[10] =	f[2]*rhs.f[8]  +  f[6]*rhs.f[9]  +  f[10]*rhs.f[10]+  f[14]*rhs.f[11];
			result.f[11] =	f[3]*rhs.f[8]  +  f[7]*rhs.f[9]  +  f[11]*rhs.f[10]+  f[15]*rhs.f[11];

			result.f[12] =	f[0]*rhs.f[12] +  f[4]*rhs.f[13] +  f[8]*rhs.f[14] +  f[12]*rhs.f[15];
			result.f[13] =	f[1]*rhs.f[12] +  f[5]*rhs.f[13] +  f[9]*rhs.f[14] +  f[13]*rhs.f[15];
			result.f[14] =	f[2]*rhs.f[12] +  f[6]*rhs.f[13] +  f[10]*rhs.f[14]+  f[14]*rhs.f[15];
			result.f[15] =	f[3]*rhs.f[12] +  f[7]*rhs.f[13] +  f[11]*rhs.f[14]+  f[15]*rhs.f[15];

			return result;
		}

		Matrix4& Matrix4::operator*=(const Matrix4& rhs)
		{
			Matrix4 result;

			result.f[0] =	f[0]*rhs.f[0]  +  f[4]*rhs.f[1]  +  f[8]*rhs.f[2]  +  f[12]*rhs.f[3];
			result.f[1] =	f[1]*rhs.f[0]  +  f[5]*rhs.f[1]  +  f[9]*rhs.f[2]  +  f[13]*rhs.f[3];
			result.f[2] =	f[2]*rhs.f[0]  +  f[6]*rhs.f[1]  +  f[10]*rhs.f[2] +  f[14]*rhs.f[3];
			result.f[3] =	f[3]*rhs.f[0]  +  f[7]*rhs.f[1]  +  f[11]*rhs.f[2] +  f[15]*rhs.f[3];

			result.f[4] =	f[0]*rhs.f[4]  +  f[4]*rhs.f[5]  +  f[8]*rhs.f[6]  +  f[12]*rhs.f[7];
			result.f[5] =	f[1]*rhs.f[4]  +  f[5]*rhs.f[5]  +  f[9]*rhs.f[6]  +  f[13]*rhs.f[7];
			result.f[6] =	f[2]*rhs.f[4]  +  f[6]*rhs.f[5]  +  f[10]*rhs.f[6] +  f[14]*rhs.f[7];
			result.f[7] =	f[3]*rhs.f[4]  +  f[7]*rhs.f[5]  +  f[11]*rhs.f[6] +  f[15]*rhs.f[7];

			result.f[8] =	f[0]*rhs.f[8]  +  f[4]*rhs.f[9]  +  f[8]*rhs.f[10] +  f[12]*rhs.f[11];
			result.f[9] =	f[1]*rhs.f[8]  +  f[5]*rhs.f[9]  +  f[9]*rhs.f[10] +  f[13]*rhs.f[11];
			result.f[10] =	f[2]*rhs.f[8]  +  f[6]*rhs.f[9]  +  f[10]*rhs.f[10]+  f[14]*rhs.f[11];
			result.f[11] =	f[3]*rhs.f[8]  +  f[7]*rhs.f[9]  +  f[11]*rhs.f[10]+  f[15]*rhs.f[11];

			result.f[12] =	f[0]*rhs.f[12] +  f[4]*rhs.f[13] +  f[8]*rhs.f[14] +  f[12]*rhs.f[15];
			result.f[13] =	f[1]*rhs.f[12] +  f[5]*rhs.f[13] +  f[9]*rhs.f[14] +  f[13]*rhs.f[15];
			result.f[14] =	f[2]*rhs.f[12] +  f[6]*rhs.f[13] +  f[10]*rhs.f[14]+  f[14]*rhs.f[15];
			result.f[15] =	f[3]*rhs.f[12] +  f[7]*rhs.f[13] +  f[11]*rhs.f[14]+  f[15]*rhs.f[15];

			*this = result;
			return *this;
		}
		Matrix4 Matrix4::operator*(const float rhs) const
		{
			return Matrix4(f[0]*rhs,f[1]*rhs,f[2]*rhs,f[3]*rhs,f[4]*rhs,f[5]*rhs,f[6]*rhs,f[7]*rhs,f[8]*rhs,f[9]*rhs,f[10]*rhs,f[11]*rhs,
							f[12]*rhs,f[13]*rhs,f[14]*rhs,f[15]*rhs);
		}


		Matrix4& Matrix4::operator*(const float rhs)
		{
			for (int i=0; i<16; ++i)
			{
				f[i]*=rhs;
			}
			return *this;
		}
		Matrix4& Matrix4::operator*=(const float rhs)
		{
			for (int i=0; i<16; ++i)
			{
				f[i]*=rhs;
			}
			return *this;
		}

		Matrix4 Matrix4::createLookAt(const Vector3 &position,const Vector3 &at,const Vector3 &up)
		{
			Vector3 f=(at-position).normalized();
			Vector3 s=f.cross(up);
			Vector3 u=s.cross(f);

			Matrix4 out;

			out.f[ 0] = s.x;
			out.f[ 1] = u.x;
			out.f[ 2] = -f.x;
			out.f[ 3] = 0;

			out.f[ 4] = s.y;
			out.f[ 5] = u.y;
			out.f[ 6] = -f.y;
			out.f[ 7] = 0;

			out.f[ 8] = s.z;
			out.f[ 9] = u.z;
			out.f[10] = -f.z;
			out.f[11] = 0;

			out.f[12] = 0;
			out.f[13] = 0;
			out.f[14] = 0;
			out.f[15] = 1;

			return out*Matrix4::createTranslation(-position.x,-position.y,-position.z);

		}

		void Matrix4::makeTransform(float *matrixArray,const Vector3& position, const Vector3& scale, const Quat& orientation)
		{
			Matrix3 rot3x3, scale3x3;
			orientation.toRotationMatrix(rot3x3);
			
			memset(scale3x3.f,0,sizeof(float)*9);			
			scale3x3.f[0] = scale.x;
			scale3x3.f[4] = scale.y;
			scale3x3.f[8] = scale.z;
			Matrix3 res3=scale3x3*rot3x3 ;
            matrixArray[0] = res3.f[0]; matrixArray[1] = res3.f[1]; matrixArray[2] = res3.f[2];
            matrixArray[4] = res3.f[3]; matrixArray[5] = res3.f[4]; matrixArray[6] = res3.f[5];
            matrixArray[8] = res3.f[6]; matrixArray[9] = res3.f[7]; matrixArray[10] = res3.f[8];
			matrixArray[3]=0;	
			matrixArray[7]=0;
			matrixArray[11]=0;
			matrixArray[12] = position.x;
			matrixArray[13] = position.y;
			matrixArray[14] = position.z;
			matrixArray[15]=1;	

		}


		void Matrix4::makeTransform(const Vector3& position, const Vector3& scale, const Quat& orientation)
		{
			Matrix3 rot3x3, scale3x3;
			orientation.toRotationMatrix(rot3x3);
			
			memset(scale3x3.f,0,sizeof(float)*9);

			scale3x3.f[0] = scale.x;
			scale3x3.f[4] = scale.y;
			scale3x3.f[8] = scale.z;
			*this =   scale3x3*rot3x3 ;
			f[3]=0;	
			f[7]=0;
			f[11]=0;
			f[12] = position.x;
			f[13] = position.y;
			f[14] = position.z;
			f[15]=1;		

		}


		void Matrix4::operator = ( const Matrix3& mat3 )
        {
            f[0] = mat3.f[0]; f[1] = mat3.f[1]; f[2] = mat3.f[2];
            f[4] = mat3.f[3]; f[5] = mat3.f[4]; f[6] = mat3.f[5];
            f[8] = mat3.f[6]; f[9] = mat3.f[7]; f[10] = mat3.f[8];
        }
  

		Matrix4  Matrix4::createPerspectiveFov(float fov, float aspect, float nearClip, float farClip, float rotationDegrees)
		{

	
			float height = (2*nearClip)*tan(fov*0.5f); 
			float width =height*aspect;

			float n2 =2.f*nearClip;
			float rcpnmf = 1.0f/(nearClip - farClip);

			Matrix4 result;

			result.f[0] = n2/width;	result.f[4]=0;	result.f[8]=0;	result.f[12]=0;
			result.f[1] = 0;	result.f[5]=n2/height;	result.f[9]=0;	result.f[13]=0;			
			result.f[2] = 0;	result.f[6]=0;	result.f[10]=(farClip+nearClip)*rcpnmf;	result.f[14]=(farClip*rcpnmf)*n2;			
			result.f[3] = 0;	result.f[7]=0;	result.f[11]=-1;	result.f[15]=0;

			if(rotationDegrees!=0)
			{
				Matrix4 rotation=Matrix4::createRotationZ(MathUtils::DegreeToRadian(rotationDegrees));
				result=rotation*result;
			}
			
			return result;
		}


		Matrix4  Matrix4::inverse() const
		{
			Matrix4 result;

			float num5 = this->f[MAT00];
			float num4 = this->f[MAT01];
			float num3 = this->f[MAT02];
			float num2 = this->f[MAT03];
			float num9 = this->f[MAT10];
			
			float num8 = this->f[MAT11];
			float num7 = this->f[MAT12];
			float num6 = this->f[MAT13];
			float num17 = this->f[MAT20];
			float num16 = this->f[MAT21];
			float num15 = this->f[MAT22];
			float num14 = this->f[MAT23];
			float num13 = this->f[MAT30];
			float num12 = this->f[MAT31];
			float num11 = this->f[MAT32];
			float num10 = this->f[MAT33];
			float num23 = (num15 * num10) - (num14 * num11);
			float num22 = (num16 * num10) - (num14 * num12);
			float num21 = (num16 * num11) - (num15 * num12);
			float num20 = (num17 * num10) - (num14 * num13);
			float num19 = (num17 * num11) - (num15 * num13);
			float num18 = (num17 * num12) - (num16 * num13);
			float num39 = ((num8 * num23) - (num7 * num22)) + (num6 * num21);
			float num38 = -(((num9 * num23) - (num7 * num20)) + (num6 * num19));
			float num37 = ((num9 * num22) - (num8 * num20)) + (num6 * num18);
			float num36 = -(((num9 * num21) - (num8 * num19)) + (num7 * num18));
			float num = 1.0f / ((((num5 * num39) + (num4 * num38)) + (num3 * num37)) + (num2 * num36));
			result.f[MAT00] = (num39 * num);
			result.f[MAT10] = (num38 * num);
			result.f[MAT20] = (num37 * num);
			result.f[MAT30] = (num36 * num);
			result.f[MAT01] = (-(((num4 * num23) - (num3 * num22)) + (num2 * num21)) * num);
			result.f[MAT11] = ((((num5 * num23) - (num3 * num20)) + (num2 * num19)) * num);
			result.f[MAT21] = (-(((num5 * num22) - (num4 * num20)) + (num2 * num18)) * num);
			result.f[MAT31] = ((((num5 * num21) - (num4 * num19)) + (num3 * num18)) * num);
			float num35 = (num7 * num10) - (num6 * num11);
			float num34 = (num8 * num10) - (num6 * num12);
			float num33 = (num8 * num11) - (num7 * num12);
			float num32 = (num9 * num10) - (num6 * num13);
			float num31 = (num9 * num11) - (num7 * num13);
			float num30 = (num9 * num12) - (num8 * num13);
			result.f[MAT02] = ((((num4 * num35) - (num3 * num34)) + (num2 * num33)) * num);
			result.f[MAT12] = (-(((num5 * num35) - (num3 * num32)) + (num2 * num31)) * num);
			result.f[MAT22] = ((((num5 * num34) - (num4 * num32)) + (num2 * num30)) * num);
			result.f[MAT32] = (-(((num5 * num33) - (num4 * num31)) + (num3 * num30)) * num);
			float num29 = (num7 * num14) - (num6 * num15);
			float num28 = (num8 * num14) - (num6 * num16);
			float num27 = (num8 * num15) - (num7 * num16);
			float num26 = (num9 * num14) - (num6 * num17);
			float num25 = (num9 * num15) - (num7 * num17);
			float num24 = (num9 * num16) - (num8 * num17);
			result.f[MAT03] = (-(((num4 * num29) - (num3 * num28)) + (num2 * num27)) * num);
			result.f[MAT13] = ((((num5 * num29) - (num3 * num26)) + (num2 * num25)) * num);
			result.f[MAT23] = (-(((num5 * num28) - (num4 * num26)) + (num2 * num24)) * num);
			result.f[MAT33] = ((((num5 * num27) - (num4 * num25)) + (num3 * num24)) * num);

			return result;
		}
	
		void Matrix4::multiply(float *matrices, const float *matrix, size_t count)
		{

			float tmpMat[16];

			for(size_t i=0;i<count;i++)
			{

				tmpMat[0] =	matrix[0]*matrices[0]  + matrix[4]*matrices[1]  + matrix[8]*matrices[2]  + matrix[12]*matrices[3];
				tmpMat[1] =	matrix[1]*matrices[0]  + matrix[5]*matrices[1]  + matrix[9]*matrices[2]  + matrix[13]*matrices[3];
				tmpMat[2] =	matrix[2]*matrices[0]  + matrix[6]*matrices[1]  + matrix[10]*matrices[2] + matrix[14]*matrices[3];
				tmpMat[3] =	matrix[3]*matrices[0]  + matrix[7]*matrices[1]  + matrix[11]*matrices[2] + matrix[15]*matrices[3];

				tmpMat[4] =	matrix[0]*matrices[4]  + matrix[4]*matrices[5]  + matrix[8]*matrices[6]  + matrix[12]*matrices[7];
				tmpMat[5] =	matrix[1]*matrices[4]  + matrix[5]*matrices[5]  + matrix[9]*matrices[6]  + matrix[13]*matrices[7];
				tmpMat[6] =	matrix[2]*matrices[4]  + matrix[6]*matrices[5]  + matrix[10]*matrices[6] + matrix[14]*matrices[7];
				tmpMat[7] =	matrix[3]*matrices[4]  + matrix[7]*matrices[5]  + matrix[11]*matrices[6] + matrix[15]*matrices[7];

				tmpMat[8] =	matrix[0]*matrices[8]  + matrix[4]*matrices[9]  + matrix[8]*matrices[10] + matrix[12]*matrices[11];
				tmpMat[9] =	matrix[1]*matrices[8]  + matrix[5]*matrices[9]  + matrix[9]*matrices[10] + matrix[13]*matrices[11];
				tmpMat[10] =	matrix[2]*matrices[8]  + matrix[6]*matrices[9]  + matrix[10]*matrices[10]+ matrix[14]*matrices[11];
				tmpMat[11] =	matrix[3]*matrices[8]  + matrix[7]*matrices[9]  + matrix[11]*matrices[10]+ matrix[15]*matrices[11];

				tmpMat[12] =	matrix[0]*matrices[12] + matrix[4]*matrices[13] + matrix[8]*matrices[14] + matrix[12]*matrices[15];
				tmpMat[13] =	matrix[1]*matrices[12] + matrix[5]*matrices[13] + matrix[9]*matrices[14] + matrix[13]*matrices[15];
				tmpMat[14] =	matrix[2]*matrices[12] + matrix[6]*matrices[13] + matrix[10]*matrices[14]+ matrix[14]*matrices[15];
				tmpMat[15] =	matrix[3]*matrices[12] + matrix[7]*matrices[13] + matrix[11]*matrices[14]+ matrix[15]*matrices[15];


				memcpy(matrices,tmpMat,16*sizeof(float));

				matrices+=16;

			}

		}
	

}
