// mainwindow.h
// Photoshop-inspired main window for the Image Editor Qt GUI.
// Owns all GUI components. Application logic lives in main.cpp.

#pragma once

#include <QMainWindow>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QLabel>
#include <QScrollArea>
#include <QWidget>
#include <QPropertyAnimation>
#include <QPoint>
#include <QSlider>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QButtonGroup>
#include <functional>

// ─── Forward declarations ──────────────────────────────────────────────────────
class FilterPanel;

// ─── MainWindow ────────────────────────────────────────────────────────────────
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

    // ── Close handler — set by main.cpp to check unsaved changes ──
    void setCloseHandler(std::function<bool()> handler);

    // ── Navbar button access ──
    QPushButton* btnNew()    const { return m_btnNew; }
    QPushButton* btnSave()   const { return m_btnSave; }
    QPushButton* btnSaveAs() const { return m_btnSaveAs; }
    QPushButton* btnUndo()   const { return m_btnUndo; }
    QPushButton* btnRemove() const { return m_btnRemove; }

    // ── Direct filter button access ──
    QPushButton* btnGrayscale()     const { return m_btnGrayscale; }
    QPushButton* btnBlackAndWhite() const { return m_btnBlackAndWhite; }
    QPushButton* btnInvert()        const { return m_btnInvert; }
    QPushButton* btnSunlight()      const { return m_btnSunlight; }
    QPushButton* btnTv()            const { return m_btnTv; }
    QPushButton* btnPurple()        const { return m_btnPurple; }
    QPushButton* btnInfrared()      const { return m_btnInfrared; }
    QPushButton* btnDetectEdges()   const { return m_btnDetectEdges; }
    QPushButton* btnOilPainting()   const { return m_btnOilPainting; }
    QPushButton* btnBlur()          const { return m_btnBlur; }

    // ── Parametric filter button access ──
    QPushButton* btnApplyFlip()       const { return m_btnApplyFlip; }
    QPushButton* btnRotate90L()       const { return m_btnRotate90L; }
    QPushButton* btnRotate90R()       const { return m_btnRotate90R; }
    QPushButton* btnRotate180()       const { return m_btnRotate180; }
    QPushButton* btnApplyBrightness() const { return m_btnApplyBrightness; }
    QPushButton* btnApplyResize()     const { return m_btnApplyResize; }
    QPushButton* btnApplyMerge()      const { return m_btnApplyMerge; }
    QPushButton* btnApplyCrop()       const { return m_btnApplyCrop; }
    QPushButton* btnApplyFrame()      const { return m_btnApplyFrame; }
    QPushButton* btnApplySkew()       const { return m_btnApplySkew; }

    // ── Parameter value getters ──
    int     flipDirection()   const;
    int     brightnessValue() const;
    int     resizeWidth()     const;
    int     resizeHeight()    const;
    QString mergePath()       const;
    int     cropX()           const;
    int     cropY()           const;
    int     cropWidth()       const;
    int     cropHeight()      const;
    int     frameSize()       const;
    int     frameType()       const;
    int     skewValue()       const;

    // ── Public GUI methods ──
    void    displayPreview(const QString& filePath);
    void    clearPreview();
    void    resetSliders();
    QString showSaveAsDialog(const QString& currentName = "");
    int     showUnsavedChangesDialog();  // 0=cancel, 1=discard, 2=save, 3=saveas

protected:
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    // ── GUI-only window control slots ──
    void onMinimize();
    void onMaximizeRestore();

    // ── UI builders ──
    QWidget*     buildNavbar();
    QScrollArea* buildSidebar();
    QWidget*     buildPreviewArea();

    QWidget* buildFlipPanel();
    QWidget* buildRotatePanel();
    QWidget* buildBrightnessPanel();
    QWidget* buildResizePanel();
    QWidget* buildMergePanel();
    QWidget* buildCropPanel();
    QWidget* buildAddFramePanel();
    QWidget* buildSkewPanel();

    QPushButton* makeNavButton(const QString& svgIcon, const QString& text);
    QPushButton* makeWindowButton(const QString& svgIcon, bool isDanger = false);

    // ── Close handler ──
    std::function<bool()> m_closeHandler;

    // ── Layout widgets ──
    QWidget*     m_navbar       = nullptr;
    QScrollArea* m_sidebar      = nullptr;
    QWidget*     m_previewArea  = nullptr;
    QLabel*      m_previewLabel = nullptr;

    // ── Currently expanded filter panel ──
    FilterPanel* m_expandedPanel = nullptr;

    // ── Navbar buttons ──
    QPushButton* m_btnNew       = nullptr;
    QPushButton* m_btnSave      = nullptr;
    QPushButton* m_btnSaveAs    = nullptr;
    QPushButton* m_btnUndo      = nullptr;
    QPushButton* m_btnRemove    = nullptr;
    QPushButton* m_btnMaximize  = nullptr;

    // ── Direct filter buttons ──
    QPushButton* m_btnGrayscale     = nullptr;
    QPushButton* m_btnBlackAndWhite = nullptr;
    QPushButton* m_btnInvert        = nullptr;
    QPushButton* m_btnSunlight      = nullptr;
    QPushButton* m_btnTv            = nullptr;
    QPushButton* m_btnPurple        = nullptr;
    QPushButton* m_btnInfrared      = nullptr;
    QPushButton* m_btnDetectEdges   = nullptr;
    QPushButton* m_btnOilPainting   = nullptr;
    QPushButton* m_btnBlur          = nullptr;

    // ── Parametric filter buttons ──
    QPushButton* m_btnApplyFlip       = nullptr;
    QPushButton* m_btnRotate90L       = nullptr;
    QPushButton* m_btnRotate90R       = nullptr;
    QPushButton* m_btnRotate180       = nullptr;
    QPushButton* m_btnApplyBrightness = nullptr;
    QPushButton* m_btnApplyResize     = nullptr;
    QPushButton* m_btnApplyMerge      = nullptr;
    QPushButton* m_btnApplyCrop       = nullptr;
    QPushButton* m_btnApplyFrame      = nullptr;
    QPushButton* m_btnApplySkew       = nullptr;

    // ── Drag support for frameless window ──
    bool   m_dragging   = false;
    QPoint m_dragOffset;

    // ── Parametric filter widget refs (private — exposed via getters) ──
    QButtonGroup* m_flipDirectionGroup = nullptr;
    QSlider*      m_brightnessSlider   = nullptr;
    QLabel*       m_brightnessLabel    = nullptr;
    QSpinBox*     m_resizeW            = nullptr;
    QSpinBox*     m_resizeH            = nullptr;
    QLineEdit*    m_mergePathEdit      = nullptr;
    QPushButton*  m_mergeBrowseBtn     = nullptr;
    QSpinBox*     m_cropXSpin          = nullptr;
    QSpinBox*     m_cropYSpin          = nullptr;
    QSpinBox*     m_cropWSpin          = nullptr;
    QSpinBox*     m_cropHSpin          = nullptr;
    QSpinBox*     m_frameSizeSpin      = nullptr;
    QButtonGroup* m_frameTypeGroup     = nullptr;
    QSlider*      m_skewSlider         = nullptr;
    QLabel*       m_skewLabel          = nullptr;
};

// ─── FilterPanel ───────────────────────────────────────────────────────────────
// A collapsible sidebar entry: header button + animated content panel.
class FilterPanel : public QWidget
{
    Q_OBJECT

public:
    FilterPanel(const QString& svgIcon,
                const QString& label,
                QWidget*       content,
                QWidget*       parent = nullptr);

    void expand();
    void collapse();
    bool isExpanded() const { return m_expanded; }

    bool isDirect() const { return m_isDirect; }
    void setDirect(bool d) { m_isDirect = d; }

signals:
    void headerClicked(FilterPanel* self);

private:
    void toggleExpand();

    QPushButton*        m_header    = nullptr;
    QWidget*            m_content   = nullptr;
    QPropertyAnimation* m_animation = nullptr;
    bool                m_expanded  = false;
    bool                m_isDirect  = false;
};
