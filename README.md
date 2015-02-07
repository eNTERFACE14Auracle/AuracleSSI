# AuracleSSI
Stripped-down fork of the Social Signal Interpretation (SSI) framework used for the eNTERFACE'14 Auracle project experiments.

For more information on the eNTERFACE'14 Auracle project, read: 
Christian Frisson, Nicolas Riche, Antoine Coutrot, Charles-Alexandre Delestage, Stéphane Dupont, Onur Ferhat, Nathalie Guyader, Sidi Ahmed Mahmoudi, Matei Mancas, Parag K. Mital, Alicia Prieto Echániz, François Rocca, Alexis Rochette, Willy Yvart, "Auracle: how are salient cues situated in audiovisual content?", Proceedings of the 10th International Summer Workshop on Multimodal Interfaces ([eNTERFACE'14](http://aholab.ehu.es/eNTERFACE14/)), Bilbao, Spain, 2014, June 9 - July 4 

## Description

The Social Signal Interpretation (SSI) framework offers tools to record, analyse and recognize human behavior in real-time, such as gestures, mimics, head nods, and emotional speech. Following a patch-based design pipelines are set up from autonomic components and allow the parallel and synchronized processing of sensor data from multiple input devices. In particularly SSI supports the machine learning pipeline in its full length and offers a graphical interface that assists a user to collect own training corpora and obtain personalized models. In addition to a large set of built-in components SSI also encourages developers to extend available tools with new functions. For inexperienced users an easy-to-use XML editor is available to draft and run pipelines without special programming skills. SSI is written in C++ and optimized to run on computer systems with multiple CPUs. Binaries and source code are freely available under GPL.

The original Social Signal Interpretation (SSI) framework distribution can be found on:
http://www.openssi.net

See the README file for more information on SSI.

This fork packs the following plugins:

 * `asio`: realtime audio playback
 * `ffmpeg`: video playback
 * `microsoftkinect`: Microsoft Kinect sensor
 * `theyetribe`: Eye Tribe eye tracker sensor


## Compiling

Compiling SSI requires Microsoft Visual Studio, at least version 10 (not the Express Edition).

Compiling the `asio` plugin requires the non-distributable third-party ASIO 2.2 SDK from Steinberg to be [downloaded](http://www.steinberg.net/nc/en/company/developers/sdk_download_portal.html) and extracted in:
  * `plugins/asio/build/dlls/AsioSDK2`

Compiling the `microsoftkinect` plugin requires Microsoft Kinect Developer Toolkit and SDK to be placed in:
  * `plugins/microsoftkinect/build/bin/KinectDeveloperToolkit-v1.6.0-Setup.exe`
  * `plugins/microsoftkinect/build/bin/KinectSDK-v1.6-Setup.exe`

Compiling the `theyetribe ` plugin requires the Eye Tribe SDK to be placed in:
  * `plugins/theeyetribe/Installer/TheEyeTribeSDK-0.9.35-x86.exe`

