# Introduction #

Ok, ya dirty little scurvy-eaten wretches. This page is all about adding a new race into the game.


I wrote this because I've now done it 5 times, each time having a couple of months in between - just enough to make one forget completely how it was actually done, resulting in several extra hours of programming 'fun'.

Any way, enough talk, let's walk the walk.

# Code: #

_1. Races.h:_
a) Add a NEWRACE\_ID (into the SPECIES\_ID enum list), preferably after ISD\_ID, and before SLYLANDRO\_KOHRAH\_ID.

b) Add a NEWRACE\_SHIP (into the enum list which ends in NUM\_AVAILABLE\_RACES.)
Put it in the same position as in the SPECIES\_ID enum list.

c) Add your new race's CONVERSATION, SHIP, SHIP COST, IP SPEED, HYPERSPACE SPEED, IP ENCOUNTER PERCENT, HYPERSPACE ENCOUNTER PERCENT, TASKFORCE SHIP STRENGTH BOUNDS, and RACE COLOR into the  corresponding #define lists.

Most of these things are pretty self-explanatory. The NEWRACE\_ID is used in most every place as a "master" numbering of the races.

(Note: If you add the new race after SLYLANDRO\_KOHRAH\_ID, you're gonna have to also muck around the master.c file. And if you add the new race after LAST\_MELEE\_ID, you will have to mess with InitGameStructures in globdata.c also.)


_2. globdata.h:_
Add NEWRACE\_CONVERSATION to globdata.h's CONVERSATION enum list (with the same name you added it to races.h's RACE\_COMMUNICATION define list).

_2b. commglue.h:_
In commglue.h, declare the init\_newrace\_comm function you'll define in comm/newrace/newracec.c.

_2c. commglue.c:_
Add the case NEWRACE\_CONVERSATION to commglue.c's init\_race switch function.


_3. dummy.c:_
Add your NEWRACE\_CODE\_RES into the ShipCodeRes enum list. Confusingly enough, the order of the races in dummy.c differs from the lists in races.h: in dummy.c the original SCII races are ordered by alphabet. However, always add your new races AFTER all the original SCII races regardless of the alphabetic order, don't mix original and new races.
Otherwise the universe explodes.

_4. dummy.h:_
Add "#include "ships/newrace/newrace.h"
This makes dummy.c see which source files your NEWRACE\_CODE\_RES points to.

_5. Loadship.c:_
Add a NEWRACE\_CODE into the code list. Put it into the same position as in races.h's lists.
This list acts as a "link" between the numberings different in dummy.c and races.h.

_6. corecode.h:_
Add #include "ships/newrace/icode.h".
This makes loadship.c see which source files your NEWRACE\_CODE points to.

_7. master.c:_
Edit LoadMasterShipList in such way that your new race is loaded into the supermelee ship pick
list as one of the last races (see examples in the file). This ensures your race isn't loaded
in place of the original SCII ones.

_8. grpinfo.c:_
Add your NEWRACE\_SHIP into the BuildGroups function. Put it into the same position as in races.h's
lists. This ensures your race's homeworld/other important systems work OK.

_9. supermelee/melee.h:_
To make the (normally 5x5) ship selection grid in supermelee house more ships, edit the number of NUM\_PICK\_COLS and NUM\_PICK\_ROWS to suit your needs. You also need to photoshop a new grid graphic to make the ships fit :)

_10. New files, ship:_
Add your new ship files by creating folder sc2/src/uqm/ships/newrace. Remember to edit the makeinfo files in sc2/src/uqm/ships/newrace and sc2/src/uqm/ships to compile your new files. (newrace.c, newrace.h, Makeinfo, resinst.h, plus the .h files the gen\_refiles.py utility SHOULD create.)

_11. New files, comm:_
Add your new comm files into sc2/src/uqm/comm/newrace. Edit the makeinfo files also, as in the previous item. (newracec.c, strings.h, Makeinfo, resinst.h, plus the .h files the gen\_refiles.py utility SHOULD create.)

# Contents: #

_12. New content files, ship:_
Add your race's ship's new graphics, sound files and captainnames to sc2/content/base/ships

_13. New content files, comm:_
Add your race's comm graphics, music and dialogue into sc2/content/base/comm

_14. resources.csv:_
Then, let's modify the resources.csv file in tools/resmap.
a) Add your race's communication resource identifiers:
(NEWRACE\_MUSIC,  NEWRACE\_FONT, NEWRACE\_CONVERSATION\_PHRASES,  NEWRACE\_COLOR\_MAP and NEWRACE\_PMAP\_ANIM)

b) Add your race's ship resource identifiers:
(NEWRACE\_CODE,
NEWRACE\_VICTORY\_SONG,
NEWRACE\_RACE\_STRINGS,
NEWRACE\_SHIP\_SOUNDS,
NEWRACE\_ICON\_MASK\_PMAP\_ANIM,
NEWRACE\_MICON\_MASK\_PMAP\_ANIM,
NEWRACE\_BIG\_MASK\_PMAP\_ANIM,
NEWRACE\_MED\_MASK\_PMAP\_ANIM,
NEWRACE\_SML\_MASK\_PMAP\_ANIM,
NEWRACEPRIMARY\_BIG\_MASK\_PMAP\_ANIM,
NEWRACEPRIMARY\_MED\_MASK\_PMAP\_ANIM,
NEWRACEPRIMARY\_SML\_MASK\_PMAP\_ANIM,
NEWRACESECONDARY\_BIG\_MASK\_PMAP\_ANIM,
NEWRACESECONDARY\_MED\_MASK\_PMAP\_ANIM,
NEWRACESECONDARY\_SML\_MASK\_PMAP\_ANIM and
NEWRACE\_CAPTAIN\_MASK\_PMAP\_ANIM)

-Remember to make these identifiers to point into your new files.

-Remember to change also the 'ship.xxx.graphics' in addition to the NEWRACE\_XXX constants.

-Remember to make your NEWRACE\_CODE to correspond to its location in the DUMMY.C's list.

-Remember, after you've changed your race's ship code, also increment those ships' RACE\_CODEs that come after your ship.

_15. gen\_resfiles.py:_
Run the gen\_resfiles utility at the tools/resmap folder to propagate the changes to resources.csv
to all the necessary .h files in the source code.

_16. resinst.h:_
In sc2/src/uqm/ships/newrace, copy the resource identifiers from igfxres.h, imusicre.h, isndres.h
and istrtab.h to resinst.h.

_17. COMPILE!!!_


# Epilogue #

Well, my young padawan. Now that I've guided you through this little exercise we can all agree that this has been as fun as pulling rotten, inflamed, plague-ridden wisdom teeth that have their twisted roots stuck so deep that they're tickling your anal canal, if you know what I mean.