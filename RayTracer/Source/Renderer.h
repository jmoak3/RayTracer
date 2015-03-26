#ifndef RENDERER_H	
#define RENDERER_H
#include "Shape.h"
#include "Camera.h"
#include "QualityDesc.h"

#include <vector>

class Renderer
{
public:
	Renderer(std::vector<Primitive*>* scene, const Camera &ccamera, const QualityDesc &quality);
	~Renderer() {delete Lights;}

	void Render();
	RGB RayTrace(const Ray &reflRay);
	RGB PathTrace(const Ray &reflRay);
	RGB computeColor(const Ray &reflRay, const Hit & hit);

private:
	bool FindClosest(const Ray &ray, Hit *hit); 
	bool ShadowTest(const Ray &ray); 
	std::vector<Primitive*> *Scene;
	std::vector<Primitive*> *Lights; // for shadow tests (speed!)
	Camera Cam;
	int Samples;
	int LightSamples;
	int GlossyReflectiveSamples;
	bool Path;
	float InvSamples;
	float InvLightSamples;
	float InvGlossyReflectiveSamples;
	int Depth;
};

#endif