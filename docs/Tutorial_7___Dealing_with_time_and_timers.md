# Tutorial 7 – Dealing with time and timers

Original URL: http://www.drmop.com/index.php/2011/10/01/marmalade-sdk-tutorial-time-and-timers/

This tutorial is part of the Marmalade SDK tutorials collection. To see the tutorials index [click here](http://www.drmop.com/?page_id=91 "Marmalade SDK tutorials index")

A little bit rushed today, so please forgive and point out any mistakes that I’ve made. In today’s tutorial we are going to cover time and timers. As usual if you just want the associated article source code then you can [Download it from here](http://www.drmop.com/wp-content/uploads/2011/10/Time.zip "Marmalade SDK tutorial example source code - Time and Timers"). If you want the details on time and timers then please do read on.

When developing apps and games you will quickly run into situations where you need to deal with time. For example, you need to fire off an event at some point in the future, have something occur at a set regular interval or time how long something takes to execute.

The Marmalade SDK provides functionality for dealing with both time and timers.

Reading Time
------------

Marmalade has two main functions for reading time:

* s3eTimerGetMs() – Returns the Uninterrupted System Time (UST) in milliseconds. This is the number of milliseconds that have passed since Marmalade started running
* s3eTimerGetUTC() – Returns the Universal Coordinated Time (UCT) in millseconds. This is the number of milliseconds that have passed since the epoch (00:00:00 UTC, January 1, 1970)

I am only going to cover s3eTimerGetMs() in this tutorial as Marmalade have kindly provided a great example of s3eTimerGetUTC() [here](http://www.madewithmarmalade.com/devnet/docs#/api/api/ExampleS3ETimerBasic.html "Marmalade SDK s3eTimerGetUTC usage example")

Frame Rate and Animation Stabilisation Using Time
-------------------------------------------------

Ok, the main reason I wanted to cover time is that it is incredibly important when it comes to game development and app development that features animating components. In game development there are a few terms that you should be aware of:

* FPS – Frame per second – All hardware with a display usually refreshes the display a certain number of times per second to give the impression to the user of smooth animation. The refresh rate is usually specified in frames per second (FPS) and varies from 10 to 60 fps, with 30 fps being about the average for many devices. If a game or app can carry out all of its logic and rendering in one frame (1/30th of a second if running at 30 fps) then it is said to run in a frame. If the game does not then animation will drop out of a frame into 2 or even 3 frames. When this happens the user will see a noticeable slow down of all animations (looks like slow motion animation), which looks pretty ugly and can put a lot of users off.
* Frame time – Frame time is the amount of time it takes for the hardware to refresh one display frame. if the hardware update frame rate is 30 fps then the frame time is 1 / 30 = 0.033333 = 33 milliseconds.

One of the first problems developers usually hit is how to get their game to run smoothly across a large range of unknown spec devices. Its simply not possible to optimise your game for every phone and tablet out there, the costs for producing the game or app would be staggering. Luckily there is a simple solution

Each animating component in our game will usually have some variable that tells it how often to change; usually a velocity of some kind.. If our game does take longer to update and render than we expect then we can simply scale these variables to ensure that the animating components animate smoothly even if our frame rate changes.

As an example, lets assume we have a ship that flies across the screen from left to right at a given velocity, lets call it ship\_vx. If the game is designed to run at 30 fps (33 milliseconds per frame update) and we want the ship to move at a rate of 30 pixels every second then it should move by 1 pixel every time we update the ships position (as there are 30 game frames in one second), so our ships velocity ship\_vx would be 1.0. However, what if we ran our game on a very slow phone and our main game loop is running at 15 fps? If we put our 15 fps and 30 fps phones side by side and watched the ship on both displays then the ship would reach the other side of the screen on the 30 fps phone much faster, twice as fast in fact,

Now for the fix, how would we make our ship on the slower phone move at the same rate as our faster phone? If we made the ships velocity ship\_vx = 2.0 on the slow phone then both ships would travel at the same speed on both phones.

This sounds good, but our ship can be running across a whole range of phones at different frame rates, so how do we know how much to change the ships velocity?

We can use time to figure this out. If we measure how long the last frame took to update and render then we can determine how much we need to scale our ships velocity by in the next game loop. Here’s some basic code (based on the code we have added to Main.cpp), showing how to do this:

```
> float duration_of_one_frame = 1000.0f / 30.0f;    // 33 milliseconds
> uint64 last_frame_time = s3eTimerGetMs();
> float time_scale = 1.0;
> 
> ship_vx = 1.0f;
> 
> while (main_game_loop_alive)
> {
>     // Move ship by its velocity
>     ship_pos.x += ship_vx * time_scale;
>     RenderShip();
> 
>     // Calculate the next frame scale value
>     uint64 current_frame_time = s3eTimerGetMs();
>     time_scale = (current_frame_time - last_frame_time) / duration_of_one_frame;
>     last_frame_time = current_frame_time;
> 
> }
```

This code isn’t perfect however, it has a few flaws such as time\_scale is always lagging by one frame, which can cause a little jitter if your game is quickly falling in and out of 30 fps. It also does not cap time\_scale, just in case your game takes much too long to update and render. To fix this you could add the following line:

```
if (time_scale > 4.0f) time_scale = 4.0f;
```

Note that value of 4.0 caps time animation to 7.5 fps, so if your game does drop below 7.5 fps then you will suffer slow motion problems. That said, If your game is consistently running as slowly as around 15 fps or less then its probably time to look at optimising it or reporting to the user that the game is not suitable for their device.

In the code that comes with this tutorial you will note that we have made a few minor changes:

In Main,cpp you will notice that we now spin our boxes based on time, which should ensure that they spin at the same rate on different hardware:

```
// Spin our awesome sprite
sprite_angle += sprite_angle_velocity * time_scale;
```

Note that we switched over from fixed point to floating point at this point just to make the implementation clearer

To confirm that frame scale works, change the s3eDeviceYield(0) statement to something like s3eDeviceYield(50). The frame rate wont look as smooth but the boxes will still spin at the same rate.

Timers and Alarms
-----------------

Timers (with alarms) are amazing little inventions, they allow us to fire off events at some time in the future and at regular intervals. The benefits for a game and app developer stretch far and wide.

To create a timer using the Marmalade SDK you call:

```
s3eTimerSetTimer(unit32 ms, s3eCallback fn, void* userData)
```

ms is the number of milliseconds to wait from now before calling our alarm call-back function defined by fn.

Lets take a quick look at the Timer project code file Main.cpp

We set up the initial alarm in main() using:

```
> // Start our timer alarm to go off 200 milliseconds from now
> s3eTimerSetTimer(200, &Alarm, NULL);
```

And here is the implementation of our Alarm call-back function:

```
> // Alarm call back function, called by Marmalade when the timer expires
> static int32 Alarm(void *systemData, void *userData)
> {
>     // Restart the alarm again for 200 millseconds from now
>     s3eTimerSetTimer(200, &Alarm, NULL);
>     g_AlarmOccured = true;
> 
>     return 0;
> }
```

Note that in the Alarm call-back, we reset the timer so that it fires again, causing a timer alarm that will continue to fire every 200 milliseconds.

Lastly, its possible to cancel timers using s3eTimerCancelTimer()

Well thats it for this tutorial. I hope you all find it of some use. You can download the associated [Time project source code from here](http://www.drmop.com/wp-content/uploads/2011/10/Time.zip "Marmalade SDK tutorial example source code - Time and Timers").  
Happy coding and make sure that your shoe laces are tied!