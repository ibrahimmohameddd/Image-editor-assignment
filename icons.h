// icons.h
// Inline SVG icon data for all filter buttons.
// All icons are thin, monochrome, outlined SVG paths in a 24x24 viewBox.

#pragma once
#include <QString>

// Each icon is a complete SVG string ready for QSvgRenderer or similar.
// Color is set to currentColor so it respects stylesheet.

namespace Icons {

// Helper: wrap path data in a full SVG
inline QString makeSvg(const QString& pathData, const QString& extraElements = "")
{
    return QString(
        R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="%1" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round">)"
        "%2%3"
        "</svg>"
    ).arg("#B8B8B8", pathData, extraElements);
}

// Grayscale — palette icon
inline QString grayscale() {
    return makeSvg(
        R"(<circle cx="12" cy="12" r="9"/>)"
        R"(<path d="M12 3v18"/>)"
        R"(<path d="M3 12h9"/>)"
    );
}

// Black & White — half-filled circle
inline QString blackAndWhite() {
    return makeSvg(
        R"(<circle cx="12" cy="12" r="9"/>)"
        R"(<path d="M12 3a9 9 0 0 1 0 18z" fill="#B8B8B8" stroke="none"/>)"
    );
}

// Invert — swap arrows
inline QString invertImage() {
    return makeSvg(
        R"(<path d="M7 16V4m0 0L4 7m3-3l3 3"/>)"
        R"(<path d="M17 8v12m0 0l3-3m-3 3l-3-3"/>)"
    );
}

// Add Frame — square with inner border
inline QString addFrame() {
    return makeSvg(
        R"(<rect x="3" y="3" width="18" height="18" rx="1"/>)"
        R"(<rect x="6" y="6" width="12" height="12" rx="0.5"/>)"
    );
}

// Flip — two opposing arrows
inline QString flipImage() {
    return makeSvg(
        R"(<path d="M12 3v18"/>)"
        R"(<path d="M6 8l-4 4 4 4"/>)"
        R"(<path d="M18 8l4 4-4 4"/>)"
    );
}

// Rotate — circular arrow
inline QString rotateImage() {
    return makeSvg(
        R"(<path d="M21 12a9 9 0 1 1-9-9c2.5 0 4.8.9 6.5 2.5L21 3"/>)"
        R"(<path d="M21 3v5h-5"/>)"
    );
}

// Brightness — sun with rays
inline QString brightness() {
    return makeSvg(
        R"(<circle cx="12" cy="12" r="4"/>)"
        R"(<line x1="12" y1="2" x2="12" y2="5"/>)"
        R"(<line x1="12" y1="19" x2="12" y2="22"/>)"
        R"(<line x1="4.22" y1="4.22" x2="6.34" y2="6.34"/>)"
        R"(<line x1="17.66" y1="17.66" x2="19.78" y2="19.78"/>)"
        R"(<line x1="2" y1="12" x2="5" y2="12"/>)"
        R"(<line x1="19" y1="12" x2="22" y2="12"/>)"
        R"(<line x1="4.22" y1="19.78" x2="6.34" y2="17.66"/>)"
        R"(<line x1="17.66" y1="6.34" x2="19.78" y2="4.22"/>)"
    );
}

// Resize — expand arrows
inline QString resizeImage() {
    return makeSvg(
        R"(<polyline points="15 3 21 3 21 9"/>)"
        R"(<polyline points="9 21 3 21 3 15"/>)"
        R"(<line x1="21" y1="3" x2="14" y2="10"/>)"
        R"(<line x1="3" y1="21" x2="10" y2="14"/>)"
    );
}

// Merge — layers
inline QString mergeImages() {
    return makeSvg(
        R"(<polygon points="12 2 2 7 12 12 22 7 12 2"/>)"
        R"(<polyline points="2 17 12 22 22 17"/>)"
        R"(<polyline points="2 12 12 17 22 12"/>)"
    );
}

// Detect Edges — vector square
inline QString detectEdges() {
    return makeSvg(
        R"(<path d="M5 3H3v2"/>)"
        R"(<path d="M19 3h2v2"/>)"
        R"(<path d="M5 21H3v-2"/>)"
        R"(<path d="M19 21h2v-2"/>)"
        R"(<rect x="7" y="7" width="10" height="10"/>)"
    );
}

// Crop — crop tool
inline QString cropImage() {
    return makeSvg(
        R"(<polyline points="6 2 6 16 20 16"/>)"
        R"(<polyline points="2 6 16 6 16 20"/>)"
    );
}

// Blur — radial blur lines
inline QString blurImage() {
    return makeSvg(
        R"(<circle cx="12" cy="12" r="3"/>)"
        R"(<circle cx="12" cy="12" r="7" stroke-dasharray="2 2"/>)"
        R"(<circle cx="12" cy="12" r="10" stroke-dasharray="1 3"/>)"
    );
}

// Sunlight — sun
inline QString sunlightFilter() {
    return makeSvg(
        R"(<circle cx="12" cy="12" r="5"/>)"
        R"(<line x1="12" y1="1" x2="12" y2="3"/>)"
        R"(<line x1="12" y1="21" x2="12" y2="23"/>)"
        R"(<line x1="4.22" y1="4.22" x2="5.64" y2="5.64"/>)"
        R"(<line x1="18.36" y1="18.36" x2="19.78" y2="19.78"/>)"
        R"(<line x1="1" y1="12" x2="3" y2="12"/>)"
        R"(<line x1="21" y1="12" x2="23" y2="12"/>)"
        R"(<line x1="4.22" y1="19.78" x2="5.64" y2="18.36"/>)"
        R"(<line x1="18.36" y1="5.64" x2="19.78" y2="4.22"/>)"
    );
}

// TV Filter — monitor/tv
inline QString tvFilter() {
    return makeSvg(
        R"(<rect x="2" y="7" width="20" height="14" rx="2" ry="2"/>)"
        R"(<polyline points="17 2 12 7 7 2"/>)"
    );
}

// Purple Filter — color fill
inline QString purpleFilter() {
    return makeSvg(
        R"(<path d="M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z"/>)"
    );
}

// Infrared Filter — thermometer
inline QString infraredFilter() {
    return makeSvg(
        R"(<path d="M14 14.76V3.5a2.5 2.5 0 0 0-5 0v11.26a4.5 4.5 0 1 0 5 0z"/>)"
    );
}

// Skew — slant/shear
inline QString skewImage() {
    return makeSvg(
        R"(<path d="M5 20h14"/>)"
        R"(<path d="M5 4h14"/>)"
        R"(<path d="M8 4l-3 16"/>)"
        R"(<path d="M16 4l3 16"/>)"
    );
}

// Oil Painting — paintbrush
inline QString oilPainting() {
    return makeSvg(
        R"(<path d="M18.37 2.63L14 7l-1.59-1.59a2 2 0 0 0-2.82 0L8 7l9 9 1.59-1.59a2 2 0 0 0 0-2.82L17 10l4.37-4.37a2.12 2.12 0 0 0-3-3z"/>)"
        R"(<path d="M9 8c-2 3-4 3.5-7 4l8 8c1-.5 3.5-2 4-7"/>)"
        R"(<path d="M14.5 17.5L4.5 15"/>)"
    );
}

// ─── Global action icons ───────────────────────────────────────────────────────

// New / Plus — folder with plus
inline QString newFile() {
    return makeSvg(
        R"(<path d="M22 19a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h5l2 3h9a2 2 0 0 1 2 2z"/>)"
        R"(<line x1="12" y1="11" x2="12" y2="17"/>)"
        R"(<line x1="9" y1="14" x2="15" y2="14"/>)"
    );
}

// Save — floppy disk
inline QString save() {
    return makeSvg(
        R"(<path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"/>)"
        R"(<polyline points="17 21 17 13 7 13 7 21"/>)"
        R"(<polyline points="7 3 7 8 15 8"/>)"
    );
}

// Save As — save with arrow
inline QString saveAs() {
    return makeSvg(
        R"(<path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"/>)"
        R"(<polyline points="17 21 17 13 7 13 7 21"/>)"
        R"(<polyline points="7 3 7 8 15 8"/>)"
        R"(<polyline points="14 17 17 20 20 17"/>)"
    );
}

// Undo — counter-clockwise arrow
inline QString undo() {
    return makeSvg(
        R"(<polyline points="1 4 1 10 7 10"/>)"
        R"(<path d="M3.51 15a9 9 0 1 0 .49-4.63"/>)"
    );
}

// Remove / Delete / Trash icon
inline QString removeIcon() {
    return makeSvg(
        R"(<path d="M3 6h18"/>)"
        R"(<path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"/>)"
        R"(<line x1="10" y1="11" x2="10" y2="17"/>)"
        R"(<line x1="14" y1="11" x2="14" y2="17"/>)"
    );
}

// Minimize window — clean horizontal minus bar
inline QString minimize() {
    return makeSvg(R"(<path d="M6 12h12" stroke-width="2"/>)");
}

// Maximize window — clean square outline
inline QString maximize() {
    return makeSvg(R"(<rect x="5" y="5" width="14" height="14" rx="1.5" stroke-width="1.8"/>)");
}

// Restore window — overlapping windows
inline QString restore() {
    return makeSvg(
        R"(<rect x="5" y="9" width="10" height="10" rx="1.5" stroke-width="1.8"/>)"
        R"(<path d="M9 5h8a2 2 0 0 1 2 2v8" stroke-width="1.8"/>)"
    );
}

// Close window — clean modern X
inline QString close() {
    return makeSvg(
        R"(<path d="M7 7l10 10M17 7L7 17" stroke-width="2"/>)"
    );
}

// Image placeholder (shown when no image is loaded)
inline QString imagePlaceholder() {
    return makeSvg(
        R"(<rect x="3" y="3" width="18" height="18" rx="2" ry="2"/>)"
        R"(<circle cx="8.5" cy="8.5" r="1.5"/>)"
        R"(<polyline points="21 15 16 10 5 21"/>)"
    );
}

} // namespace Icons
