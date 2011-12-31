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

import java.util.ArrayList;

/**
 *
 * @author joris
 */
public class Constellation {

    public String constellationName;
    public ArrayList<ConstellationLine> cElements;
    private static ArrayList<String> nameposhints;

    public Constellation(String constellationName) {
        this.constellationName = constellationName;
        this.cElements = new ArrayList<ConstellationLine>();
    }

    private static ArrayList<String> getNamePosHints() {
        if (nameposhints == null) {
            ArrayList<String> retval = new ArrayList<String>();
            retval.add("n");
            retval.add("e");
            retval.add("s");
            retval.add("w");
            retval.add("ne");
            retval.add("nw");
            retval.add("se");
            retval.add("sw");
            nameposhints = retval;
        }
        return nameposhints;
    }

    public static Constellation getInstanceFromMapTool(String maptoolFormat, StarmapManager manager) {
        //Apollonius,n,IE,ED,DC,CB,GH,HF,FI,GB,FA
        try {
            String[] splitted = maptoolFormat.split(",");
            Constellation ret = new Constellation(splitted[0]);
            for (int i = 2; i < splitted.length; i++) {
                String spted = splitted[i];
                if (!getNamePosHints().contains(spted)) {
                    if (spted.length() == 2) {
                        String s1 = spted.substring(0, 1);
                        String s2 = spted.substring(1, 2);
                        Star star1 = null;
                        Star star2 = null;
                        for (Star s : manager.getStars()) {
                            if (s.getName_constellation().equals(ret.constellationName)) {
                                if (s.getStarALPHA().equals(s1)) {
                                    star1 = s;
                                }
                                if (s.getStarALPHA().equals(s2)) {
                                    star2 = s;
                                }
                            }
                        }
                        if (star1 != null && star2 != null) {
                            ret.cElements.add(new ConstellationLine(star1, star2));
                        }
                    } else {
                        System.out.println("Ignoring constellation line: " + spted);
                    }
                }
            }
            return ret;
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    @Override
    public String toString() {
        String s = "";
        for (int i = 0; i < cElements.size(); i++) {
            if (i == 0) {
                s = s + cElements.get(i);
            } else {
                s = s + "," + cElements.get(i);
            }
        }

        return constellationName + ",n," + s;
    }

    public void addConstellationLine(ConstellationLine cl) {
        cElements.add(cl);
    }
}
