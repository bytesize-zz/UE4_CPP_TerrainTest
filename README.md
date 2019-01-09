TerrainTest

This Part of my Terrain Test includes a map, that spawns chunks in a radius arround the player and deletes out of range chunks.
A combination of different noise functions are building the heightmap. 
Each chunk got his part of it and calculates a simple surface with the help of the heightmap. After that a automaterial uses the surface normals to blend between different textures depending on the slope and height value.

The possibilities of this architecture are limited. There is not much room for runtime manipulation, so i will move on to a voxel setup using this project as starting point. 
