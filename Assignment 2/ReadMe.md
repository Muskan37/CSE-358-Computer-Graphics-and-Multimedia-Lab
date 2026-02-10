# Setup

## 1.  Windows ##

   * ### Download glfw libray from https://www.glfw.org/download.html. Download 64-bit Windows binaries from Windows pre-compiled binaries. ###  
   * ### Create *build* and *lib* folder in Code Repo. Put glfw3.dll from lib-mingw-w64 folder of glfw library to *build* and *lib* folder.  ###
   * ### For C++ compiler and make, Download MSYS2 Package Manager from https://www.msys2.org/ . ###  
   * ### Run following commands in MSYS2 terminal: pacman -S base-devel and pacman -S gcc . ### 
   * ### add your bin folder (which includes g++.exe and make.exe) PATH (C:\msys64\usr\bin) of msys64 installation directory in your environment variable.
   * ### Run make win in Terminal. ###
   * ### .exe file will be in *build* folder. ###
   * ### if your code does not run, then you have to check openGL version by installing GLview from http://www.realtech-vr.com/home/glview . If your openGL version is below 3.3, then update windows. ### 
​

Lab 02: Drawing Two Cyan Obtuse Triangles Using OpenGL
📋 Assignment Description
Develop an OpenGL application that renders two cyan-colored obtuse triangles positioned at two distant corners of the screen on an orange background. The program window must display the student’s full name as the title and should terminate when the user presses the first letter of their name on the keyboard.

Requirements Fulfilled
Created a dedicated GitHub repository for CGM course
Orange background window (RGB: 1.0, 0.5, 0.0)
Two cyan-colored obtuse triangles rendered using OpenGL
Triangles positioned at top-left and bottom-right corners
Window title displays student’s full name
Window closes when the initial letter of the name is pressed
Used GLFW + GLAD (Modern OpenGL Core Profile)
Proper code structure, formatting, and comments
Original work implemented and tested locally
README documentation and output screenshot attached
Program Features
Graphics Library: OpenGL 3.3 Core Profile
Window Management: GLFW
Function Loader: GLAD
Window Properties

Size: 800 × 600 pixels
Background Color: Orange (1.0, 0.5, 0.0)

Title: Shobnom Sultana Muskan
Rendering

Two cyan obtuse triangles
Rendered using glDrawArrays(GL_TRIANGLES)
Vertex Array Object (VAO) and Vertex Buffer Object (VBO) used
Keyboard Interaction

Press first letter of your name (e.g., M) to close the window

Technologies:
C++, OpenGL, GLFW, GLAD

