#pragma once
#include <algorithm>
#include <tuple>
#include <string>
#include "geometry.h"
#include <vector>
#include <fstream>
#include <cmath>
#include <Eigen/Dense>
#include "imagedata.h"
#include "shader.h"

typedef std::tuple<int, int> tuple2;
typedef std::tuple<int, int, vec2> tuple3;

using Eigen::MatrixXd;
using Eigen::Vector4d;



int roundToInt(double n) {
    if (((int)(n * 10) % 10) >= 5) {
        return (int)n + 1;
    }
    else {
        return (int)n;
    }
}



void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror = 2 * std::abs(dy);
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
            error -= 2 * dx;
        }
    }

}



void triangle_lines(int x0, int y0, int x1, int y1, int x2, int y2, TGAImage &image, TGAColor color) {
    std::vector <tuple2> v;
    tuple2 t0 (y0, x0);
    tuple2 t1 (y1, x1);
    tuple2 t2 (y2, x2);
    v.push_back(t0);
    v.push_back(t1);
    v.push_back(t2);
    std::sort(v.begin(), v.end());
    
    
    // Now we guarantee that y0 < y1 < y2
    x0 = std::get<1>(v[0]);
    y0 = std::get<0>(v[0]);
    x1 = std::get<1>(v[1]);
    y1 = std::get<0>(v[1]);
    x2 = std::get<1>(v[2]);
    y2 = std::get<0>(v[2]);

    // Now, we start drawing lines between the two sides up to the next highest value
    float m02 = (x2 - x0) / (float)(y2 - y0);
    float m01 = (x1 - x0) / (float)(y1 - y0);
    int x02;
    int x01;
    // Since we have to iterate through y, we have to calculate x from y. we do that by knowing
    for (int y = y0; y < y1; y++) {
        x02 = (y - y0)*m02 + x0;
        x01 = (y - y0)*m01 + x0;
        line(x01, y, x02, y, image, color);
    }
    // Now we draw the top half
    int xInt = x02;
    m02 = (x2 - x02) / (float)(y2 - y1);
    float m12 = (x2 - x1) / (float)(y2 - y1);
    for (int y = y1; y < y2; y++) {
        x02 = (y - y1) * m02 + xInt;
        x01 = (y - y1) * m12 + x1;
        line(x01, y, x02, y, image, color);
    }
}




vec3 bary_coords(vec3 xVec, vec3 yVec) {
    vec3 bary = cross(xVec, yVec);
    if (std::abs(bary.z) >= 1e-2) {
        bary = vec3{ 1.0 - (bary.x + bary.y) / bary.z, bary.y / bary.z, bary.x / bary.z };
        return bary;
        
    }
    return vec3{ -1.0, 1.0, 1.0 };
}



vec3 barycentric(vec3 v0, vec3 v1, vec3 v2, vec2 P) {
    Eigen::Matrix3d ABC;
    ABC(0, 0) = v0.x;
    ABC(1, 0) = v0.y;
    ABC(2, 0) = v0.z;

    ABC(0, 1) = v1.x;
    ABC(1, 1) = v1.y;
    ABC(2, 1) = v1.z;

    ABC(0, 2) = v2.x;
    ABC(1, 2) = v2.y;
    ABC(2, 2) = v2.z;

    if (ABC.determinant() < 1e-3) {
        return vec3{ -1.0, 1.0, 1.0 };
    }
    Eigen::Vector3d p3( P.x, P.y, 1.0 );
    p3 = ABC.inverse().transpose() * p3;
    return vec3{ p3[0], p3[1], p3[2] };
}


void triangle_bary(int x0, int y0, int x1, int y1, int x2, int y2, TGAImage& image, TGAColor color) {
    // Get the bounding box based on the sizes
    int xVals[] = { x0, x1, x2 };
    int yVals[] = { y0, y1, y2 };
    int* minX = std::min_element(std::begin(xVals), std::end(xVals));
    int* maxX = std::max_element(std::begin(xVals), std::end(xVals));
    int* minY = std::min_element(std::begin(yVals), std::end(yVals));
    int* maxY = std::max_element(std::begin(yVals), std::end(yVals));
    
    // Get our barycentric vectors set up
    double ABx = (double) x1 - x0;
    double ABy = (double) y1 - y0;
    double ACx = (double) x2 - x0;
    double ACy = (double) y2 - y0;
    double PAx;
    double PAy;
    vec3 xVec;
    vec3 yVec;
    vec3 bary;

    for (int x = *minX; x < *maxX; x++) {
        for (int y = *minY; y < *maxY; y++) {
            PAx = (double) x0 - x;
            PAy = (double) y0 - y;
            xVec = vec3{ ACx, ABx, PAx };
            yVec = vec3{ ACy, ABy, PAy };
            bary = bary_coords(xVec, yVec);
            if (!((bary.x < 0) || (bary.y < 0) || (bary.z < 0))) {
                image.set(x, y, color);
            }
        }
    }
}



void triangle_bary_hidden(int x0, int y0, double z0, int x1, int y1, double z1, int x2, int y2, double z2, TGAImage& image, TGAColor color, double* zbuff, int width) {
    // Get the bounding box based on the sizes
    int xVals[] = { x0, x1, x2 };
    int yVals[] = { y0, y1, y2 };
    int* minX = std::min_element(std::begin(xVals), std::end(xVals));
    int* maxX = std::max_element(std::begin(xVals), std::end(xVals));
    int* minY = std::min_element(std::begin(yVals), std::end(yVals));
    int* maxY = std::max_element(std::begin(yVals), std::end(yVals));

    // Get our barycentric vectors set up
    double ABx = (double)(x1 - x0);
    double ABy = (double)(y1 - y0);
    double ACx = (double)(x2 - x0);
    double ACy = (double)(y2 - y0);
    double PAx;
    double PAy;
    vec3 xVec;
    vec3 yVec;
    vec3 bary;
    int idx;
    double z;


    for (int x = *minX; x < *maxX; x++) {
        for (int y = *minY; y < *maxY; y++) {
            idx = y * width + x;
            PAx = (double)(x0 - x);
            PAy = (double)(y0 - y);
            xVec = vec3{ ACx, ABx, PAx };
            yVec = vec3{ ACy, ABy, PAy };
            bary = bary_coords(xVec, yVec);
            if (!((bary.x < 0) || (bary.y < 0) || (bary.z < 0))) {
                z = bary.x * z0 + bary.y * z1 + bary.y * z2;
                if (z > zbuff[idx]) {
                    zbuff[idx] = z;
                    image.set(x, y, color);
                }
            }
        }
    }
}



void rasterize(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color, int buff[]) {
    
    if (x0 > x1) {
        std::swap(x0, x1);
    }

    // Goal: if the original buffer's y is less in the dimension we want than the new one, replace it using the new one
    for (int x = x0; x < x1; x++) {
        float t = (x - x0) / (float)(x1 - x0);
        int y = y0*(1.0 - t) + y1*t;;
        if (y > buff[x]) {
            buff[x] = y;
            // Normally we want to do y, but we only have 1d image this time
            for (int j = 0; j < 10; j++) {
                image.set(x, j, color);
            }
            
        }
    }
}



void triangle_bary_texture(Shader* shader, Eigen::Vector4d V0, Eigen::Vector4d V1, Eigen::Vector4d V2, TGAImage& image, TGAImage& diffMap, TGAImage& specMap, double* zbuff, double* xbuff, double* ybuff) {
    // Get the bounding box based on the sizes

    vec3 v0 = shader->project(V0);
    vec3 v1 = shader->project(V1);
    vec3 v2 = shader->project(V2);

    int x0 = v0.x;
    int x1 = v1.x;
    int x2 = v2.x;
    int y0 = v0.y;
    int y1 = v1.y;
    int y2 = v2.y;

    
    
    int xVals[] = { x0, x1, x2 };
    int yVals[] = { y0, y1, y2 };
    int* minX = std::min_element(std::begin(xVals), std::end(xVals));
    int* maxX = std::max_element(std::begin(xVals), std::end(xVals));
    int* minY = std::min_element(std::begin(yVals), std::end(yVals));
    int* maxY = std::max_element(std::begin(yVals), std::end(yVals));

    // Get our barycentric vectors set up
    double ABx = (double)(x1 - x0);
    double ABy = (double)(y1 - y0);
    double ACx = (double)(x2 - x0);
    double ACy = (double)(y2 - y0);
    double PAx;
    double PAy;

    vec3 xVec;
    vec3 yVec;
    vec3 bary;
    int idx;
    double z;
    bool discard;
    TGAColor color = TGAColor(255, 255, 255, 255);
    


    for (int x = *minX; x <= *maxX; x++) {
        for (int y = *minY; y <= *maxY; y++) {
            idx = y * shader->imWidth + x;
            PAx = (double)(x0 - x);
            PAy = (double)(y0 - y);
            xVec = vec3{ ACx, ABx, PAx };
            yVec = vec3{ ACy, ABy, PAy };
            bary = bary_coords(xVec, yVec);
            // bary = barycentric(v0, v1, v2, vec2(x, y));
            z = bary.x * V0[2] + bary.y * V1[2] + bary.y * V2[2];
            // w = bary.x * V0[3] + bary.y * V1[3] + bary.y * V2[3]; 
            vec3 bary_vp = vec3{ bary.x / V0[3], bary.y / V1[3], bary.z / V2[3] };
            bary_vp = bary_vp / (bary_vp.x + bary_vp.y + bary_vp.z);
            // int frag_depth = std::max(0, std::min(255, int(z/w + 0.5)));
            
            double frag_depth = bary_vp * shader->unview(V0, V1, V2);
            if (!((bary.x < 0) || (bary.y < 0) || (bary.z < 0) || (zbuff[idx] > frag_depth))) {
                /* Recall that barycentric coordinates are themselves an interpolation within a triangle. In essence, the 
                   x y and z for the barycentric coordinates can b multiplied with the x, y, and z components of the three 
                   vertices to interpolate inside a triangle. This lets us map a point inside the triangle to a point in
                   another image or map corresponding to the image. */
                discard = shader->frag(bary, diffMap, specMap, color);
                if (!discard) {
                    zbuff[idx] = frag_depth;
                    image.set(x, y, color);
                }
            }
        }
    }
}



void y_buffer_test(TGAImage &image) {
    int ybuffer[800];
    for (int i = 0; i < 800; i++) {
        ybuffer[i] = INT16_MIN;
    }
    rasterize(20, 34, 744, 400, image, TGAColor(255, 0, 0, 255), ybuffer);
    rasterize(120, 434, 444, 400, image, TGAColor(0, 255, 0, 255), ybuffer);
    rasterize(330, 463, 594, 200, image, TGAColor(0, 0, 255, 255), ybuffer);
}



void render_wireframe(Model *model, int width, int height, TGAImage &image, TGAColor color) {
    // loop through the faces, which hold a bunch of vertices which have coords
    for (int i = 0; i < model->nfaces(); i++) {
        for (int j = 0; j < 3; j++) {
            vec3 v0 = model->vert(i, j);
            vec3 v1 = model->vert(i, (j + 1) % 3);
            int x0 = roundToInt((v0.x + 1.0) * width / 2.0);
            int y0 = roundToInt((v0.y + 1.0) * height / 2.0);
            int x1 = roundToInt((v1.x + 1.0) * width / 2.0);
            int y1 = roundToInt((v1.y + 1.0) * height / 2.0);
            line(x0, y0, x1, y1, image, color);
        }
    }
    // image.flip_vertically();
}

double norm(vec3 v) {
    return std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2) + std::pow(v.z, 2));
}



double dot(vec3 v, vec3 u) {
    return v.x * u.x + v.y * u.y + v.z * u.z;
}



int compute_intensity(ImageData* imData, vec3 v0, vec3 v1, vec3 v2) {
    vec3 n = cross(v2 - v0, v1 - v0);
    n.normalize();

    // Compute the intensity of the light on the face
    double intensity = n * imData->get_ld();
    return intensity * 255;
}


int cull(ImageData* imData, vec3 v0, vec3 v1, vec3 v2) {
    vec3 n = cross(v2 - v0, v1 - v0);
    n.normalize();
    vec3 view = imData->get_cam_origin() - v1;
    view.normalize();

    // Compute the intensity of the light on the 
    return (view * n) < 0;
}



void render_triangles(Model* model, Shader* shader, TGAImage& image, double* zbuff, double* xbuff, double* ybuff, bool texture, bool hidden) {
    TGAImage diffMap = model->diffuse();
    TGAImage specMap = model->specular();

    
    //Debugging only: 
    // std::ofstream out;
    // out.open("log_coords.txt"); 


    for (int i = 0; i < model->nfaces(); i++) {
        // bool goCull = cull(imData, v0, v1, v2);
        

        // Find the perspective transform, and apply it
        Vector4d V0 = shader->vertex(i, 0);
        Vector4d V1 = shader->vertex(i, 1);
        Vector4d V2 = shader->vertex(i, 2);

        
        /* Debugging only
        out << v0p << std::endl;
        out << v1p << std::endl;
        out << v2p << std::endl;*/ 
        // out << i << std::endl;

        

        // Render the non-negatively oriented vector 
       

        // If we are applying texture, then we find the values
        if ((hidden) && (texture)) {
            triangle_bary_texture(shader, V0, V1, V2, image, diffMap, specMap, zbuff, xbuff, ybuff);
        }
        /*else if (hidden) {
            triangle_bary_hidden(x0, y0, z0, x1, y1, z1, x2, y2, z2, image, TGAColor(scaledInt, scaledInt, scaledInt, 255), zbuff, width);
        }
        else {
            triangle_bary(x0, y0, x1, y1, x2, y2, image, TGAColor(scaledInt, scaledInt, scaledInt, 255));
        }*/
        
    }
    // out.close();
    
}




void write_image(std::string fname, TGAImage &image) {
    fname.append(".tga");
    image.write_tga_file(fname);
}