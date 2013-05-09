
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


#ifndef VECTOR4_H
#define VECTOR4_H

#include "Vector3.h"
#include <math.h>

namespace MiniEngine
{

	class Vector4
	{
	public:

		float x;
		float y;
		float z;
		float w;

		inline Vector4() {}

		inline Vector4(const float vx, const float vy, const float vz,const float vw)
		{
			x=vx; y=vy, z=vz; w=vw;	
		}

		inline explicit Vector4(const float v)
		{
			x=v; y=v, z=v;	w=v;
		}
		inline explicit Vector4(const float v[])
		{
			x=v[0];y=v[1];z=v[2]; w=v[3];
		}
		inline explicit Vector4(const int v[])
		{
			x=(float)v[0];y=(float)v[1];z=(float)v[2]; w=(float)v[3];
		}

		inline Vector4( const Vector4& rhs )
		{
			x=rhs.x;y=rhs.y;z=rhs.z;w=rhs.w;
		}



        inline explicit Vector4(const Vector3& rhs)
        {
			x=rhs.x; y=rhs.y; z=rhs.z, w=1.0f;
        }

        inline Vector4& operator = ( const Vector4& rhs )
        {
            x=rhs.x;y=rhs.y;z=rhs.z; w=rhs.w;
            return *this;
        }

        inline Vector4& operator = (const Vector3& rhs)
        {
            x = rhs.x;y = rhs.y;z = rhs.z;w = 1.0f;
            return *this;
        }

        inline Vector4& operator = ( const float v )
        {
			x=v; y=v, z=v; w=v;	
            return *this;
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


        inline bool operator == ( const Vector4& rhs ) const
        {
            return ( x == rhs.x && y == rhs.y && z == rhs.z  && w == rhs.w);
        }

        inline bool operator != ( const Vector4& rhs ) const
        {
            return ( x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w);
        }

        inline Vector4 operator + ( const Vector4& rhs ) const
        {
            return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w+ rhs.w);
        }

        inline Vector4 operator - ( const Vector4& rhs ) const
        {
            return Vector4(x - rhs.x,y - rhs.y,z - rhs.z, w- rhs.w);
        }

        inline Vector4 operator * ( const float val ) const
        {
            return Vector4(x * val,y * val,z * val, w*val);
        }

        inline Vector4 operator * ( const Vector4& rhs) const
        {
            return Vector4(x * rhs.x,y * rhs.y,z * rhs.z, w * rhs.w);
        }

        inline Vector4 operator / ( const float val ) const
        {
            return Vector4(x/val, y/val, z/val, w/val);
        }

        inline Vector4 operator / ( const Vector4& rhs) const
        {
            return Vector4(x / rhs.x, y / rhs.y, z / rhs.z, w/rhs.w);
        }

        inline const Vector4& operator + () const
        {
            return *this;
        }

        inline Vector4 operator - () const
        {
            return Vector4(-x, -y, -z, -w);
        }

        inline bool operator < ( const Vector4& rhs ) const
        {
            if( x < rhs.x && y < rhs.y && z < rhs.z && w < rhs.w )
                return true;
            return false;
        }

        inline bool operator > ( const Vector4& rhs ) const
        {
            if( x > rhs.x && y > rhs.y && z > rhs.z && w > rhs.w)
                return true;
            return false;
        }

		
		// overloaded friends

        inline friend Vector4 operator * ( const float val, const Vector4& rhs )
        {
            return Vector4(val * rhs.x,val * rhs.y,val * rhs.z, val * rhs.w);
        }

        inline friend Vector4 operator / ( const float val, const Vector4& rhs )
        {
            return Vector4(val / rhs.x,val / rhs.y,val / rhs.z,  val/ rhs.w);
        }

        inline friend Vector4 operator + (const Vector4& lhs, const float rhs)
        {
            return Vector4(lhs.x + rhs,lhs.y + rhs,lhs.z + rhs, lhs.w + rhs );
        }

        inline friend Vector4 operator + (const float lhs, const Vector4& rhs)
        {
            return Vector4(lhs + rhs.x,lhs + rhs.y,lhs + rhs.z, lhs + rhs.w);
        }

        inline friend Vector4 operator - (const Vector4& lhs, const float rhs)
        {
            return Vector4(lhs.x - rhs,lhs.y - rhs,lhs.z - rhs,lhs.w - rhs);
        }

        inline friend Vector4 operator - (const float lhs, const Vector4& rhs)
        {
            return Vector4(lhs - rhs.x,lhs - rhs.y,lhs - rhs.z, lhs - rhs.w);
        }


	// updates

    inline Vector4& operator += ( const Vector4& rhs )
        {
            x += rhs.x; y += rhs.y;z += rhs.z; w +=rhs.w;
            return *this;
        }

        inline Vector4& operator += ( const float val )
        {
            x += val;y += val;z += val; w +=val;
            return *this;
        }

        inline Vector4& operator -= ( const Vector4& rhs )
        {
            x -= rhs.x;y -= rhs.y; z -= rhs.z; w -= rhs.w;

            return *this;
        }

        inline Vector4& operator -= ( const float val )
        {
            x -= val;y -= val;z -= val; w -=val;
            return *this;
        }

        inline Vector4& operator *= ( const float val )
        {
            x *= val;y *= val;z *= val; w *=val;
            return *this;
        }

        inline Vector4& operator *= ( const Vector4& rhs )
        {
            x *= rhs.x;y *= rhs.y;z *= rhs.z; w *= rhs.w;

            return *this;
        }

        inline Vector4& operator /= ( const float val )
        {

            x /= val; y /= val;z /= val; w /= val;

            return *this;
        }

        inline Vector4& operator /= ( const Vector4& rhs )
        {
            x /= rhs.x; y /= rhs.y; z /= rhs.z; w /= rhs.w;

            return *this;
        }
		
		// methods

        inline float length () const
        {
			return sqrtf( x * x + y * y + z * z  + w * w);
        }
        inline float lengthSqr () const
        {
            return x * x + y * y + z * z + w * w;
        }
        inline float dot(const Vector4& rhs) const
        {
            return x * rhs.x + y * rhs.y + z * rhs.z + w *rhs.w;
        }
        inline float dotAbs(const Vector4& rhs) const
        {
			return MathUtils::Abs(x * rhs.x) + MathUtils::Abs(y * rhs.y) + MathUtils::Abs(z * rhs.z) + MathUtils::Abs(w * rhs.w);
        }

        inline Vector4 normalized(void) const
        {
            Vector4 ret(x,y,z,w);
            ret.normalize();
            return ret;
        }

        inline float normalize()
        {
			float len = sqrtf( x * x + y * y + z * z + w*w);

            if ( len > 1e-08 )
            {
                float invLen = 1.0f / len;
                x *= invLen;
                y *= invLen;
                z *= invLen;
				w *= invLen;
            }
            return len;
        }     

        inline void floor( const Vector4& rhs )
        {
            if( rhs.x < x ) x = rhs.x;
            if( rhs.y < y ) y = rhs.y;
            if( rhs.z < z ) z = rhs.z;
			if( rhs.w < w ) w = rhs.w;
        }

        inline void ceil( const Vector4& rhs )
        {
            if( rhs.x > x ) x = rhs.x;
            if( rhs.y > y ) y = rhs.y;
            if( rhs.z > z ) z = rhs.z;
			if( rhs.w > w ) w = rhs.w;
        }

		inline void zero()	{ x=y=z=w=0;}


		// static stuff

		static inline void createMin(const Vector4& value1, const Vector4& value2, Vector4& result)
		{
			result.x = (value1.x < value2.x) ? value1.x : value2.x;
			result.y = (value1.y < value2.y) ? value1.y : value2.y;
			result.z = (value1.z < value2.z) ? value1.z : value2.z;
			result.w = (value1.w < value2.w) ? value1.w : value2.w;
		}

 		static void createMax(const Vector4& value1, const Vector4& value2, Vector4& result)
		{
			result.x = (value1.x > value2.x) ? value1.x : value2.x;
			result.y = (value1.y > value2.y) ? value1.y : value2.y;
			result.z = (value1.z > value2.z) ? value1.z : value2.z;
			result.w = (value1.w > value2.w) ? value1.w : value2.w;
		}

        inline  friend std::ostream& operator <<
            ( std::ostream& o, const Vector4& v )
        {
            o << "Vector4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
            return o;
        }


		static const Vector4 Zero;
	};
}

#endif