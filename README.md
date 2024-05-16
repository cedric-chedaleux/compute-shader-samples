# compute_shader_samples

This repository holds a list of samples to illustrate the usage of Compute Shaders for GPGPU (General Purpose GPU) tasks.

## List of samples

| Name | Description |
|---|---|
| ssbo_sample | OpenGL basic sample with use of Shader Storage Buffer Objects |


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
```

## License

This project is licensed under MIT license. See LICENSE file for any further information.