What is this?
#############

Loot Racer, a game in which you zoom around a level and attempt to pick up shiny things.
The shiny things are mostly in midair, and you need to speed off of ramps and possibly bounce off level placements in order to get at them.

### Why? ###

The final project for my computer graphics course @ UBC http://www.ugrad.cs.ubc.ca/~cs314/Vjan2013/. 
Also as a practice exercise in writing an application engine and resource loading pipeline.


### How to play: ###

Mouse aims, left mouse button moves forward, right mouse button jumps.
For purists, you can also use WASD + space.

The project requirements were that the game have things like an objective, a score, losing conditions, and so on. Accordingly, There are shiny things placed throughout the level, which will add to your score if you manage to run into them. The orange ones are worth ten points, and the blue ones are worth three. If you don't get any within the time limit, you lose

That said the recommended mode of play is to zoom around the level trying to smash into as many things as possible. Occasionally the physics engine will get confused and send you hurtling a kilometer into the air -- way more fun than collecting things.


### Features (this was for the original project grading): ###
The game includes all the required functionality, plus the following advanced features:

* Advanced Rendering,
* Shaders,
* sort-of-particle systems,
* Collision Detection,
* LOD control,
* Animation,
* Positional audio

Advanced rendering: Per pixel normal mapping on all level objects, reactive deformation of the terrain, decals and floating "loot" are rendered from distance field textures -- these stay smooth at any zoom level, and allow for dynamic blurring and halos around object edges(with accompanying shaders). This was first brought to mainstream graphics by valve: http://www.valvesoftware.com/publications/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf

All drawing is done through the use of on-video-card vertex buffers. The level geometry gets pushed to the video card on start up, and all the draw calls after that are basically just references to buffer indices.

Shaders: The game uses none of the GL fixed function pipeline. I wrote a per-pixel Phong Lighting Shader and a decal drawing shader instead. It's certainly more complicated to set-up, but after that it behaves similarly to old-school openGL (except that I have to explicitly upload transformation matrices when needed)

Particle systems: Technically the text drawing system is a particle system. The motion is meant to be computed on the video card (with shaders vertices can have arbitrary values, and I set aside four for the 2D velocity and acceleration). It's implemented using two dynamic vertex buffers, used in a double buffered setup. As one buffer is moving through the pipeline, we can bind the other and copy new text/particles to it. Unfortunately I never found the time to actually enable the motion in game, so now there's just really efficient stationary text.

Collision Detection: The game motion is governed by a kinematic simulation, and that includes bouncing off any object in the game in the appropriate direction. This is implemented using a grid lookup for the ground plane (~32000 triangles), and per-object BSP trees for the rest of the static placements (~20000 triangles). The player is modeled as a 20cm radius bouncy sphere attached to a vertical spring:

					 __
					/  \
					\__/
					  \
					  /
					  \
					  /
					  \
					  /
					 ===`

The spring interacts with the ground only, and the sphere interacts with everything. The simulation is kept sane by doing a couple height checks, a forward raycast, and a did-I-pass-through-something check at every frame.

LOD control: The game is fully set up to use lower poly-count geometry for the collisions, and I included a reduced geometry map file as well, but in the end I didn't find that it was required, so it's disabled in the last build.

Animation: The player and the ground and dynamic objects in the game are animated. The objects spin and bounce, and the ground deforms like a trampoline.


### License/Credit: ###
You're free to use everything that I made in whatever way you like. Please let me know if you find any of it useful :) 


The following parts were not made by me, and have their own licences:

1. The stb image library (stb_image.h, stb_image.c), used to load in texture images. The API takes a filename, and provides pointer to image data. Public domain 'license' in the source files.

2. The Eigen Matrix library, which I only use to invert a matrix once, but didn't have time to remove. LGPL, http://eigen.tuxfamily.org/index.php?title=Main_Page

3. the google protobuf Library, used to package+serialize distance field fonts (the packaged fonts I made in a another project of mine, the render engine I wrote for this project). 3-clause BSD License, https://code.google.com/p/protobuf/

4. SDL v1.2, used for window + event management. LGPL, http://libsdl.org/index.php

5. Google Roboto font: Apache License, version 2.0, https://www.google.com/fonts/specimen/Roboto

When I remembered, I copied down websites that I found helpful:

http://www.gamedev.net/page/resources/_/technical/game-programming/practical-collision-detection-r736
http://www.stroustrup.com/C++11FAQ.html#variadic-templates
http://stackoverflow.com/questions/3740905/what-is-gl-modelviewmatrix-and-gl-modelviewprojectionmatrix-in-modern-opengl
http://www.opengl.org/wiki/GLAPI/glVertexAttribPointer
http://cgm.cs.mcgill.ca/~orm/cslines.html
http://web.cs.wpi.edu/~matt/courses/cs563/talks/bsp/bsp.html
http://www.gamedev.net/topic/516608-finding-closest-point-on-tri-to-point/
http://www.gamedev.net/topic/552906-closest-point-on-triangle/
http://www.terathon.com/code/tangent.html
