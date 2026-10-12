// filters.h
// Declarations of all image-filter functions.
//
// Students:                                  IDs:                       Filters:
// 1- Youssef Ehab Ahmed            -         20242428         -         grayScale / flipImage / mergeImages / SunlightFilter
// 2- Ibrahim Mohamed Hosny         -         20250006         -         blackAndWhite / rotateImage / detectEdges / oilPainting
// 3- Seif Khaled Ragab             -         20251205         -         invertImage / darkenAndLighten / cropImage / purpleEffect
// 4- Youssef Saied Helmy           -         20251496         -         addFrame / imageResize / blurImage / infraredFilter

#pragma once

#include "Image_Class.h"

// ─── Filter function declarations ─────────────────────────────────────────────
void sunlightFilter   (Image& img);
void mergeImages      (Image& img1, Image img2);
void grayscale        (Image& image);

// NOTE: flipImage direction parameter added for GUI (was previously cin-based).
//   direction == 1  → vertical flip
//   direction == 2  → horizontal flip
void flipImage        (Image& image, int direction);

void blackAndWhite    (Image& image);
void rotateImage      (Image& image, int angle);
void invertImage      (Image& image);

// darkAndlightImage — signature unchanged.
//   choice == 1  → lighten   choice == 2  → darken
void darkAndlightImage(Image& image, int percentage, int choice);

void resizeImage      (Image& image, int new_width, int new_height);
void tvFilter         (Image& image);
void detectEdges      (Image& image);
void oilPainting      (Image& image);
void cropImage        (Image& image, int x, int y, int width, int height);
void purpleFilter     (Image& image);
void addFrame         (Image& image, int frame, int type);
void blurImage        (Image& image);
void infraredFilter   (Image& original);
void skewImage        (Image& img, int percentage);
