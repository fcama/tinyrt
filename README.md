# tinyrt

Tinyrt is a compact path tracer written in modern C++ supporting of real-time features.

<img src="https://user-images.githubusercontent.com/70849941/161048235-8ff199fa-b47b-4706-94ff-48bd29849e48.png" width="700" height="700">

## Building
This project currently only supports Linux, either natively or though WSL \
\
Pre-requisites:
* libglfw3
* libglfw3-dev
* cmake 3.18

#### Build Instructions
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cd ..
./tinyrt
```

## Features
Tinyrt supports Diffuse, Reflective and Dielectric materials. \
It comes with a GUI which can be used to set different debug views or control the camera. \
<img src="https://user-images.githubusercontent.com/70849941/161051198-58a6d640-fd75-4d52-94b7-afbbe1ea2868.png" width="500" height="500">
<img src="https://user-images.githubusercontent.com/70849941/161051203-52bc71fb-4e86-4a9f-b09c-f4aa9955dd36.png" width="500" height="500">
<img src="https://user-images.githubusercontent.com/70849941/161051206-6482e84d-bdd6-4112-ab07-5c42328150cf.png" width="500" height="500">

## Progress
<!--- 
![output](https://user-images.githubusercontent.com/70849941/161052165-ddd29096-27f6-4119-9b2a-e412ad448946.png)
![output copy 2](https://user-images.githubusercontent.com/70849941/161052183-94dce0e1-ab32-42c0-9987-6e0e0c9d8db1.png)
![output copy 4](https://user-images.githubusercontent.com/70849941/161052193-4833bde1-e1d8-4ae3-836e-ef0d8f83a02b.png)
-->
<p float="left">
  <img src="https://user-images.githubusercontent.com/70849941/161052197-ed6fec67-89e0-4672-b236-90fe123fc640.png" width="500">
  <img src="https://user-images.githubusercontent.com/70849941/161052206-482b47bc-a738-4a05-9ddc-b9e7a1e62815.png" width="500">
</p>

<p float="left">
  <img src="https://user-images.githubusercontent.com/70849941/161052233-84b5b03d-1b6c-4390-a83e-43fa0c2191cf.png" width="500">
  <img src="https://user-images.githubusercontent.com/70849941/161052237-ab405052-c351-421f-8b14-91fc87e15216.png" width="500">
</p>

<img src="https://user-images.githubusercontent.com/70849941/161052275-1334c022-81fb-4f60-8793-9ed93e9b93c7.png" width="1005">

## References

Links to the resources used during development of this project:
- [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
- [PBRT book](https://www.pbr-book.org/)
- [etx-tracer](https://github.com/serhii-rieznik/etx-tracer)
