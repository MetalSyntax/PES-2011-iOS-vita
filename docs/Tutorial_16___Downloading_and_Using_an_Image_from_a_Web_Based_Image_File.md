# Tutorial 16 – Downloading and Using an Image from a Web Based Image File

Original URL: http://www.drmop.com/index.php/2011/11/12/marmalade-sdk-tutorial-downloading-and-using-image-from-a-web-based-image-file/

This tutorial is part of the Marmalade SDK tutorials collection. To see the tutorials index [click here](http://www.drmop.com/?page_id=91 "Marmalade SDK tutorials index")

Unfortunately my brother rang me last night and asked me to have a few quick battles with him on World of Warcraft (I just can’t resist PvP) so as it happens, I forgot all about the time and missed publishing my next blog. In case you plaw Wow and you’re wondering I have Level 85 Mage, Warlock, Paladin and Druid!

In our previous tutorial we added the ability to send and receive data to and from a web server. We also created a new file class that deals with local and remote file access, allowing us to download files from the web and use them in our game. We also briefly mentioned that we had updated the CIwGameImage class to support JPEG’s and creation of images from memory based files. In this tutorial we are going to go into that a little more as well as show you how to use CIwGameFile and CIwGameImage to download and display images from the web inside your games and apps. If you just want the code to this tutorial then [download it from here](http://www.drmop.com/wp-content/uploads/2011/11/IwGame_v0220.zip "IwGame - Marmalade SDK Game Engine").

Imagine a situation whereby you can distribute your basic game without having to carry around any of its content, so no graphics, no audio, no level files etc. Whilst that may not be completely practical for some types of games it is for many others. Imagine your game running completely on dynamic content that lives on a server and you can modify it at any time, Your players will benefit greatly from new fresh content regularly. Well, this is all possible using the Marmalade SDK and IwGame (wow that sounded a bill salesy!)

IwGameImage Changes
-------------------

Ok the first change we made was to add the ability to create a CIwGameImage image from a JPEG. This feature is unfortunately not supported by Marmalades image classes and missing out on JPEG compression for larger images is a bit of a pain. That said, Marmalade does provide access to LibJPG, allowing you to implement your own JPEG loader. However, seeing as we have already done the work for it, you may as well just rip the code straight out of IwGameImage.cpp.

The main method that takes care of JPEG decompression is CIwGameImage::DecompressJPEG(). Note that this method will create a RGB888 image from the JPEG file, I will add support for conversion to different image pixel formats in a future release.

The next change to CIwGameImage is the ability to create an image from a file that has already been loaded into memory. This is very useful if you want to cache the compressed versions of images in memory:

bool CIwGameImage::Init(const char\* name, void\* memory\_file, int memory\_file\_size, bool is\_jpeg)

name – The name you would like to assign to the file  
memory\_file – A pointer to the memory based file  
memory\_file\_size – The size of the memory based file  
is\_jpeg – Determines if the memory based image file is a jpeg

Note that future versions will not rely on is\_jpeg and instead, the header data will be checked to determine file type.

How to Create a CIwImage or CIw2DImage from a Memory Based File

Ok, lets take a look how we create an image from a memory based file in the CIwGameImage::Init() method:

```
> bool CIwGameImage::Init(const char* name, void* memory_file, int memory_file_size, bool is_jpeg)
> {
>     CIwGameFile file;
>     if (file.Open(memory_file, memory_file_size))
>     {
>         if (is_jpeg)
>         {
>             if (!DecompressJPEG((char*)memory_file, memory_file_size))
>                 return false;
>         }
>         else
>         {
>             CIwImage image;
>             image.ReadFile(file.getFileHandle());
>             Image2D = Iw2DCreateImage(image);
>             Width = Image2D->GetWidth();
>             Height = Image2D->GetHeight();
>         }
>     }
> 
>     State = CIwGameImage_State_Loaded;
> 
>     return true;
> }
```

As you can see we create a CIwImage by calling CIwImage::ReadFile() passing in the file handle of the memory based file (to find out how to create a file from a memory buffer take a look at CIwGameFile::Open(void\* memory\_buffer, int memory\_buffer\_len)).  
To turn the CIwImage to a CIw2DImage we call Iw2DCreateImage() passing in the CIwImage.

How to Create a CIwImage or CIw2DImage from a Pixel Data
--------------------------------------------------------

In our CIwGameImage::Init() method you will notice that if the image file is a jpeg then we call DecompressJPEG(). If you look towards the end of our DecompressJPEG() method you will notice that we create our CIwImage / CIw2DImage a slightly different way.

Because we are manually loading and decompressing the pixel data within the JPEG image we are left with a memory buffer containing our raw uncompressed images pixels. Marmalade’s usual create from file style image creation methods do not cover this, so instead we create the image ourselves from the pixel data like so:

```
> CIwImage image;
> image.SetFormat(CIwImage::RGB_888);
> image.SetWidth(Width);
> image.SetHeight(Height);
> image.SetOwnedBuffers((uint8*)data, 0);
> Image2D = Iw2DCreateImage(image);
```

Basically we create a CIwImage, set its format and dimensions then tell it where our pixel data is using SetOwnedBuffers(). Like we did in the previous example, to turn the CIwImage into a CIw2DImage we call Iw2DCreateImage() passing in the CIwImage.

Creating an Image from a Web Based Image File
---------------------------------------------

The new demo shows how to download an image file from the web and create an image from it then display that image as a sprite. Lets firstly take a look at how we download and create the image:

```
> // Download an image file from the web
> CIwGameFile* image_file = new CIwGameFile();
> image_file->Open("http://www.battleballz.com/bb_icon.gif", false, true);
> image_file->Close();
> 
> // Create an image from the downloaded gif file
> CIwGameImage* image = new CIwGameImage();
> if (image_file->isFileAvailable() && image_file->getError() == CIwGameFile::ErrorNone)
>     image->Init("", image_file->getContent(), image_file->getContentLength(), false);
> delete image_file; // We are done with the image file so we can delete it
```

Now we will create a sprite to display our downloaded web image::

```
> // Create a test sprite to display our downloaded gif
> CIwGameBitmapSprite* test_sprite = new CIwGameBitmapSprite();
> test_sprite->setImage(image);
> test_sprite->setDestSize(image->getWidth(), image->getHeight());
> test_sprite->setSrcRect(0, 0, image->getWidth(), image->getHeight());
> test_sprite->setPosition(image->getWidth() / 2, image->getHeight() / 2);
```

And that’s about all there is to it.

Some of you probably already have ideas buzzing around your heads on how to use this cool new system. I’m going to put some more time into the image class this week because I’m not happy about the way loading a jpeg and a web based image is different to the way we load images from a resource collection. I want to change the CIwGameImage class to basically allow you to specify not just a resource name, but also local file names and remote file names.

Well that’s it for this blog, as usual you can [download the latest code from here](http://www.drmop.com/wp-content/uploads/2011/11/IwGame_v0220.zip "IwGame Marmalade SDK Game Engine")

Happy coding and don’t eat too much tuna!