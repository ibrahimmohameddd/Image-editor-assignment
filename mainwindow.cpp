#include "mainwindow.h"
#include "icons.h"

#include <QApplication>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QCloseEvent>
#include <QDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QResizeEvent>
#include <QScreen>
#include <QScrollArea>
#include <QSize>
#include <QSlider>
#include <QSpinBox>
#include <QSvgRenderer>
#include <QPainter>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSizePolicy>
#include <QFont>

// ─── Palette constants ─────────────────────────────────────────────────────────
static const char* C_BG        = "#1E1E1E";
static const char* C_PANEL     = "#252525";
static const char* C_PANEL2    = "#2B2B2B";
static const char* C_HOVER     = "#3A3A3A";
static const char* C_ACTIVE    = "#454545";
static const char* C_BLUE      = "#1473E6";
static const char* C_TEXT      = "#F0F0F0";
static const char* C_TEXT2     = "#B8B8B8";
static const char* C_DIVIDER   = "#3D3D3D";

static const int SIDEBAR_W     = 230;
static const int NAVBAR_H      = 54;
static const int ICON_SIZE     = 18;
static const int PANEL_ANIM_MS = 180;

// ─── Utility: render SVG string to QIcon ──────────────────────────────────────
static QIcon svgToIcon(const QString& svgData, int size = ICON_SIZE)
{
    QSvgRenderer renderer(svgData.toUtf8());
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    renderer.render(&painter);
    return QIcon(pix);
}

// ─── Utility: create a styled spin box ────────────────────────────────────────
static QSpinBox* makeSpinBox(int min, int max, int value, QWidget* parent = nullptr)
{
    auto* sb = new QSpinBox(parent);
    sb->setRange(min, max);
    sb->setValue(value);
    sb->setFixedHeight(28);
    sb->setStyleSheet(QString(
        "QSpinBox { background:#1E1E1E; color:%1; border:none; border-radius:4px; padding:2px 8px; }"
        "QSpinBox::up-button, QSpinBox::down-button { width:16px; background:%2; border-radius:2px; }"
    ).arg(C_TEXT, C_HOVER));
    return sb;
}

// ─── Utility: create a styled label ───────────────────────────────────────────
static QLabel* makeLabel(const QString& text, QWidget* parent = nullptr, bool secondary = false)
{
    auto* lbl = new QLabel(text, parent);
    lbl->setStyleSheet(QString("QLabel { color:%1; font-size:11px; }").arg(secondary ? C_TEXT2 : C_TEXT));
    return lbl;
}

// ─── Utility: create a small apply/action button inside a panel ───────────────
static QPushButton* makeApplyButton(const QString& text, QWidget* parent = nullptr)
{
    auto* btn = new QPushButton(text, parent);
    btn->setFixedHeight(28);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet(QString(
        "QPushButton { background:%1; color:white; border:none; border-radius:14px; font-size:12px; font-weight:600; padding:0 12px; }"
        "QPushButton:hover { background:#1a82ff; }"
        "QPushButton:pressed { background:#0f5dc4; }"
    ).arg(C_BLUE));
    return btn;
}

// ─── Utility: compact choice button (rotate/flip options) ─────────────────────
static QPushButton* makeChoiceButton(const QString& text, QWidget* parent = nullptr)
{
    auto* btn = new QPushButton(text, parent);
    btn->setCheckable(true);
    btn->setFixedHeight(26);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet(QString(
        "QPushButton { background:%1; color:%2; border:none; border-radius:13px; font-size:11px; font-weight:500; padding:0 8px; }"
        "QPushButton:hover { background:%3; }"
        "QPushButton:checked { background:%4; color:white; font-weight:600; }"
    ).arg(C_HOVER, C_TEXT, C_ACTIVE, C_BLUE));
    return btn;
}

// ─── Utility: panel separator line ────────────────────────────────────────────
static QFrame* makeSeparator(QWidget* parent = nullptr)
{
    auto* sep = new QFrame(parent);
    sep->setFrameShape(QFrame::HLine);
    sep->setFixedHeight(1);
    sep->setStyleSheet(QString("QFrame { background:%1; border:none; }").arg(C_DIVIDER));
    return sep;
}

// ═══════════════════════════════════════════════════════════════════════════════
// FilterPanel
// ═══════════════════════════════════════════════════════════════════════════════

FilterPanel::FilterPanel(const QString& svgIcon,
                         const QString& label,
                         QWidget*       content,
                         QWidget*       parent)
    : QWidget(parent), m_content(content)
{
    setContentsMargins(0, 0, 0, 0);
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Header button ──
    m_header = new QPushButton(this);
    m_header->setCheckable(true);
    m_header->setFixedHeight(34);
    m_header->setCursor(Qt::PointingHandCursor);
    m_header->setIcon(svgToIcon(svgIcon, 17));
    m_header->setIconSize(QSize(17, 17));
    m_header->setText("   " + label);
    m_header->setToolTip(label);
    m_header->setStyleSheet(QString(
        "QPushButton { background:transparent; color:%1; border:none; text-align:left;"
        "  font-size:14px; font-weight:500; padding-top:1px; padding-bottom:1px; padding-left:14px; padding-right:12px;"
        "  border-radius:17px; margin:0px 6px; font-family:'Segoe UI', sans-serif; }"
        "QPushButton:hover { background:%2; border:none; border-radius:17px; }"
        "QPushButton:checked { background:%3; color:white; font-weight:600; border:none; border-radius:17px; }"
        "QPushButton:pressed { background:%3; border:none; border-radius:17px; }"
    ).arg(C_TEXT, C_HOVER, C_ACTIVE));

    root->addWidget(m_header);

    // ── Content panel (animated) ──
    if (content) {
        content->setParent(this);
        content->setMaximumHeight(0);
        content->setVisible(true);
        content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
        content->setStyleSheet(QString(
            "QWidget { background:%1; border-radius:12px; margin:2px 6px; font-family:'Segoe UI', sans-serif; }"
        ).arg(C_PANEL));
        root->addWidget(content);

        m_animation = new QPropertyAnimation(content, "maximumHeight", this);
        m_animation->setDuration(PANEL_ANIM_MS);
        m_animation->setEasingCurve(QEasingCurve::InOutQuad);
    }

    connect(m_header, &QPushButton::clicked, this, [this]() {
        emit headerClicked(this);
    });
}

void FilterPanel::expand()
{
    if (m_expanded || !m_content) return;
    m_expanded = true;
    m_header->setChecked(true);

    int targetH = m_content->sizeHint().height();
    if (targetH < 20) targetH = m_content->layout()
        ? m_content->layout()->sizeHint().height() + 16
        : 80;

    m_animation->stop();
    m_animation->setStartValue(0);
    m_animation->setEndValue(targetH + 8);
    m_animation->start();
}

void FilterPanel::collapse()
{
    if (!m_expanded || !m_content) return;
    m_expanded = false;
    m_header->setChecked(false);

    m_animation->stop();
    m_animation->setStartValue(m_content->maximumHeight());
    m_animation->setEndValue(0);
    m_animation->start();
}

void FilterPanel::toggleExpand()
{
    if (m_expanded) collapse(); else expand();
}

// ═══════════════════════════════════════════════════════════════════════════════
// MainWindow — construction
// ═══════════════════════════════════════════════════════════════════════════════

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // Frameless window with custom title bar
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, false);
    resize(1280, 800);

    // ── Global stylesheet ──
    setStyleSheet(QString(
        "QMainWindow { background:%1; font-family:'Segoe UI', -apple-system, Roboto, sans-serif; }"
        "QWidget { font-family:'Segoe UI', -apple-system, Roboto, sans-serif; }"
        "QScrollArea { border:none; background:%1; }"
        "QScrollBar:vertical { background:%2; width:6px; margin:0; border:none; }"
        "QScrollBar::handle:vertical { background:%3; border-radius:3px; min-height:20px; }"
        "QScrollBar::handle:vertical:hover { background:#555; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }"
        "QScrollBar:horizontal { background:%2; height:6px; border:none; }"
        "QScrollBar::handle:horizontal { background:%3; border-radius:3px; }"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width:0; }"
        "QToolTip { background:#333; color:%4; border:1px solid %5; font-size:11px; padding:4px 6px; }"
    ).arg(C_BG, C_PANEL, C_HOVER, C_TEXT, C_DIVIDER));

    // ── Root layout ──
    auto* central  = new QWidget(this);
    auto* rootVBox = new QVBoxLayout(central);
    rootVBox->setContentsMargins(0, 0, 0, 0);
    rootVBox->setSpacing(0);
    setCentralWidget(central);

    // Navbar
    m_navbar = buildNavbar();
    rootVBox->addWidget(m_navbar);

    // Body: sidebar + preview
    auto* bodyWidget = new QWidget;
    bodyWidget->setStyleSheet(QString("background:%1;").arg(C_BG));
    auto* bodyHBox = new QHBoxLayout(bodyWidget);
    bodyHBox->setContentsMargins(0, 0, 0, 0);
    bodyHBox->setSpacing(0);

    m_sidebar = buildSidebar();
    bodyHBox->addWidget(m_sidebar);

    m_previewArea = buildPreviewArea();
    bodyHBox->addWidget(m_previewArea, 1);

    rootVBox->addWidget(bodyWidget, 1);

    // Center on screen
    QScreen* screen = QApplication::primaryScreen();
    if (screen) {
        QRect sg = screen->availableGeometry();
        move(sg.center() - rect().center());
    }
}

// ─── Navbar ───────────────────────────────────────────────────────────────────

QWidget* MainWindow::buildNavbar()
{
    auto* bar = new QWidget;
    bar->setFixedHeight(NAVBAR_H);
    bar->setStyleSheet(QString(
        "QWidget { background:%1; border:none; }"
    ).arg(C_PANEL));

    auto* hbox = new QHBoxLayout(bar);
    hbox->setContentsMargins(10, 0, 8, 0);
    hbox->setSpacing(0);

    // ── Left: branding ──
    auto* brandWidget = new QWidget;
    brandWidget->setFixedWidth(SIDEBAR_W);
    brandWidget->setStyleSheet(QString("background:%1;").arg(C_PANEL));
    auto* brandHBox = new QHBoxLayout(brandWidget);
    brandHBox->setContentsMargins(18, 0, 14, 0);

    auto* brandLabel = new QLabel("Image Editor");
    brandLabel->setStyleSheet(QString(
        "QLabel { color:%1; font-size:15px; font-weight:700; letter-spacing:0.5px; background:transparent; }"
    ).arg(C_TEXT));
    brandHBox->addWidget(brandLabel);
    brandHBox->addStretch();

    hbox->addWidget(brandWidget);

    // ── Center: file actions ──
    auto* centerWidget = new QWidget;
    centerWidget->setStyleSheet("background:transparent;");
    auto* centerHBox = new QHBoxLayout(centerWidget);
    centerHBox->setContentsMargins(12, 0, 12, 0);
    centerHBox->setSpacing(8);
    centerHBox->setAlignment(Qt::AlignCenter);

    m_btnNew    = makeNavButton(Icons::newFile(),    "New");
    m_btnSave   = makeNavButton(Icons::save(),       "Save");
    m_btnSaveAs = makeNavButton(Icons::saveAs(),     "Save As");
    m_btnUndo   = makeNavButton(Icons::undo(),       "Undo");
    m_btnRemove = makeNavButton(Icons::removeIcon(), "Remove");
    // No connect — application connections established in main.cpp

    centerHBox->addWidget(m_btnNew);
    centerHBox->addWidget(m_btnSave);
    centerHBox->addWidget(m_btnSaveAs);
    centerHBox->addWidget(m_btnUndo);
    centerHBox->addWidget(m_btnRemove);

    hbox->addWidget(centerWidget, 1);

    // ── Right: window controls ──
    auto* winCtrlWidget = new QWidget;
    winCtrlWidget->setStyleSheet("background:transparent;");
    auto* winHBox = new QHBoxLayout(winCtrlWidget);
    winHBox->setContentsMargins(0, 0, 0, 0);
    winHBox->setSpacing(0);

    auto* btnMin   = makeWindowButton(Icons::minimize());
    m_btnMaximize  = makeWindowButton(Icons::maximize());
    auto* btnClose = makeWindowButton(Icons::close(), true);

    // Window controls are GUI-only — connected here
    connect(btnMin,        &QPushButton::clicked, this, &MainWindow::onMinimize);
    connect(m_btnMaximize, &QPushButton::clicked, this, &MainWindow::onMaximizeRestore);
    connect(btnClose,      &QPushButton::clicked, this, &QWidget::close);

    winHBox->addWidget(btnMin);
    winHBox->addWidget(m_btnMaximize);
    winHBox->addWidget(btnClose);

    hbox->addWidget(winCtrlWidget);

    return bar;
}

QPushButton* MainWindow::makeNavButton(const QString& svgIcon, const QString& text)
{
    auto* btn = new QPushButton;
    btn->setIcon(svgToIcon(svgIcon, 18));
    btn->setIconSize(QSize(18, 18));
    btn->setText(text);
    btn->setFixedHeight(36);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet(QString(
        "QPushButton { background:transparent; color:%1; border:none; "
        "  font-size:13px; font-weight:500; padding:0 16px; border-radius:18px; }"
        "QPushButton:hover { background:%2; border:none; border-radius:18px; }"
        "QPushButton:pressed { background:%3; border:none; border-radius:18px; }"
    ).arg(C_TEXT, C_HOVER, C_ACTIVE));
    return btn;
}

QPushButton* MainWindow::makeWindowButton(const QString& svgIcon, bool isDanger)
{
    auto* btn = new QPushButton;
    btn->setIcon(svgToIcon(svgIcon, 16));
    btn->setIconSize(QSize(16, 16));
    btn->setFixedSize(54, NAVBAR_H);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFlat(true);
    if (isDanger) {
        btn->setStyleSheet(QString(
            "QPushButton { background:transparent; border:none; }"
            "QPushButton:hover { background:#C42B1C; }"
            "QPushButton:pressed { background:#8B1E13; }"
        ));
    } else {
        btn->setStyleSheet(QString(
            "QPushButton { background:transparent; border:none; }"
            "QPushButton:hover { background:%1; }"
            "QPushButton:pressed { background:%2; }"
        ).arg(C_HOVER, C_ACTIVE));
    }
    return btn;
}

// ─── Sidebar ──────────────────────────────────────────────────────────────────

QScrollArea* MainWindow::buildSidebar()
{
    auto* scroll = new QScrollArea;
    scroll->setFixedWidth(SIDEBAR_W);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet(QString("background:%1; border:none;").arg(C_PANEL2));

    auto* container = new QWidget;
    container->setStyleSheet(QString("background:%1;").arg(C_PANEL2));
    auto* vbox = new QVBoxLayout(container);
    vbox->setContentsMargins(6, 6, 6, 6);
    vbox->setSpacing(0);

    // ── Section header helper ──
    auto addSectionHeader = [&](const QString& title) {
        auto* lbl = new QLabel(title.toUpper());
        lbl->setFixedHeight(22);
        lbl->setStyleSheet(QString(
            "QLabel { color:%1; font-size:11px; font-weight:700; letter-spacing:1px;"
            " background:transparent; padding:0 8px; margin:0; }"
        ).arg(C_TEXT2));
        vbox->addWidget(lbl);
    };

    // ── Direct-apply filter buttons ──
    vbox->addSpacing(5);
    addSectionHeader("Quick Filters");

    auto addDirect = [&](const QString& icon, const QString& label) -> QPushButton* {
        auto* btn = new QPushButton;
        btn->setIcon(svgToIcon(icon, 17));
        btn->setIconSize(QSize(17, 17));
        btn->setText("   " + label);
        btn->setFixedHeight(34);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(QString(
            "QPushButton { background:transparent; color:%1; border:none; text-align:left;"
            "  font-size:14px; font-weight:500; padding-top:1px; padding-bottom:1px; padding-left:14px; padding-right:12px;"
            "  border-radius:17px; margin:0px 6px; }"
            "QPushButton:hover { background:%2; border:none; border-radius:17px; }"
            "QPushButton:pressed { background:%3; border:none; border-radius:17px; }"
        ).arg(C_TEXT, C_HOVER, C_ACTIVE));
        vbox->addWidget(btn);
        // No connect — application connections in main.cpp
        return btn;
    };

    m_btnGrayscale     = addDirect(Icons::grayscale(),      "Grayscale");
    m_btnBlackAndWhite = addDirect(Icons::blackAndWhite(),  "Black & White");
    m_btnInvert        = addDirect(Icons::invertImage(),    "Invert");
    m_btnSunlight      = addDirect(Icons::sunlightFilter(), "Sunlight");
    m_btnTv            = addDirect(Icons::tvFilter(),       "TV Filter");
    m_btnPurple        = addDirect(Icons::purpleFilter(),   "Purple");
    m_btnInfrared      = addDirect(Icons::infraredFilter(), "Infrared");
    m_btnDetectEdges   = addDirect(Icons::detectEdges(),    "Detect Edges");
    m_btnOilPainting   = addDirect(Icons::oilPainting(),    "Oil Painting");
    m_btnBlur          = addDirect(Icons::blurImage(),      "Blur");

    // ── Parametric filters (click = expand panel) ──
    vbox->addSpacing(10);
    addSectionHeader("Adjustments");

    auto addParametric = [&](FilterPanel* panel) {
        vbox->addWidget(panel);
        connect(panel, &FilterPanel::headerClicked, this, [this](FilterPanel* clicked) {
            if (m_expandedPanel && m_expandedPanel != clicked) {
                m_expandedPanel->collapse();
            }
            if (clicked->isExpanded()) {
                clicked->collapse();
                m_expandedPanel = nullptr;
            } else {
                clicked->expand();
                m_expandedPanel = clicked;
            }
        });
    };

    addParametric(new FilterPanel(Icons::flipImage(),    "Flip Image",   buildFlipPanel(),       container));
    addParametric(new FilterPanel(Icons::rotateImage(),  "Rotate Image", buildRotatePanel(),     container));
    addParametric(new FilterPanel(Icons::brightness(),   "Brightness",   buildBrightnessPanel(), container));
    addParametric(new FilterPanel(Icons::resizeImage(),  "Resize Image", buildResizePanel(),     container));
    addParametric(new FilterPanel(Icons::mergeImages(),  "Merge Images", buildMergePanel(),      container));
    addParametric(new FilterPanel(Icons::cropImage(),    "Crop Image",   buildCropPanel(),       container));
    addParametric(new FilterPanel(Icons::addFrame(),     "Add Frame",    buildAddFramePanel(),   container));
    addParametric(new FilterPanel(Icons::skewImage(),    "Skew Image",   buildSkewPanel(),       container));

    vbox->addStretch();
    scroll->setWidget(container);
    return scroll;
}

// ─── Preview Area ─────────────────────────────────────────────────────────────

QWidget* MainWindow::buildPreviewArea()
{
    auto* outer = new QWidget;
    outer->setStyleSheet(QString("background:%1;").arg(C_BG));
    outer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* vbox = new QVBoxLayout(outer);
    vbox->setContentsMargins(57, 57, 57, 57);

    // White card container
    auto* card = new QWidget;
    card->setStyleSheet("background: white; border-radius: 2px;");
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* cardVBox = new QVBoxLayout(card);
    cardVBox->setContentsMargins(0, 0, 0, 0);

    m_previewLabel = new QLabel;
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setStyleSheet("background:white;");
    m_previewLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_previewLabel->setMinimumSize(200, 200);

    // Placeholder text when no image is loaded
    m_previewLabel->setText(
        "<div style='color:#010101; font-size:14px; text-align:center;'>"
        "<br><br>No image loaded<br>"
        "<span style='font-size:11px;'>Click <b>New</b> to open an image</span>"
        "</div>"
    );

    cardVBox->addWidget(m_previewLabel);
    vbox->addWidget(card, 1);

    return outer;
}

// ─── Filter parameter panels ──────────────────────────────────────────────────

QWidget* MainWindow::buildFlipPanel()
{
    auto* w = new QWidget;
    auto* vbox = new QVBoxLayout(w);
    vbox->setContentsMargins(12, 8, 12, 10);
    vbox->setSpacing(6);

    auto* lbl = makeLabel("Direction:", w, true);
    vbox->addWidget(lbl);

    auto* hbox = new QHBoxLayout;
    hbox->setSpacing(6);

    auto* btnH = makeChoiceButton("Horizontal", w);
    auto* btnV = makeChoiceButton("Vertical",   w);
    btnH->setChecked(true);

    m_flipDirectionGroup = new QButtonGroup(w);
    m_flipDirectionGroup->setExclusive(true);
    m_flipDirectionGroup->addButton(btnH, 1); // 1 = horizontal
    m_flipDirectionGroup->addButton(btnV, 2); // 2 = vertical

    hbox->addWidget(btnH);
    hbox->addWidget(btnV);
    vbox->addLayout(hbox);

    m_btnApplyFlip = makeApplyButton("Apply Flip", w);
    vbox->addWidget(m_btnApplyFlip);

    return w;
}

QWidget* MainWindow::buildRotatePanel()
{
    auto* w = new QWidget;
    auto* vbox = new QVBoxLayout(w);
    vbox->setContentsMargins(12, 8, 12, 10);
    vbox->setSpacing(6);

    auto* lbl = makeLabel("Rotation:", w, true);
    vbox->addWidget(lbl);

    auto* hbox = new QHBoxLayout;
    hbox->setSpacing(6);

    m_btnRotate90R = makeChoiceButton("90°",  w);
    m_btnRotate90L = makeChoiceButton("270°", w);
    m_btnRotate180 = makeChoiceButton("180°", w);
    // No connect — application connections in main.cpp

    hbox->addWidget(m_btnRotate90L);
    hbox->addWidget(m_btnRotate90R);
    hbox->addWidget(m_btnRotate180);
    vbox->addLayout(hbox);

    return w;
}

QWidget* MainWindow::buildBrightnessPanel()
{
    auto* w = new QWidget;
    auto* vbox = new QVBoxLayout(w);
    vbox->setContentsMargins(12, 8, 12, 10);
    vbox->setSpacing(6);

    auto* topRow = new QHBoxLayout;
    auto* lbl = makeLabel("Brightness:", w, true);
    m_brightnessLabel = makeLabel("0", w);
    m_brightnessLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    topRow->addWidget(lbl);
    topRow->addStretch();
    topRow->addWidget(m_brightnessLabel);
    vbox->addLayout(topRow);

    m_brightnessSlider = new QSlider(Qt::Horizontal, w);
    m_brightnessSlider->setRange(-100, 100);
    m_brightnessSlider->setValue(0);
    m_brightnessSlider->setStyleSheet(QString(
        "QSlider::groove:horizontal { background:%1; height:4px; border-radius:2px; }"
        "QSlider::handle:horizontal { background:%2; width:14px; height:14px;"
        "  border-radius:7px; margin:-5px 0; }"
        "QSlider::sub-page:horizontal { background:%3; border-radius:2px; }"
    ).arg(C_HOVER, C_BLUE, C_BLUE));

    // GUI-only: slider value → label text
    connect(m_brightnessSlider, &QSlider::valueChanged, this, [this](int v) {
        m_brightnessLabel->setText(QString(v >= 0 ? "+%1" : "%1").arg(v));
    });

    // Tick labels
    auto* tickRow = new QHBoxLayout;
    tickRow->addWidget(makeLabel("-100", w, true));
    tickRow->addStretch();
    tickRow->addWidget(makeLabel("0", w, true));
    tickRow->addStretch();
    tickRow->addWidget(makeLabel("+100", w, true));

    vbox->addWidget(m_brightnessSlider);
    vbox->addLayout(tickRow);

    m_btnApplyBrightness = makeApplyButton("Apply", w);
    vbox->addWidget(m_btnApplyBrightness);

    return w;
}

QWidget* MainWindow::buildResizePanel()
{
    auto* w = new QWidget;
    auto* vbox = new QVBoxLayout(w);
    vbox->setContentsMargins(12, 8, 12, 10);
    vbox->setSpacing(6);

    auto* wRow = new QHBoxLayout;
    wRow->addWidget(makeLabel("Width:", w, true));
    m_resizeW = makeSpinBox(1, 9999, 800, w);
    wRow->addWidget(m_resizeW);
    vbox->addLayout(wRow);

    auto* hRow = new QHBoxLayout;
    hRow->addWidget(makeLabel("Height:", w, true));
    m_resizeH = makeSpinBox(1, 9999, 600, w);
    hRow->addWidget(m_resizeH);
    vbox->addLayout(hRow);

    m_btnApplyResize = makeApplyButton("Apply Resize", w);
    vbox->addWidget(m_btnApplyResize);

    return w;
}

QWidget* MainWindow::buildMergePanel()
{
    auto* w = new QWidget;
    auto* vbox = new QVBoxLayout(w);
    vbox->setContentsMargins(12, 8, 12, 10);
    vbox->setSpacing(6);

    vbox->addWidget(makeLabel("Second image:", w, true));

    auto* pathRow = new QHBoxLayout;
    m_mergePathEdit = new QLineEdit(w);
    m_mergePathEdit->setPlaceholderText("File path…");
    m_mergePathEdit->setFixedHeight(28);
    m_mergePathEdit->setStyleSheet(QString(
        "QLineEdit { background:%1; color:%2; border:none;"
        "  border-radius:4px; padding:2px 8px; font-size:11px; }"
    ).arg(C_BG, C_TEXT));

    m_mergeBrowseBtn = new QPushButton("…", w);
    m_mergeBrowseBtn->setFixedSize(28, 28);
    m_mergeBrowseBtn->setCursor(Qt::PointingHandCursor);
    m_mergeBrowseBtn->setStyleSheet(QString(
        "QPushButton { background:%1; color:%2; border:none; border-radius:4px; }"
        "QPushButton:hover { background:%3; }"
    ).arg(C_HOVER, C_TEXT, C_ACTIVE));

    // GUI-only: browse button opens file picker and fills the path field
    connect(m_mergeBrowseBtn, &QPushButton::clicked, this, [this]() {
        QString path = QFileDialog::getOpenFileName(this, "Select Second Image", "",
            "Images (*.jpg *.jpeg *.png *.bmp)");
        if (!path.isEmpty()) m_mergePathEdit->setText(path);
    });

    pathRow->addWidget(m_mergePathEdit, 1);
    pathRow->addWidget(m_mergeBrowseBtn);
    vbox->addLayout(pathRow);

    m_btnApplyMerge = makeApplyButton("Apply Merge", w);
    vbox->addWidget(m_btnApplyMerge);

    return w;
}

QWidget* MainWindow::buildCropPanel()
{
    auto* w = new QWidget;
    auto* vbox = new QVBoxLayout(w);
    vbox->setContentsMargins(12, 8, 12, 10);
    vbox->setSpacing(4);

    auto addRow = [&](const QString& labelText, QSpinBox*& sbRef) {
        auto* row = new QHBoxLayout;
        row->addWidget(makeLabel(labelText, w, true));
        sbRef = makeSpinBox(0, 9999, 0, w);
        row->addWidget(sbRef);
        vbox->addLayout(row);
    };

    addRow("Start X:",   m_cropXSpin);
    addRow("Start Y:",   m_cropYSpin);
    addRow("Width:",     m_cropWSpin);
    addRow("Height:",    m_cropHSpin);
    m_cropWSpin->setValue(100);
    m_cropHSpin->setValue(100);

    m_btnApplyCrop = makeApplyButton("Apply Crop", w);
    vbox->addWidget(m_btnApplyCrop);

    return w;
}

QWidget* MainWindow::buildAddFramePanel()
{
    auto* w = new QWidget;
    auto* vbox = new QVBoxLayout(w);
    vbox->setContentsMargins(12, 8, 12, 10);
    vbox->setSpacing(6);

    auto* sizeRow = new QHBoxLayout;
    sizeRow->addWidget(makeLabel("Frame size:", w, true));
    m_frameSizeSpin = makeSpinBox(1, 200, 20, w);
    sizeRow->addWidget(m_frameSizeSpin);
    vbox->addLayout(sizeRow);

    vbox->addWidget(makeLabel("Frame style:", w, true));
    auto* typeRow = new QHBoxLayout;
    typeRow->setSpacing(6);

    auto* btnSimple = makeChoiceButton("Simple", w);
    auto* btnFancy  = makeChoiceButton("Fancy",  w);
    btnSimple->setChecked(true);

    m_frameTypeGroup = new QButtonGroup(w);
    m_frameTypeGroup->setExclusive(true);
    m_frameTypeGroup->addButton(btnSimple, 1);
    m_frameTypeGroup->addButton(btnFancy,  2);

    typeRow->addWidget(btnSimple);
    typeRow->addWidget(btnFancy);
    vbox->addLayout(typeRow);

    m_btnApplyFrame = makeApplyButton("Apply Frame", w);
    vbox->addWidget(m_btnApplyFrame);

    return w;
}

QWidget* MainWindow::buildSkewPanel()
{
    auto* w = new QWidget;
    auto* vbox = new QVBoxLayout(w);
    vbox->setContentsMargins(12, 8, 12, 10);
    vbox->setSpacing(6);

    auto* topRow = new QHBoxLayout;
    topRow->addWidget(makeLabel("Skew %:", w, true));
    m_skewLabel = makeLabel("20", w);
    m_skewLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    topRow->addStretch();
    topRow->addWidget(m_skewLabel);
    vbox->addLayout(topRow);

    m_skewSlider = new QSlider(Qt::Horizontal, w);
    m_skewSlider->setRange(0, 100);
    m_skewSlider->setValue(20);
    m_skewSlider->setStyleSheet(QString(
        "QSlider::groove:horizontal { background:%1; height:4px; border-radius:2px; }"
        "QSlider::handle:horizontal { background:%2; width:14px; height:14px;"
        "  border-radius:7px; margin:-5px 0; }"
        "QSlider::sub-page:horizontal { background:%3; border-radius:2px; }"
    ).arg(C_HOVER, C_BLUE, C_BLUE));

    // GUI-only: slider value → label text
    connect(m_skewSlider, &QSlider::valueChanged, this, [this](int v) {
        m_skewLabel->setText(QString("%1%").arg(v));
    });

    vbox->addWidget(m_skewSlider);

    m_btnApplySkew = makeApplyButton("Apply Skew", w);
    vbox->addWidget(m_btnApplySkew);

    return w;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Parameter value getters
// ═══════════════════════════════════════════════════════════════════════════════

int     MainWindow::flipDirection()   const { return m_flipDirectionGroup->checkedId(); }
int     MainWindow::brightnessValue() const { return m_brightnessSlider->value(); }
int     MainWindow::resizeWidth()     const { return m_resizeW->value(); }
int     MainWindow::resizeHeight()    const { return m_resizeH->value(); }
QString MainWindow::mergePath()       const { return m_mergePathEdit->text().trimmed(); }
int     MainWindow::cropX()           const { return m_cropXSpin->value(); }
int     MainWindow::cropY()           const { return m_cropYSpin->value(); }
int     MainWindow::cropWidth()       const { return m_cropWSpin->value(); }
int     MainWindow::cropHeight()      const { return m_cropHSpin->value(); }
int     MainWindow::frameSize()       const { return m_frameSizeSpin->value(); }
int     MainWindow::frameType()       const { return m_frameTypeGroup->checkedId(); }
int     MainWindow::skewValue()       const { return m_skewSlider->value(); }

// ═══════════════════════════════════════════════════════════════════════════════
// Public GUI methods
// ═══════════════════════════════════════════════════════════════════════════════

void MainWindow::setCloseHandler(std::function<bool()> handler)
{
    m_closeHandler = std::move(handler);
}

void MainWindow::displayPreview(const QString& filePath)
{
    QPixmap pix(filePath);
    if (pix.isNull()) return;

    QSize available = m_previewLabel->size();
    if (available.isEmpty()) available = QSize(750, 550);

    m_previewLabel->setPixmap(
        pix.scaled(available, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::clearPreview()
{
    m_previewLabel->clear();
    m_previewLabel->setText(
        "<div style='color:#010101; font-size:14px; text-align:center;'>"
        "<br><br>No image loaded<br>"
        "<span style='font-size:11px;'>Click <b>New</b> to open an image</span>"
        "</div>"
    );
}

void MainWindow::resetSliders()
{
    // Reset brightness slider to 0
    if (m_brightnessSlider) {
        m_brightnessSlider->setValue(0);
    }
    if (m_brightnessLabel) {
        m_brightnessLabel->setText("0");
    }

    // Reset skew slider to default 20
    if (m_skewSlider) {
        m_skewSlider->setValue(20);
    }
    if (m_skewLabel) {
        m_skewLabel->setText("20%");
    }
}

QString MainWindow::showSaveAsDialog(const QString& currentName)
{
    QDialog dlg(this);
    dlg.setWindowTitle("Save As");
    dlg.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    dlg.setFixedWidth(360);
    dlg.setStyleSheet(QString(
        "QDialog { background:%1; border:1px solid %2; border-radius:6px; }"
        "QLabel  { color:%3; }"
    ).arg(C_PANEL, C_DIVIDER, C_TEXT));

    auto* vbox = new QVBoxLayout(&dlg);
    vbox->setContentsMargins(24, 20, 24, 20);
    vbox->setSpacing(14);

    // Title row
    auto* titleRow = new QHBoxLayout;
    auto* titleLbl = new QLabel("Save As");
    titleLbl->setStyleSheet("font-size:15px; font-weight:700;");
    titleRow->addWidget(titleLbl);
    titleRow->addStretch();
    auto* closeBtn = new QPushButton("✕");
    closeBtn->setFixedSize(24, 24);
    closeBtn->setFlat(true);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(QString(
        "QPushButton { color:%1; background:transparent; border:none; font-size:12px; }"
        "QPushButton:hover { color:white; }"
    ).arg(C_TEXT2));
    connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::reject);
    titleRow->addWidget(closeBtn);
    vbox->addLayout(titleRow);

    // Filename input
    vbox->addWidget(makeLabel("Filename:", &dlg, true));
    QFileInfo fi(currentName);
    auto* fileRow = new QHBoxLayout;
    fileRow->setSpacing(4);

    auto* prefixLbl = new QLabel("images/", &dlg);
    prefixLbl->setStyleSheet(QString("QLabel { color:%1; font-size:12px; font-weight:600; }").arg(C_TEXT2));
    fileRow->addWidget(prefixLbl);

    auto* nameEdit = new QLineEdit(fi.baseName(), &dlg);
    nameEdit->setFixedHeight(30);
    nameEdit->setStyleSheet(QString(
        "QLineEdit { background:%1; color:%2; border:1px solid %3;"
        "  border-radius:3px; padding:2px 8px; font-size:12px; }"
    ).arg(C_BG, C_TEXT, C_DIVIDER));
    fileRow->addWidget(nameEdit, 1);
    vbox->addLayout(fileRow);

    // Format selection
    vbox->addWidget(makeLabel("Format:", &dlg, true));
    auto* fmtGroup = new QButtonGroup(&dlg);
    fmtGroup->setExclusive(true);

    auto* fmtRow = new QHBoxLayout;
    fmtRow->setSpacing(6);
    QStringList fmts = {"JPG", "JPEG", "PNG", "BMP"};
    int fmtIdx = 0;
    for (const auto& fmt : fmts) {
        auto* rb = makeChoiceButton(fmt, &dlg);
        if (fmtIdx == 0) rb->setChecked(true);
        fmtGroup->addButton(rb, fmtIdx++);
        fmtRow->addWidget(rb);
    }
    vbox->addLayout(fmtRow);

    // Save button
    auto* saveBtn = makeApplyButton("Save", &dlg);
    vbox->addWidget(saveBtn);

    QString result;
    connect(saveBtn, &QPushButton::clicked, [&]() {
        QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) {
            QMessageBox::warning(&dlg, "Save As", "Please enter a filename.");
            return;
        }
        int fmtId = fmtGroup->checkedId();
        QString ext = fmts[fmtId].toLower();
        result = "images/" + name + "." + ext;
        dlg.accept();
    });

    dlg.exec();
    return result;  // empty string if cancelled
}

int MainWindow::showUnsavedChangesDialog()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Unsaved Changes");
    dlg.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    dlg.setFixedWidth(380);
    dlg.setStyleSheet(QString(
        "QDialog { background:%1; border:1px solid %2; border-radius:6px; }"
        "QLabel  { color:%3; }"
    ).arg(C_PANEL, C_DIVIDER, C_TEXT));

    auto* vbox = new QVBoxLayout(&dlg);
    vbox->setContentsMargins(24, 20, 24, 20);
    vbox->setSpacing(16);

    // Title row
    auto* titleRow = new QHBoxLayout;
    auto* titleLbl = new QLabel("Unsaved Changes");
    titleLbl->setStyleSheet("font-size:15px; font-weight:700;");
    titleRow->addWidget(titleLbl);
    titleRow->addStretch();

    auto* closeBtn = new QPushButton("✕");
    closeBtn->setFixedSize(24, 24);
    closeBtn->setFlat(true);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(QString(
        "QPushButton { color:%1; background:transparent; border:none; font-size:12px; }"
        "QPushButton:hover { color:white; }"
    ).arg(C_TEXT2));
    connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::reject);
    titleRow->addWidget(closeBtn);
    vbox->addLayout(titleRow);

    auto* msgLbl = new QLabel("You have unsaved changes. What would you like to do?");
    msgLbl->setWordWrap(true);
    msgLbl->setStyleSheet(QString("color:%1; font-size:12px;").arg(C_TEXT2));
    vbox->addWidget(msgLbl);

    // Buttons
    auto* btnRow = new QHBoxLayout;
    btnRow->setSpacing(8);

    auto* btnDiscard = new QPushButton("Discard Changes");
    auto* btnSaveCh  = new QPushButton("Save Changes");
    auto* btnSaveAs2 = new QPushButton("Save As…");

    auto styleSecondary = [&](QPushButton* b) {
        b->setFixedHeight(32);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet(QString(
            "QPushButton { background:%1; color:%2; border:1px solid %3; border-radius:3px; font-size:12px; }"
            "QPushButton:hover { background:%4; }"
        ).arg(C_HOVER, C_TEXT, C_DIVIDER, C_ACTIVE));
    };
    styleSecondary(btnDiscard);
    styleSecondary(btnSaveCh);

    btnSaveAs2->setFixedHeight(32);
    btnSaveAs2->setCursor(Qt::PointingHandCursor);
    btnSaveAs2->setStyleSheet(QString(
        "QPushButton { background:%1; color:white; border:none; border-radius:3px; font-size:12px; font-weight:600; }"
        "QPushButton:hover { background:#1a82ff; }"
    ).arg(C_BLUE));

    btnRow->addWidget(btnDiscard);
    btnRow->addWidget(btnSaveCh);
    btnRow->addWidget(btnSaveAs2);
    vbox->addLayout(btnRow);

    int result = 0; // 0=cancel, 1=discard, 2=save, 3=saveas
    connect(btnDiscard, &QPushButton::clicked, [&]() { result = 1; dlg.accept(); });
    connect(btnSaveCh,  &QPushButton::clicked, [&]() { result = 2; dlg.accept(); });
    connect(btnSaveAs2, &QPushButton::clicked, [&]() { result = 3; dlg.accept(); });

    dlg.exec();
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Window controls (GUI-only)
// ═══════════════════════════════════════════════════════════════════════════════

void MainWindow::onMinimize() { showMinimized(); }

void MainWindow::onMaximizeRestore()
{
    if (isMaximized()) {
        showNormal();
        m_btnMaximize->setIcon(svgToIcon(Icons::maximize(), 16));
    } else {
        showMaximized();
        m_btnMaximize->setIcon(svgToIcon(Icons::restore(), 16));
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Window events
// ═══════════════════════════════════════════════════════════════════════════════

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (m_closeHandler && !m_closeHandler()) {
        event->ignore();
        return;
    }
    event->accept();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    if (m_previewLabel && !m_previewLabel->pixmap().isNull()) {
        displayPreview("images/temp.jpg");
    }
}

// ─── Frameless window drag support ────────────────────────────────────────────

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_navbar && event->position().y() <= NAVBAR_H) {
            m_dragging   = true;
            m_dragOffset = event->globalPosition().toPoint() - frameGeometry().topLeft();
        }
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        if (isMaximized()) {
            showNormal();
            m_btnMaximize->setIcon(svgToIcon(Icons::maximize(), 14));
        }
        move(event->globalPosition().toPoint() - m_dragOffset);
    }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    m_dragging = false;
    QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (m_navbar && event->position().y() <= NAVBAR_H) {
        onMaximizeRestore();
    }
    QMainWindow::mouseDoubleClickEvent(event);
}
