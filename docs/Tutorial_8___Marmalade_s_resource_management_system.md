# Tutorial 8 – Marmalade’s resource management system

Original URL: http://www.drmop.com/index.php/2011/10/01/marmalade-sdk-tutorial-marmalades-resource-management-system/

This tutorial is part of the Marmalade SDK tutorials collection. To see the tutorials index [click here](http://www.drmop.com/index.php/marmalade-sdk-tutorials/ "Marmalade SDK tutorials index")

Well, its Saturday evening and instead of being slumped up against a bar talking nonsense and singing songs of old bloody coding battles with my fellow drunks, I’m sat here writing Tutorials to educate the masses on how cool and easy the Marmalade SDK is to use 🙂

In today’s (this evenings?) tutorial we are going to cover the Marmalade SDK’s resource management system. As usual if you just want the associated article source code then you can [Download it from here](http://www.drmop.com/wp-content/uploads/2011/10/Resources.zip). If you want the details on the resource management system then please do read on.

Marmalade’s resource managements system is a big subject, but I am going to attempt to summarise and pick out the most important bits to help get you up and running with it quickly

Most if not all games and apps should use some kind of resource system to handle the large amount of graphical, audio and other resources that constitute the game or app. Marmalade provides a text based resource group file system that allows you to specify groups of resources that you would like to include in your app.

IwResManager – The Marmalade SDK resource manager
-------------------------------------------------

IwResManager is the name given to the Marmalades utility resource management system. To include the module into your build, you will need to edit your project MKB file and add the iwresmanager module to your sub projects:

```
> subprojects
> {
>     iwresmanager
> }
```

You will also need to include the IwResManager.h header file

The resource manager has three functions:

* Loading of resources file from their original source files. For example inefficient BMP’s , huge model text descriptor files etc..
* Serialisation of resources to and from their compact binary format. As an example, the serialisation module would save the raw BMP’s, model text descriptors etc.. in a more compact format file called group\_name.group.bin
* Organising resources into groups using group text files. For example, you may want to organise all of the bitmaps for each level into a separate group then load the whole group per level

So as you can see IwResManager has quite a few uses, but its most important use is allowing you to specify your assets in a bulky format and have the resource manager compact them down into a more efficient format for deployment.

Oh, IwResManager will only rebuild the compact binary files from the original sources if you run the x86 Debug build. So if you wondering why your resource group bin files never change, you now know why.

To initialise the resource manager in code you need to call IwResManagerInit() at boot and then IwResManagerTerminate() to close it down at the end of your app.

Resource Manager Groups
-----------------------

All resources exist inside the resource manager in groups. You create groups using a simple text file format (Level1.group for example) that looks something like this:

```
> CIwResGroup
> {
>     name "Level1"
> 
>     // Level 1 image files
>     "Level1/background.bmp"
>     "Level1/SpriteSheet1.png"
>     "Level1/SpriteSheet1.png"
> 
>     // level 1 audio files
>     “Level1/music.wav”
>     “Level1/sfx1.wav”
>     “Level1/sfx2.wav”
>     “Level1/sfx3.wav”
>     “Level1/sfx4.wav”
> 
>     // Child groups
>     “Characters\buddy_bod.group”
>     “Characters\evil_twiggy.group”
> }
```

This group is named Level1 and contains 3 images, 5 sound effects and two other group. Note that the groups included here are known as a child groups. These groups are independent group files, just like this one but containing resources specific to buddy\_bod and evil\_twiggy.

Resources in the child groups will not be included in the parent resource binary file when serialised as they are groups in their own right, instead a link between them and the parent Level 1 group will be kept so that when you load the parent group, the child groups will also be loaded.

Loading Groups
--------------

Firstly, in order for Marmalade to see your group file on the target phone, tablet etc.. the corresponding group\_name.group.bin file needs to be added to assets section like this:

```
> assets
> {
>     (data-ram/data-gles1, data)
>     Level1.group.bin
> }
```

But where does this “Level1.group.bin” file come from? Remember I mentioned earlier that a debug build will build the compact binary versions of your resources, well this file is the compact binary version of your Level1 resources group file, which contains your BMP’s and WAV’s.

Next, to load the group in code you would call:

IwGetResManager()->LoadGroup(“Level1.group”);

Note that each time you call LoadGroup() each new group will be added to the resource manager.

Searching Groups
----------------

To find a particular named group in the resource manager you would call something like:

CIwResGroup\* pLevel1Group = IwGetResManager()->GetGroupNamed(“Level1”);

To find a resource within that group you would call something like:

CIwTexture\* pBackgroundTexture = (CIwTexture\*)pLevel1Group->GetResNamed(“background”, “CIwTexture”);

A few notes:

* Searching for a resource within a group will by default also search all child groups for the resource
* You do not need to supply the type of resource such as “CIwTexture”, but it is recommended for speeding up searches
* You can add a flag (3rd parameter) to GetResName() to modify the search. For example, if you want to ignore child group resources whilst searching then pass IW\_RES\_IGNORE\_CHILDREN\_F. See IwGxResManagerFlags definition for a complete list of flags

Destroying Groups
-----------------

To destroy a resource group and all its associated resources you would use:

IwGetResManager()->DestroyGroup(“Level1”); or

CIwResGroup\* pLevel1Group = IwGetResManager()->GetGroupNamed(“Level1”);  
IwGetResManager()->DestroyGroup(pLevel1Group);

Compressing your resources with Derbh
-------------------------------------

The Marmalade SDK provides a mechanism by which you can compress your data to DZ / dzip files prior to or during deployment and then access the files within the compressed archive within your app or game with very little hassle. The module used for compression / decompression is called Derbh and to use it you need to add the derhb module to your list of sub projects in your project MKB file, like so:

```
> subprojects
> {
>     derbh
> }
```

You can completely automate the process of compressing all of your assets during deployment by adding the following line to the deployments section of your project MKB file:

```
> deployments
> {
>     auto-derbh
> }
```

You do not have to do anything different in your code to access your compressed assets as the decompression is transparent.

Note that you can take finer control over compression and decompression in your app as Marmalade provides derbh access via low and high level API’s that enable you to load archives on demand as well as read individual files.

And that’s about as far as I’m going to go with the resource manager for the moment. If you would like to learn more in-depth details, especially regarding the resource asset conversion process then you should check out the Marmalade SDK docs at [here](http://www.madewithmarmalade.com/devnet/docs#/api/iwresmanagerapidocumentation/iwresmanagerapioverview.html "Marmalade SDK resource manager")

What’s changed in our example
-----------------------------

Ok, now we get to the accompanying example program. Lets take a look to see what has changed (this example is based upon the Keys example from one of our previous tutorials)

Firstly we will take a look at our Resources.mkb project file. You will have noticed that our test1.png and test2.png files are no longer listed as assets. Instead our group binary file is listed as:

> (data-ram/data-gles1, data)  
> Level1.group.bin

Level1.group.bin now contains our PNG files

Also iwresmanager has been added to our list of subprojects

We have also added a new file to the project called Level1.group, which looks like this:

```
> CIwResGroup
> {
>     name "Level1"
> 
>     // Graphics
>     "./test1.png"
>     "./test2.png"
> }
```

This is our resource group definition file, which the Marmalade resource manager will use to build our Level1.group.bin file (when we run the debug version of the app).

Next we will take a look at what’s changed in Main.cpp. Firstly we have included the resource managers header file IwResManager.h. We have also added code to initialise the resource management system and load our Level1 group file:

```
> // Initialise the rsource manager
> IwResManagerInit();
> 
> // Load our Level1 resource group into the resource manager
> IwGetResManager()->LoadGroup("Level1.group");
```

Ok, this next code change may seem a bit odd:

```
> // Create two images from PNG resource files
> IwGetResManager()->SetCurrentGroup(IwGetResManager()->GetGroupNamed("Level1"));        // Ensure that Level1 is the current resource group
> CIw2DImage* image1 = Iw2DCreateImageResource("test1");
> CIw2DImage* image2 = Iw2DCreateImageResource("test2");
```

In the Keys code example, we created CIw2DImage’s from our test PNG files. As our PNG’s now live inside our Level1 group file instead, we need to firstly set the Level1 group as our current group and then create images from the test1 and test2 resources located inside that group.

Lastly, we shut down the resource manager

```
> // Shut down the resource manager
> IwResManagerTerminate();
```

Ok, that’s it. You should now have enough info to start using Marmalade’s resource manager and resource groups in your own code, You can download the associated [Resource project source code from here](http://www.drmop.com/wp-content/uploads/2011/10/Resources.zip)

Happy coding and don’t go to work naked!