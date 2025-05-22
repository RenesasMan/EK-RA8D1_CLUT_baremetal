/*
 * graphics_rendering.c
 *
 *  Created on: Apr 10, 2025
 *      Author: a5138119
 */

#include "graphics_rendering.h"
#include "common_utils.h"
#include "common_data.h"
#include "stdint.h"
#include "SEGGER_RTT/SEGGER_RTT.h"
#include "time_counter/time_counter.h"


/* User defined functions */
static void display_draw_rgb565 (uint32_t * framebuffer);
static void display_draw_clut (uint8_t * framebuffer);


void CLUT_init(void);
void RGB_init(void);


/* Variables used for buffer usage */
uint32_t g_buffer_size, g_hstride;
uint32_t * gp_single_buffer = NULL;
uint32_t * gp_double_buffer = NULL;
uint32_t * gp_frame_buffer  = NULL;

//uint32_t timer_top = &g_time_counter.p_cfg->period_counts;





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


/*******************************************************************************************************************//**
 * @brief      This initializes the CLUT drawing, sets the timers, commands the CLUT draw, and outputs timing on RTT
 *
 * @param[in]  none
 * @retval     none
 **********************************************************************************************************************/
void CLUT_init(void)
{
    volatile uint32_t timer_count_before, timer_count_reg_before, timer_count_after, timer_count_reg_after;
    volatile uint32_t timer_ms_decimal = 0;

    //Prepare timer to measure the duration of rendering
    TimeCounter_CountReset();
    timer_count_before = TimeCounter_CurrentCountGet();
    timer_count_reg_before = TimeCounter_CurrentCountRegGet();

    display_draw_clut( (void *) &fb_foreground);

    TimeCounter_Stop();

    timer_count_after = TimeCounter_CurrentCountGet();
    timer_count_reg_after = TimeCounter_CurrentCountRegGet();

    //combine 100ms inverval interrupt and counter register to get precise timing
    timer_ms_decimal = ((timer_count_after - timer_count_before)%10)*100 + ((timer_count_reg_after - timer_count_reg_before)*100)/g_time_counter.p_cfg->period_counts;

    APP_PRINT("\r\nCLUT Draw Time: %u.%u ms\tFrame Buffer Size: %u Bytes\r\n", (timer_count_after - timer_count_before)/10, timer_ms_decimal, sizeof(fb_foreground) );

    TimeCounter_Start();
}

/*******************************************************************************************************************//**
 * @brief      This initializes the RGB565 drawing, sets the timers, commands the CLUT draw, and outputs timing on RTT
 *
 * @param[in]  none
 * @retval     none
 **********************************************************************************************************************/
void RGB_init(void)
{
    volatile uint32_t timer_count_before, timer_count_reg_before, timer_count_after, timer_count_reg_after;
    volatile uint32_t timer_ms_decimal = 0;

    /* Double buffer for drawing color bands with good quality */
    gp_double_buffer = gp_single_buffer + g_buffer_size;

    //Prepare timer to measure the duration of rendering
    TimeCounter_Stop();
    TimeCounter_CountReset();
    timer_count_before = TimeCounter_CurrentCountGet();
    timer_count_reg_before = TimeCounter_CurrentCountRegGet();


    /* Display color bands on LCD screen */
    display_draw_rgb565( (void *) &fb_background);

    TimeCounter_Stop();

    timer_count_after = TimeCounter_CurrentCountGet();
    timer_count_reg_after = TimeCounter_CurrentCountRegGet();

    //combine 100ms inverval interrupt and counter register to get precise timing
    timer_ms_decimal = ((timer_count_after - timer_count_before)%10)*100 + ((timer_count_reg_after - timer_count_reg_before)*100)/g_time_counter.p_cfg->period_counts;

    APP_PRINT("\r\nRGB565 Draw Time: %u.%u ms\tFrame Buffer Size: %u Bytes\r\n", (timer_count_after - timer_count_before)/10, timer_ms_decimal, sizeof(fb_background) );

    TimeCounter_Start();

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
    /* Variables to store resolution information */
    uint16_t g_hz_size, g_vr_size;

    /* Draw buffer */
    /* Get LCDC configuration */
    g_hz_size = (g_display_cfg.input[0].hsize);
    g_vr_size = (g_display_cfg.input[0].vsize);
    g_hstride = (g_display_cfg.input[0].hstride);

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


/*******************************************************************************************************************//**
 * @brief      User-defined function to draw the current display to a framebuffer.
 *
 * @param[in]  framebuffer    Pointer to frame buffer.
 * @retval     None.
 **********************************************************************************************************************/
static void display_draw_clut (uint8_t * framebuffer)
{
//    fsp_err_t err;
    /* Variables to store resolution information */
    uint16_t g_hz_size1, g_vr_size1;

    /* Variables used for buffer usage */
    uint32_t g_hstride1;

    //CLUT Setup
    /* Get LCDC configuration */
    g_hz_size1 = (g_display_cfg.input[1].hsize); //480
    g_vr_size1 = (g_display_cfg.input[1].vsize); //854
    g_hstride1 = (g_display_cfg.input[1].hstride); // DISPLAY_HSIZE_INPUT1;// 512


    /* Draw buffer */
    uint16_t bit_width = (uint16_t)(g_vr_size1 / COLOR_BAND_COUNT_CLUT); //56
    volatile uint8_t bit       = 0;

    for (uint32_t y = 0; y < g_vr_size1; y++)
    {
        bit = 0;
#if CLUT_MODE == CLUT8
        bit = (uint8_t)(y / bit_width);
        if( bit > COLOR_BAND_COUNT_CLUT ) //overflow protection
        {
            bit = COLOR_BAND_COUNT_CLUT;
        }
#elif CLUT_MODE == CLUT4
        bit = (uint8_t)(((y)/bit_width)<<4 | ((y)/bit_width));
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



#if CLUT_MODE == CLUT8
        for (uint32_t x = 0; x < g_hz_size1; x++)
        {

            framebuffer[x] = bit;
        }
        framebuffer += g_hstride1;
#elif CLUT_MODE == CLUT4
        for (uint32_t x = 0; x < g_hz_size1/2; x++)
        {

            framebuffer[x] = bit;
        }
        framebuffer += g_hstride1/2;
#elif CLUT_MODE == CLUT1
        for (uint32_t x = 0; x < g_hz_size1/8; x++)
        {

            framebuffer[x] = bit;
        }
        framebuffer += g_hstride1/8;
#endif
    }
}
