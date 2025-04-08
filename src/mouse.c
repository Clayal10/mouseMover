// CONTROLS:
// - TURN ON/OFF: Right-CTRL
// - MOVEMENT: Arrow Keys
// - LEFT-CLICK: Right-ALT

#include <linux/module.h>
#include <linux/init.h>
#include <linux/tty.h>          /* For fg_console, MAX_NR_CONSOLES */
#include <linux/kd.h>           /* For KDSETLED */
#include <linux/vt.h>
#include <linux/vt_kern.h>
#include <linux/console_struct.h>       /* For vc_cons */
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/keyboard.h>

#define SPEED 1

int state = 0;
struct input_dev *ex3_dev;
struct timer_list my_timer;
struct notifier_block nb;

int kb_notifier_fn(struct notifier_block *pnb, unsigned long action, void* data){
	struct keyboard_notifier_param *kp = (struct keyboard_notifier_param*)data;
	if(kp->down){
		if(kp->value == 97){ // Right CTRL
			state = !state;
		}
		if(kp->value == 100){ // Right ALT
			input_report_key(ex3_dev, BTN_LEFT, 1);
		}
		if(!state){
			return 0;
		}
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

	if(kp->down == 0 && kp->value == 100) { // Release 'mouse click'
		input_report_key(ex3_dev, BTN_LEFT, 0);
	}
	
	input_sync(ex3_dev);

	return 0;
}

static int __init ex3_init(void){
	int error;

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
	error = input_register_device(ex3_dev);
	if(error){
		printk(KERN_ERR "Failed to register device\n");
		goto err_free_dev;
	}

	// Initilize keyboard notifier
	nb.notifier_call = kb_notifier_fn;
	register_keyboard_notifier(&nb);

	return 0;
err_free_dev:
	input_free_device(ex3_dev); // Kernel.org thinks this is important
	return error;
}

static void __exit ex3_exit(void){
	del_timer(&my_timer);
	input_unregister_device(ex3_dev);
	unregister_keyboard_notifier(&nb);
}

MODULE_LICENSE("GPL"); 
module_init(ex3_init);
module_exit(ex3_exit);

