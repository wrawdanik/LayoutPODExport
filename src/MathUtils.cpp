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


#include "MathUtils.h"



using namespace MiniEngine;


bool	MathUtils::mSinCosTableDone=false;
float	MathUtils::mSinbuf[]; 


const float MathUtils::PIW = float( 4.0f * atan( 1.0f ) );
const float MathUtils::TwoPI = float( 2.0f * PIW );
const float MathUtils::HalfPI = float( 0.5f * PIW );
const float MathUtils::PiOver180 = float( PIW/180.0f);
const float MathUtils::PiUnder180 = float( 180.0f/PIW );



float MathUtils::ACos (float val)
{
	if ( -1.0f < val )
	{
		if ( val < 1.0f )
			return acos(val);
		else
			return 0.0f;
	}
	else
	{
		return PIW;
	}
}

float MathUtils::ASin (float val)
{
	if ( -1.0 < val )
	{
		if ( val < 1.0 )
			return asin(val);
		else
			return HalfPI;
	}
	else
	{
		return -HalfPI;
	}
}

 void MathUtils::initSinCosTable()
{
	if (mSinCosTableDone)
		return;
	double angle = TwoPI / (4 * 256);
	unsigned int ang;
	for (ang = 0; ang <= 128; ang++) 
	{
		mSinbuf[ ang] = (float)(::sin(angle * ang) * 2.0f);
		mSinbuf[256 - ang] = (float)(::cos(angle * ang) * 2.0f);
	}

	mSinCosTableDone = true;
}

