/*---------------------------------------------------------------------
*
* Copyright © 2015  Minsi Chen
* E-mail: m.chen@derby.ac.uk
*
* The source is written for the Graphics I and II modules. You are free
* to use and extend the functionality. The code provided here is functional
* however the author does not guarantee its performance.
---------------------------------------------------------------------*/
#include <math.h>
#ifdef WIN32
#include <Windows.h>
#include <gl/GL.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#endif

#include "RayTracer.h"
#include "Ray.h"
#include "Scene.h"
#include "SpotLight.h"
#include "Camera.h"

RayTracer::RayTracer()
{
	m_buffHeight = m_buffWidth = 0.0;
	m_renderCount = 0;
	SetTraceLevel(5);
	TP = timer.now();
	for (int i = 0; i < maxThreads; i++)
	{
		tstate.push_back(FINISHED);
	}

	m_traceflag = (TraceFlag)(TRACE_AMBIENT | TRACE_DIFFUSE_AND_SPEC |
		TRACE_SHADOW | TRACE_REFLECTION | TRACE_REFRACTION);
}

RayTracer::RayTracer(int Width, int Height)
{
	m_buffWidth = Width;
	m_buffHeight = Height;
	pixelColour = new Colour[m_buffHeight * m_buffWidth];
	m_renderCount = 0;
	SetTraceLevel(5);
	TP = timer.now();
	for (int i = 0; i < maxThreads; i++)
	{
		tstate.push_back(FINISHED);
		tPos.push_back({ i, 0 });
	}
	m_traceflag = (TraceFlag)(TRACE_AMBIENT | TRACE_DIFFUSE_AND_SPEC | 
		TRACE_REFLECTION | TRACE_REFRACTION | TRACE_SHADOW );
}

RayTracer::~RayTracer()
{
	bool terminate = false;

	while (!terminate)
	{
		terminate = true;

		for (int i = 0; i < maxThreads; i++)
		{
			if (tstate[i] != FINISHED)
			{
				terminate = false;
				break;
			}
		}
	}
}

void RayTracer::Update(Scene * pScene)
{
	bool threadsFinish = true;

	for (int i = 0; i < maxThreads; i++)
	{
		if (tstate[i] != FINISHED)
		{
			threadsFinish = false;
			break;
		}
	}

	if (threadsFinish)
	{
		//apply a rotation for the camera to use
		if (m_traceflag & TRACE_CAMERAROTATION)
		{
			AoR += 10;
			if (AoR >= 360)
				AoR = 0;
			rotationZ.SetRotationZAxis(AoR);
		}

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		for (int i = 0; i < m_buffWidth; i++)
		{
			for (int j = 0; j < m_buffHeight; j++)
			{
				/*
				* The only OpenGL code we need
				* Draw the pixel as a coloured rectangle
				*/
				glColor3f(pixelColour[i + (j * m_buffWidth)].red, pixelColour[i + (j * m_buffWidth)].green, pixelColour[i + (j * m_buffWidth)].blue);
				glRecti(i, j, i + 1, j + 1);
			}
		}

		glFlush();
		threads.clear();

		currentPixel.x = -1;
		currentPixel.y = 0;

		fprintf(stdout, "FPS: %5.5f \n", 1.0 / (duration<float, milli>((timer.now() - TP)).count() * 0.001));
		TP = timer.now();

		fprintf(stdout, "Trace start.\n");
		for (int i = 0; i < maxThreads; i++)
		{
			tstate[i] = READY;
			threads.push_back(thread(&RayTracer::DoRayTrace, this, pScene, i));
			threads[i].detach();
		}

		fprintf(stdout, "Done!!!\n");
	}
}

void RayTracer::DoRayTrace( Scene* pScene, int threadID )
{
	tstate[threadID] = TRACING;
	Camera* cam = pScene->GetSceneCamera();
	
	Vector4D camRightVector = rotationZ * cam->GetRightVector();
	Vector4D camUpVector = rotationZ * cam->GetUpVector();
	Vector4D camViewVector = rotationZ * cam->GetViewVector();
	Vector4D centre = cam->GetViewCentre();
	Vector4D camPosition = cam->GetPosition();

	double sceneWidth = pScene->GetSceneWidth();
	double sceneHeight = pScene->GetSceneHeight();

	double pixelDX = sceneWidth / m_buffWidth;
	double pixelDY = sceneHeight / m_buffHeight;
	
	int total = m_buffHeight*m_buffWidth;
	int done_count = 0;
	
	Vector4D start;

	start[0] = centre[0] - ((sceneWidth * camRightVector[0])
		+ (sceneHeight * camUpVector[0])) / 2.0;
	start[1] = centre[1] - ((sceneWidth * camRightVector[1])
		+ (sceneHeight * camUpVector[1])) / 2.0;
	start[2] = centre[2] - ((sceneWidth * camRightVector[2])
		+ (sceneHeight * camUpVector[2])) / 2.0;
	
	Colour scenebg = pScene->GetBackgroundColour();

	double DX = pixelDX * 0.75;
	double DY = pixelDY * 0.75;

	if (m_renderCount == 0)
	{
		while (tstate[threadID] != FINISHED)
		{
			PixelPosition pixelPos = NextPixel();
			if (pixelPos.y >= m_buffHeight)
			{
				tstate[threadID] = FINISHED;
				break;
			}

			//calculate the metric size of a pixel in the view plane (e.g. framebuffer)
			Vector4D pixel;

			pixel[0] = start[0] + (pixelPos.y + 0.5) * camUpVector[0] * pixelDY
				+ (pixelPos.x + 0.5) * camRightVector[0] * pixelDX;
			pixel[1] = start[1] + (pixelPos.y + 0.5) * camUpVector[1] * pixelDY
				+ (pixelPos.x + 0.5) * camRightVector[1] * pixelDX;
			pixel[2] = start[2] + (pixelPos.y + 0.5) * camUpVector[2] * pixelDY
				+ (pixelPos.x + 0.5) * camRightVector[2] * pixelDX;

			/*
			* setup view ray
			* In perspective projection, each view ray originates from the eye (camera) position
			* and pierces through a pixel in the view plane
			*
			* TODO: For a little extra credit, set up the view rays to produce orthographic projection
			*/
			Ray viewray;
			//viewray.SetRay(pixel, camViewVector);
			viewray.SetRay(camPosition, (pixel - camPosition).Normalise());
			//trace the scene using the view ray
			//the default colour is the background colour, unless something is hit along the way
			Colour colour = this->TraceScene(pScene, viewray, scenebg, m_traceLevel);

			if (m_traceflag & TRACE_ANTIALIASING)
			{
				Vector4D tempPixel = pixel;

				tempPixel[0] += DX;
				viewray.SetRay(camPosition, (tempPixel - camPosition).Normalise());
				colour += this->TraceScene(pScene, viewray, scenebg, m_traceLevel);

				tempPixel = pixel;
				tempPixel[0] -= DX;
				viewray.SetRay(camPosition, (tempPixel - camPosition).Normalise());
				colour += this->TraceScene(pScene, viewray, scenebg, m_traceLevel);

				tempPixel = pixel;
				tempPixel[1] += DY;
				viewray.SetRay(camPosition, (tempPixel - camPosition).Normalise());
				colour += this->TraceScene(pScene, viewray, scenebg, m_traceLevel);

				tempPixel = pixel;
				tempPixel[1] -= DY;
				viewray.SetRay(camPosition, (tempPixel - camPosition).Normalise());
				colour += this->TraceScene(pScene, viewray, scenebg, m_traceLevel);

				tempPixel = pixel;
				tempPixel[0] += DX;
				tempPixel[1] -= DY;
				viewray.SetRay(camPosition, (tempPixel - camPosition).Normalise());
				colour += this->TraceScene(pScene, viewray, scenebg, m_traceLevel);

				tempPixel = pixel;
				tempPixel[0] -= DX;
				tempPixel[1] += DY;
				viewray.SetRay(camPosition, (tempPixel - camPosition).Normalise());
				colour += this->TraceScene(pScene, viewray, scenebg, m_traceLevel);

				tempPixel = pixel;
				tempPixel[0] += DX;
				tempPixel[1] += DY;
				viewray.SetRay(camPosition, (tempPixel - camPosition).Normalise());
				colour += this->TraceScene(pScene, viewray, scenebg, m_traceLevel);

				tempPixel = pixel;
				tempPixel[0] -= DX;
				tempPixel[1] -= DY;
				viewray.SetRay(camPosition, (tempPixel - camPosition).Normalise());
				colour += this->TraceScene(pScene, viewray, scenebg, m_traceLevel);
				colour *= 0.111111111111111;
			}
			pixelColour[pixelPos.x + (m_buffWidth * pixelPos.y)] = colour;
		}
	}
}

RayTracer::PixelPosition RayTracer::NextPixel()
{
	pixelPos.lock();
	if (currentPixel.x >= m_buffWidth)
	{
		currentPixel.y++;
		currentPixel.x = -1;
	}
	currentPixel.x++;
	pixelPos.unlock();
	return currentPixel;
}

Colour RayTracer::TraceScene(Scene* pScene, Ray& ray, Colour incolour, int tracelevel, bool shadowray)
{
	RayHitResult result;
	Colour outcolour = incolour;
	std::vector<Light*>* light_list = pScene->GetLightList();
	Colour refract;
	Colour reflect;
	bool hasRefracted = false;
	bool hasReflected = false;

	if (tracelevel <= 0) // reach the MAX depth of the recursion.
	{
		return outcolour;
	}

	result = pScene->IntersectByRay(ray, shadowray);

	if (result.data) //the ray has hit something
	{
		if (shadowray)
		{
			return outcolour * 0.3;
		}

		Vector4D start = ray.GetRayStart();
		outcolour = CalculateLighting(light_list,
			&start,
			&result);
		
		if(m_traceflag & TRACE_REFLECTION)
		{
			//Only consider reflection for spheres and boxes
			if (((Primitive*)result.data)->m_primtype == Primitive::PRIMTYPE_Sphere ||
				((Primitive*)result.data)->m_primtype == Primitive::PRIMTYPE_Box)
			{
				Vector4D tempReflect = ray.GetRay().Reflect(result.normal).Normalise();

				Ray tempRay;
				tempRay.SetRay(result.point +(tempReflect*EPSILON), tempReflect);
				reflect = TraceScene(pScene, tempRay, outcolour, tracelevel - 1);

				hasReflected = true;
			}
		}

		if (m_traceflag & TRACE_REFRACTION)
		{
			//Only consider refraction for spheres and boxes
			if (((Primitive*)result.data)->m_primtype == Primitive::PRIMTYPE_Sphere ||
				((Primitive*)result.data)->m_primtype == Primitive::PRIMTYPE_Box)
			{
				Vector4D tempRefract = ray.GetRay().Refract(result.normal, 1.0).Normalise();
				
				Ray tempRay;
				tempRay.SetRay(result.point + (tempRefract * EPSILON), tempRefract);
				refract = TraceScene(pScene, tempRay, outcolour, tracelevel - 1);

				hasRefracted = true;
			}
		}
		
		//////Check if this is in shadow
		if ( m_traceflag & TRACE_SHADOW )
		{
			std::vector<Light*>::iterator lit_iter = light_list->begin();
			while (lit_iter != light_list->end())
			{
				Vector4D tempShadow = ((*lit_iter)->GetLightPosition() - result.point).Normalise();

				Ray tempRay;
				tempRay.SetRay(result.point + (tempShadow* 0.001), tempShadow);
				outcolour = TraceScene(pScene, tempRay, outcolour, tracelevel - 1, true);
				lit_iter++;
			}
		}

		if (hasReflected && hasRefracted)
		{
			outcolour *= (reflect + refract) * 0.5;
		}
		else if (hasReflected)
		{
			outcolour *= reflect;
		}
		else if (hasRefracted)
		{
			outcolour *= refract;
		}
	}
		
	return outcolour;
}

Colour RayTracer::CalculateLighting(std::vector<Light*>* lights, Vector4D* campos, RayHitResult* hitresult)
{
	Colour outcolour;
	std::vector<Light*>::iterator lit_iter = lights->begin();

	//Retrive the material for the intersected primitive
	Primitive* prim = (Primitive*)hitresult->data;
	Material* mat = prim->GetMaterial();

	double attenuation = 0.0;
	double radialAttenuation = 0.0;
	double angularAttenuation = 0.0;
	const double att1 = 0.01;
	const double att2 = 0.0009;
	//the default output colour is the ambient colour
	outcolour = mat->GetAmbientColour();
	
	//This is a hack to set a checker pattern on the planes
	//Do not modify it
	if (((Primitive*)hitresult->data)->m_primtype == Primitive::PRIMTYPE_Plane)
	{
		int dx = (hitresult->point[0] / 2.0);
		int dy = (hitresult->point[1] / 2.0);
		int dz = (hitresult->point[2] / 2.0);

		if (dx % 2 || dy % 2 || dz % 2)
		{
			outcolour = { 1.0f, 1.0f, 1.0f };
			
		}
		else
		{
			outcolour = { 0.0f,0.0f,0.0f };
		}
	}

	////Go through all the light sources in the scene
	//and calculate the lighting at the intersection point
	if (m_traceflag & TRACE_DIFFUSE_AND_SPEC)
	{
		while (lit_iter != lights->end())
		{
			Vector4D lightDirection;
			Vector4D tempCampos = *campos;
			double angleS = 0.0; //Specular cos theta
			attenuation = 0.0;
			
			Colour lightColour = (*lit_iter)->GetLightColour(); //Colour of light source
			Vector4D light_pos = (*lit_iter)->GetLightPosition();  //position of the light source
				
			Vector4D normal = hitresult->normal; //surface normal at intersection
			Vector4D surface_point = hitresult->point; //location of the intersection on the surface

			Vector4D light_vec = light_pos - surface_point; //surface to light vector
			Vector4D inverseLight_vec = surface_point - light_pos; //light to surface vector

			//radial attenuation
			const double attDist = inverseLight_vec.Length();
			radialAttenuation = 1.0 / (1.0 + (att1 * attDist) + (att2 * attDist * attDist));

			inverseLight_vec.Normalise();
			light_vec.Normalise();

			SpotLight* spotlight = dynamic_cast<SpotLight*>((*lit_iter));
			if (spotlight != nullptr)
			{
				lightDirection = spotlight->GetLightDirection().Normalise();
				double theta = fmax(0.0, fmin(1.0, inverseLight_vec.DotProduct(lightDirection)));
				angularAttenuation = spotlight->AngularAttenuation(theta);
				attenuation = angularAttenuation * radialAttenuation;
			}
			else
			{
				attenuation = radialAttenuation;
			}

			//diffuse cos theta
			const double angleD = fmax(0.0, fmin(1.0, normal.DotProduct(light_vec)));

			//blinn-phong half vector
			const Vector4D lightcam_vec = light_vec.Normalise() + tempCampos.Normalise();
			const double sqrlightcam = lightcam_vec.Length();

			Vector4D half_vec;
			half_vec.SetVector(lightcam_vec[0] / sqrlightcam, lightcam_vec[1] / sqrlightcam, lightcam_vec[2] / sqrlightcam);
			half_vec.Normalise();

			angleS = fmax(0.0, fmin( 1.0, normal.DotProduct(half_vec)));

			//Phong reflection vector
			/*Vector4D reflect_vec;
			Vector4D cam_vec = *campos - surface_point;
			cam_vec.Normalise();
			if (angleD > 0.0)
			{
				reflect_vec = ((normal + normal) * (normal.DotProduct(light_vec))) - light_vec;
				reflect_vec.Normalise();
				angleS = reflect_vec.DotProduct(cam_vec);
			}

			if (angleS < 0.0)
				angleS = 0.0;
			else if (angleS > 1.0)
				angleS = 1.0;*/

			angleS = pow(angleS, mat->GetSpecPower());

			lightColour = lightColour * attenuation;
			Colour difColour = { 0.0f, 0.0f, 0.0f };
			Colour white = { 1.0f,1.0f,1.0f };

			//Depending on the type of object depends on diffuse colour used
			if (hitresult->texColour.red >= 0)
				difColour = (hitresult->texColour * lightColour * angleD);
			else if (((Primitive*)hitresult->data)->m_primtype == Primitive::PRIMTYPE_Plane && outcolour == white)
			{
				outcolour *= attenuation;
				difColour = (white * lightColour * angleD);
			}
			else
				difColour = (mat->GetDiffuseColour() * lightColour * angleD);

			const Colour specColour = (mat->GetSpecularColour() * lightColour * angleS);

			outcolour += difColour;
			outcolour += specColour;

			lit_iter++;
		}
	}

	return outcolour;
}
