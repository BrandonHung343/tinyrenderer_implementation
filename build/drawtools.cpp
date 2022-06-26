void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
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
    int derror = 2 * abs(dy);
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

void triangle(int x0, int y0, int x1, int y1, int x2, int y2, TGAImage& image, TGAColor color) {
    line(x0, y0, x1, y1, image, color);
    line(x1, y1, x2, y2, image, color);
    line(x0, y0, x2, y2, image, color);
}