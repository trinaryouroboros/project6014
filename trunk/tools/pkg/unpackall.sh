#!/usr/bin/bash
# Extracts 3do starcon.pkg file, using information from the new tree
# as how to name the files.
# By Serge van den Boom, 2002-10-23
# The GPL applies.

if [ $# -ne 4 ]; then
	echo "Usage: unpackall.sh <3docdpath> <uqmcontentpath> <respath> <destpath>"
	echo -n "3docontentpath is the path to the starcon2/ dir from the "
	echo "original SC2 CD-ROM."
	echo -n "uqmocntentpath is the path to the content/ dir from the UQM cvs "
	echo "tree."
	echo "respath is the path to the .res files generated by collectres.sh"
	echo "destpath is the path to place the extracted files"
	exit 1;
fi

CDPATH="${1%/}"/
UQMPATH="${2%/}"/
RESPATH="${3%/}"/
DESTDIR="${4%/}"/
TEMPDIR=/tmp/unpacksc2.$$/

UNPKG=./unpkg
PARSERES=./parseres

if [ ! -e "$CDPATH"starcon.pkg ]; then
	echo "\"${CDPATH}\" is not a valid 3DO SC2 content dir"
	echo "It should be the dir starcon2/ from the original CD-ROM."
	exit 1
fi

if [ ! -e "${UQMPATH}starcon.ndx" ]; then
	echo "\"${UQMPATH}\" is not a valid 3DO SC2 content dir"
	exit 1
fi

if [ ! -e "${RESPATH}starcon.pkg.pc.res" -o \
		! -e "${RESPATH}starcon.pkg.3do.res" ]; then
	echo "\"${RESPATH}\" does not contain the needed .res files"
	exit 1
fi

if [ ! -d "$DESTDIR" -o ! -w "$DESTDIR" ]; then
	echo "\"${DESTDIR}\" is not a good destination"
	exit 1
fi

mkdir "$TEMPDIR"
if [ $? -ne 0 ]; then
	echo "Could not create temp dir '$TEMPDIR'"
	exit 1
fi

export CDPATH UQMPATH RESPATH DESTDIR TEMPDIR 
unpackone() {
	PACKAGECD="$1"
	PACKAGEUQM="$2"
	SRCPATH="$3"

	echo "$@"
	echo -n "Package $PACKAGECD: "
	$UNPKG -l "${UQMPATH}$PACKAGEUQM" > "${TEMPDIR}${PACKAGECD}.fil"

	echo "unpacking... "
	$UNPKG -o "${DESTDIR}" "${SRCPATH}$PACKAGECD"

	echo "renaming files... "
	local RES FILE
	while read RES FILE; do
		RES="${RES#0x}"
		echo "- ${DESTDIR}${RES} ==> ${DESTDIR}$FILE"
		mv -- "${DESTDIR}${RES}" "${DESTDIR}$FILE"
	done < "${RESPATH}${PACKAGECD}.3do.res"

	while read RES FILE; do
		RES="${RES#0x}"
		echo "- ${DESTDIR}$FILE ==> ${DESTDIR}$RES"
		mv -- "${DESTDIR}$FILE" "${DESTDIR}$RES"
	done < "${RESPATH}${PACKAGECD}.pc.res"

	while read RES FILE; do
		RES="${RES#0x}"
		DIR="${FILE%/*}"
		if [ "$DIR" != "$FILE" -a ! -d "${DESTDIR}${DIR}" ]; then
			mkdir -p -- "${DESTDIR}${DIR}"
		fi
		echo "- ${DESTDIR}${RES} ==> ${DESTDIR}${FILE}"
		mv -- "${DESTDIR}${RES}" "${DESTDIR}${FILE}"
	done < "${TEMPDIR}${PACKAGECD}.fil"

	rm -- "${TEMPDIR}${PACKAGECD}.fil"
	echo
}

unpackone starcon.pkg starcon.ndx "$CDPATH"

for PACKAGE in androsyn.shp arilou.shp blackurq.shp chenjesu.shp chmmr.shp \
		druuge.shp human.shp ilwrath.shp melnorme.shp mmrnmhrm.shp \
		mycon.shp orz.shp pkunk.shp shofixti.shp slylandr.shp spathi.shp \
		supox.shp syreen.shp thradd.shp umgah.shp urquan.shp utwig.shp \
		vux.shp yehat.shp zoqfot.shp \
		probe.sc2 lastbat.sc2 sis.dat \
		arilou.con blackur.con chmmr.con comandr.con druuge.con \
		ilwrath.con melnorm.con mycon.con orz.con pkunk.con shofixt.con \
		slyhome.con slyland.con spathi.con supox.con syreen.con \
		talkpet.con thradd.con umgah.con urquan.con utwig.con vux.con \
		yehat.con zoqfot.con; do
	unpackone "$PACKAGE" "$PACKAGE" "$DESTDIR"

	# Remove the packed files which came from starcon.pkg once
    # we unpacked them
	rm -- "${DESTDIR}${PACKAGE}"
done

rmdir "$TEMPDIR"

