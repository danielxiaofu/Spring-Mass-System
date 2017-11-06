#pragma once
#include "BaseSystem.h"
#include <vector>

class Emitter;

enum Integration {euler, symplectic, verlet};

class ParticleSystem :
	public BaseSystem
{
public:
	ParticleSystem(const std::string& name);
	
	Integration integration;

	int command(int argc, myCONST_SPEC char **argv);

	virtual void getState(double *p);
	virtual void setState(double  *p);
	void reset(double time);

	void display(GLenum mode = GL_RENDER);

	void SetEmitter(Emitter* e) { emitter = e; }
	Emitter* GetEmitter() { return emitter; }



protected:
	Emitter* emitter;
	double deltaTime;

	double data[1];
	bool firstStep;

	/* package all important local variables into data array
	 * 0: deltaTime
	*/
	void PackageData();

	// unpack data array and store data in local variables
	void UnpackData();

	// update particle position
	void UpdateParticle();

	void IntegrateEuler();

	void IntegrateSymplectic();

	void IntegrateVerlet();
};

