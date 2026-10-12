// filters.cpp
// Contains all image-filter implementations.
// Extracted from the original main.cpp (terminal main() removed).
//
// Students:                                  IDs:                       Filters:
// 1- Youssef Ehab Ahmed            -         20242428         -         grayScale / flipImage / mergeImages / SunlightFilter
// 2- Ibrahim Mohamed Hosny         -         20250006         -         blackAndWhite / rotateImage / detectEdges / oilPainting
// 3- Seif Khaled Ragab             -         20251205         -         invertImage / darkenAndLighten / cropImage / purpleEffect
// 4- Youssef Saied Helmy           -         20251496         -         addFrame / imageResize / blurImage / infraredFilter

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "Image_Class.h"
#include "filters.h"

using namespace std;

// ─── Filter implementations ───────────────────────────────────────────────────

void sunlightFilter(Image& img)
{
    for (int i = 0; i < img.width; i++)
    {
        for (int j = 0; j < img.height; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                if (k == 0)
                {
                    int newValue = (img(i,j,k)* 1.25) + 15;
                    newValue = newValue > 255? 255: newValue;
                    img(i,j,k) = newValue;
                }
                else if (k==1)
                {
                    int newValue = (img(i,j,k)* 1.05);
                    newValue = newValue > 255? 255: newValue;
                    img(i,j,k) = newValue;
                }
                else{
                    int newValue = (img(i,j,k)* 0.9);
                    newValue = newValue > 255? 255: newValue;
                    img(i,j,k) = newValue;
                }
            }
        }
    }
}

void mergeImages(Image& img1, Image img2)
{
    try
    {
        int width = min(img1.width, img2.width);
        int height = min(img1.height, img2.height);
        Image mergeImages(width, height);
        {
            for (int w = 0; w < width; w++)
            {
                for (int h = 0; h < height; h++)
                {
                    int red1 = img1.getPixel(w, h, 0);
                    int red2 = img2.getPixel(w, h, 0);

                    int green1 = img1.getPixel(w, h, 1);
                    int green2 = img2.getPixel(w, h, 1);

                    int blue1 = img1.getPixel(w, h, 2);
                    int blue2 = img2.getPixel(w, h, 2);

                    int newRed = (int)(red1 + red2) / 2;
                    int newGreen = (int)(green1 + green2) / 2;
                    int newBlue = (int)(blue1 + blue2) / 2;

                    mergeImages.setPixel(w, h, 0, newRed);
                    mergeImages.setPixel(w, h, 1, newGreen);
                    mergeImages.setPixel(w, h, 2, newBlue);
                }
            }
        }
        img1 = mergeImages;
    }
    catch (const exception& y)
    {
        cerr << "ERROR!!!" << y.what();
    }
}


void grayscale(Image& image)
{
    for (int i = 0; i < image.width; ++i)
    {
        for (int j = 0; j < image.height; ++j)
        {
            unsigned int avg = 0;

            for (int k = 0; k < 3; ++k)
            {
                avg += image(i, j, k);
            }

            avg /= 3;

            image(i, j, 0) = avg;
            image(i, j, 1) = avg;
            image(i, j, 2) = avg;
        }
    }
}


// CHANGED from original: accepts 'direction' parameter instead of using cin.
//   direction == 1  → vertical flip
//   direction == 2  → horizontal flip
// The body of each case is identical to the original.
void flipImage(Image& image, int direction)
{
    switch (direction)
    {
    case 1:
    {
        for(int i = 0; i < image.width/2;i++){
            for(int j = 0; j <image.height;j++){
                for(int k = 0; k < 3; k++){
                    unsigned char temp = image(i,j,k);
                    image(i,j,k) = image(image.width-i-1,j,k);
                    image(image.width-i-1,j,k) = temp;
                }
            }
        }
    };
    break;

    case 2:
    {
        for (int j = 0; j < image.height / 2; j++)
        {
            for (int i = 0; i < image.width; i++)
            {
                for (int k = 0; k < 3; k++)
                {
                    unsigned char temp = image(i, j, k);
                    image(i,j,k) = image(i,image.height-j-1,k);
                    image(i,image.height-1-j,k) = temp;
                }
            }
        }
    }
    break;
    }
}


void blackAndWhite(Image& image) {
    for(int y = 0; y < image.height; ++y) {
        for(int x = 0; x < image.width; ++x) {
            int r = image.getPixel(x, y, 0);
            int g = image.getPixel(x, y, 1);
            int b = image.getPixel(x, y, 2);

            int avg = (r + g + b) / 3;
            if(avg < 128) {
                image.setPixel(x, y, 0, 0);
                image.setPixel(x, y , 1, 0);
                image.setPixel(x, y, 2, 0);
            } else {
                image.setPixel(x, y, 0, 255);
                image.setPixel(x, y , 1, 255);
                image.setPixel(x, y, 2, 255);
            }
        }
    }
}


void rotateImage(Image& image, int angle) {
    int height = image.height;
    int width = image.width;
    int newWidth = width, newHeight = height;

    if(angle == 0) return;
    if(angle == 90 || angle == 270) {newWidth = height; newHeight = width;}
    else if(angle == 180) {newWidth= width; newHeight = height;}
    else return;

    Image temp(newWidth, newHeight);

    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            int newX, newY;
            if(angle == 90 ) {newX = height - 1 - y; newY = x;}
            if(angle == 180) {newX = width - 1 - x; newY = height - 1 - y;}
            if(angle == 270) {newX = y; newY = width - 1 - x;}

            int r = image.getPixel(x, y, 0);
            int g = image.getPixel(x, y, 1);
            int b = image.getPixel(x, y, 2);

            temp.setPixel(newX, newY, 0, r);
            temp.setPixel(newX, newY, 1, g);
            temp.setPixel(newX, newY, 2, b);
        }
    }

    image = temp;
}


void invertImage(Image& image){
    for(int y=0;y<image.height;y++){
        for(int x=0;x<image.width;x++){
            int r=255-image(x,y,0);
            int g=255-image(x,y,1);
            int b=255-image(x,y,2);
            image(x,y,0)=r;
            image(x,y,1)=g;
            image(x,y,2)=b;
            }
    }
}


// UNCHANGED — signature and body identical to original.
//   choice == 1  → lighten   choice == 2  → darken
void darkAndlightImage(Image& image, int percentage, int choice)
{
    for (int y = 0; y < image.height; y++)
    {
        for (int x = 0; x < image.width; x++)
        {
            int r = image(x, y, 0);
            int g = image(x, y, 1);
            int b = image(x, y, 2);

            if (choice == 1)
            {
                r = r + (255 - r) * percentage / 100;
                g = g + (255 - g) * percentage / 100;
                b = b + (255 - b) * percentage / 100;
            }
            else if (choice == 2)
            {
                r = r - r * percentage / 100;
                g = g - g * percentage / 100;
                b = b - b * percentage / 100;
            }

            image(x, y, 0) = r;
            image(x, y, 1) = g;
            image(x, y, 2) = b;
        }
    }
}


void resizeImage(Image& image, int new_width, int new_height)
{
    Image result(new_width, new_height);

    for (int y = 0; y < new_height; y++)
    {
        for (int x = 0; x < new_width; x++)
        {
            int x_original = x * image.width / new_width;
            int y_original = y * image.height / new_height;

            result(x, y, 0) = image(x_original, y_original, 0);
            result(x, y, 1) = image(x_original, y_original, 1);
            result(x, y, 2) = image(x_original, y_original, 2);
        }
    }

    image = result;
}



void tvFilter(Image& image) {
    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            if (y % 2 != 0) {
                for (int k = 0; k < 3; k++) {
                    image(x, y, k) = image(x, y, k) / 1.5;
                }
            }
        }
    }
}



void detectEdges(Image& image)
{
    int height = image.height;
    int width = image.width;

    Image gray = image;
    grayscale(gray);

    int sobelX[3][3] = {
        {-1,  0,  1},
        {-2,  0,  2},
        {-1,  0,  1}
    };

    int sobelY[3][3] = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };

    Image edges(width, height);
    int threshold = 150;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int gradX = 0;
            int gradY = 0;

            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx < 0) nx = 0;
                    if (nx >= width) nx = width - 1;
                    if (ny < 0) ny = 0;
                    if (ny >= height) ny = height - 1;

                    int pixel1 = gray(nx, ny, 0);

                    gradX += sobelX[dy + 1][dx + 1] * pixel1;
                    gradY += sobelY[dy + 1][dx + 1] * pixel1;
                }
            }

            int magnitude = sqrt(gradX * gradX + gradY * gradY);
            int result = (magnitude > threshold) ? 0 : 255;

            edges(x, y, 0) = result;
            edges(x, y, 1) = result;
            edges(x, y, 2) = result;
        }
    }

    image = edges;
}



void oilPainting(Image& image) {
    int height = image.height, width = image.width;
    Image temp(width, height);

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            int intensity, levels = 20, bucket, radius = 2;
            int count[256] = {0}, sumR[256] = {0}, sumG[256] = {0}, sumB[256] = {0};

            for(int dy = -radius; dy <= radius; dy++) {
                for (int dx = -radius; dx <= radius; dx++) {
                    int nx = min(max(x + dx, 0), width - 1);
                    int ny = min(max(y + dy, 0), height - 1);
                    int r = image.getPixel(nx, ny, 0);
                    int g = image.getPixel(nx, ny, 1);
                    int b = image.getPixel(nx, ny, 2);
                    intensity = (r + g + b) / 3;
                    bucket = intensity * (levels - 1) / 255;
                    count[bucket]++  ;
                    sumR[bucket] += r;
                    sumG[bucket] += g;
                    sumB[bucket] += b;
                }
            }
            int best = 0;
            for(int i = 0; i < levels; i++) { if(count[i] > count[best]) best = i; }
            temp.setPixel(x, y, 0, sumR[best]/count[best]);
            temp.setPixel(x, y, 1, sumG[best]/count[best]);
            temp.setPixel(x, y, 2, sumB[best]/count[best]);
        }
    }
    image = temp;
}


void cropImage(Image& image, int x, int y, int width2, int height2){
    try{
        Image img(width2,height2);
        for(int w= 0; w < width2; w++){
            for(int h=0; h< height2; h++){
                int red   = image.getPixel(x+w, y+h, 0);
                int green = image.getPixel(x+w, y+h, 1);
                int blue  = image.getPixel(x+w, y+h, 2);

                img.setPixel(w,h,0,red);
                img.setPixel(w,h,1,green);
                img.setPixel(w,h,2,blue);
            }
        }
        image = img;
    }
    catch(const exception& y){
        cerr<<"ERROR!!!"<<y.what();
    }
}


void purpleFilter(Image& image){
    try{
        for(int w =0 ; w< image.width ; w++){
            for(int h = 0 ; h< image.height ; h++){
                int red   = image.getPixel(w,h,0);
                int green = image.getPixel(w,h,1);
                int blue  = image.getPixel(w,h,2);
                int NewRed   = min(255, (int) (red*1.3));
                int NewGreen = (int) (green*.5);
                int NewBlue  = min(255,(int) (blue*1.3));
                image.setPixel(w,h,0,NewRed);
                image.setPixel(w,h,1,NewGreen);
                image.setPixel(w,h,2,NewBlue);
            }
        }
    }
    catch(const exception& y){
        cerr<<"ERROR"<<y.what();
    }
}


void addFrame(Image& image, int frame, int type){
    try{
        int newWidth  = image.width  + (2*frame);
        int newHeight = image.height + (2*frame);
        Image border_img(newWidth,newHeight);

        if(type == 1){
            for(int w =0;w<image.width;w++){
                for(int h = 0;h<image.height;h++){
                    int red   = image.getPixel(w,h,0);
                    int green = image.getPixel(w,h,1);
                    int blue  = image.getPixel(w,h,2);

                    border_img.setPixel(w+frame,h+frame,0,red);
                    border_img.setPixel(w+frame,h+frame,1,green);
                    border_img.setPixel(w+frame,h+frame,2,blue);
                }
            }
        }
        else{
            int thin  = max(1, frame / 10);
            int outer = max(0, frame - 3 * thin);

            for(int x = 0; x < newWidth; x++){
                for(int y = 0; y < newHeight; y++){
                    bool insidePhoto = x >= frame && x < frame + image.width &&
                                       y >= frame && y < frame + image.height;

                    int r, g, b;

                    if(insidePhoto){
                        r = image.getPixel(x - frame, y - frame, 0);
                        g = image.getPixel(x - frame, y - frame, 1);
                        b = image.getPixel(x - frame, y - frame, 2);
                    }
                    else{
                        int d = min(min(x, y), min(newWidth - 1 - x, newHeight - 1 - y));
                        int color;

                        if(d < outer) color = 0;
                        else if(d < outer + thin) color = 255;
                        else if(d < outer + 2 * thin) color = 0;
                        else color = 255;

                        r = color;
                        g = color;
                        b = color;
                    }

                    border_img.setPixel(x, y, 0, r);
                    border_img.setPixel(x, y, 1, g);
                    border_img.setPixel(x, y, 2, b);
                }
            }
        }

        image = border_img;
    }
    catch(const exception& y){
        cerr<<"ERROR!!"<<y.what();
    }
}


void blurImage(Image& image) {
    try {
        Image blured(image.width, image.height);

        for (int w = 0; w < image.width; w++) {
            for (int h = 0; h < image.height; h++) {

                int sumRed = 0, sumGreen = 0, sumBlue = 0;
                int count = 0;

                for (int dw = -7; dw <= 7; dw++) {
                    for (int dh = -7; dh <= 7; dh++) {

                        int besideW = w + dw;
                        int besideH = h + dh;

                        if (besideW >= 0 && besideW < image.width &&
                            besideH >= 0 && besideH < image.height) {

                            sumRed   += image.getPixel(besideW, besideH, 0);
                            sumGreen += image.getPixel(besideW, besideH, 1);
                            sumBlue  += image.getPixel(besideW, besideH, 2);

                            count++;
                        }
                    }
                }

                blured.setPixel(w, h, 0, sumRed   / count);
                blured.setPixel(w, h, 1, sumGreen / count);
                blured.setPixel(w, h, 2, sumBlue  / count);
            }
        }
        image = blured;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}


void infraredFilter(Image& original){
    try{
        for(int w =0; w<original.width; w++){
            for(int h=0; h<original.height; h++){
                int red   = original.getPixel(w,h,0);
                int green = original.getPixel(w,h,1);
                int blue  = original.getPixel(w,h,2);
                int newRed   = 255;
                int newGreen = 255 - green;
                int newBlue  = 255 - blue;
                original.setPixel(w,h,0,newRed);
                original.setPixel(w,h,1,newGreen);
                original.setPixel(w,h,2,newBlue);
            }
        }
    }
    catch(exception& e){
        cerr<<"Error: "<<e.what()<<endl;
    }
}


void skewImage(Image& img, int percentage)
{
    int oldWidth  = img.width;
    int oldHeight = img.height;

    int maxShift = oldWidth * percentage / 100;

    Image result(oldWidth + maxShift, oldHeight);

   for (int i = 0; i < result.width; i++)
    {
        for (int j = 0; j < result.height; j++)
        {
            result(i, j, 0) = 255;
            result(i, j, 1) = 255;
            result(i, j, 2) = 255;
        }
    }

    for (int y = 0; y < oldHeight; y++)
    {
        // Top row = maxShift, Bottom row = 0
        int shift = maxShift * (oldHeight - 1 - y) / (oldHeight - 1);

        for (int x = 0; x < oldWidth; x++)
        {
            int newX = x + shift;

            result(newX, y, 0) = img(x, y, 0);
            result(newX, y, 1) = img(x, y, 1);
            result(newX, y, 2) = img(x, y, 2);
        }
    }

    img = result;
}
