# ScareCrow EngineEngine
 This is a 3D engine using OpenGL, designed for developing a video game project as part of the UPC master's degree.

 At the moment, this is a prototype engine which can render GLFT files with texturing through a 3D scene and move around the camera freely.

 In future updates, new functionalities will be added in order to offer more resources and tools to develop amazing videogames from diferent genre. 

 Here you have the link to the GitHub repository: https://github.com/MarcLopezS/Engine/ 

## How to use

### Menus
Inside the Engine, you have diferent types of menu, depenging on the information you want to read or modify.

* **General**: Here you have general information about the engine, including a button link to go to the repository of this Engine and an exit button.
* **View**: This section it contains the console LOG, the Texture Menu (to modify properties from rendered texture) and properties that contains information about the Model rendered.
* **Configuration**: in this part, it will display all the configurations of the Engine, showing: performance, window properties, hardware characteristics, software versions and input debugger.

### Controls
The following list has all the inputs required in order to move the camera though the 3D space in the scene:

* `W/S/A/D keys`: Move Forward/Backwards/Left/Right respectively.
* `ARROW keys`: Rotate camera in Z axis (left/right key arrows) or Y axis (up/down key arrows).
* `Hold LEFT/WHEEL mouse button`: Drag the camera, moving x and y camera axis position.
* `Hold RIGHT mouse button`: Displays camera free look around movement.
* `WHEEL UP/DOWN movement`: Zoom In/Out respectively.
* `Hold SHIFT key`: Duplicates movement speed.
* `F key`: focus the camera around the geometry.
* `Hold ALT + LEFT mouse button`: Orbital camera movement.

### Specifications

At first, a house model will be displayed by default. Consider that the model is small, so is easy to not see it.

ScareCrow engine interptrets GLTF files, in order to load meshes and textures to form the Model. To load new gltf models into the engine, consider the following instructions:

1. All GLTF files, binaries and image textures needs to be located in `./Resources/Models` directory.
2. Once files placed, you can drag and drop the GLTF file in the Engine Window. By doing so, it will be removed the previous model and render the dropped GLTF file.
3. Once the model is loaded, the camera will make focus automatically and move far or close on the model rendered, depending on the scale of the geometry.

Note: The texture displayed in models are color based textures. At the moment, no other type of textures (normals, roughness,...) will be displayed. In the future, that functionality will be incorporated.
