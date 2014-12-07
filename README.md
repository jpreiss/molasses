molasses
========

software renderer.

vertex/fragment shader system using C++ templates.

Game-style controls:

    W - forward
    S - back
    A - left
    D - right
    Space - up
    Ctrl - down
    

The core renderer draws into a user-supplied buffer; it has no dependencies on OS or a graphics library.

The `/test/` folder uses SFML to draw into a window and read the keyboard/mouse.

It only compiles on Windows currently because it has no Make file and it is not tested on POSIX compilers, but it is fundamentally platform-independent so that will be easy to fix when I get time.

Here's the current status.
The framerate is already poor just running per-pixel Phong lighting on the Teapot with a single light.
Until I can speed it up, I don't want to continue with more advanced gfx stuff like shadows, bump mapping, etc.

I previously thought I was spending all my time in the shaders, which was disappointing, but now I have gotten better at using profilers so I don't trust my previous results.

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

![ScreenShot](https://raw.github.com/jpreiss/molasses/master/molasses.png)
