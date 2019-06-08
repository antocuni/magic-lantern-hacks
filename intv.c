#define CONFIG_DIGIC_678

#include <dryos.h>
#include <module.h>
#include <menu.h>
#include <config.h>
#include <console.h>
#include <shoot.h>

static CONFIG_INT("intv.counter", intv_counter, 0);
static int intv_enabled = 0;


static void intv_task()
{
    msleep(2000);
    console_clear();
    console_show();

    printf("Intervalometer started\n");
    printf("You have run this demo %d times.\n", ++intv_counter);
    printf("Press the shutter halfway to take a pic\n");

    while (!get_halfshutter_pressed())
    {
        msleep(100);
    }

    printf("Taking a pic...\n");
    take_a_pic(false);
    printf("Press the shutter halfway to exit.\n");
    while (!get_halfshutter_pressed())
    {
        msleep(100);
    }

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
                .name       = "Start intv",
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
    MODULE_CONFIG(intv_counter)
MODULE_CONFIGS_END()
