# raytracer
A simple ray tracer using my own linear algebra header lib.
The output is written to ppm file. (Ideally to a png once the core components are complete.)

## Progress Images
Camera Fov and Aspect Ratio
![Camera Has Fov And Aspect Ratio](/progress/4_fov_and_rectangular.png)
Basic shadows
![Point Light](/progress/5_shadows.png)
RGB Materials
![RGB Materials](/progress/6_materials.png)
Current Progress - implementing diffuse lighting
![Current Progress](/progress/7_currentprogress.png)

## References
https://www.realtimerendering.com/raytracing/Ray%20Tracing%20in%20a%20Weekend.pdf

### Purpose
This project's purpose was to trial and experiment with some of the more advanced c++ styles and techniques.
- Focus on a data oriented design. This allows for ideal cpu caching as objects are inefficient when theyre data is unrelated.
- Use an imperative style over an object oriented.
- Comments should be only for purpose, function names and lambda names should say what they are doing.
