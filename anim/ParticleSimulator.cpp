#include "ParticleSimulator.h"
#include "ParticleSystem.h"
#include "Emitter.h"
#include "GlobalResourceManager.h"

ParticleSimulator::ParticleSimulator(const std::string& name, BaseSystem* target) :
	BaseSimulator(name),
	m_object(target)
{
	lastTime = 0;
	springCount = 0;
	k = 0;
	g = 0;
	groundKs = 0;
	groundKd = 0;
	setVector(groundNormal, 0.0, 1.0, 0.0);

}

int ParticleSimulator::command(int argc, myCONST_SPEC char ** argv)
{
	if (argc < 1)
	{
		animTcl::OutputMessage("simulator %s: wrong number of params.", m_name);
		return TCL_ERROR;
	}
	else if (strcmp(argv[0], "link") == 0)
	{
		if (argc == 3)
		{
			BaseSystem* sys = GlobalResourceManager::use()->getSystem(argv[1]);
			assert(sys);
			linkedSystem = dynamic_cast<ParticleSystem*>(sys);
			emitter = linkedSystem->GetEmitter();
			assert(emitter);
			animTcl::OutputMessage("particle count = %d", emitter->ParticleCount());
			springCount = atoi(argv[2]);

			return TCL_OK;
		}
		else
		{
			animTcl::OutputMessage("Usage:  link <sys name> <Number of Springs>");
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "spring") == 0)
	{
		if (argc == 6)
		{
			if (springs.size() >= springCount)
			{
				animTcl::OutputMessage("Add spring failed, maximum spring number reached");
				return TCL_ERROR;
			}

			if (!linkedSystem)
			{
				animTcl::OutputMessage("No particle system linked to this simulator");
				return TCL_ERROR;
			}

			int i1 = atoi(argv[1]);
			int i2 = atoi(argv[2]);
			double ks = atoi(argv[3]);
			double kd = atoi(argv[4]);
			double l = atoi(argv[5]);

			if (l < 0)
			{
				Vector pos1, pos2, dir;
				linkedSystem->GetEmitter()->GetPosition(i1, pos1);
				linkedSystem->GetEmitter()->GetPosition(i2, pos2);
				VecSubtract(dir, pos1, pos2);
				l = VecLength(dir);
			}

			springs.push_back(Spring(i1, i2, ks, kd, l));
			glutPostRedisplay();
			return TCL_OK;
		}
		else
		{
			animTcl::OutputMessage("Usage: spring <index1> <index2> <ks> <kd> <restlength>");
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "fix") == 0)
	{
		if (argc == 2)
		{
			int index = atoi(argv[1]);
			emitter->FixParticle(index);
			return TCL_OK;
		}
		else
		{
			animTcl::OutputMessage("Usage:  fix <index>");
			return TCL_ERROR;
		}
	}

	else if (strcmp(argv[0], "gravity") == 0)
	{
		if (argc == 2)
		{
			g = atof(argv[1]);
			return TCL_OK;
		}
		else
		{
			animTcl::OutputMessage("Usage: gravity <g>");
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "drag") == 0)
	{
		if (argc == 2)
		{
			k = atof(argv[1]);
			return TCL_OK;
		}
		else
		{
			animTcl::OutputMessage("Usage: drag <kdrag>");
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "ground") == 0)
	{
		if (argc == 3)
		{
			groundKs = atof(argv[1]);
			groundKd = atof(argv[2]);
			return TCL_OK;
		}
		else
		{
			animTcl::OutputMessage("Usage: ground <ks> <kd>");
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "integration") == 0)
	{
		if (argc == 3)
		{
			if (!linkedSystem)
			{
				animTcl::OutputMessage("Set link system first before setting integration");
				return TCL_ERROR;
			}
			if (strcmp(argv[1], "euler") == 0)
				linkedSystem->integration = euler;
			else if (strcmp(argv[1], "symplectic") == 0)
				linkedSystem->integration = symplectic;
			else if (strcmp(argv[1], "verlet") == 0)
				linkedSystem->integration = verlet;

			double timeStep = atof(argv[2]);
			GlobalResourceManager::use()->setCustomSimulationTimeStep(timeStep);
		}
	}
	return 0.0;
}

int ParticleSimulator::step(double time)
{
	double deltaTime = time - lastTime;
	lastTime = time;

	if (linkedSystem)
	{
		UpdateGravity();
		UpdateGlobalDamp();
		UpdateSprings();
		CheckGroundCollision();
	}
	
	m_object->getState(data);
	data[0] = deltaTime;

	m_object->setState(data);

	return 0;
}

int ParticleSimulator::init(double time)
{

	return 0;
}

void ParticleSimulator::display(GLenum mode)
{

	for (Spring spring : springs)
	{
		Vector pos1, pos2;
		emitter->GetPosition(spring.i1, pos1);
		emitter->GetPosition(spring.i2, pos2);
		glLineWidth(5.0);
		glBegin(GL_LINES);
		glVertex3d(pos1[0], pos1[1], pos1[2]);
		glVertex3d(pos2[0], pos2[1], pos2[2]);
		glEnd();
	}
	
}

void ParticleSimulator::reset(double time)
{
	springs.clear();
	lastTime = 0;
	springCount = 0;
	k = 0;
	g = 0;
	groundKs = 0;
	groundKd = 0;
	setVector(groundNormal, 0.0, 1.0, 0.0);
}

void ParticleSimulator::UpdateSprings()
{
	for (Spring spring : springs)
	{
		Vector pos1, pos2, vel1, vel2;
		emitter->GetPosition(spring.i1, pos1);
		emitter->GetPosition(spring.i2, pos2);
		emitter->GetVelocity(spring.i1, vel1);
		emitter->GetVelocity(spring.i2, vel2);

		// Compute pure spring force
		double dst; // distance between two points
		Vector sub, f1, f2;
		VecSubtract(sub, pos1, pos2);
		dst = VecLength(sub);
		VecScale(sub, dst);
		VecScale(sub, spring.ks * (spring.rest - dst));
		VecCopy(f1, sub);
		VecScale(sub, -1.0);
		VecCopy(f2, sub);
		emitter->AddForce(spring.i1, f1);
		emitter->AddForce(spring.i2, f2);

		// Compute damp force
		Vector fd1, fd2, velSub;
		VecSubtract(velSub, vel1, vel2);
		VecScale(velSub, spring.kd * -1.0);
		VecCopy(fd1, velSub);
		VecScale(velSub, -1.0);
		VecCopy(fd2, velSub);
		emitter->AddForce(spring.i1, fd1);
		emitter->AddForce(spring.i2, fd2);

	}
}

void ParticleSimulator::UpdateGlobalDamp()
{
	Vector vel;

	for (int i = 0; i < emitter->ParticleCount(); i++)
	{
		emitter->GetVelocity(i, vel);
		VecScale(vel, k * -1.0);
		emitter->AddForce(i, vel);
	}
}

void ParticleSimulator::UpdateGravity()
{
	Vector mg;
	
	for (int i = 0; i < emitter->ParticleCount(); i++)
	{
		setVector(mg, 0.0, 1.0, 0.0);
		double mass = emitter->GetMass(i);
		VecScale(mg, mass * g);
		emitter->AddForce(i, mg);
	}
}

void ParticleSimulator::CheckGroundCollision()
{
	Vector pos;
	for (int i = 0; i < emitter->ParticleCount(); i++)
	{
		// check if is underground
		emitter->GetPosition(i, pos);
		if (pos[1] < 0.0)
		{
			// add support force
			Vector totalForce;
			emitter->GetTotalForce(i, totalForce);
			if (totalForce[1] < 0.0)
			{
				totalForce[0] = 0.0;
				totalForce[2] = 0.0;
				totalForce[1] *= -1.0;
				emitter->AddForce(i, totalForce);
			}

			// if this particle was not underground at last frame, set its entry point
			if (!emitter->isUnderGround(i))
			{
				emitter->setUnderGround(i, true);
				emitter->SetEntryPos(i, pos);
			}
			// if this particle was already underground, compute penalty force
			else
			{
				Vector entry, vel, dir;
				emitter->GetEntryPos(i, entry);
				emitter->GetVelocity(i, vel);
				VecSubtract(dir, pos, entry);
				// compute spring force
				Vector fsp;
				VecScale(dir, groundKs * -1.0);
				VecScale(vel, groundKd * -1.0);
				VecAdd(fsp, dir, vel);
				
				// split spring force
				Vector fn, ft; // sub spring force along normal and tangent
				double s = VecDotProd(fsp, groundNormal);
				VecCopy(fn, groundNormal);
				VecScale(fn, s);
				VecSubtract(ft, fsp, fn);
				// clamp tangent to mFn
				double lFt = VecLength(ft);
				double lFn = VecLength(fn);
				double m = emitter->GetMass(i);
				if (lFt > m * lFn)
				{
					
					VecScale(ft, lFn / lFt);
					VecAdd(fsp, ft, fn);
					// update new spring force and entry point
					entry[0] *= lFn / lFt;
					entry[2] *= lFn / lFt;
					emitter->SetEntryPos(i, entry);
				}
				emitter->AddForce(i, fsp);
			}

			

		}
		else
		{
			emitter->setUnderGround(i, false);
		}
	}
}


