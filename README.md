# My attempt to implement "Tiny Renderer or how OpenGL works: software rendering"

# Check [the wiki](https://github.com/ssloy/tinyrenderer/wiki) for the detailed lessons.

## compilation
```sh
git clone https://github.com/ssloy/tinyrenderer.git &&
cd tinyrenderer &&
mkdir build &&
cd build &&
cmake .. &&
cmake --build . -j &&
./tinyrenderer ../obj/diablo3_pose/diablo3_pose.obj ../obj/floor.obj
```
## The main idea

A renderer with no graphical interface implemented. Implemented entirely in C++.

```C++
#include "tgaimage.h"
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
int main(int argc, char** argv) {
        TGAImage image(100, 100, TGAImage::RGB);
        image.set(52, 41, red);
        image.write_tga_file("output.tga");`
        return 0;
}
```

output.tga should look something like this:

![](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/00-home/reddot.png)


# Teaser: few examples made with the renderer

![](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/00-home/demon.png)

![](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/00-home/diablo-glow.png)

![](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/00-home/boggie.png) 

![](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/00-home/diablo-ssao.png)
