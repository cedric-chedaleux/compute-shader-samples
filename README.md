# compute_shader_samples

This repository holds a list of samples to illustrate the usage of Compute Shaders for GPGPU (General Purpose GPU) tasks.

## List of samples

| Name | Description |
|---|---|
| ssbo_sample | OpenGL sample that performs parallel operation on a vector of integers using Shader Storage Buffer Objects and workgroups |
| img_generation | OpenGL sample that generates a procedural image thanks to workgroups and ImageStore() method |


## Build

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