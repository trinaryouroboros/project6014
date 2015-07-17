#How to create ship graphics for the game

# Developer's Guide: Creating Ships #

  * 2D or 3D work is okay.
  * Transparencies are okay.
  * Must be in .PNG format



## What's Required ##

### Creating the Ship: ###
  * 16 PNG images of the ship (for each of the 3 sizes).
  * 000 is the first file. It starts off facing 12 o clock (0 degrees).
  * Every image after 000 is rotated 22.5 degrees. (16 x 22.5 = 360 degrees)
Create (or edit) the ship's .ANI file.
  * 015 is the last image file.
  * There are 3 sets of these 16 images: Big, Medium, and small for a total of 48 images.
  * MED is 50% the size of BIG files.
  * SML is 50% the size of MED files.

### Ship Angles ###
```
000 - 0 degrees  (12 o'clock)
001 - 22.5 degrees
002 - 45 degrees
003 - 67.5 degrees
004 - 90 degrees (facing completely and perfectly to the right)
005 - 112.5 degrees
006 - 135 degrees.
007 - 157.5 degrees
008 - 180 degrees (facing perfectly down)
009 - 202.5 degrees
010 - 225 degrees
011 - 247.5 degrees
012 - 270 degrees  (facing perfectly to the left)
013 - 292.5 degrees
014 - 315 degrees
015 - 337.5 degrees
```

### Filenames: ###
Generally this is the name of the ship. Otherwise it's usually the name of the alien. There are 3 views. BIG, MED, and SML.  BIG is the largest size. MED is 50% the size of BIG.  SML is 50% the size of MED.

Each of the 3 sizes of the ship is made up of 16 angles for a total of 48 images.

### Naming Conventions ###

```
Androsynth Guardian:  guardian-big-000.png
Arilou Skiff: skif-big-000.png
Baul: Baul-big-000.png
Chenjesu Broodhome: broodhome-big-002.png
CHMMR Avatar: avatar-big-000.png
Druuge Mauler: mauler-big-000.png
Explorer: explorer-big-000.png
Flagship: flagship-big-000.png
Foon-Foon Typhoon: foonfoon-big-000.png
Human Cruiser: cruiser-big-000.png
Ilwrath Avenger: avenger-big-000.png
ISD: isd-big-000.png
Kohr-Ah Maurader: marauder-big-000.png
Lurg Prawn: lurg-big-000.png
Melnorme Trader: trader-big-000.png
Mmrnmhrm X-Form: xform-big-000.png
Mmrnmhrm X-Form: xform-big-001.png
Mycon Podship: podship-big-000.png
Orz Nemesis: nemesis-big-000.png
Pkunk Fury: fury-big-000.png
Samatra: samatra-big-000.png
Shofixti Scout: scout-big-000.png
Slylandro Probe: probe-big-000.png
Spathi Eluder: eluder-big-000.png
Supox Blade: blade-big-000.png
Syreen Penetrator: penetrator-big-000.png 
Thraddash Torch: torch-big-000.png
Umgah Drone: drone-big-000.png
Ur-Quan Dreadnaught: dreadnaught-big-000.png
Utwig Jugger: jugger-big-000.png
Vux Intruder: intruder-big-000.png
Yehat Terminator: terminator-big-000.png
Zoq-Fot-Pik Stinger: singer-big-000.png
```


## Aligning the Ship ##
### Tools Needed ###
  * The UQM animation tool available here: http://mooses.nl/uqm/tools/
  * Wordpad, or other text editor.

Using the UQM-Animation tool, align the red dot so it's on the pivot point (usually the center) of the ship. You'll need to do change the X,Y coordinates in the ship's .ANI file to do so. These are the last 2 numbers on each line of the .ANI file.