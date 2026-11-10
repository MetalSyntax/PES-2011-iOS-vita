# Tutorial 6 – Handling key inputs and the on screen keyboard

Original URL: http://www.drmop.com/index.php/2011/09/28/marmalade-sdk-tutorial-handling-key-inputs-and-the-on-screen-keyboard/

This tutorial is part of the Marmalade SDK tutorials collection. To see the tutorials index [click here](http://www.drmop.com/?page_id=91 "Marmalade SDK tutorials index")

In today’s tutorial we are going to cover key input and on the on screen keyboard (OSK).

As usual if you just want the associated article source code then you can [Download it from here](http://www.drmop.com/wp-content/uploads/2011/09/Keys.zip). If you want the details on the updated CInput class then please feel free to read on.

Keys and Key States
-------------------

Marmalade maps device keys to an enum called s3eKey. if you take a look at the Marmalade SDK header file s3eKeyboard.h you will see that the list of mapped keys is quite extensive.

Marmalade uses key states to let us know what is currently happening to all the available keys on the device, for example we make queries such as “is the back key pressed” or “is the menu key up or down”. The following states are supported:

* S3E\_KEY\_STATE\_DOWN – The user is holding down the key
* S3E\_KEY\_STATE\_UP – The key is up and not being held down by the user
* S3E\_KEY\_STATE\_PRESSED – The user has just pressed their finger on the key
* S3E\_KEY\_STATE\_RELEASED – The user has just released the key

To query the state of a particular key we need to call s3eKeyboardGetState(key), which returns the states of the specified key. Note that its possible that a key can have more than one state. For example, if the user has just pressed the key then it would have both the S3E\_KEY\_STATE\_DOWN and the S3E\_KEY\_STATE\_PRESSED states.

On Screen Keyboard
------------------

The on screen keyboard is a software based keyboard that is available to phones and tablets that support a touch sensitive screen (You can call s3eOSReadStringAvailable() to determine if it is supported). You can bring up the OS specific on screen keyboard quite easily using Marmalade’s s3eOSReadStringUTF8() and s3eOSReadStringUTF8WithDefault(). These blocking functions display the operating system specific keyboard and then return any entered text. Its possible to suggest to the system (by passing a flag to these functions) what kind of keyboard you want the user to be presented with to cater for different types of input. The following types are available:

* No flags – Standard keyboard
* S3E\_OSREADSTRING\_FLAG\_PASSWORD – A password entry keyboard
* S3E\_OSREADSTRING\_FLAG\_EMAIL – An email address entry keyboard
* S3E\_OSREADSTRING\_FLAG\_URL – A URL entry keyboard
* S3E\_OSREADSTRING\_FLAG\_NUMBER – A number entry keyboard

CInput Changes
--------------

Ok, now that the basics are out the way lets take a look at the changes to the CInput class. If you haven’t read the previous article that introduced this class then you can [check it out here](http://www.drmop.com/?p=200 "Marmalade SDK Tutorial - Touch and Multi-touch")

If you take a look at CInput.h, you will notice a few major additions. The first few additions include header files required by the key and on screen keyboard systems:

```
> #include "s3eKeyboard.h"
> #include "s3eOSReadString.h"
```

Switching over to the CInput.cpp class source file you will notice that we have added a few extra checks into CInput::Init()

```
> // Check to see if the device that we are running on supports the keyboard
>     KeysAvailable = (s3eKeyboardGetInt(S3E_KEYBOARD_HAS_KEYPAD) || s3eKeyboardGetInt(S3E_KEYBOARD_HAS_ALPHA));
> 
>     // Check to see if the device that we are running on supports the on screen keyboard
>     OSKeyboardAvailable = s3eOSReadStringAvailable() == S3E_TRUE;
```

This code basically ensures that the modules we are wanting to use are available on the device that we are running on.

Next we add the following code to CInput::Update() to ensure that the system updates key states

```
> // Update key system if it is available
> if (KeysAvailable)
>     s3eKeyboardUpdate();
```

Lastly, we add code to query key states and show the on screen keyboard:

```
> bool CInput::isKeyDown(s3eKey key) const
> {
>     if (!KeysAvailable)
>         return false;
> 
>     // Return down state of queried key
>     return (s3eKeyboardGetState(key) & S3E_KEY_STATE_DOWN) == S3E_KEY_STATE_DOWN;
> }
> 
> bool CInput::isKeyUp(s3eKey key) const
> {
>     if (!KeysAvailable)
>         return false;
> 
>     // Return up state of queried key
>     return (s3eKeyboardGetState(key) & S3E_KEY_STATE_UP) == S3E_KEY_STATE_UP;
> }
> 
> bool CInput::wasKeyPressed(s3eKey key) const
> {
>     if (!KeysAvailable)
>         return false;
> 
>     // Return pressed state of queried key
>     return (s3eKeyboardGetState(key) & S3E_KEY_STATE_PRESSED) == S3E_KEY_STATE_PRESSED;
> }
> 
> bool CInput::wasKeyReleased(s3eKey key) const
> {
>     if (!KeysAvailable)
>         return false;
> 
>     // Return released state of queried key
>     return (s3eKeyboardGetState(key) & S3E_KEY_STATE_RELEASED) == S3E_KEY_STATE_RELEASED;
> }
> 
> const char* CInput::showOnScreenKeyboard(const char* prompt, int flags, const char* default_text)
> {
>     if (!OSKeyboardAvailable)
>         return NULL;
> 
>     // Show on screen keyboard and return the input string
>     if (default_text != NULL)
>         return s3eOSReadStringUTF8WithDefault(prompt, default_text, flags);
>     else
>         return s3eOSReadStringUTF8(prompt, flags);
> }
```

Using the new CInput features
-----------------------------

If we now turn our attention towards Main.cpp, we can take a quick look at what has changed from our previous Touch example.

Firstly note that we got rid of s3eKeyboardUpdate(); as this is now handled by g\_Input.Update()

We have also replaced the following code:

```
> if (s3eKeyboardGetState(s3eKeyAbsBSK) & S3E_KEY_STATE_DOWN)    // Back key is used to exit on some platforms
>     break;
```

With this code:

```
> if (g_Input.isKeyDown(s3eKeyAbsBSK))    // Back key is used to exit on some platforms
>     break;
```

Not a great deal but a little more readable.

Now, we add a check into our main loop to see if the user has tapped the top of the screen. If they have then we show the on screen keyboard

```
> // if user taps at top of screen then activate on screen keyboard
> if (touch->y < 50)
> {
>     // Show on screen keyboard then print out the returned text to the debug trace
>     const char* text = g_Input.showOnScreenKeyboard("Enter Text");
>     if (text != NULL)
>         s3eDebugOutputString(text);
> }
```

If the user enters some text then we display the entered text to the debug console using s3eDebugOutputString()

Handling Android Back and Menu Buttons
--------------------------------------

Its worth noting that we’ve come across a number of Android app stores that require special handling of home and menu buttons as part of their certification process. These buttons should be implemented as follows:

* Back Button – Navigate backwards in your application to the previous screen / menu
* Home Button – Bring up the in-app or in-game menu

Well that concludes this tutorial. I hope you all find it of some use. You can download the associated [Keys project source code from here](http://www.drmop.com/wp-content/uploads/2011/09/Keys.zip)

Happy coding and don’t sit on two legged chairs!