#define CONFIG_DIGIC_678

#include <dryos.h>
#include <module.h>
#include <menu.h>
#include <config.h>
#include <console.h>
#include <shoot.h>

#define TIME_MAX_VALUE 28800 //8 hours

static CONFIG_INT("intv.start_delay", intv_start_delay, 2);
static CONFIG_INT("intv.count", intv_count, 0);
static int intv_enabled = 0;

static void wait_for_half_shutter(void)
{
    while (!get_halfshutter_pressed())
    {
        msleep(100);
    }
}

static void intv_task()
{
    msleep(2000);
    console_clear();
    console_show();

    /* wait to start */
    printf("Pictures to take: %d\n", intv_count);
    for(int i=intv_start_delay; i>0; i--) {
        printf("%d...\n", i);
        msleep(1000);
    }

    for(int taken = 0; taken < intv_count; taken++) {
        take_a_pic(false);
        printf("Pictures taken: %d\n", taken+1);
    }

    printf("Intervalometer done\n");
    printf("Press the shutter halfway to finish\n");
    wait_for_half_shutter();
    console_hide();
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
                .name = "Pictures to take",
                .priv = &intv_count,
                .min = 0,
                .max = 999,
                .unit = UNIT_DEC,
                .help = "Number of pictures to take (0 for infinite)"
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
                .name       = "Start shooting!",
                .select     = run_in_separate_task,
                .priv       = intv_task,
                .help       = "Start intervalometer",
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
MODULE_CONFIGS_END()
