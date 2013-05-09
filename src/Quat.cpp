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

#include "Quat.h"
#include "Vector3.h"
#include "Matrix3.h"
#include "MathUtils.h"
namespace MiniEngine
{
	const Quat Quat::Zero( 0, 0, 0,0 );
	const Quat Quat::Identity( 0, 0, 0 ,1);



	void Quat::fromAngleAxis (const float& radianAngle, const Vector3& axis)
	{
	
		float vSin;

		vSin=sin(0.5f*radianAngle);
		w=cos(0.5f*radianAngle);

		x = vSin*axis.x;
		y = vSin*axis.y;
		z = vSin*axis.z;
	}



	void Quat::toAngleAxis (float& radianAngle, Vector3& axis) const
	{
		float	fSinAngle;
		double	temp;

		temp		= 1.0f - w*w;
		radianAngle	= MathUtils::ACos(w)*2.0f;
		fSinAngle	= (float)sqrtf((float)temp);

		if ((float)MathUtils::Abs(fSinAngle)<0.0005f)
			fSinAngle = 1.0f;

		axis.x = x / fSinAngle;
		axis.y = y / fSinAngle;
		axis.z = z / fSinAngle;
	}

    void Quat::fromAxes (const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
	{
		Matrix3 mat;

		mat[0][0] = xAxis.x;
        mat[1][0] = xAxis.y;
        mat[2][0] = xAxis.z;

        mat[0][1] = yAxis.x;
        mat[1][1] = yAxis.y;
        mat[2][1] = yAxis.z;

        mat[0][2] = zAxis.x;
        mat[1][2] = zAxis.y;
        mat[2][2] = zAxis.z;

        fromRotationMatrix(mat);
	}
    void Quat::toAxes (Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const
	{
        Matrix3 mat;

        toRotationMatrix(mat);

        xAxis.x = mat[0][0];
        xAxis.y = mat[1][0];
        xAxis.z = mat[2][0];

        yAxis.x = mat[0][1];
        yAxis.y = mat[1][1];
        yAxis.z = mat[2][1];

        zAxis.x = mat[0][2];
        zAxis.y = mat[1][2];
        zAxis.z = mat[2][2];

	}

	Vector3 Quat::xAxis(void) const
	{
		float fTy  = 2.0f*y;
		float fTz  = 2.0f*z;
		float fTwy = fTy*w;
		float fTwz = fTz*w;
		float fTxy = fTy*x;
		float fTxz = fTz*x;
		float fTyy = fTy*y;
		float fTzz = fTz*z;

		return Vector3(1.0f-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);
	}
	Vector3 Quat::yAxis(void) const
	{
		float fTx  = 2.0f*x;
		float fTy  = 2.0f*y;
		float fTz  = 2.0f*z;
		float fTwx = fTx*w;
		float fTwz = fTz*w;
		float fTxx = fTx*x;
		float fTxy = fTy*x;
		float fTyz = fTz*y;
		float fTzz = fTz*z;

		return Vector3(fTxy-fTwz, 1.0f-(fTxx+fTzz), fTyz+fTwx);
	}
	Vector3 Quat::zAxis(void) const
	{
		float fTx  = 2.0f*x;
		float fTy  = 2.0f*y;
		float fTz  = 2.0f*z;
		float fTwx = fTx*w;
		float fTwy = fTy*w;
		float fTxx = fTx*x;
		float fTxz = fTz*x;
		float fTyy = fTy*y;
		float fTyz = fTz*y;

		return Vector3(fTxz+fTwy, fTyz-fTwx, 1.0f-(fTxx+fTyy));
	}

	void Quat::toRotationMatrix(float *mat3x3) const 
	{
		Quat q;
		q.x=-x;
		q.y=-y;
		q.z=-z;
 
		float fTx  = 2.0f*q.x;
		float fTy  = 2.0f*q.y;
		float fTz  = 2.0f*q.z;
		float fTwx = fTx*w;
		float fTwy = fTy*w;
		float fTwz = fTz*w;
		float fTxx = fTx*q.x;
		float fTxy = fTy*q.x;
		float fTxz = fTz*q.x;
		float fTyy = fTy*q.y;
		float fTyz = fTz*q.y;
		float fTzz = fTz*q.z;

		mat3x3[0] = 1.0f-(fTyy+fTzz);
		mat3x3[1] = fTxy-fTwz;
		mat3x3[2] = fTxz+fTwy;
		mat3x3[3] = fTxy+fTwz;
		mat3x3[4] = 1.0f-(fTxx+fTzz);
		mat3x3[5] = fTyz-fTwx;
		mat3x3[6] = fTxz-fTwy;
		mat3x3[7] = fTyz+fTwx;
		mat3x3[8] = 1.0f-(fTxx+fTyy);

	}

	void Quat::toRotationMatrix (Matrix3& mOut)  const
	{
	
		Quat q;
		q.x=-x;
		q.y=-y;
		q.z=-z;

		//q.x=x;
		//q.y=y;
		//q.z=z;
		//q.w=w;
 
		float fTx  = 2.0f*q.x;
		float fTy  = 2.0f*q.y;
		float fTz  = 2.0f*q.z;
		float fTwx = fTx*w;
		float fTwy = fTy*w;
		float fTwz = fTz*w;
		float fTxx = fTx*q.x;
		float fTxy = fTy*q.x;
		float fTxz = fTz*q.x;
		float fTyy = fTy*q.y;
		float fTyz = fTz*q.y;
		float fTzz = fTz*q.z;

		mOut.f[0] = 1.0f-(fTyy+fTzz);
		mOut.f[1] = fTxy-fTwz;
		mOut.f[2] = fTxz+fTwy;
		mOut.f[3] = fTxy+fTwz;
		mOut.f[4] = 1.0f-(fTxx+fTzz);
		mOut.f[5] = fTyz-fTwx;
		mOut.f[6] = fTxz-fTwy;
		mOut.f[7] = fTyz+fTwx;
		mOut.f[8] = 1.0f-(fTxx+fTyy);


	}

	void Quat::fromRotationMatrix(const Matrix3& mMatrix)
	{
		float fTrace = mMatrix[0][0]+mMatrix[1][1]+mMatrix[2][2];
		float fRoot;

		if ( fTrace > 0.0 )
		{
			fRoot = sqrtf(fTrace + 1.0f);  
			w = 0.5f*fRoot;
			fRoot = 0.5f/fRoot;  
			x = (mMatrix[2][1]-mMatrix[1][2])*fRoot;
			y = (mMatrix[0][2]-mMatrix[2][0])*fRoot;
			z = (mMatrix[1][0]-mMatrix[0][1])*fRoot;
		}
		else
		{
			static size_t s_iNext[3] = { 1, 2, 0 };
			size_t i = 0;
			if ( mMatrix[1][1] > mMatrix[0][0] )
				i = 1;
			if ( mMatrix[2][2] > mMatrix[i][i] )
				i = 2;
			size_t j = s_iNext[i];
			size_t k = s_iNext[j]; 

			fRoot = sqrtf(mMatrix[i][i]-mMatrix[j][j]-mMatrix[k][k] + 1.0f);
			float* apkQuat[3] = { &x, &y, &z };
			*apkQuat[i] = 0.5f*fRoot;
			fRoot = 0.5f/fRoot;
			w = (mMatrix[k][j]-mMatrix[j][k])*fRoot;
			*apkQuat[j] = (mMatrix[j][i]+mMatrix[i][j])*fRoot;
			*apkQuat[k] = (mMatrix[k][i]+mMatrix[i][k])*fRoot;
		} 

	}

	float Quat::getYaw() const
	{
		float fTx  = 2.0f*x;
		float fTy  = 2.0f*y;
		float fTz  = 2.0f*z;
		float fTwy = fTy*w;
		float fTxx = fTx*x;
		float fTxz = fTz*x;
		float fTyy = fTy*y;


		return atan2(fTxz+fTwy, 1.0f-(fTxx+fTyy));
	}

	float Quat::getPitch() const
	{
			float fTx  = 2.0f*x;
			float fTy  = 2.0f*y;
			float fTz  = 2.0f*z;
			float fTwx = fTx*w;
			float fTxx = fTx*x;
			float fTyz = fTz*y;
			float fTzz = fTz*z;

			return atan2(fTyz+fTwx, 1.0f-(fTxx+fTzz));
	}

	float Quat::getRoll() const
	{
			float fTx  = 2.0f*x;
			float fTy  = 2.0f*y;
			float fTz  = 2.0f*z;
			float fTwz = fTz*w;
			float fTxy = fTy*x;
			float fTyy = fTy*y;
			float fTzz = fTz*z;

			return atan2(fTxy+fTwz, 1.0f-(fTyy+fTzz));
	}

	Quat Quat::slerp (float fT, const Quat& rkP, const Quat& rkQ, bool shortestPath)
	{
        float fCos = rkP.dot(rkQ);
        Quat rkT;


        if (fCos < 0.0f && shortestPath)
        {
            fCos = -fCos;
            rkT = -rkQ;
        }
        else
        {
            rkT = rkQ;
        }

		if (MathUtils::Abs(fCos) < 1 - 1e-03)
        {

			float fSin = sqrtf(1 - fCos*fCos);
            float fAngle = atan2(fSin, fCos);
            float fInvSin = 1.0f / fSin;
			float fCoeff0 = MathUtils::Sin((1.0f - fT) * fAngle) * fInvSin;
            float fCoeff1 = MathUtils::Sin(fT * fAngle) * fInvSin;
            return fCoeff0 * rkP + fCoeff1 * rkT;
        }
        else
        {

            Quat t = (1.0f - fT) * rkP + fT * rkT;

            t.normalize();
            return t;
        }

	}

	Quat Quat::clerp(float fT, const Quat& rkP, const Quat& rkQ, bool shortestPath)
	{
		Quat result;
		float fCos = rkP.dot(rkQ);

		const float val=(1-MathUtils::Cos(fT*MathUtils::PIW))/2;

		if (fCos < 0.0f && shortestPath)
		{
			result = rkP + val * ((-rkQ) - rkP);
		}
		else
		{
			result = rkP + val * (rkQ - rkP);
		}
		result.normalize();
		return result;
	}

	Quat Quat::nlerp(float fT, const Quat& rkP, const Quat& rkQ, bool shortestPath)
	{
		Quat result;
		float fCos = rkP.dot(rkQ);
		if (fCos < 0.0f && shortestPath)
		{
			result = rkP + fT * ((-rkQ) - rkP);
		}
		else
		{
			result = rkP + fT * (rkQ - rkP);
		}
		result.normalize();
		return result;
	}

	Quat Quat::squad (float fT,const Quat& rkP, const Quat& rkA,const Quat& rkB, const Quat& rkQ, bool shortestPath)
	{
        float fSlerpT = 2.0f*fT*(1.0f-fT);
        Quat kSlerpP = slerp(fT, rkP, rkQ, shortestPath);
        Quat kSlerpQ = slerp(fT, rkA, rkB);
        return slerp(fSlerpT, kSlerpP ,kSlerpQ);
	}

	

    Vector3 Quat::operator* (const Vector3& v) const
    {
		Vector3 uv, uuv;
		Vector3 qvec(x, y, z);
		uv = qvec.cross(v);
		uuv = qvec.cross(uv);
		uv *= (2.0f * w);
		uuv *= 2.0f;

		return v + uv + uuv;
    }

	Quat Quat::inverse () const
    {
        float fNorm = w*w+x*x+y*y+z*z;
        if ( fNorm > 0.0 )
        {
            float fInvNorm = 1.0f/fNorm;
            return Quat(-x*fInvNorm,-y*fInvNorm,-z*fInvNorm,w*fInvNorm);
        }
        else
        {
			return Quat::Zero;
        }
    }

    //-----------------------------------------------------------------------
    Quat Quat::log () const
    {

        Quat kResult;
        kResult.w = 0.0;

        if ( MathUtils::Abs(w) < 1.0 )
        {
            float fAngle ( acos(w) );
			float fSin = MathUtils::Sin(fAngle);
            if ( MathUtils::Abs(fSin) >= 1e-03f )
            {
                float fCoeff = fAngle/fSin;
                kResult.x = fCoeff*x;
                kResult.y = fCoeff*y;
                kResult.z = fCoeff*z;
                return kResult;
            }
        }

        kResult.x = x;
        kResult.y = y;
        kResult.z = z;

        return kResult;
    }

    Quat Quat::exp () const
    {

		float fAngle ( sqrtf(x*x+y*y+z*z) );

		float fSin;
        Quat kResult;

		MathUtils::SinCos(fAngle,fSin,kResult.w);

        if ( MathUtils::Abs(fSin) >= 1e-03f )
        {
            float fCoeff = fSin/(fAngle);
            kResult.x = fCoeff*x;
            kResult.y = fCoeff*y;
            kResult.z = fCoeff*z;
        }
        else
        {
            kResult.x = x;
            kResult.y = y;
            kResult.z = z;
        }

        return kResult;
    }

}
