# modern-opengl-app
opengl 4.6 + approaching zero driver overhead


## Vulkan vs Opengl

reduce CPU time.
GPU performance same


## C++ Build Deps management
Bazel

Bootstrap

## online generate specs glad

https://gen.glad.sh/generated/tmp10hq4vf8glad/

## volk 

To use volk, you have to include volk.h instead of vulkan/vulkan.h

https://github.com/zeux/volk

```c++
#define VK_NO_PROTOTYPES //for volk

#define VOLK_IMPLEMENTATION
#include "volk.h"


```



## Build Example

```shell
cd opengl/hello-world
mkdir -p build & cd build
cmake .. -G "Unix Makefiles"  --config Debug

cmake --build . --config Debug
make -j 8
make clean


cmake .. -G "Unix Makefiles"  -DCMAKE_BUILD_TYPE=Debug
```

## Build Debug with Ninja
```shell
mkdir debug
cd debug
cmake .. -G "Ninja"  -DCMAKE_BUILD_TYPE=Debug
ninja
```

## issue of undefined reference
/home/xiao/modern-opengl-app/./shared/di.hpp:1847: undefined reference to `SharedUtils::UnixPlatform::UnixPlatform()'
collect2: error: ld returned 1 exit status
ninja: build stopped: subcommand failed.

do cmake again