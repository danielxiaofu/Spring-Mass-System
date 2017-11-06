#include "Emitter.h"

void Particle::AddForce(const Vector force)
{
	Vector f;
	VecCopy(f, force);
	forces.push_back(Force(f));
}

void Particle::GetTotalForce(Vector outForce)
{
	zeroVector(outForce);
	Force total;
	if (!isFixed)
	{
		for (Force force : forces)
			total += force;

		total.GetVector(outForce);
	}
}

void Particle::ClearForces()
{
	// before clear, store current total force to lastTotalForce
	Vector totalForce;
	GetTotalForce(totalForce);
	VecCopy(lastTotalForce, totalForce);
	forces.clear();
}

void Particle::Fix()
{
	isFixed = true;
	zeroVector(vel);
}


Emitter::Emitter(const std::string& name) :
	BaseObject(name)
{
}

void Emitter::display(GLenum mode)
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glPointSize(8);
	glBegin(GL_POINTS);
	
	for (Particle particle : particles)
	{
		Vector pos;
		particle.GetPosition(pos);
		glVertex3d(pos[0], pos[1], pos[2]);
	}

	glEnd();
}

void Emitter::reset(double time)
{
	particles.clear();

}

void Emitter::AddParticle()
{
	Particle newParticle;
	particles.push_back(newParticle);
}

bool Emitter::SetPosition(int index, Vector position)
{
	if (index >= particles.size())
		return false;

	particles[index].SetPosition(position);
	return true;
}

bool Emitter::SetVelocity(int index, Vector velocity)
{
	if (index >= particles.size())
		return false;

	particles[index].SetVelocity(velocity);
	return true;
}

bool Emitter::SetEntryPos(int index, Vector entry)
{
	if (index >= particles.size())
		return false;

	particles[index].SetEntryPos(entry);
	return true;
}

bool Emitter::SetMass(int index, double mass)
{
	if (index >= particles.size())
		return false;

	particles[index].SetMass(mass);
	return true;
}

bool Emitter::AddForce(int index, Vector force)
{
	if (index >= particles.size())
		return false;

	particles[index].AddForce(force);
}

void Emitter::GetPosition(int index, Vector outPosition)
{
	if (index >= particles.size())
		return;

	particles[index].GetPosition(outPosition);
}

void Emitter::GetVelocity(int index, Vector outVelocity)
{
	if (index >= particles.size())
		return;

	particles[index].GetVelocity(outVelocity);
}

void Emitter::GetEntryPos(int index, Vector outEntry)
{
	if (index >= particles.size())
		return;

	particles[index].GetEntryPos(outEntry);
}

double Emitter::GetMass(int index)
{
	if (index >= particles.size())
		return 0.0;
	return particles[index].GetMass();
}

void Emitter::GetTotalForce(int index, Vector outForce)
{
	if (index >= particles.size())
		return;
	particles[index].GetTotalForce(outForce);
}

void Emitter::GetLastTotalForce(int index, Vector outForce)
{
	if (index >= particles.size())
		return;

	particles[index].GetLastTotalForce(outForce);
}

void Emitter::ClearForce(int index)
{
	if (index >= particles.size())
		return;

	particles[index].ClearForces();
}

void Emitter::FixParticle(int index)
{
	if (index >= particles.size())
		return;

	particles[index].Fix();
}

bool Emitter::isUnderGround(int index)
{
	if (index >= particles.size())
		return false;
	return particles[index].isUnderGround;
}

void Emitter::setUnderGround(int index, bool underGround)
{
	if (index >= particles.size())
		return;
	particles[index].isUnderGround = underGround;
}


