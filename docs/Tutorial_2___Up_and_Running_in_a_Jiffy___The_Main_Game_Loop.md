# Tutorial 2 – Up and Running in a Jiffy – The Main Game Loop

Original URL: http://www.drmop.com/index.php/2011/09/16/marmalade-sdk-up-and-running-in-a-jiffy-the-main-game-loop/

This tutorial is part of the Marmalade SDK tutorials collection. To see the tutorials index [click here](http://www.drmop.com/?page_id=91 "Marmalade SDK tutorials index")

Ok, so you downloaded and installed the Marmalade SDK, took a quick scan over the help files and thought “Hmm, this is a biggy, where do I start?” When we start something new we all feel like complete and utter noobs. Al you want to do is get a “hello world” application up and running so you can have some kind of sense of achievement and feel comfortable with the process of updating code, compiling it and seeing it actually working. With this in mind, this tutorial will help you accomplish just that. On the other hand, if you just want the source code to the tutorial then you can [download it here](http://www.drmop.com/wp-content/uploads/2011/09/BasicMKB.zip "Marmalade SDK Basic MKB Tutorial Source Code").

By the way, if you haven’t already done so then [http://www.madewithmarmalade.com/download](ttps://www.madewithmarmalade.com/download "download a free version of of the Marmalade SDK") and grab a copy of the free trial version of the Marmalade SDK and install it.

We will begin with a few Marmalade basics so you don’t get lost during the tutorial.

**Project files (MKB’s)**
-------------------------

To begin with, Marmalade uses the concept of project files to organise your source files, data files and deployment settings. The MKB file is basically a text file that when opened up will generate a Visual Studio or XCode project containing your source files etc.

The MKB file is split into a number of sections, each with a heading name followed by curly braces containing the sections data. Here’s an example of a section:

```
> subprojects
> {
>     iw2d
> }
```

For now you only need to be concerned with the following basic sections of the MKB:

* options – Options to pass to the build system
* includepath – Tells marmalade where to look for your header files
* subprojects – This section tells Marmalade which parts of the SDK you would like to use. Here you put the names of the SDK parts you would like to access from your code, for example, the 2D API is called Iw2D
* files – This section tells Marmalade which editable files you would like including in your generated project, such as source files, XML files, configuration files etc..
* assets – This section tells Marmalade which assets you would like to include when deploying your app or game, assets include files such as bitmaps, audio files, meshes, fonts, data files etc.. You can define assets in groups if you need a particular set of assets specific platforms
* deployments – This section allows you to define certain parameters on a per target platform basis. We won’t be covering this section in this tutorial but its worth mentioning.

Ok, now we have explained a few things about MKB files, here is a very basic one to get you started:

```
> #!/usr/bin/env mkb
> options
> {
> }
> 
> subprojects
> {
>     iw2d
> }
> 
> includepath
> {
>     ./source
> }
> 
> files
> {
>     [Source]
>     (source)
>     Main.cpp
> }
> 
> assets
> {
> }
```

To generate a project from this file you need to:

* Create a folder on your hard drive, lets call it BasicMKB
* Create a text file called BasicMKB.mkb and add the above lines to it
* Create a sub folders called source
* Add a Main.cpp file to the source folder
* Double click the BasicMKB.mkb file to have the Marmalade SDK generate your Visual Studio or XCode project

Notes:

* Marmalade will generate a data folder containing two .icf files and a build folder, you do not need to be concerned with these for the time being.
* You should not add additional source files via XCode or Visual Studio. instead, edit the MKB file, add your source files then re-launch the MKB file regenerate the project.

Now we are finally have the basics of creating a Marmalade SDK project out of the way, we now need something to compile and run.

**A very basic game loop**
--------------------------

In this section we will cover creating a very basic game loop that checks for the user quitting and clears the screen.

Ok, re-using the previous example, you need to edit the Main.cpp file that you created and add the following code:

```
> // Marmalade headers
> #include "s3e.h"
> #include "Iw2D.h"
> #include "IwGx.h"
> 
> int main()
> {
>     // Initialise Marmalade graphics system and Iw2D module
>     IwGxInit();
>     Iw2DInit();
> 
>     // Set the default background clear colour
>     IwGxSetColClear(0x40, 0x40, 0x40, 0);
> 
>     // Main Game Loop
>     while (!s3eDeviceCheckQuitRequest())
>     {
>         // Update keyboard system
>         s3eKeyboardUpdate();
>         if (s3eKeyboardGetState(s3eKeyAbsBSK) & S3E_KEY_STATE_DOWN)    // Back key is used to exit on some platforms
>             break;
> 
>         // Update pointer system
>         s3ePointerUpdate();
> 
>         // Clear the screen
>         IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);
> 
>         // Update the game
> 
>         // Render the games view
> 
>         // Show the surface
>         Iw2DSurfaceShow();
> 
>         // Yield to the operating system
>         s3eDeviceYield(0);
>     }
> 
>     // Shut down Marmalade graphics system and the Iw2D module
>     Iw2DTerminate();
>     IwGxTerminate();
> 
>     return 0;
> }
```

You can download a zip archive containing the project MKB and source code [from here](http://www.drmop.com/wp-content/uploads/2011/09/BasicMKB.zip "Marmalade SDK Basic MKB Tutorial Source Code")

If you build and run this example you will be presented with a grey screen. yes I know its a bit boring, but we will cover drawing sprites and other interesting topics very soon.