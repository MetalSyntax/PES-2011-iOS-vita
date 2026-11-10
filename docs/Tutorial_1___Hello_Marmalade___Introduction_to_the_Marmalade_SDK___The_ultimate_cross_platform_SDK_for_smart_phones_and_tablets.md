# Tutorial 1 – Hello Marmalade – Introduction to the Marmalade SDK – The ultimate cross platform SDK for smart phones and tablets

Original URL: http://www.drmop.com/index.php/2011/09/12/hello-marmalade-introduction-to-the-marmalade-sdk-the-ultimate-cross-platform-sdk-for-smart-phones-and-tablets/

This tutorial is part of the Marmalade SDK tutorials collection. To see the tutorials index [click here](http://www.drmop.com/?page_id=91 "Marmalade SDK tutorials index")

So you are a smart phone, tablet, smart TV or desktop developer (or at least want to be) and you want to have your next hit game or app run across a huge range of smart phones and tablets. Well, you have come to the right place to find out how to do just that.

No matter whether you are a professional games developer, part time hobbyist coder or the technical director of a large corporation researching how to support you work force across a huge range of varied phones and tablets, the basic principles remain the same. By choosing to develop your products across multiple platforms:

* You benefit from a much wider audience for your apps and games
* You save a lot of money and time on development, testing and updating
* You can perform updates to your existing apps quickly and easily
* You can share the same unified C / C++ code base across all devices and platforms
* No need to learn (or hire professionals that know) multiple languages, UI’s or SDK’s
* Regularly updated SDK with new platforms and features
* Some of the smaller app stores offer increased visibility and more stable long term sales

And by choosing the Marmalade SDK you also get:

* A FREE license/li>* Amazing support, including the apps program and device loan program to aid testing
  * A simulator that lets you test across an unlimited set of screen resolutions and simulated access to Accelerometer, GPS, Camera, Audio, Multi-touch screen, SMS, Compass and more
  * Test actual ARM code without even deploying to an ARM based device
  * Support for iOS specific features such as App Store Billing, iAd, Game Center etc..
  * Support for Android specific features such as Android Market Billing
  * Access to a large collection of open API’s such as Box2D, AdMob, Flurry, Chipmunk,SVG, Python, LUA and tonnes of other cool stuff (Full list available at http://github.com/marmalade)

Marmalade is also partnered with the likes of Shiva3D, Scoreloop, Tapjoy, Raknet and many others, so you know this is an SDK that’s here to stay

Ok so what platforms does Marmalade actually support? The list to date is as follows:

* iPhone, iPod Touch and iPad
* Android
* Blackberry Playbook
* Blackberry 10
* Windows
* Windows Phone 8
* OSX
* Tizen
* Roku

I know what you are thinking, can I really write my code once and run it across all of these platforms? The straight answer is “absolutely!” as we at Pocketeers have proven. We have already released [BattleBallz Chaos (Arcade action game)](http://www.battleballz.com) across iOS, Android, Bada and Blackberry Playbook using the Marmalade SDK, as well as [Funky Cam 3D (a fun photography app)](http://www.funkycam3d.co.uk) across iOS, Android and Bada.

Ok, so if you can write your code once and deploy to so many platforms then why is your [BattleBallz Chaos](http://www.battleballz.com) not available on the likes of Symbian or webOS? The simple answer is that some of those platform markets are not currently where we want to go for a variety of reasons, but none of those reasons relate to the Marmalade SDK. We may choose to support them in the future and we may not.

Righty ho, you’ve decided that you quite like the sound of this Marmalade SDK and you’re considering saving yourself a boat load of time and money developing your cross platform games and apps, but what does code look like with this SDK?

Well here’s a basic game loop:

#include “IwGx.h”

```
int main()
{
    // Initialise Marmalade graphics system
    IwGxInit();

    // Main Game Loop
    while (!s3eDeviceCheckQuitRequest())
    {
        // Clear the screen and depth buffer
        IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

        // Update my awesome game
        PleaseUpdateMyGame();

        // Render my awesome games view
        RenderMyGameViewThankyou();

        // Flush graphics system
        IwGxFlush();

        // Display the rendered frame
        IwGxSwapBuffers();

        // Yield to the operating system
        s3eDeviceYield(0);
    }

    // Shut down Marmalade graphics system
    IwGxTerminate();

    return 0;
}
```

You will find that the graphical system in Marmalade is similar to Open GL, which for me makes the SDK very simple to use. All other sub systems are equally as easy to use for example:

To create a texture from a bitmap file and upload it to the GPU:

```
CIwTexture* texture = new CIwTexture();
texture->LoadFromFile(“AwesomeSpriteAtlas.png”)
texture->Upload();
```

As you can see the code is ultra simple and very easy to use but most of all “cross platform compatible!”. Imagine having to do this on iOS using XCode / Objective C and then again using Java on Android and then again using Flash on Playbook, the list goes on.

So unless you enjoy self punishment, lots of extra work and the pain of tracking the same bugs across multiple SDK’s, languages and platforms, I suggest you take a short trip over to Marmalade’s SDK home page at <http://www.madewithmarmalade.com>. Take a look at the SDK, its features, read some tutorials and even sneak a peek in the forums (I bite but not many of the other developers do!)

Over the coming weeks / months I will be writing a number of tutorials covering various aspects of the Marmalade SDK, associated tools and extensions, so keep an eye out.