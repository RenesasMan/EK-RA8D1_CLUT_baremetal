/*
 * graphics_rendering.h
 *
 *  Created on: Apr 10, 2025
 *      Author: a5138119
 */

#ifndef GRAPHICS_RENDERING_H_
#define GRAPHICS_RENDERING_H_

#define CLUT8 1U
#define CLUT4 2U
#define CLUT1 3U
//#define CLUT_MODE CLUT8

#if defined(DISPLAY_IN_FORMAT_CLUT8_1)
#define CLUT_MODE CLUT8
#elif defined(DISPLAY_IN_FORMAT_CLUT4_1)
#define CLUT_MODE CLUT4
#else
#define CLUT_MODE CLUT1
#endif

#define  TRANS_LEVEL        0x8F000000                    //transparency level
#define  CLUT_BLACK         0x00000000 | TRANS_LEVEL     // Black
#define  CLUT_WHITE         0x00FFFFFF | TRANS_LEVEL     // White
#define  CLUT_BLUE          0x000000FF | TRANS_LEVEL     // Blue
#define  CLUT_TURQOISE      0x000080FF | TRANS_LEVEL     // Turquoise
#define  CLUT_CYAN          0x0000FFFF | TRANS_LEVEL     // Cyan
#define  CLUT_MINT          0x0000FF80 | TRANS_LEVEL     // Mint Green
#define  CLUT_GREEN         0x0000FF00 | TRANS_LEVEL     // Green
#define  CLUT_LIME          0x0080FF00 | TRANS_LEVEL     // Lime Green
#define  CLUT_YELLOW        0x00FFFF00 | TRANS_LEVEL     // Yellow
#define  CLUT_ORANGE        0x00FF8000 | TRANS_LEVEL     // Orange
#define  CLUT_RED           0x00FF0000 | TRANS_LEVEL     // Red
#define  CLUT_PINK          0x00FF0080 | TRANS_LEVEL     // Pink
#define  CLUT_MAGENTA       0x00FF00FF | TRANS_LEVEL     // Magenta
#define  CLUT_PURPLE        0x008000FF | TRANS_LEVEL     // Purple
#define  CLUT_GRAY          0x00808080 | TRANS_LEVEL     // Gray
#define  CLUT_TRANSPARENT   0x00000000                    // Transparent

#define BYTES_PER_PIXEL_RGB565                       (4)
#define BYTES_PER_PIXEL_CLUT8                        (1)
#define COLOR_BAND_COUNT                             (8)

#if (CLUT_MODE == CLUT1)
#define COLOR_BAND_COUNT_CLUT                        (2)
#else
#define COLOR_BAND_COUNT_CLUT                        (15)
#endif

#define BLUE                                         (0x000000FF)
#define LIME                                         (0xFF00FF00)
#define RED                                          (0x00FF0000)
#define BLACK                                        (0x00000000)
#define WHITE                                        (0xFFFFFFFF)
#define YELLOW                                       (0xFFFFFF00)
#define AQUA                                         (0xFF00FFFF)
#define MAGENTA                                      (0x00FF00FF)


void CLUT_init(void);
void RGB_init(void);

#endif /* GRAPHICS_RENDERING_H_ */
