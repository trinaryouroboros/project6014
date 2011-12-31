/*
Copyright (C) 2011 Joris van de Donk

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

Joris van de Donk - joris@mooses.nl
 */

package uqmstarmap;

import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.util.HashMap;
import java.util.Locale;

/**
 *
 * @author joris
 */
public class Star {

    public int x_coord;
    public int y_coord;
    public int startype;
    public int starcolor;
    public int owner;
    public int alien_presence;
    public int name_prefix;
    public String name_constellation;
    private StarmapManager manager;

    public Star(int x_coord, int y_coord, int startype, int bodycolor, int owner, int alien_presence, int name_prefix, int name_postfix, StarmapManager manager) {
        this.x_coord = x_coord;
        this.y_coord = y_coord;
        this.startype = startype;
        this.starcolor = bodycolor;
        this.owner = owner;
        this.alien_presence = alien_presence;
        this.name_prefix = name_prefix;
        this.manager = manager;
        name_constellation = getStarConst(name_postfix);
    }

    public Star(int x_coord, int y_coord, int startype, int starcolor, int name_prefix, String name_constellation, StarmapManager manager) {
        this.x_coord = x_coord;
        this.y_coord = y_coord;
        this.startype = startype;
        this.starcolor = starcolor;
        this.name_prefix = name_prefix;
        this.name_constellation = name_constellation;
        this.manager = manager;
        this.owner = -1337;
        this.alien_presence = -1337;
    }

    public static Star getInstanceFromMapTool(String maptoolFormat, StarmapManager manager) {
        //Favonius,C,520.7,8.3,R,1
        //Electra,*,425.6,12.2,O,1
        try {
            String[] splitted = maptoolFormat.split(",");
            if (splitted.length < 6) {
                System.out.println("Ignored: " + maptoolFormat);
                return null; // can't parse this
            }
            String constName = splitted[0];
            String Sprefix = splitted[1];
            String SdX = splitted[2];
            String SdY = splitted[3];
            String Sstarcolor = splitted[4];
            String Sstartype = splitted[5];

            int startype = Integer.valueOf(Sstartype) - 1;
            int x_coord = (int) (Double.valueOf(SdX) * 10);
            int y_coord = (int) (Double.valueOf(SdY) * 10);
            int starcolor = -1;
            for (Integer i : getColMap().keySet()) {
                if (getColMap().get(i).equals(Sstarcolor)) {
                    starcolor = i;
                }
            }
            int prefix = -1;
            for (Integer i : getPrefixMap().keySet()) {
                if (getPrefixMap().get(i).equals(Sprefix)) {
                    prefix = i;
                }
            }

            return new Star(x_coord, y_coord, startype, starcolor, prefix, constName, manager);
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    @Override
    public String toString() {
        return "STAR:[x_coord=" + x_coord + ", y_coord=" + y_coord + ", startype=" + startype + ", bodycolor=" + starcolor + ", owner=" + owner + ", alien_presence=" + alien_presence + ", name_prefix=" + name_prefix + ", name_constellation=" + name_constellation + "]";
    }

    public String toMapToolString() {
        return name_constellation + "," + getPrefixMap().get(name_prefix) + "," + getDecimalFormat().format((double) ((double) x_coord / 10)) + "," + getDecimalFormat().format((double) ((double) y_coord / 10)) + "," + getColMap().get(starcolor) + "," + (startype + 1);
    }

    public int getAlien_presence() {
        return alien_presence;
    }

    public void setAlien_presence(int alien_presence) {
        this.alien_presence = alien_presence;
    }

    public int getName_prefix() {
        return name_prefix;
    }

    public String getName_constellation() {
        return name_constellation;
    }

    public void setName_prefix(int name_prefix) {
        this.name_prefix = name_prefix;
    }

    public int getOwner() {
        return owner;
    }

    public void setOwner(int owner) {
        this.owner = owner;
    }

    public int getStartype() {
        return startype;
    }

    public void setStartype(int startype) {
        this.startype = startype;
    }

    public int getX_coord() {
        return x_coord;
    }

    public void setX_coord(int x_coord) {
        this.x_coord = x_coord;
    }

    public int getY_coord() {
        return y_coord;
    }

    public void setY_coord(int y_coord) {
        this.y_coord = y_coord;
    }

    public int getStarcolor() {
        return starcolor;
    }

    public void setStarcolor(int starcolor) {
        this.starcolor = starcolor;
    }
    private static HashMap<Integer, String> colmap;

    private static HashMap<Integer, String> getColMap() {
        if (colmap == null) {
            HashMap<Integer, String> lolMap = new HashMap<Integer, String>();
            lolMap.put(0, "B");
            lolMap.put(1, "G");
            lolMap.put(2, "O");
            lolMap.put(3, "R");
            lolMap.put(4, "W");
            lolMap.put(5, "Y");
            lolMap.put(6, "C");
            lolMap.put(7, "P");
            lolMap.put(8, "V");
            colmap = lolMap;
        }
        return colmap;
    }
    private static DecimalFormat dformat;

    private static DecimalFormat getDecimalFormat() {
        if (dformat == null) {
            dformat = new DecimalFormat("#.#");
            dformat.setMinimumFractionDigits(1);
            dformat.setDecimalFormatSymbols(DecimalFormatSymbols.getInstance(Locale.US));
        }
        return dformat;
    }
    private static HashMap<Integer, String> prefixMap;

    public static HashMap<Integer, String> getPrefixMap() {
        if (prefixMap == null) {
            prefixMap = new HashMap<Integer, String>();
            prefixMap.put(0, "*");
            prefixMap.put(1, "A");
            prefixMap.put(2, "B");
            prefixMap.put(3, "C");
            prefixMap.put(4, "D");
            prefixMap.put(5, "E");
            prefixMap.put(6, "F");
            prefixMap.put(7, "G");
            prefixMap.put(8, "H");
            prefixMap.put(9, "I");
            prefixMap.put(10, "J");
            prefixMap.put(11, "K");
            prefixMap.put(12, "L");
            prefixMap.put(13, "M");
            prefixMap.put(14, "N");
            prefixMap.put(15, "O");
            prefixMap.put(16, "P");
            prefixMap.put(17, "Q");
            prefixMap.put(18, "R");
            prefixMap.put(19, "S");
            prefixMap.put(20, "T");
            prefixMap.put(21, "U");
        }
        return prefixMap;
    }

    public String getStarALPHA() {
        return getPrefixMap().get(name_prefix);
    }

    public String getStarConst(Integer starIndex) {
        return manager.getStarConstellation(starIndex);
    }
}
