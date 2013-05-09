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

#include "Vector3.h"



namespace MiniEngine
{
    const Vector3 Vector3::Zero( 0, 0, 0 );

    const Vector3 Vector3::UnitX( 1, 0, 0 );
    const Vector3 Vector3::UnitY( 0, 1, 0 );
    const Vector3 Vector3::UnitZ( 0, 0, 1 );
    const Vector3 Vector3::NegUnitX( -1,  0,  0 );
    const Vector3 Vector3::NegUnitY(  0, -1,  0 );
    const Vector3 Vector3::NegUnitZ(  0,  0, -1 );
    const Vector3 Vector3::One(1, 1, 1); 


	Quat Vector3::getRotationTo(const Vector3& toDest, const Vector3& fallbackAxis ) const
	{
		Quat q;
		// Copy, since cannot modify local
		Vector3 v0 = *this;
		Vector3 v1 = toDest;
		v0.normalize();
		v1.normalize();

		float d = v0.dot(v1);
		// If dot == 1, vectors are the same
		if (d >= 1.0f)
		{
			return Quat::Identity;
		}
		if (d < (1e-6f - 1.0f))
		{
			if (fallbackAxis != Vector3::Zero)
			{
				// rotate 180 degrees about the fallback axis
				q.fromAngleAxis(MathUtils::PIW, fallbackAxis);
			}
			else
			{
				// Generate an axis
				Vector3 axis = Vector3::UnitX.cross(*this);
				if (axis.isZeroLength()) // pick another if colinear
					axis = Vector3::UnitY.cross(*this);
				axis.normalize();
				q.fromAngleAxis(MathUtils::PIW, axis);
			}
		}
		else
		{
			float s = sqrtf( (1+d)*2 );
			float invs = 1 / s;

			Vector3 c = v0.cross(v1);

			q.x = c.x * invs;
			q.y = c.y * invs;
			q.z = c.z * invs;
			q.w = s * 0.5f;
			q.normalize();
		}
		return q;

	}

}
