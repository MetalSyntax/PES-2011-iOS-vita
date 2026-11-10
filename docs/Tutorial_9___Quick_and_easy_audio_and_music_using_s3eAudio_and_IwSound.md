# Tutorial 9 – Quick and easy audio and music using s3eAudio and IwSound

Original URL: http://www.drmop.com/index.php/2011/10/07/quick-and-easy-audio-and-music-using-s3eaudio-and-iwsound/

This tutorial is part of the Marmalade SDK tutorials collection. To see the tutorials index [click here](http://www.drmop.com/index.php/marmalade-sdk-tutorials/ "Marmalade SDK tutorials index")

Today we are going to cover playing sound and music the very quick and very easy way using the Marmalade SDK. As usual if you just want the associated article source code then you can [Download it from here](http://www.drmop.com/wp-content/uploads/2011/10/Audio.zip). If you want the details on sound and audio with Marmalade then please read on.

I like to separate audio playback into two distinct categories, which are music and sound (or sound effects). The Marmalade SDK has two systems for playing audio:

* S3E Audio – The S3E audio system uses the devices built-in codec’s to play back compressed audio files such as MP3. QCP and AAC (good for playing background music)
* S3E Sound – The S3E sound system uses software mixing to play multiple uncompressed sounds simultaneously (good for playing back sound effects, such as button clicks or in-game sound effects)

Now a little more detail

S3E Audio
---------

S3E audio uses the devices built in audio codec’s to play back compressed audio files such as MP3. QCP and AAC, although you do need to query the system to find out if the audio format is supported using  
 s3eBool s3eAudioIsCodecSupported ( s3eAudioCodec codec )

Valid codecs include:

* S3E\_AUDIO\_CODEC\_MIDI – MIDI files.
* S3E\_AUDIO\_CODEC\_MP3 – MP3 files.
* S3E\_AUDIO\_CODEC\_AAC – Raw AAC files.
* S3E\_AUDIO\_CODEC\_AACPLUS – AAC plus files.
* S3E\_AUDIO\_CODEC\_QCP – QCP files.
* S3E\_AUDIO\_CODEC\_PCM – PCM files.
* S3E\_AUDIO\_CODEC\_SPF – SPF files.
* S3E\_AUDIO\_CODEC\_AMR – AMR files.
* S3E\_AUDIO\_CODEC\_MP4 – MP4 or M4A files with AAC audio.

This s3e audio system can play back audio from either a file or a memory buffer. Note when you play back a file you cannot play a file that is within a compressed derbh file as the device does not know about Marmalades derbh format. If you are using auto derbh to auto compress all of your data on deployment then you should include the audio file as an asset in your MKB’s asset section and not inside a resource group.

Here is a summary of a number of important s3e audio functions:

* s3eBool s3eAudioIsPlaying ()
* s3eResult s3eAudioPause ()
* s3eResult s3eAudioPlay (const char \*filename, uint32 repeatCount)
* s3eResult s3eAudioPlayFromBuffer (void \*buffer, uint32 bufferLen, uint32 repeatCount)
* s3eResult s3eAudioResume ()

Its pretty obvious what these functions are doing so I wont go into any details other than to mention that passing a value of 0 to repeatCount causes the music to repeat playback forever.

S3E Sound
---------

I’m not actually going to cover S3E in much depth as I did promise quick and easy sound, instead I will use the example sound engine that ships with Marmalade (located in $MARMALADE\_ROOT/examples/SoundEngine). SoundEngine sits on top of S3E Sound, so if you want to see the internals of the code it’s right there for you to study. If you want to implement your very own audio engine then I suggest you start by checking out Marmalade’s s3e sound docs [here](http://www.madewithmarmalade.com/devnet/docs#/api/s3eapidocumentation/s3esound/s3esoundoverview.html. "Marmalade SDK s3e Sound Overview")

Marmalade’s Sound Engine (IwSound)
----------------------------------

Marmalade’s IwSound example sound engine lets you easily organise sounds into resource groups and then playback and control these sound effects very easily in-game. Even though IwSound is an example sound engine it is very capable and covers much of which most developers need. In my opinion it should be included as a standard Marmalade SDK module.

Before we begin looking at the code we will take a quick look at some of the important classes that make up IwSound.

* CIwSoundData – This class represents the actual data of your sounds (you do not need to deal with this class directly)
* CIwSoundSpec – This class describes a sound, think of it as a sound material that specifies details about your sound data and how you want it played. You create instances of sounds from their sound specs.
* CIwSoundInst – This class represents a playing instance of a sound spec, a new CIwSoundInst is returned each time you play a sound from a sound spec (internally these are pooled to reduce memory fragmentation). Its worth noting here that you can only play a finite number of sound effects simultaneously.
* CIwSoundManager – The sound manager, used to update the sound engine and gives access to such things as master volume, pan and pitch etc.. The sound manager also uses the following app.icf setting to allow you to set the maximum number of simultaneous sound effects that can be played:

[SOUND]  
MaxChannels=16

Ok, now we have a basic understanding of the classes we are going to use, lets take a look at how to play something.

```
> // Get a pointer to the sound group
> CIwResGroup* sound_group = IwGetResManager()->GetCurrentGroup();
> // Get the sound spec for our silly_sound sound effect
> CIwSoundSpec* SoundSpec = (CIwSoundSpec*)sound_group->GetResNamed("silly_sound", IW_SOUND_RESTYPE_SPEC);
> // Play the sound effect
> CIwSoundInst* SoundInstance = SoundSpec->Play();
```

And that’s all you need to load and play sounds from a group

If you would like to change attributes of the played back sound effect, such as its volume, frequency etc.. then you can call the following functions on the sound instance:

* void SetVol(iwsfixed vol);
* void SetPan(iwsfixed pan);
* void SetPitch(iwsfixed pitch);
* void Stop();
* void Pause();
* void Resume();

Note the vol, pan and pitch are fixed point parameters (IW\_GEOM\_ONE representing the value of 1.0f)

Also note that you should never delete sound instances as they are managed by the IwSoundManager. Sound specs are managed by the resource system so do not delete those either.

Lastly, the IwSound engine manager Update method needs to be called every frame:

IwGetSoundManager()->Update();

Creating and organising sound effects with IwSound
--------------------------------------------------

Ok, we have looked at the basics of playing sound using IwSound, now we will look at how to organise our sounds into groups to allow the resource manager to manage them for us. If you do not know what the resource manager is then take a look at one of my previous blogs on the subject [here](http://www.drmop.com/index.php/2011/10/01/marmalade-sdk-tutorial-marmalades-resource-management-system/ "Tutorial 8 – Marmalade’s resource management system")

Firstly we need to add the WAV files that represent our individual sound files to the group like this:

```
> // Sound sample WAV files
> "./silly_sound.wav"
> "./explosion.wav"
```

Next we create a CIwSoundSpec for each sound effect that we want to play:

```
> // Create sound specs (can be thought of as sound materials)
> CIwSoundSpec
> {
>     name        "explosion"        # The name we want to use to refer to this sound effect in our code
>     data        "explosion"        # The wav file name (without .wav)
>     vol         0.7                # Default volume to be played at
>     loop        false              # Do we want this sound effect to play forever?
> }
> CIwSoundSpec
> {
>     name        "silly_sound"        # The name we want to use to refer to this sound effect in our code
>     data        "silly_sound"        # The wav file name (without .wav)
>     vol         0.7                  # Default volume to be played at
>     loop        false                # Do we want this sound effect to play forever?
> }
```

Finally, we create a sound group to contain all of our sound specs:

```
> // Create a sound group to contain all of our sound specs
> CIwSoundGroup
> {
>     name        "sound_effects"    # Name of our sound group
>     addSpec     "silly_sound"      # Add the silly_sound sound spec to our sound group
>     addSpec     "explosion"        # Add the explosion sound spec to our sound group
> }
```

To save you hours of pulling your hair out and trying out different WAV formats, it’s worth noting that IwSound supports mono PCM (8 and 16 bit) and mono ADPCM (IMA but not Microsoft’s) WAV formats.

And finally finally, you need to add this code somewhere at the start of your main() function so that the resource manager can convert your WAV files into a usable format when you run the resource build mode (Win32 debug build will build resources used by the resource manager)

```
> #ifdef IW_BUILD_RESOURCES
>     IwGetResManager()->AddHandler(new CIwResHandlerWAV);
> #endif
```

This line basically adds a WAV resource handler to the resource management system so that when it loads your .WAV files during resource build mode it knows how to convert them.

What’s changed in our example code
----------------------------------

We are basing our new Audio example project on the previous Resources example code. Firstly we will deal with what’s changed in our project MKB file, then we will deal with changes to Main.cpp source code and Level1.group resource group file.

If you view the Audio.mkb project file you will notice that we have added a module\_path to the options section of our MKB:

```
> options
> {
>     module_path="$MARMALADE_ROOT/examples"
> }
```

This tells Marmalade where to look for modules. In our case we are directing Marmalade to look in the examples folder within the SDK as this is where the IwSound engine is located

Next we have added the SoundEngine module to our sub projects section to enable us to use the IwSound code:

```
> subprojects
> {
>     iw2d
>     iwresmanager
>     SoundEngine
> }
```

And lastly we add our music.mp3 test music file to the assets section of our MKB to make it available when deploying to devices:

```
> assets
> {
>     (data)
>     music.mp3
> 
>     (data-ram/data-gles1, data)
>     Level1.group.bin
> }
```

Now lets turn to Main.cpp to see what has changed. Firstly we included the header file for IwSound:

```
> #include "IwSound.h"
```

Then we initialise the IwSound module using:

```
> // Init IwSound
> IwSoundInit();
```

We also added some code to ensure that our WAV resources get built during a resource build Win32 debug build run:

```
> #ifdef IW_BUILD_RESOURCES
>     // Tell resource system how to convert WAV files
>     IwGetResManager()->AddHandler(new CIwResHandlerWAV);
> #endif
```

In addition, we added code to play our test music.mp3 file (Remember, s3e Audio uses the devices audio player so we firstly have to check that the device supports the MP3 codec):

```
> // Play some MP3 music using s3e Audio (if the codec is supported)
> if (s3eAudioIsCodecSupported(S3E_AUDIO_CODEC_MP3))
>     s3eAudioPlay("music.mp3", 1);
```

Next we tell IwSound to update every frame by placing a call to IwGetSoundManager in our main loop:

```
> // Update Iw Sound Manager
> IwGetSoundManager()->Update();
```

This ensures that the sound system and sound instances etc.. get updated regularly. Note that if you find that you play a few sound effects and then no more will play then you have missed out this line!

And now the juicy bit, here we find our explosion sound effect within the Level1 group and play it:

```
> if (!PrevTouched)
> {
>     CIwSoundSpec* SoundSpec = (CIwSoundSpec*)Level1Group->GetResNamed("explosion", IW_SOUND_RESTYPE_SPEC);
>     CIwSoundInst* SoundInstance = SoundSpec->Play();
> }
```

And finally before we exit the demo we shut down IwSound using:

```
> // Shutdown IwSound
> IwSoundTerminate();
```

Ok, so now we’ve been through the changes to Main.cpp lets take a look at the changes to our resource group file Level1.group.

The first thing you will notice is that it contains a lot more stuff, that is mainly down to the fact that we need to define CIwSoundSpec for our sound effect as well as a CIwSoundGroup to include our sound specs in.

You will notice that the first addition is our sound effect wav file:

```
> // Sound sample WAV files
> "./explosion.wav"
```

Next we create a sound spec (material) for our sound effect so that it can be played in-game:

```
> // Create sound specs (can be thought of as sound materials)
> CIwSoundSpec
> {
>     name        "explosion"        # The name we want to use to refer to this sound effect in our code
>     data        "explosion"        # The WAv file name (without .wav)
>     vol         0.7                # Default volume to be played at
>     loop        false              # Do we want this sound effect to play forever?
> }
```

Lastly, we create a group to contain our sound spec:

```
> // Create a sound group to contain all of our sound specs
> CIwSoundGroup
> {
>     name         "sound_effects"    # Name of our sound group
>     maxPolyphony 8                  # Maximum sounds that can be played simultaneously
>     killOldest   false              # Tell system not to stop the oldest sound effects from playing if we run out of channels
>     addSpec      "explosion"        # Add the explosion sound spec to our sound group
> }
```

Ok. that’s it from me for now on audio, hopefully you find enough info in this article and source to be able to add sound and music to your games and apps. You can download the associated [Audio project source code from here](http://www.drmop.com/wp-content/uploads/2011/10/Audio.zip)

I will update this blog article in the near future to act as a full reference for IwSound

Happy coding and don’t play with a grenade that has no pin!