# gba-multi

To run this example:
1) Connect two gbas with a link cable
2) turn both gbas on
3) Press the A button on each gba, to init serial comms
4) Press the B button on the master gba to start the demo, after pressing A on each linked gba that you want to use
5) Press L/R on the dpad to move colored squares.

You'll find the multiplayer code in multi.c
Usage example in example.c

I just implemented the minimal amout of stuff to demonstrate multiplayer in an example. For a real game, you would probably need to build extra stuff on top of this code to make it usable.
