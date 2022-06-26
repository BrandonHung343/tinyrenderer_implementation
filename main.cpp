#include <limits>
#include <vector>
#include <iostream>
#include "model.h"
#include "our_gl.h"
#include "tgaimage.h"
#include "drawtools.h"


constexpr int width  = 800; // output image size
constexpr int height = 800;

const vec3 light_dir(1,1,1); // light source
const vec3       eye(1,1,3); // camera position
const vec3    center(0,0,0); // camera direction
const vec3        up(0,1,0); // camera up vector

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

using namespace std;

extern mat<4,4> ModelView; // "OpenGL" state matrices
extern mat<4,4> Projection;

struct Shader : IShader {
    const Model &model;
    vec3 uniform_l;       // light direction in view coordinates
    mat<2,3> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    mat<3,3> varying_nrm; // normal per vertex to be interpolated by FS
    mat<3,3> view_tri;    // triangle in view coordinates

    Shader(const Model &m) : model(m) {
        uniform_l = proj<3>((ModelView*embed<4>(light_dir, 0.))).normalize(); // transform the light vector to view coordinates
    }

    virtual void vertex(const int iface, const int nthvert, vec4& gl_Position) {
        varying_uv.set_col(nthvert, model.uv(iface, nthvert));
        varying_nrm.set_col(nthvert, proj<3>((ModelView).invert_transpose()*embed<4>(model.normal(iface, nthvert), 0.)));
        gl_Position= ModelView*embed<4>(model.vert(iface, nthvert));
        view_tri.set_col(nthvert, proj<3>(gl_Position));
        gl_Position = Projection*gl_Position;
    }

    virtual bool fragment(const vec3 bar, TGAColor &gl_FragColor) {
        vec3 bn = (varying_nrm*bar).normalize(); // per-vertex normal interpolation
        vec2 uv = varying_uv*bar; // tex coord interpolation

        // for the math refer to the tangent space normal mapping lecture
        // https://github.com/ssloy/tinyrenderer/wiki/Lesson-6bis-tangent-space-normal-mapping
        mat<3,3> AI = mat<3,3>{ {view_tri.col(1) - view_tri.col(0), view_tri.col(2) - view_tri.col(0), bn} }.invert();
        vec3 i = AI * vec3(varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0);
        vec3 j = AI * vec3(varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0);
        mat<3,3> B = mat<3,3>{ {i.normalize(), j.normalize(), bn} }.transpose();

        vec3 n = (B * model.normal(uv)).normalize(); // transform the normal from the texture to the tangent space
        double diff = std::max(0., n*uniform_l); // diffuse light intensity
        vec3 r = (n*(n*uniform_l)*2 - uniform_l).normalize(); // reflected light direction, specular mapping is described here: https://github.com/ssloy/tinyrenderer/wiki/Lesson-6-Shaders-for-the-software-renderer
        double spec = std::pow(std::max(-r.z, 0.), 5+sample2D(model.specular(), uv)[0]); // specular intensity, note that the camera lies on the z-axis (in view), therefore simple -r.z

        TGAColor c = sample2D(model.diffuse(), uv);
        for (int i : {0,1,2})
            gl_FragColor[i] = std::min<int>(10 + c[i]*(diff + spec), 255); // (a bit of ambient light, diff + spec), clamp the result

        return false; // the pixel is not discarded
    }
};

/* void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    bool steep = false;
    if (abs(x0 - x1) < abs(y0 - y1)) {
        swap(x0, y0);
        swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror = 2*abs(dy);
    // cout << derror << endl;
    int error = 0.0;
    int y = y0;

    for (int x = x0; x <= x1; x++) {
        if (steep) {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }
        error += derror;
        if (error > dx) {
            if (y1 > y0) {
                y += 1;
            }
            else {
                y -= 1;
            }
            error -= 2*dx;
        }
    }
    
}

void triangle(int x0, int y0, int x1, int y1, int x2, int y2, TGAImage &image, TGAColor color) {
    line(x0, y0, x1, y1, image, color);
    line(x1, y1, x2, y2, image, color);
    line(x0, y0, x2, y2, image, color); 
} */


int main(int argc, char** argv) {
    TGAImage image(width, height, TGAImage::RGB);
    // TGAImage* imptr = &image;
    Model* model = new Model("../../obj/diablo3_pose/diablo3_pose.obj");
    // Model* model = new Model("../../obj/african_head/african_head.obj");
    vec3 lightdir = vec3{0.0, 0.0, -1.0};
    double* zbuff = new double[width * height];


    // render_wireframe(model, width, height, image, white);
    // triangle_bary(10, 50, 450, 290, 100, 490, image, red);
    // triangle_bary(10, 10, 100, 30, 190, 160, image, red);
    render_triangles(model, width, height, image, lightdir, true, zbuff, true);
    // y_buffer_test(image);
    // image.flip_vertically();
   

    vec3 ax = vec3{ 40., 30., -10. };
    vec3 ay = vec3{ 2., 15., -12. };
    vec3 bary = bary_coords(ax, ay);
    vec3 p = vec3{ 30, 42, 0.07 };
    cout << p * bary;
    write_image("z_buffer_test", image);
    delete[] zbuff;
    return 0;






    /* if (2>argc) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }
    TGAImage framebuffer(width, height, TGAImage::RGB); // the output image
    lookat(eye, center, up);                            // build the ModelView matrix
    viewport(width/8, height/8, width*3/4, height*3/4); // build the Viewport matrix
    projection((eye-center).norm());                    // build the Projection matrix
    std::vector<double> zbuffer(width*height, std::numeric_limits<double>::max());

    for (int m=1; m<argc; m++) { // iterate through all input objects
        Model model(argv[m]);
        Shader shader(model);
        for (int i=0; i<model.nfaces(); i++) { // for every triangle
            vec4 clip_vert[3]; // triangle coordinates (clip coordinates), written by VS, read by FS
            for (int j : {0,1,2})
                shader.vertex(i, j, clip_vert[j]); // call the vertex shader for each triangle vertex
            triangle(clip_vert, shader, framebuffer, zbuffer); // actual rasterization routine call
        }
    }
    framebuffer.write_tga_file("framebuffer.tga"); // the vertical flip is moved inside the function
    return 0; */
}

