// Students:                                  IDs:    
// 1- Youssef Ehab Ahmed            -         20242428
// 2- Ibrahim Mohamed Hosny         -         20250006
// 3- Seif Khaled Ragab             -         20251205
// 4- Youssef Saied Helmy           -         20251496






#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include "mainwindow.h"
#include "filters.h"

#include <stack>
#include <string>
#include <functional>
#include <cmath>

static Image             image;
static std::string       filename;
static bool              isSaved = true;
static std::stack<Image> versions;

static MainWindow* win = nullptr;


static void updatePreview()
{
    if (!image.imageData || image.width == 0 || image.height == 0) return;
    image.saveImage("images/temp.jpg");
    win->displayPreview("images/temp.jpg");
}

static bool checkImageLoaded()
{
    if (filename.empty() || !image.imageData) {
        QMessageBox::information(win, "No Image", "Please load an image first (New).");
        return false;
    }
    return true;
}

static void applyFilter(std::function<void()> fn)
{
    if (!checkImageLoaded()) return;
    versions.push(image);
    isSaved = false;
    try {
        fn();
    } catch (const std::exception& e) {
        image = versions.top();
        versions.pop();
        QMessageBox::warning(win, "Filter Error", QString::fromStdString(e.what()));
        return;
    }
    updatePreview();
}


static void onSave()
{
    if (filename.empty()) {
        QMessageBox::information(win, "Save", "No image loaded.");
        return;
    }
    try {
        image.saveImage(filename);
        isSaved = true;
    } catch (const std::exception& e) {
        QMessageBox::critical(win, "Save Error",
            QString("Failed to save:\n%1").arg(QString::fromStdString(e.what())));
    }
}

static void onSaveAs()
{
    if (filename.empty() || !image.imageData) {
        QMessageBox::information(win, "Save As", "No image loaded.");
        return;
    }
    QString path = win->showSaveAsDialog(QString::fromStdString(filename));
    if (path.isEmpty()) return;
    try {
        image.saveImage(path.toStdString());
        filename = path.toStdString();
        isSaved = true;
    } catch (const std::exception& e) {
        QMessageBox::critical(win, "Save Error",
            QString("Failed to save:\n%1").arg(QString::fromStdString(e.what())));
    }
}

static bool handleUnsavedChanges()
{
    if (isSaved || filename.empty()) return true;

    int result = win->showUnsavedChangesDialog();

    if (result == 0) return false;  // cancel
    if (result == 1) return true;   // discard
    if (result == 2) { onSave();   return isSaved; }
    if (result == 3) { onSaveAs(); return isSaved; }
    return false;
}

static void onNew()
{
    if (!handleUnsavedChanges()) return;

    QString path = QFileDialog::getOpenFileName(win,
        "Open Image", "",
        "Images (*.jpg *.jpeg *.png *.bmp);;All Files (*)");

    if (path.isEmpty()) return;

    filename = path.toStdString();
    try {
        Image loadedImage(filename);
        image = loadedImage;
        isSaved = true;
        while (!versions.empty()) versions.pop();
        win->resetSliders();
        updatePreview();
    } catch (const std::exception& e) {
        QMessageBox::critical(win, "Load Error",
            QString("Failed to load image:\n%1").arg(QString::fromStdString(e.what())));
        filename.clear();
    }
}

static void onRemove()
{
    if (filename.empty() && !image.imageData) {
        return; // No image loaded
    }
    if (!handleUnsavedChanges()) return;

    while (!versions.empty()) versions.pop();
    filename.clear();
    image = Image();
    isSaved = true;
    win->resetSliders();
    win->clearPreview();
}

static void onUndo()
{
    if (filename.empty()) {
        QMessageBox::information(win, "Undo", "No image loaded.");
        return;
    }
    if (versions.empty()) {
        QMessageBox::information(win, "Undo", "Nothing to undo.");
        return;
    }
    image = versions.top();
    versions.pop();
    isSaved = false;
    updatePreview();
}


static void doGrayscale()     { applyFilter([](){ grayscale(image); }); }
static void doBlackAndWhite() { applyFilter([](){ blackAndWhite(image); }); }
static void doInvert()        { applyFilter([](){ invertImage(image); }); }
static void doSunlight()      { applyFilter([](){ sunlightFilter(image); }); }
static void doTv()            { applyFilter([](){ tvFilter(image); }); }
static void doPurple()        { applyFilter([](){ purpleFilter(image); }); }
static void doInfrared()      { applyFilter([](){ infraredFilter(image); }); }
static void doDetectEdges()   { applyFilter([](){ detectEdges(image); }); }
static void doOilPainting()   { applyFilter([](){ oilPainting(image); }); }
static void doBlur()          { applyFilter([](){ blurImage(image); }); }


static void doFlip()
{
    int dir = win->flipDirection();
    applyFilter([dir](){ flipImage(image, dir); });
}

static void doRotate(int angle)
{
    if (!checkImageLoaded()) return;
    applyFilter([angle](){ rotateImage(image, angle); });
}

static void doBrightness()
{
    int val = win->brightnessValue();
    if (val == 0) return;
    int pct    = std::abs(val);
    int choice = (val > 0) ? 1 : 2;
    applyFilter([pct, choice](){ darkAndlightImage(image, pct, choice); });
}

static void doResize()
{
    int w = win->resizeWidth();
    int h = win->resizeHeight();
    applyFilter([w, h](){ resizeImage(image, w, h); });
}

static void doMerge()
{
    QString path = win->mergePath();
    if (path.isEmpty()) {
        QMessageBox::warning(win, "Merge", "Please select a second image file.");
        return;
    }
    std::string spath = path.toStdString();
    applyFilter([spath]() {
        Image secondImage(spath);
        mergeImages(image, secondImage);
    });
}

static void doCrop()
{
    int x = win->cropX();
    int y = win->cropY();
    int w = win->cropWidth();
    int h = win->cropHeight();
    if (w <= 0 || h <= 0) {
        QMessageBox::warning(win, "Crop", "Width and height must be > 0.");
        return;
    }
    applyFilter([x, y, w, h](){ cropImage(image, x, y, w, h); });
}

static void doAddFrame()
{
    int size = win->frameSize();
    int type = win->frameType();
    applyFilter([size, type](){ addFrame(image, size, type); });
}

static void doSkew()
{
    int pct = win->skewValue();
    applyFilter([pct](){ skewImage(image, pct); });
}



int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Image Editor");
    app.setApplicationVersion("1.0");

    QDir().mkpath("images");

    MainWindow w;
    win = &w;

    w.setCloseHandler(handleUnsavedChanges);

    QObject::connect(w.btnNew(),    &QPushButton::clicked, onNew);
    QObject::connect(w.btnSave(),   &QPushButton::clicked, onSave);
    QObject::connect(w.btnSaveAs(), &QPushButton::clicked, onSaveAs);
    QObject::connect(w.btnUndo(),   &QPushButton::clicked, onUndo);
    QObject::connect(w.btnRemove(), &QPushButton::clicked, onRemove);

    QObject::connect(w.btnGrayscale(),     &QPushButton::clicked, doGrayscale);
    QObject::connect(w.btnBlackAndWhite(), &QPushButton::clicked, doBlackAndWhite);
    QObject::connect(w.btnInvert(),        &QPushButton::clicked, doInvert);
    QObject::connect(w.btnSunlight(),      &QPushButton::clicked, doSunlight);
    QObject::connect(w.btnTv(),            &QPushButton::clicked, doTv);
    QObject::connect(w.btnPurple(),        &QPushButton::clicked, doPurple);
    QObject::connect(w.btnInfrared(),      &QPushButton::clicked, doInfrared);
    QObject::connect(w.btnDetectEdges(),   &QPushButton::clicked, doDetectEdges);
    QObject::connect(w.btnOilPainting(),   &QPushButton::clicked, doOilPainting);
    QObject::connect(w.btnBlur(),          &QPushButton::clicked, doBlur);

    QObject::connect(w.btnApplyFlip(),       &QPushButton::clicked, doFlip);
    QObject::connect(w.btnRotate90L(),       &QPushButton::clicked, [](){ doRotate(270); });
    QObject::connect(w.btnRotate90R(),       &QPushButton::clicked, [](){ doRotate(90);  });
    QObject::connect(w.btnRotate180(),       &QPushButton::clicked, [](){ doRotate(180); });
    QObject::connect(w.btnApplyBrightness(), &QPushButton::clicked, doBrightness);
    QObject::connect(w.btnApplyResize(),     &QPushButton::clicked, doResize);
    QObject::connect(w.btnApplyMerge(),      &QPushButton::clicked, doMerge);
    QObject::connect(w.btnApplyCrop(),       &QPushButton::clicked, doCrop);
    QObject::connect(w.btnApplyFrame(),      &QPushButton::clicked, doAddFrame);
    QObject::connect(w.btnApplySkew(),       &QPushButton::clicked, doSkew);

    w.show();
    return app.exec();
}