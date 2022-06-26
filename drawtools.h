#pragma once
#include <algorithm>
#include <tuple>
#include <string>
#include "geometry.h"
#include <vector>
#include <fstream>

typedef std::tuple<int, int> tuple2;
typedef std::tuple<int, int, vec2> tuple3;






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




void triangle_bary_hidden(int x0, int y0, double z0, int x1, int y1, double z1, int x2, int y2, double z2, TGAImage& image, TGAColor color, double *zbuff, int width) {
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
    int tex_y;
    int tex_x;


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




void triangle_bary_texture(int x0, int y0, double z0, int x1, int y1, double z1, int x2, int y2, double z2, TGAImage& image, TGAImage& diffuse_map, double* zbuff, int width, vec2 uv0, vec2 uv1, vec2 uv2, double intensity) {
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
    int tex_y;
    int tex_x;



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

                /* Recall that barycentric coordinates are themselves an interpolation within a triangle. In essence, the 
                   x y and z for the barycentric coordinates can b multiplied with the x, y, and z components of the three 
                   vertices to interpolate inside a triangle. This lets us map a point inside the triangle to a point in
                   another image or map corresponding to the image. */
                tex_x = roundToInt((uv0.x * bary.x + uv1.x * bary.y + uv2.x * bary.z) * (double)diffuse_map.width());
                tex_y = roundToInt((uv0.y * bary.x + uv1.y * bary.y + uv2.y * bary.z) * (double)diffuse_map.height());
                if (z > zbuff[idx]) {
                    TGAColor color = diffuse_map.get(tex_x, tex_y);
                    int b = roundToInt(color.bgra[0] * intensity);
                    int g = roundToInt(color.bgra[1] * intensity);
                    int r = roundToInt(color.bgra[2] * intensity);
                    int a = roundToInt(color.bgra[3]);
                    color = TGAColor(r, g, b, a);
                    zbuff[idx] = z;
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



void render_triangles(Model* model, int width, int height, TGAImage& image, vec3 lightdir, bool hidden, double * zbuff, bool texture) {
    // loop through the faces, which hold a bunch of vertices which have coords
    int x0;
    int x1;
    int x2;
    int y0;
    int y1;
    int y2;
    vec3 v0;
    vec3 v1;
    vec3 v2;

    TGAImage diff_map = model->diffuse();

    for (int i = 0; i < width * height; i++) {
        zbuff[i] = -std::numeric_limits<float>::max();
    }

    

    for (int i = 0; i < model->nfaces(); i++) {
        v0 = model->vert(i, 0);
        v1 = model->vert(i, 1);
        v2 = model->vert(i, 2);

        x0 = roundToInt((v0.x + 1.0) * width / 2.0);
        y0 = roundToInt((v0.y + 1.0) * height / 2.0);
        x1 = roundToInt((v1.x + 1.0) * width / 2.0);
        y1 = roundToInt((v1.y + 1.0) * height / 2.0);
        x2 = roundToInt((v2.x + 1.0) * width / 2.0);
        y2 = roundToInt((v2.y + 1.0) * height / 2.0);
        
        // compute the normal of the triangle by taking a cross product
        vec3 n = cross(v2 - v0, v1 - v0);
        lightdir.normalize();
        n.normalize();
        
        // Compute the intensity of the light on the face
        double intensity = n * lightdir;
        int scaledInt = intensity * 255;

        // Render the non-negatively oriented vector 
        if (scaledInt > 0) {
            scaledInt = (scaledInt > 255) ? 255 : scaledInt;

            // If we are applying texture, then we find the values
            if ((hidden) && (texture)) {
                vec2 uv0 = model->uv(i, 0);
                vec2 uv1 = model->uv(i, 1);
                vec2 uv2 = model->uv(i, 2);
                triangle_bary_texture(x0, y0, v0.z, x1, y1, v1.z, x2, y2, v2.z, image, diff_map, zbuff, width, uv0, uv1, uv2, intensity);
            }
            else if (hidden) {
                triangle_bary_hidden(x0, y0, v0.z, x1, y1, v1.z, x2, y2, v2.z, image, TGAColor(scaledInt, scaledInt, scaledInt, 255), zbuff, width);
            }
            else {
                triangle_bary(x0, y0, x1, y1, x2, y2, image, TGAColor(scaledInt, scaledInt, scaledInt, 255));
            }
        }
        
    }
    
}




void write_image(std::string fname, TGAImage &image) {
    fname.append(".tga");
    image.write_tga_file(fname);
}