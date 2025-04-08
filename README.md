# mouseMover

This is a linux kernel module for CS-430 Operating Systems.

## Description

This module acts as a solution to the burdensome issue of using the dreaded mouse :cold_sweat:

When inserting this module, you will be able to control the mouse with the arrow keys and using left click with the right alt button.
> Turn on the "no mouse" mode by pressing the right control button.

## Inserting

Use the Makefile to build and inserting with the following:

```bash
make
sudo insmod mouse.ko
```

And to clean up:

```bash
sudo rmmod mouse.ko
make clean
```

## Controls
- TURN ON/OFF: Right-CTRL
- MOVEMENT: Arrow Keys
- LEFT-CLICK: Right-ALT

You can change how much the mouse moves with each click by modifying the definition near the top of the file:

```c
#define SPEED 1 // Change this number to any int
```
