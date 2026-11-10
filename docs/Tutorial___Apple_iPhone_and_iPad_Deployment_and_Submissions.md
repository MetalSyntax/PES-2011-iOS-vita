# Tutorial – Apple iPhone and iPad Deployment and Submissions

Original URL: http://www.drmop.com/index.php/2011/12/10/marmalade-sdk-tutorial-apple-iphone-and-ipad-deployment-and-submissions/

This tutorial is part of the Marmalade SDK tutorials collection. To see the tutorials index [click here](http://www.drmop.com/?page_id=91 "Marmalade SDK tutorials index")

Its been a while since I posted any new Marmalade SDK tutorials because I have been so incredibly busy, I’ve also been focusing a lot of time lately on the soon to be released IwGameAds update to the IwGame engine (should be up on the blog today). Well, I’m now project free, nothing to work on for clients at the moment, so I have free time to blog.

Ok, lets move into the subject of the blog, debugging, deploying and app store store submissions for iOS devices (iPhone, iPod and iPad). I felt an urgent need to cover this subject as I have noticed many problems popping up on the Marmalade forums relating to iOS provisioning and app store submissions, so my insane desire to help out has got me caught up in an afternoons work of creating this useful guide.

iOS Development on Marmalade Introduction
-----------------------------------------

The great thing about developing iOS apps with Marmalade is that by following a few simple rules your aop will run on any supported device as long as it runs on the Marmalade SDK x86 / Arm simulators. If you have come from a Cocoa / Objective-C background there are a few things to note when devloping with Marmalade SDK:

* C/C++ is the primary language for development
* You don’t have access to interface builder, instead Marmalade provides UI and font builder applications for creating complex user interfaces and fonts
* Development can be done on both PC or Mac.
* You can add code that has been developed using the iOS SDK / Objective-C via Marmalades extensions developer kit (EDK), but this code will not be cross platform compatible, so will only work for iOS deployments
* Marmalade now supports native UI via IwNUI allowing you to create UI layouts that use native IOS controls (currently supported on iOS, Android, other platforms will fall back on Marmalade’s generic UI API IwUI)

For the duration of this article we are going to use a fictional app called “iScream Candy” that we are going to fictionally provision, develop and distribute.

iOS Provisioning Introduction
-----------------------------

Well, I don’t accuse Apples account set up and provisioning process to be impossible but is has had me in tears before, fortunately in this article I am going to hopefully save many of you a bucket load of tears by providing a nice list of steps showing how to sort all this our quickly and quite painlessly,

What is provisioning and why is it needed by the way? In general terms. provisioning is the process of enabling development for a developer, allowing them to build and deploy apps to Apples devices and limiting access to those apps. This is needed to a) prevent software piracy and b) keeps control of who can develop apps on Apple devices.

The first thing you are going to need is an Apple Developer account. Go to [http://developer.apple.com](http://developer.apple.com "Apple App Store Developer Centre") and create one.

Once you have your developer account, you need to click on “iOS Dev Center” to gain access to iOS developer resources. You will be asked to log in again. once logged in you will be presented with the iOS Dev Centre front page, you should note the “iOS Developer Program” menu down the right hand side of the screen

Here you will see 4 options, but we are only really interested in the first one for now “iOS Provisioning Portal”, click on it to go the provisioning portal.

Now take a look down the left hand side of the screen, you will see quite a few options. These options allow us to generate developer and distribution certificates, add devices that are allowed to run our apps, create app ID’s and provisioning profiles.

iOS Development Certificates
----------------------------

In order to develop and distribute products on the iOS platform Apple needs to know that our app is made by us and that our apps code is reliable and if something should go wrong Apple know where to go knocking. Security certificates allow developers to sign applications using their identity showing that they are “trusted” and not some malicious software developer that’s creating apps to destroy iOS devices or steal users personal details.

There are two kinds of certificates that you are going to need:

* Development certificate – You need this to deploy your app to iPhone. iPad and iPod
* Distribution certificate – You need this to deploy your app ro the Apple app store

Creating an iOS Development Certificate
---------------------------------------

In order to deploy our cool iOS apps to our iPhone, iPod or iPad we need to create a development certificate. This process starts with the Marmalade SDK. Here are the complete instructions:

1. Run the Marmalade iPhone Sign Request tool, this will launch Marmalades signing request tool which will create a developer key and a signing request for you  
2. Click “Browse” and select a folder where you want Marmalade to save the signing request  
3. Select “developer\_identity.key” from the Key File drop down  
4. Now enter your developer company name into the “Common Name” box  
5. Enter the email address that you have assigned to your Apple developer account into the Email Address box  
6. Finally hit the “Run” button

The tool will now generate a file called signing\_request.csr in the folder that you selected in step 2. It will also generate a developer\_identity.key key file in the \Marmalade\5.x\s3e\deploy\plugins\iphone\certificates folder (make a backup of this file)

Now over to the Apple side of things:

7. Go to the “iOS provisioning portal”  
8. Click on the “Certificates” menu  
9. Click on the “Development” tab  
10. Click on the “Request Certificate” button  
11. You will see a button at the bottom of the screen called “Choose File”. Click that button and select the signingrequest.csr file that you exported from Marmalades signing request tool  
12. Click the “Submit” button. The certificate will be marked as “issued”. Click on the “Development” tab again to refresh the page (sometimes you need to refresh few times), this will add a button called “Download” to the right of the screen  
13. Click on the “Download” button, this will download a file called “ios\_development.cer”, save this file  
14. Make a backup of this file and also copy and rename the file as \Marmalade\6.x\s3e\deploy\plugins\iphone\certificates\developer\_identity.cer  
15. Marmalade will now use the developer\_identity.key and developer\_identity.cer to sign your apps for deployment during development

Creating and Provisioning an iOS Application for Development
------------------------------------------------------------

You probably already have test app or even a full app that you are waiting to test out on an iOS device, but before you do, there are a couple of changes you need to make.

The first thing you will need is an Application ID (App ID) which uniquely identified your app to the world. To create one we need to head back over to Apples iOS provisioning portal and carry out the following steps:

16. Click App ID’s option on the menu  
17. Click “New App ID” button  
18. Enter “iSCream Candy” as the app description  
19. Enter “com.yourcompanyname.iscreamcandy” as the Bundle Identifier  
20. Click the “Submit” button which takes you back to the App ID’s list

In order to deploy to an actual device we need to add our Apple device to the provisioning portal like this:

21. Click on the “Devices” menu  
22. Click the “Add Devices” button  
23. Enter a name for your device (anything you like)  
24. Enter your devices Device ID. To get your devices ID carry out the following steps:

* a. Plugin your device into the USB
* b. Run the iPhone Configuration Utility
* c. Select your devices in the left hand menu
* d. In the summary tab you will see an “Identity” field, the long hexadecimal string is your Device ID
* e. Keep the configuration tool open because you will need this in a moment to install your provisioning profile to the device

25. Click the “Submit” button

You will also need a developer provisioning profile, so head back to Apples provisioning portal and carry out the following steps:

26. Click on the “Provisioning” menu  
27. Click “Development” tab  
28. Click “New Profile” button  
29. Enter “iscream\_candy” as the Profile Name  
30. Tick the box next to your certificate name to associate the certificate with this profile  
31. Select “iScream Candy” as the App ID  
32. Tick the checkbox next to your iOS device that is in the list of devices  
33. Click “Submit” button  
34. Click on the “Development” tab to refresh the screen  
35. Click the “Download” button at the side of the “iscream\_candy” profile name and save the file (will be called “iscream\_candy.mobileprovision”)  
36. Drag the provisioning profile file “iscream\_candy.mobileprovision” onto the iPhone Configuration Utility  
37. Select your iOS device in the left hand menu in the the oPhone Configuration Utility  
38. Select the “Provisioning Profiles” tab  
39. Locate “iscream\_candy” and click the “Install” button that is shown next to it

Now we go back to the Marmalade side of things. We need to let the Marmalade deployment tool know a few things about your app before we deploy:

40. Open up your projects MKB file  
41. Add a section like this:

> deployments { [“iPhone”] name=’iScreamCandy’ caption=’iScream Candy’ manufacturer=’Company Name’ provider=’Company Name’ version-major=1 version-minor=0 iphone-appid=’com.companyname.iscreamcandy’ }

This deployment section will allow us to auto fill in the information that we provided when we use Marmalades deployment tool to deploy our app to our iOS device.

We are now ready to build and deploy our app to the iPhone, iPad, and iPod.

Building and Deploying our App to an iOS Device
-----------------------------------------------

Ok, now we have provisioning for development and we have a deployment section added to our MKB project file, lets get our app installed to the device for testing.

42. In Visual Studio / XCode change the build type to GCC (ARM) Release and hit go, the project will now build. Once built, Visual Studio will launch the Marmalade Deployment Tool.  
43. Select “ARM GCC Release” as the Build Type then click the Next button  
44. Select “iPhone” as the deployment configuration then click the Next button  
45. Select “iOS” as the platform to deploy to then click the Next button  
46. Click the “Deploy All” button  
47. When the deployment tool is finished it will create two files in our iPhone release deployments folder (the deployments folder is located in our build folder build\_iscreamcandy\_vc10\deployments\iPhone\iphone\release

Its worth noting here that you should check the deployment log to ensure that all went well, to do this click the “Log” button

48. Drag the iscream\_candy.ipa file from that folder onto iTunes  
49. in iTunes select your phone on in the menu on the left of the screen  
50. Click on the apps tab and tick the checkbox next to our iScream Candy app then click on the Sync button to install the app

All being well the app should now appear on the iOS device.

Word of caution, delete your existing app from the iOS device before redeploying. Unless you change the version number of your app in the MKB file, iTunes will not install the same version number of an app if it already exists on the device.

Creating an iOS Distribution Certificate
----------------------------------------

So now you have a finished product and you want to submit it to the Apple app store for approval. The first thing you are going to need is a distribution certificate

In order to deploy our cool iOS apps to the app store we need to create a distribution certificate. This process starts with the Marmalade SDK, here are the complete instructions:

51.. Run the Marmalade iPhone Sign Request tool, this will launch Marmalades signing request tool  
52. Click “Browse” and select a folder where you want Marmalade to save the distribution signing request  
53. Change the name to “distro\_signing\_request.csr”  
54. Select “distribution\_identity.key” from the Key File drop down  
55. Now enter your developer company name into the “Common Name” box  
56. Enter the email address that you have assigned to your Apple developer account into the Email Address box  
57. Finally hit the “Run” button

The tool will now generate a file called distro\_signing\_request.csr in the folder that you selected in step 2. It will also generate a distribution\_identity.key key file in the \Marmalade\5.x\s3e\deploy\plugins\iphone\certificates folder (make a backup of this file)

Now over to the Apple side of things:

58. Go to the “iOS provisioning portal”  
59. Click on the “Certificates” menu  
60. Click on the “Distribution” tab  
61. Click on the “Request Certificate” button  
62. You will see a button at the bottom of the screen called “Choose File”. Click that button and select the distro\_signingrequest.csr file that you exported from Marmalades signing request tool  
63. Click the “Submit” button. The certificate will be marked as “issued”. Click on the “Distribution” tab again to refresh the page (sometimes you need to refresh few times), this will add a button called “Download” to the right of the screen  
64. Click on the “Download” button, this will download a file called “ios\_distribution.cer”  
65. Make a backup of this file and also copy the file as \Marmalade\6.x\s3e\deploy\plugins\iphone\certificates\distribution\_identity.cer  
66. Marmalade will now use the distribution\_identity.key and distribution\_identity.cer to sign your apps for distribution to the app store

Creating and Provisioning an iOS Application for Distribution
-------------------------------------------------------------

We need a fair few bits and bobs in order to create an app that is ready for distribution to customers that are foaming at the mouth ready to spend their hard earned dollars on your new masterpiece (I wish lol). The first thing we need is a distribution provisioning profile that we can include with the app during submission, here’s how to generate it:

67. Click on the “Provisioning” menu in the iOS Provisioning Portal  
68. Click “Distribution” tab  
69. Click “New Profile” button  
70. Ensure that “App Store” is selected  
71. Enter “distro\_iscream\_candy” as the Profile Name  
72. Select “iScream Candy” as the App ID  
73. Ensure that the “Distribution Certificate” says “Company Name (expiring on {Some date in the future})”  
74. Click “Submit” button  
75. Click on the “Distribution” tab to refresh the screen  
76. Click the “Download” button at the side of the “distro\_iscream\_candy” profile name and save the file  
77. Copy the saved distro\_iscream\_candy.mobileprovision file to the Data folder in your build

Now over to the Marmalade side

78. Open up your projects MKB file  
79. Update your deployments section to look something like this

```

deployments
{
  ["iPhone"]
  name='iScreamCandy'
  caption=''iScream Candy'
  manufacturer='Company Name'
  provider='Company Name'
  version-major=1
  version-minor=0
  iphone-appid='com.companyname.iscreamcandy'
  iphone-splash="data/Default~iphone.png"
  iphone-icon="data/icons/iPhoneIcon.png"
  iphone-icon-ipad="data/icons/iPadIcon.png"
  iphone-icon-high-res="data/icons/iPhone4Icon.png"
  iphone-sign-for-distribution='1'
  iphone-provisioning-profile="data/distro_iscream_candy.mobileprovision"
}
```

You will notice a few other extra pieces of information have appeared in the MKB file. As we are getting ready to distribute our app we also need to provide icons and splash screens. If you app is a unified iOS app (compatible with iPhone and iPad) then you need to supply splash screens and icons for both platforms.

Getting ready for iOS App Store Submission
------------------------------------------

Before we can submit our super cool but hastily named game to Apple for approval we need to create app icons, splash screens, screen shots and text / keywords etc..

80. The iOS expects the following sized icons (see above MKB deployment section to see how to include them)

* iphone-icon – Standard iPhone icon (57 x 57 pixels)
* iphone-icon-ipad – Standard iPad icon (72 x 72 pixels)
* iphone-icon-high-res – iPhone with retina display (114 x 114 pixels)

81. In addition, you need to supply 3 different splash screens that need to have specific names:

* Default~iphone.png – Standard iPhone splash screen (320 x 480 or 480 x 320)
* Default~ipad.png – Standard iPad splash screen (768 x 1024 or 1024 x 768)
* Default@2x~iphone.png – iPhone with retina display (640 x 960 or 960 x 640)

To ensure that these files get included in your build you need to add them to the assets section of your MKB file like this:

> assets { (data) “Default~iphone.png” Default@2x~iphone.png “Default~ipad.png” }

82. You also need the following additional graphical items:  
\* 512 x 512 iTunes Artwork Icon – This is a hi-resolution version of your app icon  
\* At least 3 iPhone screen shots (320 x 480 or 480 x 320)  
\* At least 3 iPad screen shots (768 x 1024 or 1024 x 320)

iOS App Store Submission
------------------------

We are finally there, woohoo, I’m so glad because my fingers are starting to break!

83. To submit an app to Apple you need to log into iTunes Connect (menu option beneath Provisioning Portal) on the iOS dev web site  
84. Click on “Manage Your Applications”  
85. Click on the “Add New App” button to create a new application  
86. Enter “iScream Candy” as the App Name  
87. Enter SKU number (put anything you like or that’s relevant to how you track apps)  
88. Select “com.companyname.iscreamcandy” as the Bundle ID  
89. Click the “Continue“ button  
90. Select availability date to some point in the future (choose 2-3 weeks from now, you can change this when the app is passed)  
91. Select price tier (to find out what the tiers mean in dollars click the “View Pricing Matrix” link)  
92. Click the “Continue“ button  
93. Fill in the app information, add app icon and screen shots etc..  
94. Click the “Save” button  
95. Click on the View Details for your app  
97. Click on the “Ready to Upload Binary” button at the top right of the screen  
98. Select “No” when asked about export compliance (unless you are using encryption) then click the “Save” button

The next screen will tell you how to download and install XCode so you can gain access to Application Loader (a small app that lets you send you binary to apply for certification)

99. Click “Continue”  
100. Locate and run “Application Loader” on the Mac (Minimum version Mac OS X 10.5).This should be located in /Developer/Applications/Utilities/Application Loader.app. if not then you will need to download and install either the latest version of XCode or iOS SDK 3.2  
101. Click “Next” on the application loader welcome screen  
102. Log in with your iTunes Connect log in details  
103. When asked to “Choose Application” select the iScream Candy application from the drop down list and click “Next”  
104. Check that the application metadata information is correct then click “Choose” to browse and select your applications binary zip file  
105. Click the “Send” button to send the application binary to Apple  
106. Once the application has finished uploading you will see a thank you screen, click the “Done” button to close it. If the application was not signed properly you will be notified here

Enabling Push notifications
---------------------------

For some odd reason Apple knocks all our apps back that are not push enabled (even though we do not use it), here’s the additional steps you will need to carry out to enable it.

107. Click “Configure” link next to the app ID for iScream Candy  
108. Tick the “Enable for Apple Push Notification service” check box  
109. At the side of the “Production Push SSL Certificate” click the “Configure” button  
110. A window will pop up, click the “Continue” button  
111. Click the “Choose File” button and select the signing\_request\_dtstro.csr file from earlier  
112. Click the “Generate” button  
113. Click “Continue” button then the “Done” button

And that’s it, you’re app is now in Apple heaven. It usually takes between 7 and 21 days to get approval, less if it is an update.

A few Additional Notes
----------------------

* I asked you to back up your certificates as you generate them earlier because when you install a new version of the Marmalade SDK you may lose them as you uninstall the previous version of the SDK. If you back them up you can simply copy them back into the \Marmalade\5.x\s3e\deploy\plugins\iphone\certificates folder
* Provisioning profiles expire so you need to renew them now and again. Your IOS device will usually remind you when you need to renew a provisioning profile.

Well that’s its for this article. With any luck I have saved some of you much pain and suffering and hopefully a few buckets of tears