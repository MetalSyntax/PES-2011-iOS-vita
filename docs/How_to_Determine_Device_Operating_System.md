# How to Determine Device Operating System

Original URL: http://www.drmop.com/index.php/2011/12/12/marmalade-sdk-bitesize-tutorialhow-to-determinedevice-operating-system/

Just though that I would sneak this quick bite sized tutorial in whilst taking a breather from documenting IwGame. Tell you something, I never realised just what a big beast IwGame has become.

Ok, I’m going to quickly cover how to determine which device operating system we are running on using the Marmalade SDK.

The Marmalade SDK has two neat little functions called s3eDeviceGetInt(s3eDeviceProperty property) and s3eDeviceGetString(s3eDeviceProperty property) which allow us to query details about the device that we are running our apps on. As the names suggest, one returns an integer value, whilst the other returns a human readable string value.

Using the property parameter you can query all sorts of things such as:

* Device operating system (S3E\_DEVICE\_OS)
* Device phone number (S3E\_DEVICE\_PHONE\_NUMBER)
* Device language (S3E\_DEVICE\_LANGUAGE)
* Battery level (S3E\_DEVICE\_BATTERY\_LEVEL)
* Operating system version (S3E\_DEVICE\_OS\_VERSION)
* Floating point unit support (S3E\_DEVICE\_FPU)
* Language- country code pair, e.g. en-us (S3E\_DEVICE\_LOCALE)
* Device UDID / IMEI (S3E\_DEVICE\_UNIQUE\_ID)

And all sorts of other useful info. Check 3seDevice.h for a comprehensive list of all properties that you can retrieve.

To find out which operating system you are running on you call:

int os = s3eDeviceGetInt(S3E\_DEVICE\_OS);

You can then test for which OS is running using a simple switch statement like this:

```
> switch (os)
> 	{
> 	case S3E_OS_ID_SYMBIAN:
> 		break;
> 	case S3E_OS_ID_WINMOBILE:
> 		break;
> 	case S3E_OS_ID_WINCE:
> 		break;
> 	case S3E_OS_ID_QNX:
> 		break;
> 	case S3E_OS_ID_BADA:
> 		break;
> 	case S3E_OS_ID_ANDROID:
> 		break;
> 	case S3E_OS_ID_IPHONE:
> 		break;
> 	case S3E_OS_ID_WEBOS:
> 		break;
> 	case S3E_OS_ID_WINDOWS:
> 		break;
> 	case S3E_OS_ID_LINUX:
> 		break;
> 	case S3E_OS_ID_OSX:
> 		break;
> 	}
```