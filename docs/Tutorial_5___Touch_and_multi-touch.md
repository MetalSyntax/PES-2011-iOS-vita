# Tutorial 5 – Touch and multi-touch

Original URL: http://www.drmop.com/index.php/2011/09/24/marmalade-sdk-tutorial-touch-and-multi-touch/

This tutorial is part of the Marmalade SDK tutorials collection. To see the tutorials index [click here](http://www.drmop.com/?page_id=91 "Marmalade SDK tutorials index")

A little rushed this weekend but I did kind of commit myself to producing another tutorial this weekend, so here goes.

This tutorial is going to cover handling touch and multi-touch pointer events using the Marmalade SDK. Thankfully the Marmalade SDK makes this pretty simple as you will soon see.

As usual if you just want the code then [grab it from here](http://www.drmop.com/wp-content/uploads/2011/09/Touch.zip "Marmalade SDK Touch and Multi-touch Tutorial Source Code"). if you want the details then read on. If you just want the details on how to use the CInput class then jump straight to the “Using CInput” section

Oh its worth noting that week by week I am going to keep updating the original Iw2D DrawSprite example with the systems that we put together. This week for example, I have added a new CInput class. I like to try and keep code for separate systems in their own neat little classes because a) it makes the code easier to understand b) you can strip out the code and use it as-is with no modification c) its good programming practice. I would ordinarily create systems such as CInput using a singleton, but for the sake of readability and so that you don’t have to run off finding out about the likes of singletons, I have simply declared the concrete version of CInput as a global (slap on the wrist for me, but whatever makes the code easier to understand)

Marmalade Events and Callbacks
------------------------------

The Marmalade pointer system handles screen touches using an event system. This means that when something happens to the pointer (the users finger or a stylus touches the phone or tablets screen for example) the system calls a function to handle it. The Marmalade SDK uses what are called **Callbacks** to implement this type of event notification system. A call back is basically a function that we define that gets called back by the Marmalade system when the event occurs. The system usually passes some parameters to the call back function to let us know what the heck it wants. Here’s a quick example:

```
> //
> // HandleSingleTouchButtonCB - The system will call this callback when the user moves their finger on the screen
> //
> void HandleSingleTouchButtonCB(s3ePointerEvent* event)
> {
>     // Please add code to do something about this important event
> }
```

In this example HandleSingleTouchButtonCB gets called by the system when the user touches the screen. Behold, the system has also provided us with some data about the event by way of a pointer to the s3ePointerEvent data type. We can now query this data to find out what the system wants to tell us.

Taking a quick look at the s3ePointerEvent data type we discover that there is some pretty interesting stuff in there that we can use:

```
> typedef struct s3ePointerTouchEvent
> {
>     /**
>      * ID of the touch. The ID given to a touch is equal to the number
>      * of simultaneous touches active at the time the touch began. This ID
>      * can be between 0 and S3E_POINTER_TOUCH_MAX-1 inclusive
>      */
>     uint32 m_TouchID;
>     /**  Whether the touch started (1) or ended (0).*/
>     uint32 m_Pressed;
>     /** Position X. */
>     int32  m_x;
>     /** Position Y. */
>     int32  m_y;
> } s3ePointerTouchEvent;
```

Ok, so some of you may be thinking, how do I tell the Marmalade SDK to use my callback when such an event does occur? Because call backs are so useful the Marmalade SDK has lots of function for registering a callback with the system. The one we are interested in for the pointer is called:

```
S3E_API s3eResult s3ePointerRegister(s3ePointerCallback cbid, s3eCallback fn, void* userData);
```

* cbid – This represents a constant that identifies which pointer event you would like to be notified about
* fn – The address of the callback function that you would like Marmalade to call when the event occurs

Here’s how we would set one up to listen for single touch events

```
s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)HandleSingleTouchButtonCB, NULL);
```

Now that is registered, whenever the system receives a screen touched event you will know about it as HandleSingleTouchButtonCB() will be called

Oh and be nice to the system and don’t forget to unregister the call back when you are done using it with, such as when you exit the game:

```
s3ePointerUnRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)HandleSingleTouchButtonCB);
```

Types of Marmalade Pointer Events
---------------------------------

The Marmalade SDk currently handles a number of pointer event types:

* Touch Event – Called when the user touches the screen (S3E\_POINTER\_BUTTON\_EVENT)
* Motion Event – Called when the user moves their finger or stylus on the screen (S3E\_POINTER\_MOTION\_EVENT)
* Multi-touch Touch Event – Called when the user touches the screen (S3E\_POINTER\_TOUCH\_EVENT)
* Multi-touch Motion Event – Called when the user moves their finger or stylus on the screen (S3E\_POINTER\_TOUCH\_MOTION\_EVENT)

To be a good Marmalade developer you should handle all four of these events

Handling the Four Marmalade Pointer Events
------------------------------------------

In the Input example (CInput.cpp) we have declared four call backs to handle all four events:

```
> //
> // HandleMultiTouchButtonCB - For multitouch devices the system will call this callback when the user touches the screen. This callback is called once for each screen touch
> //
> void HandleMultiTouchButtonCB(s3ePointerTouchEvent* event)
> {
>     // Check to see if the touch already exists
>     CTouch* touch = g_Input.findTouch(event->m_TouchID);
>     if (touch != NULL)
>     {
> 	// Yes it does, so update the touch information
>         touch->active = event->m_Pressed != 0;
>         touch->x = event->m_x;
>         touch->y = event->m_y;
>     }
> }
> //
> // HandleMultiTouchMotionCB - For multitouch devices the system will call this callback when the user moves their finger on the screen. This callback is called once for each screen touch
> //
> void HandleMultiTouchMotionCB(s3ePointerTouchMotionEvent* event)
> {
>     // Check to see if the touch already exists
>     CTouch* touch = g_Input.findTouch(event->m_TouchID);
>     if (touch != NULL)
>     {
> 	// Updates the touches positional information
>         touch->x = event->m_x;
>         touch->y = event->m_y;
>     }
> }
> //
> // HandleSingleTouchButtonCB - The system will call this callback when the user touches the screen
> //
> void HandleSingleTouchButtonCB(s3ePointerEvent* event)
> {
>     CTouch* touch = g_Input.getTouch(0);
>     touch->active = event->m_Pressed != 0;
>     touch->x = event->m_x;
>     touch->y = event->m_y;
> }
> //
> // HandleSingleTouchMotionCB - The system will call this callback when the user moves their finger on the screen
> //
> void HandleSingleTouchMotionCB(s3ePointerMotionEvent* event)
> {
>     CTouch* touch = g_Input.getTouch(0);
>     touch->x = event->m_x;
>     touch->y = event->m_y;
> }
```

Eik! I know, looks a bit messy, but callbacks usually do look a bit out of place. That said I do love callbacks!

The call back functions are very small and very simple. They basically pull the event data (pointer position and button status) and move them into a CTouch array inside the CInput class, where we can later access them in our code. Note the use of my nasty global concrete version of CInput g\_Input. Ordinarily I would use a singleton for stuff like this (mental note, topic for another blog)

Now that callbacks are more or less out of the way we will proceed with looking at the CInput class in a little more detail

The CInput Class
----------------

Firstly lets take a look at the CInput initialisation code:

```
> bool CInput::Init()
> {
>     // Check to see if the device that we are running on supports the pointer
>     Available = s3ePointerGetInt(S3E_POINTER_AVAILABLE) ? true : false;
>     if (!Available)
>         return false;    // No pointer support
> 
>     // Clear out the touches array
>     for (int t = 0; t < MAX_TOUCHES; t++)
>     {
>         Touches[t].active = false;
>         Touches[t].id = 0;
>     }
> 
>     // Determine if the device supports multi-touch
>     IsMultiTouch = s3ePointerGetInt(S3E_POINTER_MULTI_TOUCH_AVAILABLE) ? true : false;
> 
>     // For multi-touch devices we handle touch and motion events using different callbacks
>     if (IsMultiTouch)
>     {
>         s3ePointerRegister(S3E_POINTER_TOUCH_EVENT, (s3eCallback)HandleMultiTouchButtonCB, NULL);
>         s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, (s3eCallback)HandleMultiTouchMotionCB, NULL);
>     }
>     else
>     {
>         s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)HandleSingleTouchButtonCB, NULL);
>         s3ePointerRegister(S3E_POINTER_MOTION_EVENT, (s3eCallback)HandleSingleTouchMotionCB, NULL);
>     }
> 
>     return true; // Pointer support
> }
```

Like any good programmer we are checking to ensure that the pointer system is available on the device that we are running on, with so many different handsets out there, who knows if there are some with no pointer support?

```
> // Check to see if the device that we are eunning on supports the pointer
> Available = s3ePointerGetInt(S3E_POINTER_AVAILABLE) ? true : false;
```

Its better to know up front and inform the user that your game or app is not compatible with their phone because it does not support the pointer.

Next, we determine if the device supports multi-touch. Note that many Android phones and tablets do not support multi-touch, so you will have to think carefully about your game or apps design before targeting Android.

```
> // Determine if the device supports multi-touch
> IsMultiTouch = s3ePointerGetInt(S3E_POINTER_MULTI_TOUCH_AVAILABLE) ? true : false;
```

Lastly, we register two callbacks depending upon whether or not the device supports multi-touch:

```
> // For multi-touch devices we handle touch and motion events using different callbacks
> if (IsMultiTouch)
> {
>     s3ePointerRegister(S3E_POINTER_TOUCH_EVENT, (s3eCallback)HandleMultiTouchButtonCB, NULL);
>     s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, (s3eCallback)HandleMultiTouchMotionCB, NULL);
> }
> else
> {
>     s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)HandleSingleTouchButtonCB, NULL);
>     s3ePointerRegister(S3E_POINTER_MOTION_EVENT, (s3eCallback)HandleSingleTouchMotionCB, NULL);
> }
```

Ok, so now we have initialised the input system using g\_Input.Init(); we need to ensure that the pointer system gets regularly updated. To do that we call g\_Input.Update();

This method is very simple:

```
> void CInput::Update()
> {
>     // Update the pointer if it is available
>     if (Available)
>         s3ePointerUpdate();
> }
```

Update() simply calls the Marmalade SDK’s s3ePointerUpdate() function to update the pointer system and call our callbacks when pointer events occur. Note that this must be called every game frame, so ensure that its placed somewhere in your main loop (near the beginning if possible)

Using the Cinput Class
----------------------

If we now turn our attention towards Main.cpp, we can take a quick look at what has changed from DrawSprite\_Iw2D.

Well the first thing is the inclusion of the CInput.h header file.

Next we initialise the Cinput class:

```
> // Initialise the input system
> g_Input.Init();
```

Because the example supports multi-touch we create a few variables to hold the position of our two sprites so we can move them independently

```
> int        sprite1_pos_x = surface_width / 2;
> int        sprite1_pos_y = surface_height / 2;
> int        sprite2_pos_x = surface_width / 2;
> int        sprite2_pos_y = surface_height / 2;
```

In our main loop we update the sprites based on where the user touches the screen:

```
> // Update pointer system
> g_Input.Update();
> if (g_Input.getTouchCount() != 0)
> {
>     // Get the first touch position
>     CTouch* touch = g_Input.getTouch(0);
>     if (touch != NULL)
>     {
>         sprite1_pos_x = touch->x;
>         sprite1_pos_y = touch->y;
>         sprite2_pos_x = sprite1_pos_x;
>         sprite2_pos_y = sprite1_pos_y;
>     }
> 
>     // if multi-touch is available then move 2nd sprite to 2nd touch position
>     if (g_Input.isMultiTouch())
>     {
>         if (g_Input.getTouchCount() > 1)
>         {
>             touch = g_Input.getTouch(1);
>             if (touch != NULL)
>             {
>                 sprite2_pos_x = touch->x;
>                 sprite2_pos_y = touch->y;
>             }
>         }
>     }
> }
```

Ok, this bit of code is no longer a bit of code and looks a bit meaty. However, it is very simple to understand.

Firstly we check to see if there has been any touches by getting the touch count from CInput. If there are touches present then we get the first touch and set both sprites to the position of the touch. This will move both sprites to wherever the user taps the screen.

The second part checks to see if there has been more than one touch, if so then we get the 2nd touch and move the 2nd sprite to its position.

Note that I am just getting the touches by their index in the touches list and not by their ID. In a proper multi-touch system you should ideally track touches by their ID and not their index in the touches list. But for this example, this way suffices.

And finally we draw our sprites at their new positions:

```
> // Draw two sprites
> DrawSprite(image1, sprite1_pos_x, sprite1_pos_y, -sprite_angle, (iwfixed)(IW_GEOM_ONE * 2));
> DrawSprite(image2, sprite2_pos_x, sprite2_pos_y, sprite_angle, IW_GEOM_ONE);
```

Multi-touch Simulation using the Simulator
------------------------------------------

The Marmalade SDK simulator will allow you to simulate multi-touch functionality in your application but you firstly need to enable it. To enable this functionality you need to:

* Go to the simulator menu and select Configuration Pointer
* Tick “Report multi-touch available” and “enable multi-touch simulation mode”

Now that you have enabled multi-touch simulation you can use the middle mouse button to place touches. You can move the touches around by holding the middle mouse button down over the placed touch and move it. To remove a multi-touch touch, simply click the middle mouse button over the touch again.

Well that concludes this tutorial. I hope you all find it of some use. You can download the associated [touch code project from here](http://www.drmop.com/wp-content/uploads/2011/09/Touch.zip "Marmalade SDK Touch and Multi-touch Tutorial Source Code")

Happy coding and stay away from rickety old bridges!