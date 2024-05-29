# compute_shader_samples

This repository holds a list of samples to illustrate the usage of Compute Shaders for GPGPU (General Purpose GPU) tasks.

It includes samples for different platforms:
* OpenGL (with GLSL shaders)
* WebGPU (with WGSL shaders)

## OpenGL samples

| Name | Description |
|---|---|
| ssbo_sample | Sample that performs parallel operation on a vector of integers using Shader Storage Buffer Objects and workgroups |
| img_generation | Sample that generates a procedural image thanks to workgroups and ImageStore() method |
| convert2gray | Sample that converts a color image to a grayscale image using imageLoad/Store |
| boxblur | Sample that blurs an input image using box/mean blur algorithm and show usage of shared memory |

## WebGPU samples

| Name | Description |
|---|---|
| buffer | Sample that performs parallel operation on a vector of integers using GPUBuffer objects for reading and writing data |

*Note: To make the examples run, WebGPU should be supported in your browser (see https://developer.mozilla.org/en-US/docs/Web/API/Navigator/gpu#browser_compatibility for compatibility)*

## Build OpenGL examples

```
$ cmake -Bbuild .
$ cmake --build build --target install
```

`cmake` installs all binaries and shaders into the `install` directory.

# Usage

```
$ ./install/bin/ssbo_sample
...
$ ./install/bin/img_generation
Image saved to 'image.png'
```

## License

This project is licensed under MIT license. See LICENSE file for any further information.