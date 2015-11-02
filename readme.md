** Custom Theme Auto Preview Generator **
This Windows batch file will automagically generate previews for custom 3DS themes (in extdata format). This will use the backgrounds included in the body_LZ.bin file to generate CHMM compatible preview.png files. Alongside the preview.png and icon, it will also generate a visual 'catalog' of your themes (= a folder with correctly named previews of all your themes) for easier selection from PC.

Included is a small programm that I wrote myself (3DSthmXtb, source code included), DSdecmp (by barubary), NConvert (by XnSoftware team), smdhtool (by 3dstools) and batch files to make usage as easy as possible.

This will work on Windows 7/8/8.1 only! Also, it will not process 'solid color themes', however noone makes these, anyways. If you want to discuss this, use the official discussion thread: https://gbatemp.net/threads/release-custom-theme-auto-preview-generator.387711/

** How to use **
* Extract all the files from the attached archive into your theme base folder. Your 'theme base folder' is the folder that contains, inside subfolders, your custom themes. Or, in other words, don't extract it to the folder containing body_LZ.bin, extract it to the folder above that. I guess if you managed to get CHMM running, you understand what is meant.
* To process a single custom theme: Drag and drop the themes folder onto 'process.bat'.
* To process all the themes in the theme base folder: Just run 'process_all_folders.bat'.

** Bonus Folder **
The scripts in the bonus folder require manual work and are intended for use with extracted CIA theme packs. Put the contents of IconConv and TextExtr into the root folder of the decrypted & extracted CIA content 0. You may then use the respective scripts to convert the included theme icons and / or extract the theme descriptions. The alternative process.bat file included will use a icon.png you provide (inside the theme's folders) instead of generating one anew.
