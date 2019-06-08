#define CONFIG_DIGIC_678

#include <dryos.h>
#include <module.h>
#include <menu.h>
#include <config.h>
#include <console.h>
#include <shoot.h>
#include <bmp.h>

#define TIME_MAX_VALUE 28800 //8 hours

static CONFIG_INT("intv.start_delay", intv_start_delay, 2);
static CONFIG_INT("intv.count", intv_count, 0);
static CONFIG_INT("intv.delay_between", intv_delay_between, 500); /* ms */
static CONFIG_INT("intv.task_delay", intv_task_delay, 200); /* ms */
static int intv_enabled = 0;

static void wait_for_half_shutter(void)
{
    while (!get_halfshutter_pressed())
    {
        msleep(100);
    }
}

/* wait ms millisecond at most; if the use press halfshutter, exit early and
   return false, else return true */
static int wait_ms_maybe(int ms) {
    for(int i=0; i<ms/100; i++) {
        msleep(100);
        if (get_halfshutter_pressed())
            return false;
    }
    return true;
}

static void intv_task()
{
    msleep(intv_task_delay);

    /* wait to start */
    for(int i=intv_start_delay; i>0; i--) {
        clrscr();
        bmp_printf(FONT_LARGE, 50, 310,
                   "Countdown: %d\n"
                   "Pictures to take: %d\n"
                   "Press half-shutter to exit",
                   i, intv_count);
        if (!wait_ms_maybe(1000))
            goto cleanup;
    }

    for(int taken = 0; taken < intv_count; taken++) {
        clrscr();
        take_a_pic(false);
        bmp_printf(FONT_LARGE, 50, 310,
                   "Pictures taken: %d\n"
                   "Press half-shutter to exit", taken+1);
        if (!wait_ms_maybe(intv_delay_between))
            goto cleanup;
    }

 cleanup:
    clrscr();
    bmp_printf(FONT_LARGE, 50, 310, "Intervalometer stopped!");
    msleep(500);
    clrscr();
}

static struct menu_entry intv_menu[] = {
    {
        .name = "My Intervalometer",
        .priv = &intv_enabled,
        .select = menu_open_submenu,
        .max = 1,
        .help = "My custom and hackish intervalomenter",
        .children = (struct menu_entry[])
        {
            {
                .name       = "Start shooting!",
                .select     = run_in_separate_task,
                .priv       = intv_task,
                .help       = "Start intervalometer",
            },
            {
                .name = "Pictures to take",
                .priv = &intv_count,
                .min = 0,
                .max = 999,
                .unit = UNIT_DEC,
                .help = "Number of pictures to take"
            },
            {
                .name = "Start after",
                .priv = &intv_start_delay,
                .min = 0,
                .max = TIME_MAX_VALUE,
                .icon_type = IT_PERCENT,
                .unit = UNIT_TIME,
                .help = "Start shooting after X seconds",
            },
            {
                .name = "Delay between shots",
                .priv = &intv_delay_between,
                .min = 0,
                .max = 5000,
                .icon_type = IT_PERCENT,
                .unit = UNIT_DEC,
                .help = "ms to wait between each shot",
            },
            {
                .name = "Task delay (DEBUG)",
                .priv = &intv_task_delay,
                .min = 0,
                .max = 5000,
                .icon_type = IT_AUTO,
                .unit = UNIT_DEC,
                .help = "ms to wait before starting the task",
            },
            MENU_EOL,
        },
    },
};


static unsigned int intv_init()
{
    menu_add("Shoot", intv_menu, COUNT(intv_menu));
    return 0;
}

static unsigned int intv_deinit()
{
    return 0;
}

MODULE_INFO_START()
    MODULE_INIT(intv_init)
    MODULE_DEINIT(intv_deinit)
MODULE_INFO_END()

MODULE_CONFIGS_START()
    MODULE_CONFIG(intv_start_delay)
    MODULE_CONFIG(intv_count)
    MODULE_CONFIG(intv_delay_between)
    MODULE_CONFIG(intv_task_delay)
MODULE_CONFIGS_END()
