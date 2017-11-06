////////////////////////////////////////////////////
// // Template code for  CS 174C
////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <shared/defs.h>

#include "shared/opengl.h"

#include <string.h>
#include <util/util.h>
#include <GLModel/GLModel.h>
#include "anim.h"
#include "animTcl.h"
#include "myScene.h"
#include "SampleParticle.h"
#include "SampleGravitySimulator.h"
#include "Emitter.h"
#include "ParticleSystem.h"
#include "ParticleSimulator.h"
#include <util/jama/tnt_stopwatch.h>
#include <util/jama/jama_lu.h>

// register a sample variable with the shell.
// Available types are:
// - TCL_LINK_INT 
// - TCL_LINK_FLOAT

int g_testVariable = 10;

SETVAR myScriptVariables[] = {
	"testVariable", TCL_LINK_INT, (char *) &g_testVariable,
	"",0,(char *) NULL
};


//---------------------------------------------------------------------------------
//			Hooks that are called at appropriate places within anim.cpp
//---------------------------------------------------------------------------------

// start or end interaction
void myMouse(int button, int state, int x, int y)
{

	// let the global resource manager know about the new state of the mouse 
	// button
	GlobalResourceManager::use()->setMouseButtonInfo( button, state );

	if( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
	{
		animTcl::OutputMessage(
			"My mouse received a mouse button press event\n");

	}
	if( button == GLUT_LEFT_BUTTON && state == GLUT_UP )
	{
		animTcl::OutputMessage(
			"My mouse received a mouse button release event\n") ;
	}
}	// myMouse

// interaction (mouse motion)
void myMotion(int x, int y)
{

	GLMouseButtonInfo updatedMouseButtonInfo = 
		GlobalResourceManager::use()->getMouseButtonInfo();

	if( updatedMouseButtonInfo.button == GLUT_LEFT_BUTTON )
	{
		animTcl::OutputMessage(
			"My mouse motion callback received a mousemotion event\n") ;
	}

}	// myMotion


void MakeScene(void)
{

	/* 
	
	This is where you instantiate all objects, systems, and simulators and 
	register them with the global resource manager

	*/

	bool success;

	// register an emitter
	Emitter* emitter = new Emitter("Emitter1");
	success = GlobalResourceManager::use()->addObject(emitter, true);
	assert(success);
	animTcl::OutputMessage("Emitter created!");

	// register a particle system
	ParticleSystem* particle = new ParticleSystem("partSys");
	success = GlobalResourceManager::use()->addSystem(particle, true);
	assert(success);
	particle->SetEmitter(emitter);
	animTcl::OutputMessage("ParticleSytem created!");

	// register a particle simulator
	ParticleSimulator* particleSimulator =
		new ParticleSimulator("partSim", particle);
	success = GlobalResourceManager::use()->addSimulator(particleSimulator, true);
	assert(success);
	animTcl::OutputMessage("ParticleSimulator created!");

	BaseSystem* sampleSystemRetrieval;

	// retrieve the system
	sampleSystemRetrieval = 
		GlobalResourceManager::use()->getSystem( "partSys" );

	// make sure you got it
	assert( sampleSystemRetrieval );

	BaseSimulator* sampleSimulatorRetrieval;

	// retrieve the simulator
	sampleSimulatorRetrieval = 
		GlobalResourceManager::use()->getSimulator( "partSim" );

	// make sure you got it
	assert( sampleSimulatorRetrieval );

}	// MakeScene

// OpenGL initialization
void myOpenGLInit(void)
{
	animTcl::OutputMessage("Initialization routine was called.");

}	// myOpenGLInit

void myIdleCB(void)
{
	
	return;

}	// myIdleCB

void myKey(unsigned char key, int x, int y)
{
	 animTcl::OutputMessage("My key callback received a key press event\n");
	return;

}	// myKey

static int testGlobalCommand(ClientData clientData, Tcl_Interp *interp, int argc, myCONST_SPEC char **argv)
{
	 animTcl::OutputMessage("This is a test command!");
	return TCL_OK;

}	// testGlobalCommand

static int partOneGlobalCommand(ClientData clientData, Tcl_Interp *interp, int argc, myCONST_SPEC char **argv)
{
	GlobalResourceManager::use()->clearAll();
	glutPostRedisplay();

	bool success;

	// register an emitter
	Emitter* emitter = new Emitter("Emitter1");
	success = GlobalResourceManager::use()->addObject(emitter, true);
	assert(success);
	animTcl::OutputMessage("Emitter created!");

	// register a particle system
	ParticleSystem* particle = new ParticleSystem("partSys");
	success = GlobalResourceManager::use()->addSystem(particle, true);
	assert(success);
	particle->SetEmitter(emitter);
	animTcl::OutputMessage("ParticleSytem created!");

	// register a particle simulator
	ParticleSimulator* particleSimulator =
		new ParticleSimulator("partSim", particle);
	success = GlobalResourceManager::use()->addSimulator(particleSimulator, true);
	assert(success);
	animTcl::OutputMessage("ParticleSimulator created!");

	return TCL_OK;
}

void mySetScriptCommands(Tcl_Interp *interp)
{

	// here you can register additional generic (they do not belong to any object) 
	// commands with the shell

	Tcl_CreateCommand(interp, "test", testGlobalCommand, (ClientData) NULL,
					  (Tcl_CmdDeleteProc *)	NULL);
	Tcl_CreateCommand(interp, "part1", partOneGlobalCommand, (ClientData)NULL,
					  (Tcl_CmdDeleteProc *)NULL);


}	// mySetScriptCommands
