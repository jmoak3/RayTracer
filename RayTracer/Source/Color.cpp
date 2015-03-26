#include "Color.h"
#include "Ray.h"
#include <math.h>
#include "Renderer.h"
#include <assert.h>

inline float ra1()
{
	return (float)((rand()/(float)RAND_MAX));
}

inline float ra()
{
	return (float)((rand()/(float)RAND_MAX))-0.5f;
}


Ray Material::ReflectRay(const Ray &ray, const Hit &hit) const
{
	Ray r(Point(), Vector(), hit.eps, ray.maxt, ray.time, ray.depth+1, hit.material.Refractive);

	//Save the trouble
	if (Reflective < 0.001f || GlossyReflective < 0.001f || ray.depth != 0)
	{
		r.o = ray.o + ray.d*hit.tHit;
		r.d = Normalize(ray.d - Vector(hit.normal*Dot(ray.d, hit.normal)*2.f));
		return r;
	}
	return CalcReflectLerp(ray, r, hit);
	//return CalcReflectApprox(ray, r, hit);
}

Ray Material::CalcReflectLerp(const Ray &ray, Ray &r, const Hit &hit) const
{
	Vector basis1 = Dot(hit.normal, ray.d) < 0 ?
							Vector(hit.normal) :
							-1.f*Vector(hit.normal);
	/*Vector temp = Vector(Cross(Vector(0.f, 1.f, 0.f), basis1), true);
	Vector basis2 = temp.HasNans() ?
							Cross(Vector(1.f, 0.f, 0.f), basis1) :
							temp;*/
	Vector basis2 = fabsf(basis1.x) > .1f ? 
							Vector(0.f, 1.f, 0.f) :
							Cross(Vector(1.f, 0.f, 0.f), basis1);
	basis2 = Normalize(basis2);
	Vector basis3 = Cross(basis2, basis1);
	float u = ra1()*6.28319f;
	float v = ra1();
	float w = sqrtf(v);
	Vector jitter = Normalize(basis2*cosf(u)*w + basis3*sinf(u)*w + basis1*sqrtf(1.f-v));
	r.o = ray.o + ray.d*hit.tHit;
	if (GlossyReflective == 1.f)
		r.d = jitter;
	else
	{
		Vector properReflection = Normalize(ray.d - Vector(2.f*(Dot(ray.d, hit.normal))*hit.normal));
		r.d = Normalize(Lerp(properReflection, jitter, GlossyReflective));
	}
	
	return r;
}

Ray Material::CalcReflectApprox(const Ray &ray, Ray &r, const Hit &hit) const
{
	float dx = ra()*GlossyReflective;
	float dy = ra()*GlossyReflective;
	int tries = 0;
	int maxTries = 20;
	while ((dx*dx) + (dy*dy) > (GlossyReflective*GlossyReflective) && tries < maxTries)
	{
		dx = ra()*GlossyReflective; 
		dy = ra()*GlossyReflective;
		++tries;
	}
	if (tries >= maxTries)
	{
		dx = 0.f; 
		dy = 0.f;
	}

	Vector v1 = ray.d - Vector(2.f*(Dot(ray.d, hit.normal))*hit.normal);
	Vector temp = Vector(Cross(v1, Vector(0.0f, 1.0f, 0.0f)), true);
	Vector v2 = temp.HasNans() ?
					Cross(v1, Vector(0.0f, 0.f, 1.f)) :
					temp;
	Vector v3 = Cross(v2, v1);
	r.d = Normalize(v1 + v2*dx + v3*dy*GlossyReflective);
	r.o = ray.o + ray.d*hit.tHit;
	return r;
}

Ray Material::RefractRay(const Ray &ray, const Hit &hit, bool * isValid) const
{
	if (hit.material.Refractive > ray.refrIndex + 0.01f 
		&& hit.material.Refractive < ray.refrIndex - 0.01f)
		return Ray(Point(), Vector(), hit.eps, ray.maxt, ray.time, ray.depth+1, hit.material.Refractive);

	Ray r(Point(), Vector(), hit.eps, ray.maxt, ray.time, ray.depth+1, hit.material.Refractive);
	float n = ray.refrIndex/r.refrIndex;
	Normal N(hit.normal);
	float cosInc = -Dot(N, ray.d);
	if (cosInc < 0.f) 
	{
		N = -N;
		cosInc = -Dot(N, ray.d);
	}
	float cosT2 = 1.f - n*n*(1.f - (cosInc*cosInc));
	if (cosT2 <= 0.f || !(*isValid))
	{
		*isValid = false;
		return Ray();
	}
	*isValid = true;
	r.o = ray.o + ray.d*hit.tHit;
	r.d = Normalize(n*ray.d + Vector((n*cosInc - sqrt(cosT2))*N));
	return r;
}

