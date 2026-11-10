# Tutorial 15 – Sending and Retrieving Data From the Web

Original URL: http://www.drmop.com/index.php/2011/11/11/marmalade-sdk-tutorial-sending-and-retrieving-data-from-the-web/

This tutorial is part of the Marmalade SDK tutorials collection. To see the tutorials index [click here](http://www.drmop.com/?page_id=91 "Marmalade SDK tutorials index")

In our previous tutorial we tidied up the IwGame engine and properly separated our engine calls away from our game code. We ended up with a nice system that allows us to begin building a game. This week we are going on a slight diversion because I have recently been working on sending and retrieving data from the web. I think that its important for any game engine to be able to pull in data from outside itself as it allows us to create games that are easily upgradable without having to put a new version of the game through the app store approval process. Its also cool to be able to send data to the web as it allows us to do cool stuff such as communicating with web services, accessing functionality that could add a lot of value to our games or apps. If you just want the code to this tutorial then you can [download it from here](http://www.drmop.com/wp-content/uploads/2011/11/IwGame_v0.210.zip "IwGame_v0.200 - Marmalade SDK Game Engine").

I am actually going to do a couple of tutorials today that cover this subject, the first is covering the basics of interfacing with Marmalade’s IwHttp module that allows us to send GET and POST requests to a web server.

Oh, the reason for this diversion is because over the last few days I have been working to produce a cross platform ad API that allows Marmalade SDK developers to use Inner-active’s ads on any platform by writing a system that interfaces with [Inner-active’s](http://inner-active.com/ "Inner Active Ad Provider") ad server and pulling back text and image based ads. Thus far I have the system returning and displaying ads. I just need to write the display system into its own view control and get the click system working. once that’s finished, it will become part of the IwGame game engine. I will blog about that when its finished. If you are interested in beta testing it then [please get in touch](http://www.pocketeers.co.uk/?page_id=8 "Contact Pocketeers Limited").

IwHttp – Marmalade’s API for Performing HTTP Requests
-----------------------------------------------------

IwHttp is the base module used by the Marmalade SDK that allows you to send and receive data from a web server over http and https. IwHttp allows you to perform both GET and POST operations. We have gone one step further however and simplified and extended its usefulness creating a new class called CIwGameHttpManager. If you would like to see how to use the IwHttp class in the manager then take a look at IwGameHttp.cpp and iwGameHttp.h.

CIwGameHttpManager – A Queued Http Request Manager
--------------------------------------------------

CIwGameHttpManager is a singleton class that allows you to throw GET and POST requests to it. The manager will queue the requests and them one by one until no requests are left in the queue. Note that the next request is only sent after the first returns. I’m not going to go into the internals of CIwGameHttpManager because the it and its associated classes constitute a lot of code. I will however explain how to use them.

Firstly you need to instantiate and initialise the CIwGameHttpManager singleton using:

```
> // Initialise the http manager
> CIwGameHttpManager::Create();
> IW_GAME_HTTP_MANAGER->Init();
```

And before you exit your game / app clean-up the manager using:

```
> // Clean up http manager
> IW_GAME_HTTP_MANAGER->Release();
> CIwGameHttpManager::Destroy();
```

In your main loop, you will also need to call the manager update so that it can do its stuff using:

```
> // Update http manager
> IW_GAME_HTTP_MANAGER->Update();
```

Making a GET Request – Getting Data From a Server
-------------------------------------------------

To make a GET request we need to create a persistent instance of CIwGameHttpRequest and set it up, like this:

```
> CIwGameHttpRequest* Request = new CIwGameHttpRequest();
> Request->setGET();
> Request->setURI(“http://www.battleballz.com/default.aspx”);
> Request->setContentAvailableCallback(&DataRetrievedCallback, (void*)this);
> IW_GAME_HTTP_MANAGER->AddRequest(Request);
```

Note that we have set a callback function that will be called when our data is available to use. Here is an example callback:

```
> int32 DataRetrievedCallback(void* caller, void* data)
> {
>     CIwGameHttpRequest* request = (CIwGameHttpRequest*)caller;
> 
>     if (request->getProcessed())                                    // Check to see if our request was processed by the http manager
>     {
>         // To get to our data we call request->getContent();
>         // To get to our data length we call request->getContentLength();
>         IW_GAME_HTTP_MANAGER->RemoveRequest(request);                // Remove request from http manager queue
>     }
> 
>     return 0;
> }
```

As you can see its ultra simple to retrieve data from the web.

Making a POST Request – Sending Data to a Server
------------------------------------------------

To make a POST request we need to create a persistent instance of CIwGameHttpRequest and set it up, like this:

```
> CIwGameHttpRequest* Request = new CIwGameHttpRequest();;
> Request->setPOST();
> Request->setURI(“http://test.battleballz.com/Test.asmx ”);
> Request->setContentAvailableCallback(&DataRetrievedCallback, (void*)this);
```

Note that up to this point the setup is very similar except we specify setPOST() instead of setGET().

However we want to send some data to the server we have to, so we have to put it somewhere, this is done using the posts body llke this:

```
> char length[32];
> const char* body = "<?xml version=\"1.0\" encoding=\"utf-8\"?><soap12:Envelope  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\"><soap12:Body><HelloWorld  xmlns=\"http://www.battleballz.com/test\" /></soap12:Body></soap12:Envelope>";
> Request->AddHeader("Content-Type", "application/soap+xml; charset=utf-8");
> sprintf(length, "%d", strlen(body));
> Request->AddHeader("Content-Length", length);
> Request->setBody(body);
```

Then we add the request to the http manager

```
> IW_GAME_HTTP_MANAGER->AddRequest(Request);
```

In the above example the body string holds an XML based SOAP request that we are sending to my test ASP.NET web service. In this case when the server gets this SOAP request it will call a method called HelloWorld which returns some data to us.

Note that when we send data to a web server we need to send some details about that data such as its type and length. In the above example we accomplish this by adding headers to our POST request.

When the POST has been made the web server will return a response which can be picked up in our callback function.

Ok, that all looks quite straight forward, but we can make things much simpler if we just want to download files from our web server, in steps the IwGameFile class

CIwGameFile – File Class That Handles Loading of Local and External files
-------------------------------------------------------------------------

Yes, we decided to put together a quick file class that handles loading of files from local storage and from an external web server. All you have to do is pass the URL and an optional callback (my kindness knows no bounds!)

Again, I don’t want to go into the intricate details about the implementation, instead I am opting for the telling you how to use it approach. IwGame is starting to become a small beast and I would rather spend the time improving it.

CIwGameFile is a basic general purpose file system class that allows you to open, create, read and write files. In addition it allows you to open a file located somewhere on the web using the http protocol.

Heres an example on how to open a local file, read some data and close the file:

```
> CIwGameFile* file = new CIwGameFile();
> file->Open("monkey_magic.txt”);
> file->Read(my_buffer, 1024);
> file->Close();
```

And here’s an example showing how to open a read the same file located on the web:

```
> CIwGameFile* file = new CIwGameFile();
> file->Open("http://www.battleballz.com/monkey_magic.txt”, false, true);
> file->Read(my_buffer, 1024);
> file->Close();
```

Note that the above method blocks the main loop and does essentially stops all processing of the game until the file is returned. I would instead recommend using a none blocking method like this:

```
> CIwGameFile* file = new CIwGameFile();
> file->Open("http://www.battleballz.com/monkey_magic.txt”);
> file->setFileAvailableCallback(FileLoadedCallback);
```

And our callback would look something like this:

```
> int32 FileLoadedCallback(void* caller, void* data)
> {
> 	CIwGameFile* file = (CIwGameFile*)caller;
> 	file->Read(my_buffer, 1024);
> 	file->Close();
> 
> 	return 0;
> }
```

IwGameFile still requires a few upgrades to complete its functionality. Here are a few planned updates:

* None blocking local file I/O
* Timeout check needs adding to blocking http downloads
* Add methods for reading / writing common types

Other additions to IwGame
-------------------------

You will notice that a few other bits and bobs have appeared in the game engine that i have not mentioned thus far. These include:

* IwGameString – This is a basic string class that supports string builder type string building to cut down on memory allocations as well as stream type searching and some other utility stuff
* IwGameImage – This class has been upgraded to support JPEG as well as creation of images from memory based files. Will cover this in more detail in my next blog

Well that’s it for this blog, as usual you can [download the latest code from here](http://www.drmop.com/wp-content/uploads/2011/11/IwGame_v0.210.zip "IwGame_v0.200 - Marmalade SDK Game Engine")

See you soon, with another (smaller thankfully) article.