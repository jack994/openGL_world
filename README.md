# openGL_world
A 3D scene with fps camera, animations, lights and materials created in OpenGL.  
A detailed description of the project can be found in [detailed_description.pdf](https://github.com/jack994/openGL_world/blob/master/detailed_description.pdf).  
  
This project has been developped in Visual Studio 2017, therefore to run it the IDE is required.

To run the program, dowload the "TinyWorld" folder, open the project in Visual Studio and link the GLEW, GLFW nd GLM libraries as described below.  
  
Sources: [GLEW](http://glew.sourceforge.net), [GLFW](http://www.glfw.org/download.html), [GLM](https://glm.g-truc.net).  
 
1) Copy glew32.dll (glew-2.1.0\bin\Release\Win32) to your project folder.  
2) Right click on the Project name and open properties.  
3) Make sure you select All Configurations and Active(Win32).  
4) Update the links to the libraries (Use the appropriate path depending on where you saved your files  
  
**VC++ Directories >> Include Directories:**  
- D:\GATemplate-master\glfw-3.2.1.bin.WIN32\include  
- D:\GATemplate-master\glew-2.1.0\include  
- D:\GATemplate-master\glm-0.9.9.1\  
  
**VC++ Directories >> Reference Directories:**
- D:\GATemplate-master\glfw-3.2.1.bin.WIN32\lib-vc2015  
- D:\GATemplate-master\glew-2.1.0\bin\Release\Win32  
  
**VC++ Directories >> Library Directories:**  
- D:\GATemplate-master\glfw-3.2.1.bin.WIN32\lib-vc2015  
- D:\GATemplate-master\glew-2.1.0\lib\Release\Win32  
  
**C/C++ >> Additional Include Directories:**  
- D:\GATemplate-master\glfw-3.2.1.bin.WIN32\include  
- D:\GATemplate-master\glew-2.1.0\include  
  
**Linker >> Additional Library Directories:**
- D:\GATemplate-master\glfw-3.2.1.bin.WIN32\lib-vc2015
- D:\GATemplate-master\glew-2.1.0\lib\Release\Win32  
  
**Linker >> Input >> Additional Dependencies:**  
- opengl32.lib  
- glew32.lib  
- glew32s.lib  
- glfw3.lib  
- glfw3dll.lib  
