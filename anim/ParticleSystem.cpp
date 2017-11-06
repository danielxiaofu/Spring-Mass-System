#include "ParticleSystem.h"
#include "Emitter.h"


ParticleSystem::ParticleSystem(const std::string& name)
	: BaseSystem(name)
{
	deltaTime = 0;
	data[0] = deltaTime;
	integration = euler;
	firstStep = true;
}

int ParticleSystem::command(int argc, myCONST_SPEC char ** argv)
{
	if (argc < 1)
	{
		animTcl::OutputMessage("system %s: wrong number of params.", m_name);
		return TCL_ERROR;
	}
	else if (strcmp(argv[0], "dim") == 0)
	{
		if (argc == 2)
		{
			int num = atoi(argv[1]);
			for (int i = 0; i < num; i++)
				emitter->AddParticle();
			glutPostRedisplay();
			return TCL_OK;
		}
		else
		{
			animTcl::OutputMessage("Usage: dim <number of particles>");
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "particle") == 0)
	{
		if (argc == 9)
		{
			int index = atoi(argv[1]);
			double mass = atof(argv[2]);
			double x = atof(argv[3]);
			double y = atof(argv[4]);
			double z = atof(argv[5]);
			double vx = atof(argv[6]);
			double vy = atof(argv[7]);
			double vz = atof(argv[8]);

			Vector pos, vel;
			setVector(pos, x, y, z);
			setVector(vel, vx, vy, vz);
			if (!emitter->SetPosition(index, pos))
			{
				animTcl::OutputMessage("Index out of bound");
				return TCL_ERROR;
			}
			if (!emitter->SetVelocity(index, vel))
			{
				animTcl::OutputMessage("Index out of bound");
				return TCL_ERROR;
			}
			if (!emitter->SetMass(index, mass))
			{
				animTcl::OutputMessage("Index out of bound");
				return TCL_ERROR;
			}
			glutPostRedisplay();
			return TCL_OK;
		}
		else
		{
			animTcl::OutputMessage("Usage: particle <index> <mass> <x y z vx vy vz>");
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "all_velocities") == 0)
	{
		if (argc == 4)
		{
			double vx = atof(argv[1]);
			double vy = atof(argv[2]);
			double vz = atof(argv[3]);
			Vector vel;
			setVector(vel, vx, vy, vz);

			for (int i = 0; i < emitter->ParticleCount(); i++)
			{
				emitter->SetVelocity(i, vel);
			}
			glutPostRedisplay();
			return TCL_OK;
		}
		else
		{
			animTcl::OutputMessage("Usage:  all_velocities <vx vy vz>");
			return TCL_ERROR;
		}

	}


	return 0;
}

void ParticleSystem::getState(double * p)
{
	PackageData();
	for (int i = 0; i < 1; i++)
		p[i] = data[i];
}

void ParticleSystem::setState(double * p)
{
	for (int i = 0; i < 1; i++)
		data[i] = p[i];
	UnpackData();

	UpdateParticle();
	
	for(int i = 0; i < emitter->ParticleCount(); i++)
		emitter->ClearForce(i);
}

void ParticleSystem::reset(double time)
{
	deltaTime = 0;
	data[0] = deltaTime;
	integration = euler;
	firstStep = true;
}

void ParticleSystem::display(GLenum mode)
{
	glBegin(GL_QUADS);
	glVertex4f(-1, 0, 1, 0);
	glVertex4f(-1, 0, -1, 0);
	glVertex4f(1, 0, -1, 0);
	glVertex4f(1, 0, 1, 0);
	glEnd();
}

void ParticleSystem::PackageData()
{
	data[0] = deltaTime;
}

void ParticleSystem::UnpackData()
{
	deltaTime = data[0];
}

void ParticleSystem::UpdateParticle()
{
	if (!emitter)
		return;
	switch (integration)
	{
	case euler: 
		IntegrateEuler();
		break;
	case symplectic:
		IntegrateSymplectic();
		break;
	case verlet:
		IntegrateVerlet();
		break;
	}
}

void ParticleSystem::IntegrateEuler()
{
	for (int i = 0; i < emitter->ParticleCount(); i++)
	{
		Vector velOld, velNew, pos, force;
		emitter->GetTotalForce(i, force);
		VecScale(force, deltaTime / emitter->GetMass(i));
		emitter->GetVelocity(i, velOld);
		VecAdd(velNew, velOld, force); // compute new velocity
		emitter->SetVelocity(i, velNew);

		emitter->GetPosition(i, pos);
		VecScale(velOld, deltaTime);
		VecAdd(pos, pos, velOld); // compute new position
		emitter->SetPosition(i, pos);
	}
}

void ParticleSystem::IntegrateSymplectic()
{
	for (int i = 0; i < emitter->ParticleCount(); i++)
	{
		Vector vel, pos, force;
		emitter->GetTotalForce(i, force);
		VecScale(force, deltaTime / emitter->GetMass(i));
		emitter->GetVelocity(i, vel);
		VecAdd(vel, vel, force); // compute new velocity
		emitter->SetVelocity(i, vel);

		emitter->GetPosition(i, pos);
		VecScale(vel, deltaTime);
		VecAdd(pos, pos, vel); // compute new position
		emitter->SetPosition(i, pos);
	}
}

void ParticleSystem::IntegrateVerlet()
{
	if (firstStep) {
		IntegrateEuler();
		firstStep = false;
		return;
	}

	for (int i = 0; i < emitter->ParticleCount(); i++)
	{
		Vector vel, pos, acc, lastForce, force, deltaAcc;
		emitter->GetLastTotalForce(i, lastForce);
		emitter->GetTotalForce(i, force);
		emitter->GetVelocity(i, vel);
		emitter->GetPosition(i, pos);
		VecCopy(acc, force);
		VecScale(acc, 1.0 / emitter->GetMass(i));
		//animTcl::OutputMessage("vel = %f, %f, %f", vel[0], vel[1], vel[2]);
	
		VecSubtract(deltaAcc, force, lastForce);
		VecScale(deltaAcc, 1.0 / emitter->GetMass(i));

		// compute x(t - dt)
		Vector temp1, temp2, temp3, backward;
		VecCopy(temp1, acc);
		VecCopy(temp2, deltaAcc);
		VecCopy(temp3, vel);
		VecScale(temp3, deltaTime);
		VecScale(temp1, deltaTime * deltaTime / 2.0);
		VecScale(temp2, deltaTime * deltaTime * deltaTime / 6.0);
		VecSubtract(backward, pos, temp3);
		VecAdd(backward, backward, temp1);
		//VecSubtract(backward, backward, temp2);

		// compute x(t + dt)
		Vector forward;
		VecCopy(temp1, pos);
		VecScale(temp1, 2.0);
		VecScale(acc, deltaTime * deltaTime);
		VecSubtract(forward, temp1, backward);
		VecAdd(forward, forward, acc);

		// compute v(t)
		Vector newVel;
		/*VecCopy(temp1, vel);
		VecCopy(temp2, deltaAcc);
		VecScale(temp1, 2.0);
		VecScale(temp2, deltaTime * deltaTime / 3.0);
		VecAdd(newVel, temp1, temp2);*/
		VecSubtract(newVel, forward, pos);
		VecScale(newVel, 1.0 / deltaTime);

		emitter->SetPosition(i, forward);
		emitter->SetVelocity(i, newVel);

	}
}


