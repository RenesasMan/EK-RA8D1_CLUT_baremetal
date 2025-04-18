/*
 * graphics_rendering.c
 *
 *  Created on: Apr 10, 2025
 *      Author: a5138119
 */

#include "graphics_rendering.h"
#include "common_utils.h"


/* User defined functions */
static void display_draw_rgb565 (uint32_t * framebuffer);
static void display_clear_rgb565 (uint32_t * framebuffer);
static void display_draw_clut (uint8_t * framebuffer);


void CLUT_init(void);
void RGB_init(void);

/* Variables to store resolution information */
uint16_t g_hz_size, g_vr_size;
/* Variables used for buffer usage */
uint32_t g_buffer_size, g_hstride;
uint32_t * gp_single_buffer = NULL;
uint32_t * gp_double_buffer = NULL;
uint32_t * gp_frame_buffer  = NULL;


/* Variables to store resolution information */
uint16_t g_hz_size1, g_vr_size1;


/* Variables used for buffer usage */
uint32_t g_buffer_size1, g_hstride1;


/* variables used for CLUT definition */
/* Basic 4-bit (16-color) CLUT definition */
uint32_t clut_8[16] =
{
    CLUT_BLACK,                    // Black
    CLUT_WHITE,                    // White
    CLUT_BLUE,                    // Blue
    CLUT_TURQOISE,                    // Turquoise
    CLUT_CYAN,                    // Cyan
    CLUT_MINT,                    // Mint Green
    CLUT_GREEN,                    // Green
    CLUT_LIME,                    // Lime Green
    CLUT_YELLOW,                    // Yellow
    CLUT_ORANGE,                    // Orange
    CLUT_RED,                    // Red
    CLUT_PINK,                    // Pink
    CLUT_MAGENTA,                    // Magenta
    CLUT_PURPLE,                    // Purple
    CLUT_GRAY,                    // Gray
    CLUT_TRANSPARENT                     // Transparent
};

uint32_t clut_4[16] =
{
     CLUT_BLACK,    // Black
     CLUT_WHITE,    // White
     CLUT_BLUE,     // Blue
     CLUT_TURQOISE, // Turquoise
     CLUT_CYAN,     // Cyan
     CLUT_MINT,     // Mint Green
     CLUT_GREEN,    // Green
     CLUT_LIME,     // Lime Green
     CLUT_YELLOW,   // Yellow
     CLUT_ORANGE,   // Orange
     CLUT_RED,      // Red
     CLUT_PINK,     // Pink
     CLUT_MAGENTA,  // Magenta
     CLUT_PURPLE,   // Purple
     CLUT_GRAY,     // Gray
     CLUT_TRANSPARENT // Transparent
};

uint32_t clut_1[2] =
{
 CLUT_RED,
     CLUT_PURPLE,
};


/* Define the CLUT configuration */
display_clut_cfg_t clut_cfg =
{
    .start  = 0,
#if(CLUT_MODE == CLUT8 || CLUT_MODE == CLUT4)
    .size   = 16,
#elif(CLUT_MODE == CLUT1)
    .size   = 2,
#endif

#if(CLUT_MODE == CLUT8)
    .p_base = clut_8
#elif(CLUT_MODE == CLUT4)
    .p_base = clut_4
#elif(CLUT_MODE == CLUT1)
    .p_base = clut_1
#endif
};


void CLUT_init(void)
{
    fsp_err_t err;


    //CLUT Setup
    /* Get LCDC configuration */
    g_hz_size1 = (g_display_cfg.input[1].hsize);
    g_vr_size1 = (g_display_cfg.input[1].vsize);
    g_hstride1 = (g_display_cfg.input[1].hstride); // DISPLAY_HSIZE_INPUT1;//
    g_buffer_size1 = (uint32_t) (g_hz_size1 * g_vr_size1 * BYTES_PER_PIXEL_CLUT8);

    //Update the CLUT table for the foreground (layer 2)
    err = R_GLCDC_ClutUpdate(&g_display_ctrl, &clut_cfg, DISPLAY_FRAME_LAYER_2 );
    /* Handle error */
    if(err)
    {
        __BKPT();
    }

    memset(&fb_foreground, 0xff, sizeof(fb_foreground));


    display_draw_clut(&fb_foreground);
}

void RGB_init(void)
{
    /* Get LCDC configuration */
    g_hz_size = (g_display_cfg.input[0].hsize);
    g_vr_size = (g_display_cfg.input[0].vsize);
    g_hstride = (g_display_cfg.input[0].hstride);

    /* Initialize buffer pointers */
    g_buffer_size = (uint32_t) (g_hz_size * g_vr_size * BYTES_PER_PIXEL_RGB565);
    gp_single_buffer = (uint32_t*) g_display_cfg.input[0].p_base;

    /* Double buffer for drawing color bands with good quality */
    gp_double_buffer = gp_single_buffer + g_buffer_size;


    display_clear_rgb565 (gp_single_buffer);
    display_clear_rgb565 (gp_double_buffer);

    /* Display color bands on LCD screen */
    display_draw_rgb565 (gp_single_buffer);

}


/*******************************************************************************************************************/
/**
 * @brief      User-defined function to draw the current display to a framebuffer.
 *
 * @param[in]  framebuffer    Pointer to frame buffer.
 * @retval     None.
 **********************************************************************************************************************/
static void display_draw_rgb565 (uint32_t * framebuffer)
{
    /* Draw buffer */
    uint32_t color[COLOR_BAND_COUNT]= {BLUE, LIME, RED, BLACK, WHITE, YELLOW, AQUA, MAGENTA};
    uint16_t bit_width = g_hz_size / COLOR_BAND_COUNT;

    for (uint32_t y = 0; y < g_vr_size; y++)
    {
        for (uint32_t x = 0; x < g_hz_size; x ++)
        {
            uint32_t bit       = x / bit_width;
            framebuffer[x] = color [bit];
        }
        framebuffer += g_hstride;
    }
}


static void display_clear_rgb565 (uint32_t * framebuffer)
{
    /* Draw buffer */

    for (uint32_t y = 0; y < g_vr_size; y++)
    {
        for (uint32_t x = 0; x < g_hz_size; x ++)
        {
            framebuffer[x] = BLACK;
        }
        framebuffer += g_hstride;
    }
}

/*******************************************************************************************************************//**
 * @brief      User-defined function to draw the current display to a framebuffer.
 *
 * @param[in]  framebuffer    Pointer to frame buffer.
 * @retval     None.
 **********************************************************************************************************************/
static void display_draw_clut (uint8_t * framebuffer)
{
    /* Draw buffer */
    uint16_t bit_width = (uint16_t)(g_vr_size1 / COLOR_BAND_COUNT_CLUT);
    volatile uint8_t bit       = 0;

    for (uint32_t y = 0; y < g_vr_size1; y++)
    {
        bit = 0;
#if CLUT_MODE == CLUT8
        bit = y / (uint8_t)bit_width;
        if( bit > COLOR_BAND_COUNT_CLUT ) //overflow protection
        {
            bit = COLOR_BAND_COUNT_CLUT;
        }
#elif CLUT_MODE == CLUT4
        bit = ((y)/bit_width)<<4 | ((y)/bit_width);
#elif CLUT_MODE == CLUT1
        if( 0 == y/bit_width)
        {
            bit = 0x00u;
        }
        else
        {
            bit = 0xFFu;
        }
#endif
//        bit = 1;

        for (uint32_t x = 0; x < g_hz_size1; x++)
        {

            framebuffer[x] = bit;
        }
#if CLUT_MODE == CLUT8
        framebuffer += g_hstride1;
#elif CLUT_MODE == CLUT4
        framebuffer += g_hstride1/2;
#elif CLUT_MODE == CLUT1
        framebuffer += g_hstride1/8;
#endif
    }
}
