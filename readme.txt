Ray Tracer
Project 4 for Computer Graphics
By Jack Garrard

This program creates a scene that is created from a json file. The graphics procedure it uses is called ray casting and it involves testing for the nearests objects color with rays.

The output of this program is in ppm type 3 style.

Usage: raycast.c width height input.json output.ppm
width: the width of the final image in pixels
height: the height of the final image in pixels
input.json: the input json file to read the scene from
output.ppm: the output file to write the image to. 


This now can use pretty nice shading rendering along with shadow casting. It also allows for quadric rendering, even though it wasn't a requirement.

Also now has recursive rendering to allow for refraction and reflection between objects.

TODO Stuff:
Add Proper refraction using a ray cast
Add threading w/ thread pool
Add shadow refraction
Add view plane movement
