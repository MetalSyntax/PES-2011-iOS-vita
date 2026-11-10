# Tutorial 14 – Turning IwGame into a Real Game Engine

Original URL: http://www.drmop.com/index.php/2011/11/04/marmalade-sdk-tutorial-turning-iwgame-into-a-real-game-engine/

This tutorial is part of the Marmalade SDK tutorials collection. To see the tutorials index [click here](http://www.drmop.com/index.php/marmalade-sdk-tutorials/ "Marmalade SDK Tutorials")

In our previous tutorial we created created an actor, scene and camera system to automate the handling of game objects for us as well as allow us to create different types of actors derived from a common actor base. This week we are going to take the previous IwGame code base and turn it into a proper game engine. If you just want the code to this tutorial then [download it from here](http://www.drmop.com/wp-content/uploads/2011/11/IwGame_v0.200.zip "IwGame Engine Source Code").

Cleaning up the Mess
--------------------

If you take a look at last weeks Main.cpp, you may agree that it just looks static, messy and not very readable. This week we are going to clean up our act and turn IwGame into a real usable game engine. Lets take a quick look at our new Main.cpp to see how much tidying up has been done:

```
> int main()
> {
>     // Init Game
>     Game::Create();
>     GAME->Init();
> 
>     // Main Game Loop
>     while (!s3eDeviceCheckQuitRequest())
>     {
>         // Update the game
>         if (!GAME->Update())
>             break;
> 
>         // Check for back button quit
>         if (IW_GAME_INPUT->isKeyDown(s3eKeyAbsBSK))
>             return false;
> 
>         // Draw the scene
>         GAME->Draw();
>     }
> 
>     GAME->Release();
>     Game::Destroy();
> 
>     return 0;
> }
```

Wow, ok, where’s everything gone? our game has been whittled down to initialisation, update, draw and clean-up. Well not quite, the code has gone somewhere, some of it has been integrated into the IwGame engine whilst the game specific code has been moved into a proper game class. Remember last week, we created scenes to manage actors? Well this week we have created a CIwGame class to manage scenes.

IwGame Class for Managing our Game
----------------------------------

if you check out the engine code you will notice some new files:

IwGame.cpp  
IwGame.h  
IwGameImage.cpp  
IwGameImage.h

Lets take a quick look at the CIwGame class:

```
> class CIwGame
> {
> public:
>     // Public access for scene iteration
>     typedef CIwList::iterator    _Iterator;
>     _Iterator                    begin() { return Scenes.begin(); }
>     _Iterator                    end()   { return Scenes.end(); }
> 
> protected:
>     //// Properties
>     CIwGameScene*           CurrentScene;        // Scene that has current input focus
>     CIwGameScene*           NextScene;           // Scene that we wish to switch focus to
>     CIwList<CIwGameScene*>  Scenes;              // A collection of game scenes
> 
> public:
>     void                    addScene(CIwGameScene *scene);
>     void                    removeScene(CIwGameScene* scene);
>     void                    removeScene(unsigned int name_hash);
>     CIwGameScene*           findScene(unsigned int name_hash);
>     CIwGameScene*           findScene(const char* name);
>     CIwGameScene*           getScene(int index);
>     void                    clearScenes();
>     void                    changeScene(CIwGameScene *new_scene);
>     bool                    changeScene(unsigned int name_hash);
>     //// Properties end
> 
> protected:
>     uint64                  LastFrameTime;        // The time at which the last frame ended
> 
> public:
>     virtual void            Init();
>     virtual void            Release();
>     virtual bool            Update();
>     virtual void            Draw();
>     virtual void            Save() {}
>     virtual void            Load() {}
> 
> private:
> public:
>     void                    SetBackgroundColour(uint8 r, uint8 g, uint8 b, uint8 a);
> };
```

As you can see its basically a class that allows us to add / remove and change scenes. It also provides initialisation, release, update, drawing and save / load functionality.

These methods represent the most basic functionality that I believe any game would need. If you take a look at IwGame.cpp you will notice that there is quite a lot of code in there. Lets take a quick look at what CIwGame::Init() does:

CIwGame’s Init() method carries out some basic Marmalade SDK and game specific initialisation:

```
> void IwGame::Init()
> {
>     CurrentScene = NULL;
>     NextScene = NULL;
> 
>     // Initialise Marmalade SDK graphics system and Iw2D module
>     IwGxInit();
>     Iw2DInit();
> 
>     // Initialise the input system
>     CIwGameInput::Create();
>     IW_GAME_INPUT->Init();
> 
>     // Init IwSound
>     IwSoundInit();
> 
>     // Initialise Marmalade SDK resource manager
>     IwResManagerInit();
> 
> #ifdef IW_BUILD_RESOURCES
>     // Tell resource system how to convert WAV files
>     IwGetResManager()->AddHandler(new CIwResHandlerWAV);
> #endif
> 
>     // Set default background colour
>     SetBackgroundColour(0, 0, 0, 0);
> 
>     // Get initial time stamp
>     LastFrameTime = s3eTimerGetMs();
> }
```

You will notice that much of this code was present in our previous tutorials Main.cpp initialisation code. We have moved it into our CIwGame base so that we have some basic initialisation code that allows us to get our games up and running quickly.

Similarly our CIwGame::Release() method:

```
> void IwGame::Release()
> {
>     // Clean up scenes
>     for (_Iterator it = Scenes.begin(); it != Scenes.end(); ++it)
>         delete *it;
>     Scenes.clear();
> 
>     // Shut down the input system
>     IW_GAME_INPUT->Release();
>     CIwGameInput::Destroy();
> 
>     // Shut down the resource manager
>     IwResManagerTerminate();
> 
>     // Shutdown IwSound
>     IwSoundTerminate();
> 
>     // Shut down Marmalade graphics system and the Iw2D module
>     Iw2DTerminate();
>     IwGxTerminate();
> }
```

The only thing new here is the recursive release of all of the attached scenes

The last method I am going to look at is our CIwGame::Update() method

```
> bool IwGame::Update()
> {
>     // Calculate how long the last game frame took (capped at 60 and 10 fps) - We use this to scale all of our transient variables that rely
>     // upon time so that everything movess at the same rate regardless of our devices frame rate
>     float dt = (float)(s3eTimerGetMs() - LastFrameTime) / 16.67f;
>     if (dt < 1.0) dt = 1.0f;     if (dt > 6.0f) dt = 6.0f;
>     LastFrameTime = s3eTimerGetMs();
> 
>     // Clear the screen
>     IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);
> 
>     // Update pointer system
>     IW_GAME_INPUT->Update();
> 
>     // Update Iw Sound Manager
>     IwGetSoundManager()->Update();
> 
>     // Check for scene change
>     if (NextScene != CurrentScene)
>     {
>         // Notify scenes that there is a change of circumstances
>         if (CurrentScene != NextScene)
>         {
>             if (CurrentScene != NULL)
>             {
>                 CurrentScene->NotifyLostFocus(NextScene);
>                 if (CurrentScene->getAllowSuspend())
>                     CurrentScene->NotifySuspending(NextScene);
>             }
>             if (NextScene != NULL)
>             {
>                 NextScene->NotifyGainedFocus(CurrentScene);
>                 if (NextScene->getAllowSuspend())
>                     NextScene->NotifyResuming(CurrentScene);
>             }
>             CurrentScene = NextScene;
>         }
>     }
> 
>     // Update all scenes that are not suspended
>     for (_Iterator it = Scenes.begin(); it != Scenes.end(); ++it)
>     {
>         if (CurrentScene == *it)
>         {
>             (*it)->Update(dt);
>         }
>         else
>         {
>             if (!(*it)->getAllowSuspend())
>             {
>                 (*it)->Update(dt);
>             }
>         }
>     }
> 
>     return true;
> }
```

Update() basically does most of what we were doing in our previous tutorials main loop, except that we use the concept of a “current scene”. Because scenes can potentially be overlaid over one another and we do not want to always be processing all scenes within our game, we use the concept of a current scene which has the focus of the player.

Now that we have the ability to change scenes, we ideally need a mechanism for notifying the other scenes that they have been switched to or switched away from, allowing our scenes to carry out processing specific to handle such events.

Another important addition to our game update loop is the measurement of time. At the start of Update() we calculate how much time has passed since Update() was last called. We can use this value to scale all of our transient variables (animations, movement etc..) within our game to ensure that they move in a consistent manner regardless of variations in frame rate. The variable dt holds our scaling factor, which is how much we need scale our transient variables by to ensure that they remain in step with our frame rate. If we did not handle time within our game then animations and movement / spinning etc.. would slow down when our games frame rate drops and increase when our frame rate increases.

Building Our Own Game Class
---------------------------

The idea of the CIwGame class is to provide the most basic functionality required to initialise, clean-up and update / draw our game. In order to create a real game we should derive our own class from CIwGame and add our own game specific code, not forgetting to call the CIwGame base methods so we do not miss out on our already implemented functionality.

In this case we create a class called Game.cpp which is a singleton class that derives from CIwGame and implements Init(), Release(), Update() and Draw() methods. if you take a quick look at Game.cpp you will notice that the game specific code from our old Main.cpp has been moved here.

We also go a step further and remove our dependency on storing references to scenes, images and animations etc, instead opting for finding them within their respective managers as and when needed. For example:

In Game::Update() we have:

```
> CIwGameScene* scene = findScene("GameScene");
> if (scene != NULL)
> {
>     // Get tapped position in our virtual screen space
>     CIwFVec2 pos = scene->ScreenToVirtual(IW_GAME_INPUT->getTouch(0)->x, IW_GAME_INPUT->getTouch(0)->y);
>     // Find our sprite atlas
>     CIwGameImage* image = scene->getImageManager()->findImage("sprites");
>     // Find our manic face animation frames
>     CIwGameAnimImageFrame* anim = scene->getAnimFrameManager()->getImageFrames(0);
> 
>     // Create 10 player actors
>     for (int t = 0; t < 10; t++)
>     {
>         ActorPlayer::Create(pos.x, pos.y, scene, image, anim);
>     }
> }
```

Notice how we now search for our game scene, instead of accessing it via the local game\_scene variable. We also do the same with image and anim.

This brings me to another new addition to the CIwGame engine.

CIwGameImage and CIwGameImageManager – Managing Images
------------------------------------------------------

After I had finished factoring out most of the code from Main.cpp I was left with what to do about the sprite atlas images that my game is going to use. I didn’t really want to be storing pointers to images all over the place that may or may not still exist, this can lead to some nasty and hard to find bugs. instead I opted to create a class that will manage all of the images that my scene allocates. This way we have them all in one central location and the manager can manage their lifetimes for us.

In addition, I thought that I would extend the CIw2DImage class a little by wrapping it up inside the CIwGameImage class, allowing us to add other cool functionality such as on-demand load and eventually streaming from an external web server (future blog will cover this).

So you will find that all instances of CIw2DImage throughout the engine have been replaced with the more manageable CIwGameImage class.  
.  
An important thing to note about CIwGameImage based images is that they rely on a CIwResGroup resource group, this is to allow them to be loaded on-demand.

To create and add an image to the game engine its a simple case of:

```
> CIwResGroup* Level1Group = IwGetResManager()->GetGroupNamed("Level1");
> game_scene->getImageManager()->addImage("sprites", Level1Group);
```

This creates our image based on its resource name and group and adds it to our scenes image manager. Note that the underlying CIw2DImage is not yet created. This gets created when you first call CIwGameImage::getImage2D() (on-demand). You can however force the loading / creation of the underlying CIw2DImage by passing true to addImage():

game\_scene->getImageManager()->addImage(“sprites”, Level1Group, true); or by calling the Load() method on the CIwGameImage.

Well that brings us to the end of this tutorial, I’m very rushed this week so I will probably not get the time to create additional tutorials until next week. I did want to create an actual game this week using the engine, but I thought that with the engine tidy up is probably best to do that next week so readers don’t get lost. I may be able to fit a short blog update in early next week covering the wrapping up of Marmalades sample audio engine into IwGame.

If you want the source code that accompanies this tutorial then you can [download it from here](http://www.drmop.com/wp-content/uploads/2011/11/IwGame_v0.200.zip "IwGame Engine Source Code").