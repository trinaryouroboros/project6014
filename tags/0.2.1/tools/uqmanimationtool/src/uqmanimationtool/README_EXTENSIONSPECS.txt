UQMAnimationTool adds some extra features to regular UQM .ani files via an 'extension'.
This 'extension' saves extra data in .ani files by embedding them in comments which aren't
parsed by UQM at all. An 'at' sign (@) is added to the start of the comment character (#).
UQMAnimationTool parses all comments in .ani files that start with "#@".


Currently, two extra things are saved in .ani files:
-Which animations are defined (index number, type, name)
-Which frames belong to which animation, and what 'duration' they have.


Note that UQMAnimationTool reads all lines starting with "#@", stores them in memory, and
parses them AFTER having parsed the rest of the file. As such, lines starting with "#@" can
be placed anywhere, and can still reference other non-UQMAnimationTool lines in the file.
Also note that all lines starting with "#@" are parsed sequentially, so they DO have an
order in which they have to be written. As frames belong to animations (and are linked to
them via an ID number), animations have to be defined BEFORE (!) frames are being assigned
to them.



Animation definitions are specified as follows:
#@ anidef $animationID $animationTYPE [$animationNAME]
(note that the [] backets between $animationNAME are required, and are not some fancy markup)
 animationID: Integer. Specifies the animation numerical index number. 
    Should ideally be '0' for the first animation, '1' for the second, et cetera.
    Can't be negative.
 animationTYPE: String. Specifies what animation type this animation is defined as.
    Can be one of the following (case insensitive, without quotes): 
      "BACKGROUND" - Background animation. Frames belonging to these animations are ALWAYS visible.
         (Animations of type 'background' aren't actually animations at all; they're only really
          there to specify which images should always be visible. Usually, this is the
          background image of a communication screen)
      "CIRCULAR" - Circular animation.
      "YO_YO" - Yo-yo animation.
      "TALK" - Animation that's displayed whenever the alien is talking.
      "RANDOM" - Animation that consists of randomly selected frames.
 animationNAME: String. Used for the GUI tool to list the animations by name, and to save
    some extra descriptive comments in the SMIL exporter.
    Not used for anything else. Can't contain '[' and ']' characters, but this
    may change in the future (as I think I just need to change the regex to greedy)

Example:
#@ anidef 0 BACKGROUND [Static background animation]
(This creates a new animation with id #0 of type BACKGROUND with name "Static background animation")



Animation frames are specified as follows:
#@ aniframe $animationID $frameDuration [$frame]
(note that the [] backets between $frame are required, and are not some fancy markup)
 animationID: Integer. Specifies the animation numerical index number this frame is assigned to.
    Can't be negative. There HAS to be an 'anidef' statement for the specified animationID *BEFORE*
    this line is being used.
 frameDuration: Double (double pecision floating-point). Specifies for how long (in seconds)
    the specified frame will be visible during the animation. Since animations of type BACKGROUND
    do not have a duration, this can be negative. However, it shouldn't ever be negative for any
    animation of a type different from BACKGROUND.
 frame: String. Essentially, this specifies what frame we're talking about here; which frame has
    to be assigned to the animation. The string's format is equal to UQM's .ani file format 
    ("chmmr.84.png -1 10 -235 -26"). Note that this string has to appear somewhere else in the .ani
    file (after all, a frame that doesn't exist can't ever be assigned to an animation). If it doesn't,
    the line is ignored completely (yet a warning is given in the animation tool).

Example:
#@ aniframe 0 0.1 [chmmr.0.png -1 10 0 0]
(This adds frame "chmmr.0.png -1 10 0 0" to animation #0)