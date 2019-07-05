/*---------------------------------------------------------------------
*
* Copyright © 2015  Minsi Chen
* E-mail: m.chen@derby.ac.uk
*
* The source is written for the Graphics I and II modules. You are free
* to use and extend the functionality. The code provided here is functional
* however the author does not guarantee its performance.
---------------------------------------------------------------------*/
#include "Ray.h"

RayHitResult Ray::s_defaultHitResult;

Ray::Ray()
{
	s_defaultHitResult.data = nullptr;
	s_defaultHitResult.t = FARFAR_AWAY;
	s_defaultHitResult.UV = Vector4D(-1,-1,-1);
	s_defaultHitResult.texColour = {-1,-1,-1};
}


Ray::~Ray()
{
}
