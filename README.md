molasses
========

software renderer

Here's an update.
The framerate is already crappy just running per-pixel Phong lighting on the Teapot with a single light.
I used a profiler and found out that almost all execution time is spent running the shaders themselves -
not on rasterization overhead, copying frames inefficiently, or anything else.
This has seriously dampened my motivation to continue with more advanced gfx stuff like shadows, bump mapping, etc.

I would like to try to use SSE2 SIMD instructions to speed up math, but that requires a whole new shader approach.
Each arithmetic operation has to be batched together across 4 shaders.
So, the quandary:  I want to continue writing shaders as if they take one input and produce one output,
but the code that the CPU actually executes should do 4 at a time (at least).
This makes my C++ template based shader approach useless.

GPUs solve the problem by compiling your shader and introducing the parallelism in hardware.
I'm beginning to think that I need to write a shader compiler
accepting a limited mathematical input language and outputting cache-optimized SSE2 code.
I could also write an expression template system in C++.

---------------------------------------------------

`/test/` folder requires SFML dependency.

![ScreenShot](https://raw.github.com/jpreiss/molasses/master/molasses.png)
