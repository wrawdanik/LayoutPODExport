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


#ifndef VECTOR3_H
#define VECTOR3_H


#include "Quat.h"
#include "Vector2.h"
#include "MathUtils.h"
#include <math.h>

namespace MiniEngine
{

	class Vector3
	{
	public:

		float x;
		float y;
		float z;

		inline Vector3() {}

		inline Vector3(const float vx, const float vy, const float vz)
		{
			x=vx; y=vy, z=vz;	
		}

		inline explicit Vector3(const float v)
		{
			x=v; y=v; z=v;	
		}
		inline explicit Vector3(const float v[])
		{
			x=v[0];y=v[1];z=v[2];
		}
		inline explicit Vector3(const int v[])
		{
			x=(float)v[0];y=(float)v[1];z=(float)v[2];
		}

		inline explicit Vector3(const Vector2 &v, float posZ=0)
		{
			x=v.x; y=v.y; z=posZ;
		}

        inline explicit Vector3( float* const v )
        {
			x=v[0];y=v[1];z=v[2];
        }

		inline Vector3( const Vector3& rhs )
		{
			x=rhs.x;y=rhs.y;z=rhs.z;
		}

		inline float* ptr()  
		{
			return &x;
		}

		inline const float* ptr() const
		{
			return &x;
		}

        inline Vector3& operator = ( const Vector3& rhs )
        {
            x=rhs.x;y=rhs.y;z=rhs.z;
            return *this;
        }

        inline Vector3& operator = ( const float v )
        {
			x=v; y=v, z=v;	
            return *this;
        }

		inline float operator [] ( const size_t i ) const
        {
            return *(&x+i);
        }

		inline float& operator [] ( const size_t i )
        {
            return *(&x+i);
        }

		inline bool operator== (const float val) const
		{
			return (val == x) && (val == y) && (val == z);
		}


        inline bool operator == ( const Vector3& rhs ) const
        {
            return ( x == rhs.x && y == rhs.y && z == rhs.z );
        }

        inline bool operator != ( const Vector3& rhs ) const
        {
            return ( x != rhs.x || y != rhs.y || z != rhs.z );
        }

        inline Vector3 operator + ( const Vector3& rhs ) const
        {
            return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
        }

        inline Vector3 operator - ( const Vector3& rhs ) const
        {
            return Vector3(x - rhs.x,y - rhs.y,z - rhs.z);
        }

        inline Vector3 operator * ( const float val ) const
        {
            return Vector3(x * val,y * val,z * val);
        }

        inline Vector3 operator * ( const Vector3& rhs) const
        {
            return Vector3(x * rhs.x,y * rhs.y,z * rhs.z);
        }

        inline Vector3 operator / ( const float val ) const
        {
            return Vector3(x/val, y/val, z/val);
        }

        inline Vector3 operator / ( const Vector3& rhs) const
        {
            return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
        }

        inline const Vector3& operator + () const
        {
            return *this;
        }

        inline Vector3 operator - () const
        {
            return Vector3(-x, -y, -z);
        }

        inline bool operator < ( const Vector3& rhs ) const
        {
            if( x < rhs.x && y < rhs.y && z < rhs.z )
                return true;
            return false;
        }

        inline bool operator > ( const Vector3& rhs ) const
        {
            if( x > rhs.x && y > rhs.y && z > rhs.z )
                return true;
            return false;
        }

		

		// overloaded friends

        inline friend Vector3 operator * ( const float val, const Vector3& rhs )
        {
            return Vector3(val * rhs.x,val * rhs.y,val * rhs.z);
        }

        inline friend Vector3 operator / ( const float val, const Vector3& rhs )
        {
            return Vector3(val / rhs.x,val / rhs.y,val / rhs.z);
        }

        inline friend Vector3 operator + (const Vector3& lhs, const float rhs)
        {
            return Vector3(lhs.x + rhs,lhs.y + rhs,lhs.z + rhs);
        }

        inline friend Vector3 operator + (const float lhs, const Vector3& rhs)
        {
            return Vector3(lhs + rhs.x,lhs + rhs.y,lhs + rhs.z);
        }

        inline friend Vector3 operator - (const Vector3& lhs, const float rhs)
        {
            return Vector3(lhs.x - rhs,lhs.y - rhs,lhs.z - rhs);
        }

        inline friend Vector3 operator - (const float lhs, const Vector3& rhs)
        {
            return Vector3(lhs - rhs.x,lhs - rhs.y,lhs - rhs.z);
        }

    inline Vector3& operator += ( const Vector3& rhs )
        {
            x += rhs.x; y += rhs.y;z += rhs.z;
            return *this;
        }

        inline Vector3& operator += ( const float val )
        {
            x += val;y += val;z += val;
            return *this;
        }

        inline Vector3& operator -= ( const Vector3& rhs )
        {
            x -= rhs.x;y -= rhs.y; z -= rhs.z;

            return *this;
        }

        inline Vector3& operator -= ( const float val )
        {
            x -= val;y -= val;z -= val;
            return *this;
        }

        inline Vector3& operator *= ( const float val )
        {
            x *= val;y *= val;z *= val;
            return *this;
        }

        inline Vector3& operator *= ( const Vector3& rhs )
        {
            x *= rhs.x;y *= rhs.y;z *= rhs.z;

            return *this;
        }

        inline Vector3& operator /= ( const float val )
        {

            x /= val; y /= val;z /= val;

            return *this;
        }

        inline Vector3& operator /= ( const Vector3& rhs )
        {
            x /= rhs.x; y /= rhs.y; z /= rhs.z;

            return *this;
        }
		
		// methods


		inline Vector3 perpendicular() const
		{
			static const float fSquareZero = (float)(1e-06 * 1e-06);

			Vector3 perp = this->cross( Vector3::UnitX );

			if( perp.lengthSqr() < fSquareZero )
			{
				perp = this->cross( Vector3::UnitY );
			}

			return perp;
		}

        inline float length () const
        {
			return sqrtf( x * x + y * y + z * z );
        }
        inline float lengthSqr () const
        {
            return x * x + y * y + z * z;
        }
        inline float distance(const Vector3& rhs) const
        {
            return (*this - rhs).length();
        }
        inline float distanceSqr(const Vector3& rhs) const
        {
            return (*this - rhs).lengthSqr();
        }
        inline float dot(const Vector3& rhs) const
        {
            return x * rhs.x + y * rhs.y + z * rhs.z;
        }
        inline float dotAbs(const Vector3& rhs) const
        {
			return MathUtils::Abs(x * rhs.x) + MathUtils::Abs(y * rhs.y) + MathUtils::Abs(z * rhs.z);
        }

        inline Vector3 normalized(void) const
        {
            Vector3 ret(x,y,z);
            ret.normalize();
            return ret;
        }

        inline float normalize()
        {
			float len = sqrtf( x * x + y * y + z * z );

            if ( len > 1e-08 )
            {
                float invLen = 1.0f / len;
                x *= invLen;
                y *= invLen;
                z *= invLen;
            }
            return len;
        }
        inline Vector3 cross( const Vector3& rhs ) const
        {
            return Vector3(
                y * rhs.z - z * rhs.y,
                z * rhs.x - x * rhs.z,
                x * rhs.y - y * rhs.x);
        }

        inline Vector3 midPoint( const Vector3& rhs ) const
        {
            return Vector3(
                ( x + rhs.x ) * 0.5f,
                ( y + rhs.y ) * 0.5f,
                ( z + rhs.z ) * 0.5f );
        }

        inline void floor( const Vector3& rhs )
        {
            if( rhs.x < x ) x = rhs.x;
            if( rhs.y < y ) y = rhs.y;
            if( rhs.z < z ) z = rhs.z;
        }

        inline void ceil( const Vector3& rhs )
        {
            if( rhs.x > x ) x = rhs.x;
            if( rhs.y > y ) y = rhs.y;
            if( rhs.z > z ) z = rhs.z;
        }

		inline void zero()	{ x=y=z=0;}

        inline bool isZeroLength(void) const
        {
            float sqlen = (x * x) + (y * y) + (z * z);
            return (sqlen < (1e-06 * 1e-06));

        }

        inline Vector3 reflect(const Vector3& vec) const
        {
            return Vector3( *this - ( 2 * this->dot(vec) * vec ) );
        }


		inline Vector3 projectOnLine(const Vector3 &a, const Vector3 &b,float &t)
		{
			Vector3 ab = b-a;			
			t=(*this-a).dot(ab)/ab.dot(ab);
			if (t < 0.0f) t = 0.0f;
			if (t > 1.0f) t = 1.0f;
			return Vector3(a + t * ab);
		}

		Quat getRotationTo(const Vector3& toDest, const Vector3& fallbackAxis = Vector3::Zero) const;

		// static stuff

		static inline void createMin(const Vector3& value1, const Vector3& value2, Vector3& result)
		{
			result.x = (value1.x < value2.x) ? value1.x : value2.x;
			result.y = (value1.y < value2.y) ? value1.y : value2.y;
			result.z = (value1.z < value2.z) ? value1.z : value2.z;
		}

 		static inline void createMax(const Vector3& value1, const Vector3& value2, Vector3& result)
		{
			result.x = (value1.x > value2.x) ? value1.x : value2.x;
			result.y = (value1.y > value2.y) ? value1.y : value2.y;
			result.z = (value1.z > value2.z) ? value1.z : value2.z;
		}

		static void clamp(const Vector3& value1,const Vector3& min, const Vector3& max,Vector3& result)
		{
			result.x = value1.x;
			result.x = (result.x > max.x) ? max.x : result.x;
			result.x = (result.x < min.x) ? min.x : result.x;
			result.y = value1.y;
			result.y = (result.y > max.y) ? max.y : result.y;
			result.y = (result.y < min.y) ? min.y : result.y;
			result.z = value1.z;
			result.z = (result.z > max.z) ? max.z : result.z;
			result.z = (result.z < min.z) ? min.z : result.z;
		}

		static inline Vector3 lerp(const Vector3& value1, const Vector3& value2, float amount)
		{
			return Vector3(value1.x + ((value2.x - value1.x)*amount),
						   value1.y + ((value2.y - value1.y)*amount),
						   value1.z + ((value2.z - value1.z)*amount));	
		}

        inline friend std::ostream& operator <<
            ( std::ostream& o, const Vector3& v )
        {
            o << "Vector3(" << v.x << ", " << v.y << ", " << v.z << ")";
            return o;
        }


		static const Vector3 Zero;
        static const Vector3 UnitX;
        static const Vector3 UnitY;
        static const Vector3 UnitZ;
        static const Vector3 NegUnitX;
        static const Vector3 NegUnitY;
        static const Vector3 NegUnitZ;
        static const Vector3 One;



	};
}

#endif