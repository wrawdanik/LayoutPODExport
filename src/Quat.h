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


#ifndef QUAT_H
#define QUAT_H

#include <ostream>
#include <math.h>
#include "MathUtils.h"

namespace MiniEngine
{

	class Vector3;
	class Matrix3;

	class Quat
	{
	public:

		mutable float x;	
		mutable float y;	
		mutable float z;	
		mutable float w;	

		inline Quat() {}

		inline Quat(float vx, float vy, float vz, float vw) 
		{
			x=vx; y= vy; z=vz; w=vw;
		}

		inline Quat (const Quat& rhs)
		{			
			x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w;			
		}

		inline Quat(const float* val)
		{
			x=(*val++); y=(*val++); z=(*val++); w=(*val);
		}

		inline Quat(const float& radianAngle, const Vector3& axis)
		{
			this->fromAngleAxis(radianAngle, axis);
		}


        inline Quat(const Matrix3& mat)
        {
            this->fromRotationMatrix(mat);
        }
        inline Quat(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
        {
            this->fromAxes(xaxis, yaxis, zaxis);
        }


		inline float* ptr()  
		{
			return &x;
		}

		inline const float* ptr() const
		{
			return &x;
		}

		inline float operator [] ( const size_t i ) const
		{		
			return *(&x+i);
		}

		inline float& operator [] ( const size_t i )
		{
			return *(&x+i);
		}

		inline Quat& operator= (const Quat& rhs)
		{
			x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w;
			return *this;
		}

		inline bool operator== (const Quat& rhs) const
		{
			return (rhs.x == x) && (rhs.y == y) && (rhs.z == z) && (rhs.w == w);
		}
		inline bool operator!= (const Quat& rhs) const
		{
			return ( x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w);
		}

		inline Quat operator+ (const Quat& rhs) const
		{
			return Quat(x+rhs.x,y+rhs.y,z+rhs.z,w+rhs.w);
		}
		//-----------------------------------------------------------------------
		inline Quat operator- (const Quat& rhs) const
		{
			return Quat(x-rhs.x,y-rhs.y,z-rhs.z,w-rhs.w);
		}
		//-----------------------------------------------------------------------
		inline Quat operator* (const Quat& rhs) const
		{
			return Quat
				(            
				w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
				w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
				w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x,
				w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z
				);
		}


		//-----------------------------------------------------------------------
		inline Quat operator* (float val) const
		{
			return Quat(val*x,val*y,val*z,val*w);
		}
		//-----------------------------------------------------------------------
		inline Quat friend operator* (float val, const Quat& rhs)
		{
			return Quat(val*rhs.x,val*rhs.y,val*rhs.z,val*rhs.w);
		}
		//-----------------------------------------------------------------------
		inline Quat operator- () const
		{
			return Quat(-x,-y,-z,-w);
		}

		Vector3 operator* (const Vector3& rhs) const;

		// methods

		inline float dot (const Quat& rkQ) const
		{
			return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;
		}

		Quat inverse () const;

		inline Quat unitInverse () const
		{
			return Quat(-x,-y,-z,w);
		}

		inline float normalize(void)
		{
			float len = w*w+x*x+y*y+z*z;
			float factor = 1.0f / sqrtf(len);
			*this = *this * factor;
			return len;
		}

		void fromAngleAxis (const float& radianAngle, const Vector3& axis);
		void toAngleAxis (float& radianAngle, Vector3& axis) const;

        void fromAxes (const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);
        void toAxes (Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const;

		void toRotationMatrix (Matrix3& mOut) const;
		void toRotationMatrix (float  *mat3x3) const;
		void fromRotationMatrix(const Matrix3& mat);

		Vector3 xAxis(void) const;
		Vector3 yAxis(void) const;
		Vector3 zAxis(void) const;


		float getYaw() const;
		float getPitch() const;
		float getRoll() const;

		Quat log () const;
		Quat exp () const;

        inline  friend std::ostream& operator <<
            ( std::ostream& o, const Quat& q )
        {
            o << "Quat(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
            return o;
        }

		static	   Quat nlerpShortPath(float fT, const Quat &from, const Quat &to )
		{
			float cosom,scale0,scale1,s;

			cosom = from.x * to.x + from.y * to.y + from.z * to.z + from.w * to.w; 

			scale0 = 1.0f - fT; 
			scale1 = ( cosom >= 0.0f ) ? fT : -fT; 

			Quat result;

			result.x = scale0 * from.x + scale1 * to.x; 
			result.y = scale0 * from.y + scale1 * to.y; 
			result.z = scale0 * from.z + scale1 * to.z; 
			result.w = scale0 * from.w + scale1 * to.w; 

			s = 1.0f / sqrtf( result.x * result.x + result.y * result.y + result.z * result.z + result.w * result.w );

			result.x *= s; 
			result.y *= s; 
			result.z *= s; 
			result.w *= s; 

			return result;
		}


		static     Quat slerp (float fT, const Quat& rkP, const Quat& rkQ, bool shortestPath=false);
		static     Quat nlerp(float fT, const Quat& rkP, const Quat& rkQ, bool shortestPath=false);
		static	   Quat clerp(float fT, const Quat& rkP, const Quat& rkQ, bool shortestPath=false);
		static	   Quat squad (float fT,const Quat& rkP, const Quat& rkA,const Quat& rkB, const Quat& rkQ, bool shortestPath=false);
			
		static const Quat Zero;
		static const Quat Identity;
	};
}

#endif