# Tutorial 3 – Loading bitmaps and rendering sprites with Iw2D

Original URL: http://www.drmop.com/index.php/2011/09/16/marmalade-sdk-tutorial-loading-bitmaps-and-rendering-sprites-with-iw2d/

This tutorial is part of the Marmalade SDK tutorials collection. To see the tutorials index [click here](http://www.drmop.com/?page_id=91 "Marmalade SDK tutorials index")

Carrying on from my previous Marmalade SDK tutorial that covered creating a basic game loop, I promised that I would move onto something a little more interesting. Well I was going to write this some other day, but I wanted to get it out of the way. As usual if your here just for the code then you can [download it from here](http://www.drmop.com/wp-content/uploads/2011/09/DrawSprite_Iw2D.zip "DrawSprite Marmalade SDK Iw2D tutorial source code")

Ok, in this tutorial we are going to cover drawing a sprite using Marmalade’s Iw2D module. if you do not know what Iw2D is then its Marmalade’s simple 2D drawing API that you can use for drawing a whole host of simple 2D objects, including:

* Lines
* Arcs
* Rectangles
* Polygons
* Images
* Text

Primitives can be rotated, translated, scaled and flipped. You can also set the colour and opacity.

Its worth noting at this point that Iw2D is context (or state) based (like Open GL). which means that if you modify a parameter within the context, such as a transform or a colour then it will be applied to all rendered primitives until you change the parameter again. So for example, if you set the current colour to red using Iw2DSetColour(0xff0000ff) then all primitives will have a red tint until you change the colour.

Ok, so lets get to some code. I put together some quick sprite rendering code that renders two sprites using rotation, scaling and translation:

```
> // Marmalade headers
> #include "s3e.h"
> #include "Iw2D.h"
> #include "IwGx.h"
> 
> void DrawSprite(CIw2DImage* image, int at_pos_x, int at_pos_y, iwangle angle = 0, iwfixed scale = IW_GEOM_ONE)
> {
>     // Set the rotation transform
>     CIwMat2D m;
>     m.SetRot(angle);
> 
>     // Scale the transform
>     m.ScaleRot(scale);
> 
>     // Translate the transform
>     m.SetTrans(CIwSVec2(at_pos_x, at_pos_y));
> 
>     // Set this transform as the active transform for Iw2D
>     Iw2DSetTransformMatrix(m);
> 
>     // Render the sprite
>     int x = -(image->GetWidth() / 2);
>     int y = -(image->GetHeight() / 2);
>     Iw2DDrawImage(image, CIwSVec2(x, y));
> }
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
>     // Create two images from PNG files
>     CIw2DImage* image1 = Iw2DCreateImage("test1.png");
>     CIw2DImage* image2 = Iw2DCreateImage("test2.png");
> 
>     // Get the display surface width and height
>     int surface_width = Iw2DGetSurfaceWidth();
>     int surface_height = Iw2DGetSurfaceHeight();
> 
>     // Dynamic variables
>     iwangle angle = 0;
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
>         // Draw two sprites
>         DrawSprite(image1, surface_width / 2, surface_height / 2, -angle, (iwfixed)(IW_GEOM_ONE * 2));
>         DrawSprite(image2, surface_width / 2, surface_height / 2, angle, IW_GEOM_ONE);
> 
>         // Spin our awesome sprite
>         angle += IW_ANGLE_2PI / 60;
> 
>         // Show the surface
>         Iw2DSurfaceShow();
> 
>         // Yield to the opearting system
>         s3eDeviceYield(0);
>     }
> 
>     // Safely cleanup images
>     if (image2 != NULL)
>         delete image2;
>     if (image1 != NULL)
>         delete image1;
> 
>     // Shut down Marmalade graphics system and the Iw2D module
>     Iw2DTerminate();
>     IwGxTerminate();
> 
>     return 0;
> }
```

The main() function code looks pretty much the same as in our BasicMKB example except for a few additions (marked in bold). The additions are:

```
> // Create two images from PNG files
> CIw2DImage* image1 = Iw2DCreateImage("test1.png");
> CIw2DImage* image2 = Iw2DCreateImage("test2.png");
```

These two lines basically create two images from PNG files that I have added to our projects MKB file. if you open up the DrawSprite\_Iw2D.mkb file and take a look, you will notice that the following two lines have appeared in the assets section:

test1.png  
test2.png

This instructs Marmalade to include these files when we deploy our app or game to a phone, tablet or TV

Its worth noting at this point that if I had forgotten to add these two lines then my awesome sprites would not have been included in the build when I go to deploy it to an actual device, but it would still work on the emulator no problem (I’ve fallen for this many times)

We also added new code to get the width and height of the drawing surface that we are going to draw our sprites to using:

```
> // Get the display surface width and height
> int surface_width = Iw2DGetSurfaceWidth();
> int surface_height = Iw2DGetSurfaceHeight();
```

We will later use these values to centre our sprite on the display because it looks much nicer than being stuck in the top left hand corner.

Because we want our sprites to do something a little more interesting than just sitting there in the middle of the screen, we are going to spin them (spinning stuff usually impresses people). In order to do that we need track an angle and change it every game frame.

```
> // Dynamic variables
> iwangle angle = 0;
```

We create our angle variable of type iwangle. You could just use int if you like but, then the variable would lose some of its meaning. Angles in Marmalade are integer and the range of 360 degrees is defined by the constant IW\_ANGLE\_2PI

In the main loop we use the following two lines of code to render our sprites:

```
> // Draw two sprites
> DrawSprite(image1, surface_width / 2, surface_height / 2, -angle, (iwfixed)(IW_GEOM_ONE * 2));
> DrawSprite(image2, surface_width / 2, surface_height / 2, angle, IW_GEOM_ONE);
```

I decided to stick all of the sprite rendering code into a nice easy to use re-usable function so that you just whip off with it and use it. It also keeps my code nice and readable.

Lastly, we update the angle of the sprites using:

```
> // Spin our awesome sprite
> angle += IW_ANGLE_2PI / 60;
```

This spins our sprites 6 degrees every game frame (pretty fast in other words)

Ok, now those bits are out of the way, we will move onto the DrawSprite() function. Unfortunately, you do bump into matrices at this point, if you do not know what they are then do not worry as long as you know how to use them. In a nutshell, A matrix is a mathematical object that allows you to transform a set of coordinates. In our case we are using it to rotate, scale and then translate (move) the coordinates of our sprite. The important parts of code in DrawSprite() that I want to point out are:

```
> // Set this transform as the active transform for Iw2D
> Iw2DSetTransformMatrix(m);
```

Here we tell Iw2D to set the “current” transform to use when drawing “anything”. Remember that Iw2D is context based and this transform will be remembered, so if you attempt to draw something else without again changing the transform then it will be drawn in the same position, at the same angle and at the same scale. Note that you can reset the current transform back to normal using Iw2DSetTransformMatrix(CIwMat2D::g\_Identity). The identity matrix is the default transformation matrix that has no rotation, no translation and no scaling.

```
> // Render the sprite
> int x = -(image->GetWidth() / 2);
> int y = -(image->GetHeight() / 2);
> Iw2DDrawImage(image, CIwSVec2(x, y));
```

This section of code draws our sprite on screen. Note that we offset the sprite up and to the left by half its height and width so that it is positioned using its centre point rather than its top-left hand corner. This allows out sprite to be rotated and scaled around its centre.

The final result should look something l like this:

[![DrawSprite example shot](http://www.drmop.com/wp-content/uploads/2011/09/DrawSprite_Shot.png "DrawSprite_Shot")](http://www.drmop.com/wp-content/uploads/2011/09/DrawSprite_Shot.png)

DrawSprite screenshot for Marmalade SDK Iw2D Tutorial

Like anything to do with game development you will eventually come to the point where you need to squeeze every last drop of power out of your rendering engine. Each time you render a different image a new state change is sent to the underlying hardware (this is not efficient by any stretch of the imagination). To help you, Marmalade built an automated batching system into Iw2D so that multiple draw calls using the same image are collected together and batched. Batched is a term used to describe sticking lots of data together and sending it to the hardware in one go. This process is not totally automated however, you will need to draw images by group. For example, lets say you have 10 trees and 5 balls in your scene. The most optimal way would be to draw all of the trees then all of the balls, not 2 trees, 1 ball, 2 trees, 2 balls etc..The astute among you will have noticed a simple problem with batching and that is the problem of draw order. Sometimes it is not practical to draw all trees then all balls because sometimes a ball may appear behind a particular tree.

So how do we solve this problem other than using a z-buffer, which I believe is not supported by Iw2D? The solution is to arrange the ball and tree image on a single bitmap (usually called a sprite atlas or sprite sheet). When you want to draw the tree, you simply render the portion of the image that represents the tree and when you want to draw a ball, you simply render the portion of the image that represents the ball. This way, you are always rendering the same image (albeit different potions of the image) and never have to worry about batching optimisations.

As usual you can download the source code for this tutorial from [here](http://www.drmop.com/wp-content/uploads/2011/09/DrawSprite_Iw2D.zip "DrawSprite Marmalade SDK Iw2D tutorial source code")

Happy coding and until next time, be careful crossing roads!