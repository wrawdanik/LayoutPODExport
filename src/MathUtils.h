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


#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <math.h>
#include <limits>
#include <float.h>
#include <stddef.h>

namespace MiniEngine
{
    class MathUtils
    {
	public:
		static const float PIW;
        static const float TwoPI;
        static const float HalfPI;
		static const float PiOver180;
		static const float PiUnder180;

		static inline float Abs (const float val) 
		{ 
			union 
			{
				int i;
				float f;
			} fi;

			fi.f = val;
			fi.i = fi.i & 0x7FFFFFFF;
			return fi.f;
		}


		static inline int IntAbs (const int val) { return ( val >= 0 ? val : -val ); }
		static inline int IntCeil (const float val) { return int(ceil(val)); }
		static inline int IntFloor (const float val) { return int(floor(val)); }

		static float ACos (float val);
		static float ASin (float val);

		static inline  float Clamp0(const float val)
		{
			union 
			{
				int i;
				float f;
			} fi;

			fi.f=val;
			int s=fi.i >>31 ;
			s=~s;
			fi.i&=s;
			return fi.f;
		}

		static inline  float Clamp1(const float val)
		{
			union 
			{
				int i;
				float f;
			} fi;

			fi.f=val-1.0f;
			int s=fi.i >>31 ;
			fi.i&=s;
			return fi.f+1.0f;
		}
			
		static inline  float DegreeToRadian(const float val) { return (val*PiOver180); }
		static inline  float RadianToDegree(const float val) { return (val*PiUnder180); }

		

		static	inline float FRound(float n, size_t d)
		{
			return floorf(n * powf(10.0f, (float)d) + .5f) / powf(10.0f, (float)d);
		}


		static inline bool Equal(float a, float b, float tolerance = std::numeric_limits<float>::epsilon())
		{
			if (fabs(b-a) <= tolerance)
					return true;
			else
				return false;
		}
		
		// decently fast combined sin/cos implementation

		inline static void SinCos(float theta, float& s, float& c)
		{

			s=sin(theta);
			c=cos(theta);
			return;

			const float _1p5m       = 1572864.0f;
			const double _2piinv     = 1.0 / TwoPI;
			const float _pi512inv   =  TwoPI / 1024.0f;
			// ____ double-precision fmadd
			double thb = theta * _2piinv + _1p5m;
			// ____ read 32 bit integer angle
			unsigned long thi = ((unsigned int*)(&thb))[0];
			// ____ first 8 bits of argument, in [0..π/2]
			unsigned long th = (thi >> 20) & 0x03FC;
			unsigned char *sbuf = (unsigned char *)mSinbuf;
			// ____ read approximate sincos * 2
			float st = *(float*)(((unsigned char*)sbuf) + (  th));
			float ct = *(float*)(((unsigned char*)sbuf) + (1024 - th));
			unsigned long tf = (thi & 0x003FFFFF) | 0x3F800000;
			float sd = (*(float*)(&tf)) * _pi512inv - _pi512inv;
			float cd = 0.5f - sd * sd;
			float sv = sd * ct + cd * st;
			float cv = cd * ct - sd * st;
			if (long(thi) >= 0) {
				if (thi & 0x40000000)    { s = cv; c = -sv; }
				else                     { s = sv; c = cv; }
			}
			else {
				if (thi & 0x40000000)    { s = -cv; c = sv; }
				else                     { s = -sv; c = -cv; }
			}			
		}


		static inline float Sin(float val)
		{

			return sinf(val);
		}

		static inline float Cos(float val)
		{

			return cosf(val);
		}

		static void initSinCosTable();
	protected:		
		
		 
		
		static bool mSinCosTableDone;
		
		
		static float mSinbuf[257];
	
		
	};
}

#endif