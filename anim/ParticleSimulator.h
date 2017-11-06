#pragma once
#include <util/util.h>
#include "animTcl.h"
#include "BaseSimulator.h"
#include "BaseSystem.h"
#include <vector>

class ParticleSystem;
class Emitter;

struct Spring
{
	double ks, kd;
	double rest; // rest length
	int i1, i2; // index of the controlled particle in emitter's vector list

	Spring(int i1, int i2, double ks, double kd, double rest)
	{
		this->i1 = i1;
		this->i2 = i2;
		this->ks = ks;
		this->kd = kd;
		this->rest = rest;
	}

};

class ParticleSimulator : public BaseSimulator
{
public:
	ParticleSimulator(const std::string& name, BaseSystem* target);

	int step(double time);
	int init(double time);

	int command(int argc, myCONST_SPEC char **argv);
	void display(GLenum mode = GL_RENDER);
	void reset(double time);

protected:
	std::vector<Spring> springs;
	int springCount;

	BaseSystem* m_object;
	double lastTime;
	double data[1];

	double k, g; // global damp and gravity 

	double groundKs, groundKd;
	Vector groundNormal;

	ParticleSystem* linkedSystem;
	Emitter* emitter; // emitter of the linkedSystem

	// Compute forces for all springs
	void UpdateSprings();

	// Compute global friction force
	void UpdateGlobalDamp();

	// Compute gravity
	void UpdateGravity();

	// Check ground collision
	void CheckGroundCollision();

};

