# mcrt-intro

Introduction to monte carlo ray tracing

## Requirements

* C++ 17
* CMake 3.20
* OpenMP

## Build

```
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make
```

## Structure

each directory has reference implementation under `reference/`.

|Directory|Description|
|:--|:--|
|`1-classical-raytracer/`|start raytracing journey with implementation of classical raytracer|
|`2-simple-pt/`|introduce physically based rendering theory and implement simple path tracer|
|`3-pt`|implement texture, image based lighting(IBL), thin lens camera model|
|`4-bvh/`|implement bounding volume hierarchy(BVH) and render some obj model|
|`5-ggx/`|implement GGX microfacet BRDF|
|`6-disney-brdf/`|implement Disney BRDF|
|`7-nee/`|implement next event estimation(NEE)|
|`8-mis/`|implement multiple importance sampling(MIS)|

## Gallery

### 1-classical-raytracer

![](img/1-classical-raytracer.png)

### 2-simple-pt

![](img/2-simple-pt.png)