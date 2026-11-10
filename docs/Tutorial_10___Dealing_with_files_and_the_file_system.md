# Tutorial 10 – Dealing with files and the file system

Original URL: http://www.drmop.com/index.php/2011/10/07/marmalade-sdk-tutorial-dealing-with-files-and-the-file-system/

This tutorial is part of the Marmalade SDK tutorials collection. To see the tutorials index [click here](http://www.drmop.com/index.php/marmalade-sdk-tutorials/ "Marmalade SDK tutorials index")

This evening we are going to cover dealing with files and the file system in a cross platform manner using the Marmalade SDK. Note that there is no associated code with this tutorial as it is not required.

To begin with, you will notice that Marmalade’s file functions look and behave very much like those in the standard C library, for example:

```
> s3eFile *  s3eFileOpen (const char *filename, const char *mode)
> FILE *     fopen       (const char *path,     const char *mode)
```

Paths in Marmalade
------------------

Paths with the Marmalade SDK follow the same approach as many other file systems in that they follow the drive, directories and filename pattern (drive:/folder1/folder2/file.extension).

The Marmalade SDK currently supports the following drive names:

‘rom’ for the read-only portion of your apps data area and is read only.  
‘ram’ for the writable portion of your apps data area. This drive is the only drive that is guaranteed to exist across platforms  
‘rst’ for a removable memory card on the phone or tablet  
‘raw’ for paths that will be passed directly to the underlying operating system without modification. (not supported on all platforms)  
‘tmp’ for a system temp folder, outside of the s3e data area. (system temporary folder and currently only available on iOS)

Path length is limited to S3E\_FILE\_MAX\_PATH characters (that value is currently 128 bytes, including the NULL string terminator)

Paths can be relative or absolute

Note that when reading a file where no drive has been specified the file system will attempt to find the file on the ram drive. if the file was not found then the system will look to the rom drive.

Marmalade SDK s3e file functions separated into categories
----------------------------------------------------------

I have separated Marmalade’s s3e file functions into categories to make them easier to find. If you would like to see more details on each of these functions then take a look at the  [Marmalade SDK s3E file reference online help doc](http://www.madewithmarmalade.com/devnet/docs#/api/api/group__fileapigroup.html "Marmalade SDK 3SE File Reference")

### Opening and closing files

```
s3eFile *  s3eFileOpen (const char *filename, const char *mode)
s3eFile *  s3eFileOpenFromMemory (void *buffer, uint32 bufferLen)
s3eResult  s3eFileClose (s3eFile *file)
```

### File query

```
s3eBool   s3eFileCheckExists (const char *filename)
int32     s3eFileGetSize (s3eFile *file)
int32     s3eFileGetInt (s3eFileProperty property)
int32     s3eFileTell (s3eFile *file)
int64     s3eFileGetFileInt (const char *filename, s3eFileStats stat)
char *    s3eFileGetFileString (const char *filename, s3eFileStats stat, char *str, int len)
uint64    s3eFileGetLastWriteTime (const char *filename)
uint64    s3eFileGetFree (s3eFilePath path)
```

### File reading / writing / seeking

```
s3eResult s3eFileFlush (s3eFile *file)
int32     s3eFileGetChar (s3eFile *file)
s3eBool   s3eFileEOF (s3eFile *file)
int       s3eFilePrintf (s3eFile *f, const char *fmt,...)
int32     s3eFilePutChar (char c, s3eFile *file)
uint32    s3eFileRead (void *buffer, uint32 elemSize, uint32 noElems, s3eFile *file)
char *    s3eFileReadString (char *string, uint32 maxLen, s3eFile *file)
s3eResult s3eFileSeek (s3eFile *file, int32 offset, s3eFileSeekOrigin origin)
uint32    s3eFileWrite (const void *buffer, uint32 elemSize, uint32 noElems, s3eFile *file)
```

### File and folders deletion / renaming

```
s3eResult  s3eFileDelete (const char *filename)
s3eResult  s3eFileRename (const char *src, const char *dest)
s3eResult  s3eFileDeleteDirectory (const char *dirName)
s3eResult  s3eFileMakeDirectory (const char *dirName)
```

### Directory Recursion

```
s3eResult     s3eFileListClose (s3eFileList *handle)
s3eFileList*  s3eFileListDirectory (const char *dirName)
s3eResult     s3eFileListNext (s3eFileList *handle, char *filename, int filenameLen)
```

### File error checking

```
s3eFileError  s3eFileGetError ()
const char*   s3eFileGetErrorString ()
```

Reading a file example
----------------------

```
> s3eFile* file = s3eFileOpen(“myfile.txt”, "rb");
> if (file != NULL)
> {
>     if (s3eFileRead(buffer, len, 1, File) != 1)
>     {
>         s3eFileGetError();
>         s3eDebugOutputString(s3eFileGetErrorString());
>     }
>     s3eFileClose(File);
> }
> else
> {
>     s3eFileGetError();
>     s3eDebugOutputString(s3eFileGetErrorString());
> }
```

Writing a file example
----------------------

```
> s3eFile* file = s3eFileOpen(“myfile.txt”, "wb");
> if (file != NULL)
> {
>     if (s3eFileWrite(buffer, len, 1, File) != 1)
>     {
>         s3eFileGetError();
>         s3eDebugOutputString(s3eFileGetErrorString());
>     }
>     s3eFileClose(File);
> }
> else
> {
>     s3eFileGetError();
>     s3eDebugOutputString(s3eFileGetErrorString());
> }
```

As you can see the file system is pretty simple to use and doesn’t particularly warrant an example demo to accompany it, so I haven’t written one.

It is worth noting that the file system also supports a call back system that allows you to implement your own file system. I will cover this in a more advanced tutorial.

Reading files within a compressed derbh file
--------------------------------------------

It is possible to attach compressed derbh files to the file system and re-direct all file access to the archive, accessing these archives just like a regular file. To do this you call dzArchiveAttach(“my\_archive.dz”);. Note that you can attach multiple archives

To detach the archives afterwards you would call dzArchiveDetachNamed(“my\_archive.dz”); or dzArchiveDetach() to detach the last attached archive.

It’s also possible to attach and detach memory based archives using dzArchiveAttachFromMem(void\* pMem, uint32 size); and dzArchiveDetachFromMem(void\* pMem);

Using this system makes accessing data within compressed archives a doddle.

To use derbh functionality within your app or game you need to include add the ‘derbh’ sub project to the list of subprojects section of your project MKB file.

Creating Derbh (.DZ) Files Manually
-----------------------------------

Ok, so now you know how to access Marmalade compressed archive, but how do you actually create one? In the folder \Marmalade\5.1\tools\dzip you will find a tool called dzip.exe. You can use this tool to create a derbh compressed archive by supplying it with a .DCL configuration file. This file basically contains a small amount of header information along with the files that you would like to be included into the archive. Here is an example .DCL file:

```
> archive my_archive.dz
> basedir .
> file image1.png 0 zlib
> file image2.png 0 zlib
> file image3.png 0 zlib
> file image4.png 0 zlib
```

basedir – The base director of the source input files

It is possible to specify which compressor to use on a per file based. In this case we use zlib compression, other values include lzma, dz, zero and copy.

Well that’s it from me and that’s it from him for today.

Happy coding and please don’t ever become a door to door sales PERSON! Seriously, those guys / gals do not know the meaning of the word “NO”!