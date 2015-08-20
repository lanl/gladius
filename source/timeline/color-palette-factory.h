/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef COLOR_PALETTE_FACTORY_H_INCLUDED
#define COLOR_PALETTE_FACTORY_H_INCLUDED

#include <QList>
#include <QString>
#include <QObject>
#include <QColor>

class ColorPaletteFactory {
private:
    //
    ColorPaletteFactory(void) { }
public:
    // From: https://en.wikipedia.org/wiki/Help:Distinguishable_colors
    static QList<QColor>
    getColorAlphabet(void)
    {
        static const QList<QString> colors = QList<QString>()
            << "#F0A3FF" << "#0075DC" << "#993F00" << "#4C005C" << "#191919"
            << "#005C31" << "#2BCE48" << "#FFCC99" << "#808080" << "#94FFB5"
            << "#8F7C00" << "#9DCC00" << "#C20088" << "#003380" << "#FFA405"
            << "#FFA8BB" << "#426600" << "#FF0010" << "#5EF1F2" << "#00998F"
            << "#E0FF66" << "#740AFF" << "#990000" << "#FFFF80" << "#FFFF00"
            << "#FF5005";
        //
        QList<QColor> result;
        //
        foreach (const QString &color, colors) {
            result << QColor(color);
        }
        //
        return result;
    }

    // From: http://tools.medialab.sciences-po.fr/iwanthue/
    // I forgot the settings used :-(. 64 colors.
    static QList<QColor>
    getColorAlphabet2(void)
    {
        static const QList<QString> colors = QList<QString>()
            << "#A06787" << "#59E240" << "#E8A22F" << "#64E2DB" << "#D14DE0"
            << "#3C6E3C" << "#547AD9" << "#DE412C" << "#492518" << "#D2DD80"
            << "#E44094" << "#D37C68" << "#C6C0DE" << "#315470" << "#978E72"
            << "#DAE43F" << "#903587" << "#8C2B1F" << "#DD435E" << "#50A837"
            << "#5BE8B0" << "#4E4586" << "#D5DBB1" << "#5A93CA" << "#4C9DA6"
            << "#997D22" << "#396056" << "#9266D9" << "#66E47C" << "#682452"
            << "#E07427" << "#28321E" << "#8EA23C" << "#CE77C3" << "#2B253D"
            << "#51B26F" << "#7A4A1E" << "#DDAB9E" << "#A9E063" << "#AEEAA5"
            << "#9FE12D" << "#50511E" << "#7A5B52" << "#9C8CD2" << "#E0A85D"
            << "#983948" << "#52937A" << "#E17C98" << "#9D9B9F" << "#D846BA"
            << "#C6DED7" << "#76C6E3" << "#D5BF38" << "#7FCAA9" << "#83A86E"
            << "#D9C284" << "#E0A9D3" << "#9B844C" << "#B5376F" << "#BB6232"
            << "#47721E" << "#511827" << "#5E445D" << "#72788B";
        //
        QList<QColor> result;
        //
        foreach (const QString &color, colors) {
            result << QColor(color);
        }
        //
        return result;
    }

    // Adapted From: https://wiki.qt.io/Color_palette_generator
    QList<QColor>
    getColors(uint32_t numColorsNeeded) {
        static const double golden_ratio = 0.618033988749895;
        QList<QColor> brushScale;
        double h = 0;
        const uint32_t realNumColorsNeeded = numColorsNeeded + 1;
        for (uint32_t i = 0; i < realNumColorsNeeded; ++i) {
            h = golden_ratio * 360 / realNumColorsNeeded * i;
            h = floor(h * 6);
            brushScale.append(QColor::fromHsv(int(h) % 128, 245, 230, 255));
        }
        return brushScale;
    }


};

#endif // COLOR_PALETTE_FACTORY_H_INCLUDED
