# Versioning #

Each release has a version number, of the form X.Y.Z.  As at writing, the most recent release was 0.2.1.  Y is incremented when we have added lots of features and want to release a new milestone.

Z is incremented when we're fixing bugs and releasing moderately soon after the previous X.Y release (it can also contain new features).  But Z does not get incremented in response to fixes as part of the release process.

X will be incremented to 1 if and when we reach feature completeness.

# Executable packages #

These need to be built for:

  * Windows - NSIS installer.
  * OSX - installer.
  * Linux - 32- and 64-bit .deb files for Ubuntu/Mint.


TODO Fill in details for each platform


# Content files #

The content files for each release are:

  * P6014-X.Y.Z-prv-content.uqm
  * P6014-X.Y.Z-prv-voice.uqm
  * P6014-X.Y.Z-prv-3domusic.uqm
  * P6014-X.Y.Z-prv-hires2x.uqm
  * P6014-X.Y.Z-prv-hires4x.uqm

For Linux these are also packaged as .deb files.

The last two benefit from compression by the compress\_uqm.py tool.  Steps for this:

  1. Make a copy of the content directory to compress, e.g. copy addons/hires4x to tmp/hires4x.
  1. Run `compress_uqm tmp/hires4x --apply`.  This will compress that directory in place.
  1. Package the directory normally into a UQM file.
  1. Quickly test that the UQM file works by running the game with it.  It should be obvious if there are problems just starting a new game.

(See [issue 57](http://code.google.com/p/project6014/issues/detail?id=57) for additional comments).

# Tagging the source #

Once the packages (installers and content) are built, tested, and finalised, the source should be tagged.  Use a command like the following:

```
svn copy -r REV https://project6014.googlecode.com/svn/trunk/ https://project6014.googlecode.com/svn/tags/X.Y.Z -m 'Make tag for release X.Y.Z.'
```

Where REV is the revision (as near as can be determined) that was packaged.  Use the [list of changes](http://code.google.com/p/project6014/source/list) to double check.  X.Y.Z is the version number as explained above.

# Uploading the files #

Upload to Google Code and Joris's mirror.

N.B. Google Code has a size limit of 200MB.  This means the hires4x file in particular may need to be split when hosted on that location.

Update the [Downloads](Downloads.md) wiki page.

# Announcements #

This should be done with some coordination.  If possible,

  1. Do it when the above steps are mostly finalised.
  1. Agree on the text of the announcements with the team.

The announcement text can be different for each location.  It should contain a link to the [Downloads](Downloads.md) page, and a short list of new features.  Depending on the location (e.g. in general discussion forums) it may contain some project background.

Announcements should be made in these places at the very least:

  1. Update the [Home page](http://code.google.com/p/project6014/) (using the Administer tab)
  1. Start announcement topics on the two forums:
    * [UQM Forums](http://forum.uqm.stack.nl/index.php?board=1.0) -- [example](http://forum.uqm.stack.nl/index.php?topic=5178.0).
    * [Pages of Now and Forever](http://www.star-control.com/community/viewforum.php?f=9) -- [example](http://www.star-control.com/community/viewtopic.php?f=9&t=668).
  1. Post a message to the [project6014-users](http://groups.google.com/group/project6014-users/) mailing list.

We can also make announcements at places like Reddit's [r/games](http://www.reddit.com/r/Games/) for further publicity.  Here's an [example](http://www.reddit.com/r/Games/comments/omf3g/star_control_2_mod_project_6014_demo_021_released/).