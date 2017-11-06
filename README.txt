------------------------------------
Mass & Spring Simulation
------------------------------------

Prerequisites
--------------

- Windows XP or Vista
- Visual Studio 2005 / 2008

Start as follows:

- Double click on cs174.sln
- Build the solution.
- Run it

The executable is created in the "Build" folder. You can run the main program
outside Visual Studio by running the file "animp.exe"

Interaction
------------

- Moving the mouse with the right button pressed rotates the scene.
- Moving the mouse with the middle button pressed zooms the scene

The following key strokes are recognized:

- r	restore the original view
- 0	reset all systems
- a	toggle animation
- s	toggle simulation
- m	toggle frame dumping
- t	reset all timers
- q	quit
- h	print these instructions

Summary & Goal
------------

Develop a spring mass system and a corresponding simulator that support an arbitrary number of
particles. The particles can be connected with springs in arbitrary ways and are subject to
gravity. There is a flat ground oriented along the y-axis that you should model using penalty
forces. The simulator should support the following integration methods: Forward Euler,
Symplectic Euler, and Verlet.


Command Instruction
------------

See project folder for sample command

1. system <sys_name> dim <Number of Particles>
This command initializes the particle system to hold up to the given number of particles.
Particles may be initialized to the origin of the world, or may not appear until added by
the next command.

2. system <sys_name> particle <index> <mass> <x y z vx vy vz>
This command sets a position, mass and velocity for a given particle.

3. system <sys_name> all_velocities <vx vy vz>
This command sets the velocity of all particles.

4. simulator <sim_name> link <sys name> <Number of Springs>
This links the simulator to a particular particle system, and initializes it to work with a
given number of springs.

5. simulator <sim_name> spring <index1> <index2> <ks> <kd> <restlength>
This sets up a given spring. If the restlength is a negative number, the system should
automatically set the restlength of the spring to the distance between the corresponding
particles at the time the command is given.

6. simulator <sim_name> fix <index>
This command nails particle <index> to its current position.

7. simulator <sim_name> integration <euler|symplectic|verlet> <time step>
This changes the integration technique used by the given simulator and sets the time step
of the integration.

8. simulator <sim_name> ground <ks> <kd>
Sets the parameters of the penalty forces applied to particles that try to go underground.

9. simulator <sim_name> gravity <g>
Sets the acceleration due to gravity, in unit length per unit time squared.

10. simulator <sim_name> drag <kdrag>
Sets the global drag (friction) coefficient (Fi = -kdrag vi). The command expects a
positive number

============================================
MacOSX 10.6 and above
--------------------------------------------

- You need to run the program from the terminal.
- Make sure your current working directory is Build.
- The event queues seem not to be handled properly.
  You may need to press a button more than once to register
  an event.
