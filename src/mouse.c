// Requirements:
// - Move the mouse with the arrow keys
//   - Left: 	105
//   - Right: 	106
//   - Up: 	103 
//   - Down: 	109
// - Use the right-alt button as left click: 100
//
// Notes:
// - To make it smoother, we can lower SPEED, but it will be slower
//   - How can we make it call more often/quicker?

#include <linux/module.h>
//#include <linux/config.h>
#include <linux/init.h>
#include <linux/tty.h>          /* For fg_console, MAX_NR_CONSOLES */
#include <linux/kd.h>           /* For KDSETLED */
#include <linux/vt.h>
#include <linux/vt_kern.h>
#include <linux/console_struct.h>       /* For vc_cons */
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <linux/keyboard.h>

#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/ctype.h>

#define SPEED 1

int state = 0;
struct input_dev *ex3_dev;
struct timer_list my_timer;
struct notifier_block nb;

int kb_notifier_fn(struct notifier_block *pnb, unsigned long action, void* data){
	struct keyboard_notifier_param *kp = (struct keyboard_notifier_param*)data;
	if(kp->down){
		switch(kp->value){
			case 103: // Up
				for(int i=0; i<10; i++){
					input_report_rel(ex3_dev, REL_Y, -SPEED);
				}
				break;
			case 105: // Left
				for(int i=0; i<10; i++){
					input_report_rel(ex3_dev, REL_X, -SPEED);
				}
				break;
			case 106: // Right
				for(int i=0; i<10; i++){
					input_report_rel(ex3_dev, REL_X, SPEED);
				}
				break;
			case 108: // Down
				for(int i=0; i<10; i++){
					input_report_rel(ex3_dev, REL_Y, SPEED);
				}
				break;
		}
	}

	return 0;
}

void my_timer_func(struct timer_list *unused){
	/* move in a small square */
	if (state<30) 
		input_report_rel(ex3_dev, REL_X, 5);
	else if (state < 60)
		input_report_rel(ex3_dev, REL_Y, 5);
	else if (state < 90)
		input_report_rel(ex3_dev, REL_X, -5);
	else
		input_report_rel(ex3_dev, REL_Y, -5);

	if ((state++) >= 120){
		state = 0;
	}
	/*
	input_report_key(ex3_dev, BTN_LEFT, 1);
	input_sync(ex3_dev);
	input_report_key(ex3_dev, BTN_LEFT, 0);
	*/

	input_sync(ex3_dev);


	/* set timer for 0.02 seconds */
	mod_timer(&my_timer, jiffies+HZ/125);
}


static int __init ex3_init(void){
		
	/* extra safe initialization */
	ex3_dev = input_allocate_device();

	/* set up descriptive labels */
	ex3_dev->name = "Mouse Mover";
	/* phys is unique on a running system */
	ex3_dev->phys = "A/Fake/Path";
	ex3_dev->id.bustype = BUS_HOST;
	ex3_dev->id.vendor = 0x0001;
	ex3_dev->id.product = 0x0003;
	ex3_dev->id.version = 0x0100;

	/* this device has two relative axes */
	set_bit(EV_REL, ex3_dev->evbit);
	set_bit(REL_X, ex3_dev->relbit);
	set_bit(REL_Y, ex3_dev->relbit);

	/* it needs a button to look like a mouse */
	set_bit(EV_KEY, ex3_dev->evbit);
	set_bit(BTN_LEFT, ex3_dev->keybit);

	/* and finally register with the input core */
	input_register_device(ex3_dev);

	/* set up a repeating timer */
	//timer_setup(&my_timer, my_timer_func, 0);
	//my_timer.expires = jiffies + HZ/10;
	//add_timer(&my_timer);
	
	// Initilize keyboard notifier
	nb.notifier_call = kb_notifier_fn;
	register_keyboard_notifier(&nb);

	return 0;
}

static void __exit ex3_exit(void){
	del_timer(&my_timer);
	input_unregister_device(ex3_dev);
	unregister_keyboard_notifier(&nb);
}

MODULE_LICENSE("GPL"); 
module_init(ex3_init);
module_exit(ex3_exit);

