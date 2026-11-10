# How to launch an external file or web link

Original URL: http://www.drmop.com/index.php/2011/10/19/marmalade-sdk-bitesize-tutorial-how-to-launch-an-external-file-or-web-link/

Welcome to another bite size Marmalade SDK tutorial.  This tutorial covers a cross platform way of opening up web links and other types of external file using the Marmalade SDK.

The Marmalade SDK contains a cool function called s3eOSExecExecute() which will launch the file / URL that you provide as a parameter using the underlying OS’s method of handling various file types.

In our example, we want to open up a web page URL. This type of system is great for deep linking your app or game to full and paid versions of free / lite apps or even providing a simple link to your other products or web sites.

The format of s3eOSExecExecute is as follows:

s3eResult s3eOSExecExecute(const char \*url, s3eBool exit)

url – The URL you wish to open  
exit – Should the app exit when launching the URL

Before you call this function you should however check to make sure that it is supported on the device using:

s3eBool s3eOSExecAvailable()