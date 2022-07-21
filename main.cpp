#include <limits>
#include <vector>
#include <iostream>
// #include "model.h"
#include "our_gl.h"
#include "tgaimage.h"
#include "drawtools.h"
#include "imagedata.h"
#include "shader.h"

// TODO: Fix the weirdness where the z has to be inverted for some reason
// TODO: Fix the weirdness of triangles not being filled in, despite existing on the texture map
// TODO: Make the stuff more wieldy, maybe incorporate the buffers inside the image data



constexpr int width  = 800; // output image size
constexpr int height = 800;
constexpr int depth = 255;

const vec3 light_dir(1, -0.5, -3); // light source
const vec3       eye(1, 0,-3); // camera position
const vec3    center(0,0,0); // camera direction
const vec3        up(0,1,0); // camera up vector

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

using namespace std;

extern mat<4,4> ModelView; // "OpenGL" state matrices
extern mat<4,4> Projection;

/*  struct Shader : IShader {
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
};*/



int main(int argc, char** argv) {
    TGAImage image(width, height, TGAImage::RGB);
    // TGAImage* imptr = &image;
    // Model* model = new Model("../../obj/diablo3_pose/diablo3_pose.obj");
    Model* model = new Model("../../obj/african_head/african_head.obj");
    Shader* shader = new Shader(width / 8, height / 8, width * 3 / 4, height * 3 / 4, depth, light_dir, center, eye, up);
    shader->initialize(model, width);


    double* zbuff = new double[width * height];
    double* xbuff = new double[width * height];
    double* ybuff = new double[width * height];
    TGAColor test = (model->specular()).get(600, 600);
    // double specInt = 
    cout << "test" << endl;
    // cout << test.bgra[1] << test.bgra[2] << test.bgra[3] << endl;

   //  cout << imData->per << endl;
    for (int i = 0; i < width * height; i++) {
        zbuff[i] = -std::numeric_limits<int>::max();
    }

    // cout << imData->transformV(Vector4d( 0, 0, -25, 1 )) << endl;
    // render_wireframe(model, width, height, image, white);
    
    render_triangles(model, shader, image, zbuff, xbuff, ybuff, true, true);
    // image.flip_vertically();
   

    // write_image("z_buffer_test", image);
    write_image("spec_test", image);
    /*std::ofstream out;
    out.open("zbuff.txt");
    for (int i = 0; i < width * height; i++) {
        if (zbuff[i] > -std::numeric_limits<float>::max()) {
            out << xbuff[i] << " " << ybuff[i] << " " << zbuff[i] << endl;
        }
       
    }
    out.close();*/
    delete[] zbuff;
    delete[] xbuff;
    delete[] ybuff;
    delete model;
    delete shader;
    return 0;
}

