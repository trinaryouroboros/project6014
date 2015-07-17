# Porting changes from UQM into Project 6014 #

## Introduction ##

Project 6014 forked from Ur-Quan Masters on October 17th 2008, from [revision 3093](https://code.google.com/p/project6014/source/detail?r=3093).  Further development has of course occurred in UQM, some of which is useful to us (bug fixes especially).  Recently we have begun to systematically port the changes in UQM after that date to Project 6014.

The process for doing this is as follows.

  1. A branch called uqm-import was created from trunk, containing the repository at the moment it was forked into Project 6014, i.e. it is identical to [revision 3093](https://code.google.com/p/project6014/source/detail?r=3093) from UQM, and contains no Project 6014 changes.
  1. Revisions to UQM trunk from 3094 onwards will be replayed into uqm-import, in order.  No other changes to uqm-import will be made (especially not Project 6014-related changes!), except as necessary to make uqm-import reflect UQM at the given revision as accurately as possible.
  1. When a useful change from UQM is identified, the corresponding revision on uqm-import will be found.  It will then be "cherry picked" from uqm-import into trunk, using a command such as:
> > svn merge -c USEFUL\_CHANGE\_REV ^/branches/uqm-import .
  1. Once the merge has been double checked and committed, this page will be updated to reflect it.  (We don't need much detail, just make sure what's here is consistent with the merge state.)

Suggestions for useful changes to port can be acted on immediately (if you're confident), or discussed on the dev mailing list, or entered into the issue tracker (if it sounds like it involves a medium-to-large amount of work).

N.B. Merging changes involving file renames requires special care, so don't attempt that without discussion.


## Useful links ##

The original log for UQM: http://sc2.svn.sourceforge.net/viewvc/sc2/trunk/sc2/?view=log

Corresponding log for uqm-import: http://code.google.com/p/project6014/source/list?path=/branches/uqm-import

Original Project 6014 issue for porting changes: http://code.google.com/p/project6014/issues/detail?id=21 (now complete)

The Subversion replay script can be found at: http://code.google.com/p/ejrh/source/browse/trunk/utils/replay.py

Usage is:
  1. svn checkout https://project6014.googlecode.com/svn/branches/uqm-import/ uqm-import
  1. In the uqm-import working copy, run: `replay.py PASSWORD`

Password is found on https://code.google.com/hosting/settings


## Current state ##

(As at September 18 2011.)

All extant revisions from UQM have been ported; the most recent is 3695 on 2011-09-09 21:36:34.

Large changes involving content file renames have been merged into trunk, up to [r628](https://code.google.com/p/project6014/source/detail?r=628).  I believe all useful changes up to that point have been ported.

Revisions after that should be scanned for other useful changes.