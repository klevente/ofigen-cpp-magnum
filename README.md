# ofigen-cpp
C++ implementation of OFIGEN, a Blender plugin for creating training data for neural networks learning optical flow.
This version is standalone and optimized for performance to enable creating big datasets in a short-time.

## Tasks:
* Read in backgrounds as image files, which will provide a background for the scene
* Read in 3D models as `.obj` files, that will be placed on the scene
* Move the objects/camera/lights on the scene randomly
* Render the result of one iteration of movement to an image on the GPU
* Provide the result in a text file, that will be used by the neural network for learning
* Easy configuration using a config file for setting backgrounds, models, random parameters, etc.
* Ability to see the rendered pictures real-time on the screen

