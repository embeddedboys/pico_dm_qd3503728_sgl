// Copyright (c) 2024 embeddedboys developers

// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:

// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>

#include "pico/time.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "pico/stdio_uart.h"

#include "hardware/pll.h"
#include "hardware/vreg.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"

#include "ili9488.h"
#include "ft6236.h"
#include "backlight.h"

#include "sgl.h"
#include "sgl_anim.h"
#include "sgl_font.h"

static unsigned int sgl_frames = 0;
static sgl_color_t video_memory[LCD_HOR_RES * 10];

bool sgl_tick(struct repeating_timer *t)
{
	sgl_tick_inc(1);
	return true;
}

bool sgl_monitor(struct repeating_timer *t)
{
	sgl_mm_monitor_t mm = sgl_mm_get_monitor();
	printf("SGL Frame = %d\n", sgl_frames);
	printf("Memory: total: %d used: %d, free = %d\n", mm.total_size,
	       mm.used_size, mm.free_size);
	return true;
}

#define sgl_area_get_size(area) \
	((area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1))
static void sgl_flush_area(sgl_area_t *area, sgl_color_t *src)
{
	int size = sgl_area_get_size(area);
	// printf("%s, size = %d\n", __func__, size);
	ili9488_video_flush(area->x1, area->y1, area->x2, area->y2, src,
			    size * sizeof(sgl_color_t));
	sgl_fbdev_flush_ready();
}

static void sgl_logger(const char *str)
{
	// printf(str);
	puts(str);
}

int main(void)
{
	/* NOTE: DO NOT MODIFY THIS BLOCK */
#define CPU_SPEED_MHZ (DEFAULT_SYS_CLK_KHZ / 1000)
	if (CPU_SPEED_MHZ > 266 && CPU_SPEED_MHZ <= 360)
		vreg_set_voltage(VREG_VOLTAGE_1_20);
	else if (CPU_SPEED_MHZ > 360 && CPU_SPEED_MHZ <= 396)
		vreg_set_voltage(VREG_VOLTAGE_1_25);
	else if (CPU_SPEED_MHZ > 396)
		vreg_set_voltage(VREG_VOLTAGE_MAX);
	else
		vreg_set_voltage(VREG_VOLTAGE_DEFAULT);

	set_sys_clock_khz(CPU_SPEED_MHZ * 1000, true);
	clock_configure(clk_peri, 0, CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
			CPU_SPEED_MHZ * MHZ, CPU_SPEED_MHZ * MHZ);
	stdio_uart_init_full(uart0, 115200, 16, 17);
	stdio_usb_init();

	printf("\n\n\nPICO DM QD3503728 Display Template\n");

	ili9488_driver_init();
	// ft6236_driver_init();

	ili9488_fill(0xffff);

	sleep_ms(10);
	backlight_driver_init();
	backlight_set_level(100);
	printf("backlight set to 100%%\n");

	struct repeating_timer sgl_tick_timer;
	add_repeating_timer_ms(1, sgl_tick, NULL, &sgl_tick_timer);

	struct repeating_timer sgl_monitor_timer;
	add_repeating_timer_ms(2000, sgl_monitor, NULL, &sgl_monitor_timer);

	sgl_fbinfo_t fbinfo = {
		.xres = LCD_HOR_RES,
		.yres = LCD_VER_RES,
		.flush_area = sgl_flush_area,
		.buffer[0] = video_memory,
		.buffer_size = SGL_ARRAY_SIZE(video_memory),
	};
	sgl_logdev_register(sgl_logger);
	sgl_fbdev_register(&fbinfo);
	sgl_init();

	sgl_obj_t *btn = sgl_button_create(NULL);
	sgl_button_set_text(btn, "Hello, World!");
	sgl_button_set_color(btn, SGL_COLOR_CADET_BLUE);
	sgl_button_set_text_color(btn, SGL_COLOR_WHITE);
	sgl_button_set_font(btn, &song23);
	sgl_obj_set_size(btn, 200, 80);
	sgl_obj_set_pos_align(btn, SGL_ALIGN_CENTER);
	sgl_button_set_radius(btn, 10);

	printf("going to loop, %lld\n", time_us_64() / 1000);
	for (;;) {
		sgl_task_handle();
		sgl_frames++;
		// printf("%lld\n", time_us_64() / 1000);
		// sleep_ms(200);
	}

	return 0;
}
