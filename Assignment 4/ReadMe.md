ID: 0432320005101142

Dynamic Uniform Triangle (OpenGL 3.3)

A simple C++ and OpenGL application demonstrating how to use Uniforms to pass dynamic data from the CPU to the GPU.


Features
Dynamic Coloring: Uses sin and cos functions to oscillate the triangle's color over time.
Interactive Input: - Press W to turn the triangle white.
Press R to lock the triangle color to red.
Press L to close the window.
Core Profile: Uses OpenGL 3.3 Core Profile for modern rendering standards.
Dependencies
GLFW: Windowing and Input.
GLAD: OpenGL Function Loading.
How it Works
Vertex Data: Defines three points in Normalized Device Coordinates (NDC).
Uniforms: The fragment shader contains a uniform vec4 ourColor.
Render Loop:
Every frame, glfwGetTime() is used to calculate new Red and Green values.
glUniform4f updates the shader variable before glDrawArrays is called.
Compilation
Ensure you link against:

glfw3
opengl32 (Windows) or -lGL (Linux)
Include glad.c in your build process.
Keyboard Shortcuts
Key	Action
W	Color: White (while held)
R	Color: Red (Toggle)
L	Close Windo
