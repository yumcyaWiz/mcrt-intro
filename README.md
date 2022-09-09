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
|`2-ao/`|implement ray traced ambient occlusion(RTAO)|
|`3-simple-pt/`|introduce PBR theory and implement simple path tracer|
|`4-ibl/`|implement image based lighting(IBL)|
|`5-ggx/`|implement GGX microfacet BRDF|
|`6-disney-brdf/`|implement Disney BRDF|
|`7-nee/`|implement next event estimation(NEE)|
|`8-mis/`|implement multiple importance sampling(MIS)|
|`9-bvh/`|implement bounding volume hierarchy(BVH) and render some obj model|
|`10-sponza/`|implement material, texture loading and render sponza model|