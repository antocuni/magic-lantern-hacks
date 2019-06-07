/* A very simple module
 * (example for module authors)
 */

#define CONFIG_DIGIC_678

#include <dryos.h>
#include <module.h>
#include <menu.h>
#include <config.h>
#include <console.h>
#include <shoot.h>

/* Config variables. They are used for persistent variables (usually settings).
 * 
 * In modules, these variables also have to be declared as MODULE_CONFIG.
 */
static CONFIG_INT("intv.counter", intv_counter, 0);


/* This function runs as a new DryOS task, in parallel with everything else.
 * 
 * Tasks started in this way have priority 0x1A (see run_in_separate_task in menu.c).
 * They can be interrupted by other tasks with higher priorities (lower values)
 * at any time, or by tasks with equal or lower priorities while this task is waiting
 * (msleep, take_semaphore, msg_queue_receive etc).
 * 
 * Tasks with equal priorities will never interrupt each other outside the
 * "waiting" calls (cooperative multitasking).
 * 
 * Additionally, for tasks started in this way, ML menu will be closed
 * and Canon's powersave will be disabled while this task is running.
 * Both are done for convenience.
 */
static void intv_task()
{
    /* Open the console. */
    /* Also wait for background tasks to settle after closing ML menu */
    msleep(2000);
    console_clear();
    console_show();

    /* Plain printf goes to console. */
    /* There's very limited stdio support available. */
    printf("Intervalometer started\n");
    printf("You have run this demo %d times.\n", ++intv_counter);
    printf("Press the shutter halfway to take a pic\n");

    /* note: half-shutter is one of the few keys that can be checked from a regular task */
    /* to hook other keys, you need to use a keypress hook - see intv2 */
    while (!get_halfshutter_pressed())
    {
        /* while waiting for something, we must be nice to other tasks as well and allow them to run */
        /* (this type of waiting is not very power-efficient nor time-accurate, but is simple and works well enough in many cases */
        msleep(100);
    }

    printf("Taking a pic...\n");
    take_a_pic(false);
    printf("Press the shutter halfway to exit.\n");
    while (!get_halfshutter_pressed())
    {
        msleep(100);
    }

    /* Finished. */
    console_hide();
}

static struct menu_entry intv_menu[] =
{
    {
        .name       = "Start intv",
        .select     = run_in_separate_task,
        .priv       = intv_task,
        .help       = "Start intervalometer",
    },
};

/* This function is called when the module loads. */
/* All the module init functions are called sequentially,
 * in alphabetical order. */
static unsigned int intv_init()
{
    menu_add("Debug", intv_menu, COUNT(intv_menu));
    return 0;
}

/* Note: module unloading is not yet supported;
 * this function is provided for future use.
 */
static unsigned int intv_deinit()
{
    return 0;
}

/* All modules have some metadata, specifying init/deinit functions,
 * config variables, event hooks, property handlers etc.
 */
MODULE_INFO_START()
    MODULE_INIT(intv_init)
    MODULE_DEINIT(intv_deinit)
MODULE_INFO_END()

MODULE_CONFIGS_START()
    MODULE_CONFIG(intv_counter)
MODULE_CONFIGS_END()
