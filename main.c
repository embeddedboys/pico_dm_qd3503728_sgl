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
// #include "sgl_anim.h"
#include "sgl_font.h"

static sgl_color_t video_memory[LCD_HOR_RES * 10];

static sgl_obj_t *test_page = NULL;
static sgl_obj_t *btn_test = NULL;
static sgl_obj_t *label_test = NULL;
static sgl_obj_t *slider_test = NULL;
static sgl_obj_t *progress_test = NULL;
static sgl_obj_t *switch_test = NULL;

bool sgl_tick(struct repeating_timer *t)
{
	sgl_tick_inc(1);
	sgl_task_handle();
	return true;
}

bool sgl_monitor(struct repeating_timer *t)
{
	sgl_mm_monitor_t mm = sgl_mm_get_monitor();
	printf("Memory: total: %d used: %d, free = %d\n", mm.total_size,
	       mm.used_size, mm.free_size);
	return true;
}

#define sgl_area_get_size(area) \
	((area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1))
static void sgl_flush_area(sgl_area_t *area, sgl_color_t *src)
{
	int size = sgl_area_get_size(area);
	ili9488_video_flush(area->x1, area->y1, area->x2, area->y2, src,
			    size * sizeof(sgl_color_t));
	sgl_fbdev_flush_ready();
}

static void sgl_logger(const char *str)
{
	puts(str);
}

struct sgl_test {
	u8 idx;
	const char *name;
	void (*func)(void);
};
#define DEFINE_SGL_TEST(idx, name, func) { idx, name, func }

static void test_text(void)
{
	sgl_obj_t *tb = sgl_textbox_create(test_page);
	sgl_textbox_set_text(
		tb,
		"SGL (Small Graphics Library) is a lightweight and fast graphics library specifically designed to provide beautiful and lightweight GUI (Graphical User Interface) for MCU-level processors. Please refer to the docs directory for documentation. Lightweight, requiring only 3KB RAM and 15KB ROM to run at minimum. Partial frame buffer support, requiring only one line of screen resolution buffer at minimum. Bounding box + greedy algorithm based dirty rectangle algorithm. Frame buffer controller support, direct write to frame buffer controller, zero copy");
	sgl_textbox_set_text_font(tb, &consolas24);
	sgl_textbox_set_radius(tb, 0);
	sgl_obj_set_size(tb, 480, 320);
	sgl_obj_set_pos_align(tb, SGL_ALIGN_CENTER);

	sleep_ms(2000);

	sgl_obj_delete(tb);
}

static void test_drawing(void)
{
	sgl_obj_t *bg = sgl_rect_create(test_page);
	sgl_rect_set_color(bg, SGL_COLOR_BLACK);
	sgl_obj_set_size(bg, 480, 320);
	sgl_obj_set_pos_align(bg, SGL_ALIGN_CENTER);

	sgl_obj_t *line1 = sgl_line_create(test_page);
	sgl_line_set_color(line1, SGL_COLOR_RED);
	sgl_line_set_width(line1, 2);
	sgl_obj_set_size(line1, 300, 2);
	sgl_obj_set_pos(line1, 50, 100);

	sgl_obj_t *line2 = sgl_line_create(test_page);
	sgl_line_set_color(line2, SGL_COLOR_GREEN);
	sgl_line_set_width(line2, 2);
	sgl_obj_set_size(line2, 300, 150);
	sgl_obj_set_pos(line2, 50, 150);

	sgl_obj_t *circle = sgl_circle_create(test_page);
	sgl_circle_set_color(circle, SGL_COLOR_YELLOW);
	sgl_circle_set_border_color(circle, SGL_COLOR_BLACK);
	sgl_circle_set_border_width(circle, 2);
	sgl_obj_set_size(circle, 80, 80);
	sgl_obj_set_pos(circle, 260, 85);

	sgl_obj_t *arc = sgl_arc_create(test_page);
	sgl_arc_set_color(arc, SGL_COLOR_MAGENTA);
	sgl_obj_set_size(arc, 100, 100);
	sgl_obj_set_pos(arc, 150, 150);

	sleep_ms(2000);

	sgl_obj_delete(bg);
	sgl_obj_delete(line1);
	sgl_obj_delete(line2);
	sgl_obj_delete(circle);
	sgl_obj_delete(arc);
}

static void test_widgets(void)
{
	sgl_obj_t *bg = sgl_rect_create(test_page);
	sgl_rect_set_color(bg, SGL_COLOR_WHITE);
	sgl_obj_set_size(bg, 480, 320);
	sgl_obj_set_pos_align(bg, SGL_ALIGN_CENTER);

	btn_test = sgl_button_create(test_page);
	sgl_button_set_text(btn_test, "Button");
	sgl_button_set_color(btn_test, SGL_COLOR_CADET_BLUE);
	sgl_button_set_text_color(btn_test, SGL_COLOR_WHITE);
	sgl_button_set_font(btn_test, &song23);
	sgl_obj_set_size(btn_test, 150, 60);
	sgl_obj_set_pos(btn_test, 50, 50);

	label_test = sgl_label_create(test_page);
	sgl_label_set_text(label_test, "Label Test");
	sgl_label_set_text_color(label_test, SGL_COLOR_BLACK);
	sgl_label_set_font(label_test, &song23);
	sgl_obj_set_size(label_test, 200, 40);
	sgl_obj_set_pos(label_test, 50, 120);

	slider_test = sgl_slider_create(test_page);
	sgl_slider_set_value(slider_test, 50);
	sgl_obj_set_size(slider_test, 200, 30);
	sgl_obj_set_pos(slider_test, 50, 170);

	progress_test = sgl_progress_create(test_page);
	sgl_progress_set_value(progress_test, 75);
	sgl_obj_set_size(progress_test, 200, 30);
	sgl_obj_set_pos(progress_test, 50, 220);

	switch_test = sgl_switch_create(test_page);
	sgl_switch_set_bg_color(switch_test, SGL_COLOR_RED);
	sgl_switch_set_status(switch_test, true);
	sgl_obj_set_size(switch_test, 80, 40);
	sgl_obj_set_pos(switch_test, 50, 270);

	sleep_ms(2000);

	sgl_obj_delete(bg);
	sgl_obj_delete(btn_test);
	sgl_obj_delete(label_test);
	sgl_obj_delete(slider_test);
	sgl_obj_delete(progress_test);
	sgl_obj_delete(switch_test);
}

static void test_colors(void)
{
	sgl_color_t colors[] = { SGL_COLOR_WHITE, SGL_COLOR_BLACK,
				 SGL_COLOR_RED,	  SGL_COLOR_GREEN,
				 SGL_COLOR_BLUE,  SGL_COLOR_MAGENTA,
				 SGL_COLOR_CYAN,  SGL_COLOR_YELLOW };

	sgl_obj_t *bg = sgl_rect_create(test_page);
	sgl_obj_set_size(bg, 480, 320);
	sgl_obj_set_pos_align(bg, SGL_ALIGN_CENTER);

	for (int i = 0; i < SGL_ARRAY_SIZE(colors); i++) {
		sgl_rect_set_color(bg, colors[i]);
		sleep_ms(200);
	}
}

static const struct sgl_test sgl_tests[] = {
	DEFINE_SGL_TEST(0, "Text Test", test_text),
	DEFINE_SGL_TEST(1, "Drawing Test", test_drawing),
	DEFINE_SGL_TEST(2, "Widget Test", test_widgets),
	DEFINE_SGL_TEST(3, "Color Test", test_colors),
	{ /* sentinel */ },
};

static void sgl_test(void)
{
	const struct sgl_test *test = sgl_tests;
	for (; test->func; test++) {
		printf("Run test %d: %s\n", test->idx, test->name);
		test->func();
	}
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

	printf("\n\n\nPICO DM QD3503728 SGL Test\n");

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

	test_page = sgl_obj_create(NULL);
	sgl_screen_load(test_page);

	printf("going to loop, %lld\n", time_us_64() / 1000);
	for (;;) {
		sgl_test();

		tight_loop_contents();
		sleep_ms(200);
	}

	return 0;
}
