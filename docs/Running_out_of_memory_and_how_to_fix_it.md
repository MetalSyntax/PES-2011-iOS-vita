# Running out of memory and how to fix it

Original URL: http://www.drmop.com/index.php/2011/09/27/marmalade-sdk-bitesize-tutorial-running-out-of-memory-and-how-to-fix-it/

This tutorial is part of the Marmalade SDK tutorials collection. To see the tutorials index [click here](http://www.drmop.com/?page_id=91 "Marmalade SDK tutorials index")

Available Application Memory
----------------------------

Ok, this is the first of my bite-sized tutorials that I have started running alongside my usual meaty tutorials. I see these types of problems cropping on the forums often so I thought that I would cover them in small bite sized tutorials. Another good reason for bite sized tutorials is that I get the chance to blog a little more often as I can put one of these together in 10 minutes.

I’m pretty sure that most if not all of you Marmaladians have seen this dialog box before. If you haven’t then you are truly blessed 🙂

[![Marmalade out of memory exception dialog](http://www.drmop.com/wp-content/uploads/2011/09/Marmalade_out_of_memory.png "Marmalade_out_of_memory")](http://www.drmop.com/wp-content/uploads/2011/09/Marmalade_out_of_memory.png)

Marmalade out of memory exception dialog

**Heap 0 out of memory. Allocating 2048 bytes but only 576 available (540 LFB).  
Increase icf setting [s3e] MemSize (current value is 14584).**

This error is basically the Marmalade system telling you that it wants to allocate 2048 bytes but doesn’t have enough memory to allocate it, in fact in this case we only have 567 bytes left.

To solve the problem you need to edit a file that Marmalade has generated for you and placed in your data folder called app.icf. This file is the main app configuration file and is where Marmalade stores certain runtime settings such as how much memory to allocate to your program or how big to make the data cache etc..

To change the amount of memory available to your app simply add the following section to this file:

[S3E]  
MemSize=10485760

This value is the number of bytes to allocate to your app, in this case we have reserved 10MB. Note that without specifying your own MemSize you will have a default heap size of 3MB

If you are dealing with multiple heaps then you can set the the available memory per heap using:

MemSize1=your\_size  
MemSize2=your\_size  
MemSize3=your\_size  
and so on…..

Debug Memory
------------

When running an x86 Debug build the Marmalade system will usually require additional memory to build resources and such. Marmalade provides an additional S3E app.icf option that allows you to specify memory size during debug builds.

[S3E]  
MemSizeDebug=20485760

Data Cache Memory
-----------------

The Marmalade SDK uses a data caching system to store certain data that it is going to use during rendering for example vertex streams and materials. Its not possible to allocate these kinds of data on the stack as they must persist until rendering has finished (when the app hits IwGxFlush()). If you are drawing a lot of stuff using a lot of different materials then there is a good chance that you are going to run of data cache and receive an error like that shown below:

[![Marmalade Data Cache Overflow Error Dialog](http://www.drmop.com/wp-content/uploads/2011/09/Marmalade_data_cache_overflow.png "Marmalade_data_cache_overflow")](http://www.drmop.com/wp-content/uploads/2011/09/Marmalade_data_cache_overflow.png)

Marmalade Data Cache Overflow Error Dialog

**IwAssert failure (GX, 761).  
Message: Data cache overflow allocating 336. Increase [GX] DataCacheSize (currently 800)  
Callstack:  
IWGXFNI\_HW\_DrawPrims\_TSW  
\_IwGxInitPipeline**

To increase the amount of memory available to the data cache use the following app.icf setting (The default is 16384 bytes):

[GX]  
DataCacheSize=80000

   
   
   
 

Vertex Cache Memory
-------------------

When the Marmalade SDK transfoms your geometry it requires some work memory to place the transformed vertices prior to sending them to the GPU to be rendered. For example, if you are using IwGxFont to draw fonts then the vertices that make up the polygons of your glyphs will be transformed by the system into this vertex cache then rendered. You can change the amount of memory available for vertex transform using the following app.icf setting:

[GX]  
VertCacheSize=8000

Keep an eye out for more bite sized tutorials coming very soon!