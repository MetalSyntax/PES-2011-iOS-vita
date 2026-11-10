# Tutorial 12 – Creating an Extensible Animation System

Original URL: http://www.drmop.com/index.php/2011/10/21/marmalade-sdk-tutorial-creating-an-extensible-animation-system/

This tutorial is part of the Marmalade SDK tutorials collection. To see the tutorials index [click here](http://www.drmop.com/index.php/marmalade-sdk-tutorials/ "Marmalade SDK Tutorials Index")

Had flu all week and now I’ve been afflicted with a chest infection so it’s been a pretty miserable week. At least the news of our game BattleBallz Chaos being shown at the Blackberry Devcon 2011 lifted the old spirits.

In the last tutorial we covered basic sprites and more to the point bitmapped sprites. We began to create a basic API called IwGame which will eventually serve as our game engine framework that we can all hopefully build cool games around (that is once we have weeded all of my bugs out 🙂 ). The idea this week is to get those sprites animating (aside from rotating, scaling and moving), we want the image of our sprite to change. My plan for this week was to build a very basic animation class that we could use to animate our bitmap frames. Well my intentions were good and I started out creating a basic image frame animation class but decided to scrap it and create a more extensive animation system that we can use for more than just image animations. If you just need the code then you can grab it from [here](http://www.drmop.com/wp-content/uploads/2011/10/IwGame.zip "IwGame Animation Source Code"), otherwise please feel free to read on.

What is an Animation System?
----------------------------

In the context of this article, an animation system is a bunch of classes that deal with animating specific components. These components could be anything from a simple one dimensional variable that represents speed or a two dimensional variable such as a vector that represents the players position to something a little more complex such as a rectangular area of space that represents a frame in an image animation

How could an animation system be useful to us? Well we could use it to animate the texture that appears on a sprite giving it personality (a face that smiles, cries or laughs for example), or we could animate an objects position and rotation to follow a specific path around the game world. We could even animation a list of instructions or game object states using a discrete animation to give the impression that the object is intelligent.

Here are a few terms that I use during this article:

* Animation – Refers to the complete animation including all its frames, its playback rate, callbacks etc..
* Frame – Refers to a single discrete component of the animation, this could be a particular image, position or velocity etc..
* Frame Index – Refers to the position of a particular animation frame in an array of animation frames

An Extensible Animation System – IwGameAnim
-------------------------------------------

Our new animation system IwGameAnim is built around the CIwGameAnim class. This class serves as a general purpose abstract class that we can derive custom animation types from. In this weeks code we have created 3 animation classes from CIwGameAnim:

* CIwGameAnimImage – A rectangular bases image animation
* CIWGameAnimFloat – A single floating point variable animation (useful for animating one dimensional components such as angle, speed and distance)
* CIwGameAnimFVec2 – A 2D floating point vector variable animation (useful for animating two dimensional components such as velocity and position)

Its important to note at this point that I have not fully tested this code yet. I only spent a few hours writing it and testing it (time constraints), but be certain that as we use more of the animation system in our tutorials it will get fully tested eventually.

We are not going to carry out a lot of in-depth analysis of the code this seek as i want to move the Marmalade SDK tutorials series tutorials along faster, believe it or not creating these tutorials takes up a fair amount of time (although I do thoroughly enjoy writing them). That said lets take a quick look at the CIwGameAnim class:

```
> class CIwGameAnim
> {
>     // Properties
> protected:
>     bool                Playing;                     // Current playing state
>     float               CurrentFrame;                // Current animation frame
>     float               PlaybackSpeed;               // Rate at which to playback animation
>     int                 FrameCount;                  // Total number of animation frames in the animation
>     bool                FrameHasChanged;             // Set to true when a frame has changed
>     bool                Looping;                     // True if the animation loops
>     int                 NumLoops;                    // Total number of loops played back so far
>     int                 MaxLoops;                    // Maximum number of loops before animation stops (-1 to play forever)
>     int                 PlayDelayMs;                 // Amount of time to wait before starting animation
>     CIwGameCallback     StartedCallback;             // Callback which is called when the animation starts playing
>     CIwGameCallback     StoppedCallback;             // Callback which is called when the animation stops playing
>     CIwGameCallback     LoopedCallback;              // Callback which is called when the animation loops
> public:
>     void                setPlaybackSpeed(float speed)                { PlaybackSpeed = speed; }
>     bool                hasFrameChanged() const                      { return FrameHasChanged; }
>     void                setPlayDelay(int delay_ms)                   { PlayDelayMs = delay_ms; }
>     void                setStartedCallback(CIwGameCallback callback) { StartedCallback = callback; }
>     void                setStoppedCallback(CIwGameCallback callback) { StoppedCallback = callback; }
>     void                setLoopedCallback(CIwGameCallback callback)  { LoopedCallback = callback; }
>     void                setLooped(int num_loops);
>     void                setCurrentFrame(float frame);
>     float               getCurrentFrame() const        { return CurrentFrame; }
>     void                Start()                        { Playing = true;  if (PlayDelayMs != 0) PlayDelay.setDuration(PlayDelayMs); }
>     void                Stop()                         { Playing = false; }
>     bool                isPlaying() const              { return Playing; }
>     void                Restart();
>     // Properties End
> protected:
>     CIwGameTimer        PlayDelay;                    // Timer used to time start of animation
> 
> public:
>     CIwGameAnim() :     Playing(false), CurrentFrame(0), PlaybackSpeed(0), FrameHasChanged(true), Looping(false), NumLoops(0), MaxLoops(0), PlayDelayMs(0),
>                         StartedCallback(NULL), StoppedCallback(NULL), LoopedCallback(NULL) {}
>     virtual ~CIwGameAnim() {}
> 
>     virtual void        Release() = 0;
>     virtual bool        Update(float dt);
> };
```

Below is a brief description of the classes properties:

* Playing – Marks the animation as playing or stopped
* CurrentFrame – The current animation frame index (think of the integer part of this variable as the frame number of the animation frame we are currently seeing)
* PlaybackSpeed – The rate at which to play back the animation, a value of  1.0f / davice\_frame\_rate would play the animation back at one animation frame per second (quite slowly in other words)
* FrameCount – Total number of frames in this animation
* FrameHasChanged – Marked true when the animation switches from one animation frame to the next
* Looping – True if this animation loops when it reaches the end
* NumLoops – The total number of times the animation has looped around
* MaxLoops – The maximum number of times the animation is allowed to loop before stopping (a value of -1 means loop forever)
* PlayDelayMs – This can be used to delay the animation playing at the start. This can be useful if you want to fire off a group of animations at the same time, but have them start at different times
* StartedCallback – This callback (that you supply) will be called when the animation starts to play
* StoppedCallback – This callback (that you supply) will be called when the animation stops playing naturally (not when Stop() is called)
* LoopedCallback – This callback (that you supply) is called each time the animation reaches its end and loops to the start again

You never directly create an animation from this class, instead you can derive your own animation type from this class or use one of those already provided.

Bitmap Image Animation – IwGameAnimImage
----------------------------------------

Image animation in our game engine involves displaying different image frames one after the other to give the impression of giving personality to our sprite based objects. We store all of our animations on a sprite atlas (also known as a sprite sheet), which looks something like this:

[![Example sprite atlas / sheet](http://www.drmop.com/wp-content/uploads/2011/10/Image_Atlas.png "Example sprite atlas / sheet")](http://www.drmop.com/wp-content/uploads/2011/10/Image_Atlas.png)

Example sprite atlas / sheet

   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
 

A sprite atlas is basically a large image that contains a group of other images. To create an image animation we simply display different portions of the sprite atlas.

Bitmap animations are handled by the CIwGameAnimImage class. The CIwGameAnimImage class looks like this:

```
> class CIwGameAnimImage : public CIwGameAnim
> {
> protected:
>     CIwGameAnimImageFrame*  Frames;                // This class takes over management of Frames memory
> public:
>     CIwGameAnimImage() :    CIwGameAnim(), Frames(NULL) {}
>     virtual ~CIwGameAnimImage()    { Release(); }
> 
>     void                    Release();
>     CIwGameAnimImageFrame*  getCurrentImageFrame() const { if (CurrentFrame >= 0) return Frames + (int)CurrentFrame; return NULL; }
>     void                    setFrameData(CIwGameAnimImageFrame* frames, int count) { Frames = frames; FrameCount = count; }
> };
```

The class itself is very basic, it allows you to set the frame data and number of frames using setFrameData() and get the current frame using getCurrentImageFrame(). We will take a look how to use this class a little later on.

CIWGameAnimFloat and CIwGameAnimFVec2 will be discussed in more depth in a separate article that deals with animating in-game objects.

Managing Animation Frames – CIwGameAnimFrameManager
---------------------------------------------------

Once we begin building our game we are going to end up with many animations of a variety of types, animating images, paths for objects, spinning slick user interfaces and so on. It’s usually a good idea to have some class that can oversee the creation and tracking of this kind of information. In this case we use the CIwGameAnimFrameManager to help us create blocks of animation frames

This class provides us with the following functions for allocating blocks of frames:

float\* allocFloatFrames(int count);  
CIwFVec2\* allocFVec2Frames(int count);  
CIwGameAnimImageFrame\* allocImageFrames(int count);  
CIwGameAnimImageFrame\* allocImageFrames(int count, int frame\_w, int frame\_h, int start\_x, int start\_y, int pitch\_x, int pitch\_y, int image\_width);

The second version of allocImageFrames() is a helper method that will auto-generate a group of image animation frames for a sprite atlas. The parameters are:

* count – Number of frames to generate
* frame\_w, frame\_h – Width and height of each animation frame
* start\_x, start\_y – Upper left hand corner of first animation frame
* pitch\_x, pitch)y – The horizontal and vertical spacing between frames
* image\_width – Width of the sprite atlas

Managing Animations – CIwGameAnimManager
----------------------------------------

It would be great to be able to just create a bunch of animations and have something to automatically update them all for us without having to track each one individually, this is where CIwGameAnimManager comes in. CIwGameAnimManager allows us to add a bunch of animations to it and the manager will manage the updating and cleaning up of the animations and all of their frames for us.

It would also be cool if we had an object that had a collection of different animations that it could play to depict it doing different actions, such as running, walking, sleeping, falling etc.. CIwGameAnimManager allows us to add all of these animations and then select which animation is the currently active animation. Switching animations will stop the current animation and set the next one off playing.

The default behaviour of a freshly created CIwGameAnimManager object is to update all animations within the manager and not just the currently playing animation. To switch the manager into tracking and playing only the current animation call setUpdateAll(false).

What’s changed in the example code
----------------------------------

Note that our project is now called IwGame and future game engine examples will retain this name.

If you build and run the IwGame project you will see 100 sprites spinning around on the screen, but the difference will be that the sprite is of a face animating manically,.

[![IwGame Marmalade SDK Tutorial Example](http://www.drmop.com/wp-content/uploads/2011/10/Animation_Example.png "IwGame Marmalade SDK Tutorial Example")](http://www.drmop.com/wp-content/uploads/2011/10/Animation_Example.png)

IwGame Marmalade SDK Tutorial Example

   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
 

In terms of assets, we now have a single sprite atlas containing all of our sprites called sprites.png located in the data folder. We have also updated our Leve1l.group group file to include just the sprites.png file into our deployment archive.

We have added four new files to our MKB file:

* IwGameAnim.cpp
* IwGameAnim.h
* IwGameUtil.cpp
* IwGameUtil.h

IwGameAnim contains our new animation classes, whilst IwGameUtil contains a collection of utility classes, types and constants. The only class we really use from IwGameUtil is the CIwGameTimer, which is a software based polled timer that allows us to time events. We use this to delay the start of animations in the animation system. We also use the CIwGameCallback to define a callback function, although we are not using callbacks in this example, we will use them in the future to activate events such as sound playback

And without further ado, lets take a quick look at Main.cpp to see whats changed:

The first thing is the inclusion of the IwGameAnim.h header file

The next thing is the removal of loading test1 and test2 resources and the addition of loading our sprite atlas sprites.png instead:

```
> CIw2DImage* sprites_image = Iw2DCreateImageResource("sprites");
```

The next change is the creation of our animation frames and the animation itself:

```
> // Create an image frame manager (manages allocation of our animation frames)
> CIwGameAnimFrameManager* anim_frame_manager = new CIwGameAnimFrameManager();
> // Auto create a 8 frames of animation 36x40 pixels in size
> CIwGameAnimImageFrame* anim_frames = anim_frame_manager->allocImageFrames(8, 36, 40, 0, 0, 512, 40, 512);
> // Create and set up our face animation
> CIwGameAnimImage* face_anim = new CIwGameAnimImage();
> face_anim->setFrameData(anim_frames, 8);
> face_anim->setLooped(-1);
> face_anim->setPlaybackSpeed(-0.1f);
> face_anim->Start();
```

Its not a complex piece of code, just a bit wordy. Firstly we create an animation frame manager which in turn lets us create actual animation frames. In this case we use the helper method allocImageFrames() to generate the 8 frames manic head animation from our sprite atlas.

Next we create the actual image animation, set its animation frames, tell it to loop forever and then finally set it off playing.

The next change is a bit sneaky, I had to make a few upgrades to the IwGameSprite class to allow it to render portions of a larger image, more on that in a sec. In our loop where we create 100 sprites we now set the destination size (on screen size with no scaling) explicitly:

```
> sprite->setImage(sprites_image);
> sprite->setDestSize(36, 40);
```

We also only have to assign one texture to every sprite (this will speed up rendering as texture swapping no longer occurs when drawing two sprites with different textures)

If we now move to the main loop and take a look at our sprite update code:

```
> for (CIwGameSpriteManager::Iterator it = sprite_manager->begin(); it != sprite_manager->end(); ++it, speed++)
> {
>     // Set sprites rotation
>     (*it)->setAngle((*it)->getAngle() + speed);
> 
>     // Set sprites source rectangle from its current animation frame
>     CIwGameBitmapSprite* sprite = (CIwGameBitmapSprite*)*it;
>     sprite->setSrcRect(anim_frame->x, anim_frame->y, anim_frame->w, anim_frame->h);
> }
```

You will notice that we now set the sprites image source rectangle (this is the rectangular area of the source image that will be displayed on the sprite), this data is pulled directly from the animation system for the animations current frame.

We then update our animation:

```
> // Update animation
> face_anim->Update(1.0f);
```

Note that we didn’t actually place our animation inside a CIwGameAnimManager, so we call the animations Update() method manually.

And finally outside our main loop we clean up our animation data using:

```
> // Cleanup animations
> if (face_anim != NULL)
>     delete face_anim;
> if (anim_frame_manager != NULL)
>     delete anim_frame_manager;
```

The animation system is a chunky bit of code that may take a while to absorb for some of you at the moment, but it will become apparent over the coming weeks just how to utilise it as we plan on using it quite a lot.

Changes to IwGameSprite
-----------------------

I found whilst adding the animation system into the latest example code that our robust sprite class needed to be a little more robust. CIwGameBitmapSprite needed support for drawing areas of of images rather than the whole image. We replaced the previous rendering code with:

```
> int x = -(Width / 2);
> 	int y = -(Height / 2);
> 	Iw2DDrawImageRegion(Image, CIwSVec2(x, y), CIwSVec2(Width, Height), CIwSVec2(SrcX, SrcY), CIwSVec2(SrcWidth, SrcHeight));
```

This code calls the Marmalade SDK Iw2D function Iw2DDrawImageRegion() which allows us to draw a rectangular portion of a source image instead of the whole thing.

Note that we use absolutely no object pooling in any of these classes, but we will be adding it in the near future. I left it for now because the code is already quite complex in its current state.

Well that’s it for this tutorial. Our next tutorial will introduce Scenes and Actors where we get to tie our animation and sprites systems together into a core game engine that we can make some cool games with.

You can download the code that accompanies this article [from here](http://www.drmop.com/wp-content/uploads/2011/10/IwGame.zip "IwGame Animation Example Source Code").

Hope you all find this blog useful and until next time, don’t forget that…., hmmm I forgot!