#pragma once
#include "BaseObject.h"
#include <vector>

struct Force
{
	double x, y, z;

	Force()
	{
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}

	Force(Vector vec)
	{
		x = vec[0];
		y = vec[1];
		z = vec[2];
	}

	void GetVector(Vector out) { setVector(out, x, y, z); }

	Force& operator += (const Force& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		return *this;
	}
};

struct Particle
{
	Vector pos, vel;
	Vector entryPos; // position of entry when hit ground
	Vector lastTotalForce; // total force computed at previous frame
	double m;

	std::vector<Force> forces; // all forces applied to this particle

	bool isFixed;
	bool isUnderGround;

	Particle()
	{
		Vector zero;
		zeroVector(zero);
		SetPosition(zero);
		SetVelocity(zero);
		SetEntryPos(zero);
		zeroVector(lastTotalForce);
		m = 1.0;
		isFixed = false;
		isUnderGround = false;
		
	}

	void GetPosition(Vector outPosition) { VecCopy(outPosition, pos); }

	void GetVelocity(Vector outVelocity) { VecCopy(outVelocity, vel); }

	void GetEntryPos(Vector outEntry) { VecCopy(outEntry, pos); }

	double GetMass() { return m; }

	void SetPosition(const Vector position) { VecCopy(pos, position); }

	void SetVelocity(const Vector velocity) { VecCopy(vel, velocity); }

	void SetEntryPos(const Vector entry) { VecCopy(entryPos, entry); }

	void SetMass(double mass) { m = mass; }

	void AddForce(const Vector force);

	void GetTotalForce(Vector outForce);

	void GetLastTotalForce(Vector outForce) { VecCopy(outForce, lastTotalForce); }

	void ClearForces();

	// nail particle to its current position
	void Fix();
};

class Emitter :
	public BaseObject
{
public:
	Emitter(const std::string& name);

	void display(GLenum mode = GL_RENDER);

	void reset(double time);

	void AddParticle();

	bool SetPosition(int index, Vector position);

	bool SetVelocity(int index, Vector velocity);

	bool SetEntryPos(int index, Vector entry);

	bool SetMass(int index, double mass);

	bool AddForce(int index, Vector force);

	void GetPosition(int index, Vector outPosition);

	void GetVelocity(int index, Vector outVelocity);

	void GetEntryPos(int index, Vector outEntry);

	double GetMass(int index);

	void GetTotalForce(int index, Vector outForce);

	void GetLastTotalForce(int index, Vector outForce);

	void ClearForce(int index);

	void FixParticle(int index);

	bool isUnderGround(int index);

	void setUnderGround(int index, bool underGround);

	int ParticleCount() { return particles.size(); }


protected:
	std::vector<Particle> particles;


};

