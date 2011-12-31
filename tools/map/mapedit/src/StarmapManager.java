package uqmstarmap;

import com.thoughtworks.xstream.XStream;
import com.thoughtworks.xstream.io.xml.DomDriver;
import java.awt.Color;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Reads plandata.c, extracts starmap!
 */
public class StarmapManager {

    private static final String MAP_LINE_REGEX = "\\s*\\{\\{.*\\}, MAKE_STAR.*\\}.*"; //Could be improved; but works nicely on the current UQM src
    private HashMap<String, Integer> translator_alienPresence;
    private HashMap<String, Integer> translator_starTypes;
    private HashMap<String, Integer> translator_starColors;
    private static Pattern numberPattern = Pattern.compile("\\d+");
    private static Pattern nonWhiteOrComma = Pattern.compile("[\\S&&[^,]]+");
    private ArrayList<Star> stars;
    private XStream xstream;
    private ArrayList<Constellation> constellations;

    public StarmapManager() {
        translator_alienPresence = new HashMap<String, Integer>();
        translator_starTypes = new HashMap<String, Integer>();
        translator_starColors = new HashMap<String, Integer>();
        stars = new ArrayList<Star>();
        xstream = new XStream(new DomDriver()); // does not require XPP3 library
        initTranslators();
        constellations = new ArrayList<Constellation>();
    }

    public ArrayList<Constellation> getConstellations() {
        return constellations;
    }

    public void addConstellationLine(ConstellationLine cl) {
        if (cl.star1.getName_constellation().equals(cl.star2.getName_constellation()) && cl.star1.name_prefix != cl.star2.name_prefix) {
            boolean cFound = false;
            for (Constellation c : constellations) {
                if (c.constellationName.equals(cl.star1.getName_constellation())) {
                    cFound = true;
                }
            }
            if (!cFound) {
                Constellation c = new Constellation(cl.star1.getName_constellation());
                constellations.add(c);
            }

            System.out.println("s1: " + cl.star1 + "  c2: " + cl.star2);

            for (Constellation c : constellations) {
                if (c.constellationName.equals(cl.star1.getName_constellation())) {
                    boolean dupe = false;
                    for (ConstellationLine cl2 : c.cElements) {
                        if (cl2.star1 == cl.star1 && cl2.star2 == cl.star2) {
                            dupe = true;
                        }
                        if (cl2.star1 == cl.star2 && cl2.star2 == cl.star1) {
                            dupe = true;
                        }
                    }
                    if (!dupe) {
                        c.addConstellationLine(cl);
                    }
                }
            }
        }
    }

    public void writeXML(File f) throws IOException {
        FileWriter fw = new FileWriter(f);
        fw.write(xstream.toXML(stars));
        fw.close();
    }

    public void parsePlandata_C(File f) throws IOException {
        stars = new ArrayList<Star>();
        BufferedReader br = new BufferedReader(new FileReader(f));
        while (br.ready()) {
            String s = br.readLine();
            if (s.matches(MAP_LINE_REGEX)) {
                parseLine(s);
            }
        }
    }

    public void parseXML(File f) throws FileNotFoundException {
        stars = (ArrayList<Star>) xstream.fromXML(new FileInputStream(f));
    }

    /*
    //                           postfix name index (like 'Normae')
    //                         prefix name index (like 'Alpha')   |
    //                                        alien presence  |   |
    //                                     owner (unused)  |  |   |
    //   x,    y              star type   colour       |   |  |   |
    {{5007,   35}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 6, 74},
     */
    private void parseLine(String line) {
        //Trim it first
        line = line.trim();
        String tmp = "";

        //Parse x/y coordinates (the first two integers)
        Matcher matcher = numberPattern.matcher(line);
        matcher.find();
        int x_coord = Integer.valueOf(matcher.group());
        matcher.find();
        int y_coord = Integer.valueOf(matcher.group());

        line = line.substring(line.indexOf("MAKE_STAR"));
        line = line.substring(line.indexOf("("));
        line = line.replaceAll("\\},", "");
        line = line.replaceAll("\\(", "");
        line = line.replaceAll("\\)", "");

        Matcher matcher2 = nonWhiteOrComma.matcher(line);
        ArrayList<String> matches = new ArrayList<String>();
        while (matcher2.find()) {
            matches.add(matcher2.group());
        }

        int starType = translate(matches.get(0), translator_starTypes);
        int starColor = translate(matches.get(1), translator_starColors);
        int owner = Integer.valueOf(matches.get(2));
        int alienPresence = translate(matches.get(3), translator_alienPresence);
        int name_prefix = Integer.valueOf(matches.get(4));
        int name_postfix = Integer.valueOf(matches.get(5));

        Star star = new Star(x_coord, y_coord, starType, starColor, owner, alienPresence, name_prefix, name_postfix, this);
        stars.add(star);
        //System.err.println(star);

    }

    private int translate(String string, HashMap<String, Integer> map) {
        Integer i = map.get(string);
        if (i == null) {
            try {
                return Integer.valueOf(string);
            } catch (NumberFormatException ne) {
                System.err.println("NumberFormatException found for input string: '" + string + "'. -1337 returned!");
                return -1337;
            }
        } else {
            return i;
        }
    }

    public Color getColor(int starColor) {
        String colString = null;
        for (String s : translator_starColors.keySet()) {
            if (translator_starColors.get(s) == starColor) {
                colString = s;
            }
        }
        if (colString != null) {
            if (colString.contains("BLUE")) {
                return Color.BLUE;
            }
            if (colString.contains("ORANBGE")) {
                return Color.ORANGE;
            }
            if (colString.contains("GREEN")) {
                return Color.GREEN;
            }
            if (colString.contains("RED")) {
                return Color.RED;
            }
            if (colString.contains("WHITE") || colString.contains("GRAY")) {
                return Color.WHITE;
            }
            if (colString.contains("YELLOW")) {
                return Color.YELLOW;
            }
            if (colString.contains("CYAN")) {
                return Color.CYAN;
            }
            if (colString.contains("PURPLE")) {
                return new Color(128, 0, 128);
            }
            if (colString.contains("VIOLET")) {
                return new Color(127, 0, 255);
            }
        }
        return Color.PINK; //Color not found; return pink. :)
    }

    private void initTranslators() {
        //Happy fun fun tyme!
        addDef("DWARF_STAR", translator_starTypes);
        addDef("GIANT_STAR", translator_starTypes);
        addDef("SUPER_GIANT_STAR", translator_starTypes);

        addDef("BLUE_BODY", translator_starColors);
        addDef("GREEN_BODY", translator_starColors);
        addDef("ORANGE_BODY", translator_starColors);
        addDef("RED_BODY", translator_starColors);
        addDef("WHITE_BODY", translator_starColors);
        addDef("YELLOW_BODY", translator_starColors);
        addDef("CYAN_BODY", translator_starColors);
        addDef("PURPLE_BODY", translator_starColors);
        addDef("VIOLET_BODY", translator_starColors);
        translator_starColors.put("GRAY_BODY", translator_starColors.get("WHITE_BODY")); //GRAY_BODY is an alias for WHITE_BODY

        addDef("0", translator_alienPresence); //Not in-game. Use this to make sure that SOL_DEFINED == 1; and '0' == 0
        addDef("SOL_DEFINED", translator_alienPresence);
        addDef("SHOFIXTI_DEFINED", translator_alienPresence);
        addDef("MAIDENS_DEFINED", translator_alienPresence);
        addDef("START_COLONY_DEFINED", translator_alienPresence);
        addDef("SPATHI_DEFINED", translator_alienPresence);
        addDef("ZOQFOT_DEFINED", translator_alienPresence);
        addDef("MELNORME0_DEFINED", translator_alienPresence);
        addDef("MELNORME1_DEFINED", translator_alienPresence);
        addDef("MELNORME2_DEFINED", translator_alienPresence);
        addDef("MELNORME3_DEFINED", translator_alienPresence);
        addDef("MELNORME4_DEFINED", translator_alienPresence);
        addDef("MELNORME5_DEFINED", translator_alienPresence);
        addDef("MELNORME6_DEFINED", translator_alienPresence);
        addDef("MELNORME7_DEFINED", translator_alienPresence);
        addDef("MELNORME8_DEFINED", translator_alienPresence);
        addDef("TALKING_PET_DEFINED", translator_alienPresence);
        addDef("CHMMR_DEFINED", translator_alienPresence);
        addDef("SYREEN_DEFINED", translator_alienPresence);
        addDef("BURVIXESE_DEFINED", translator_alienPresence);
        addDef("SLYLANDRO_DEFINED", translator_alienPresence);
        addDef("DRUUGE_DEFINED", translator_alienPresence);
        addDef("BOMB_DEFINED", translator_alienPresence);
        addDef("AQUA_HELIX_DEFINED", translator_alienPresence);
        addDef("SUN_DEVICE_DEFINED", translator_alienPresence);
        addDef("TAALO_PROTECTOR_DEFINED", translator_alienPresence);
        addDef("SHIP_VAULT_DEFINED", translator_alienPresence);
        addDef("URQUAN_WRECK_DEFINED", translator_alienPresence);
        addDef("VUX_BEAST_DEFINED", translator_alienPresence);
        addDef("SAMATRA_DEFINED", translator_alienPresence);
        addDef("ZOQ_SCOUT_DEFINED", translator_alienPresence);
        addDef("MYCON_DEFINED", translator_alienPresence);
        addDef("EGG_CASE0_DEFINED", translator_alienPresence);
        addDef("EGG_CASE1_DEFINED", translator_alienPresence);
        addDef("EGG_CASE2_DEFINED", translator_alienPresence);
        addDef("PKUNK_DEFINED", translator_alienPresence);
        addDef("UTWIG_DEFINED", translator_alienPresence);
        addDef("SUPOX_DEFINED", translator_alienPresence);
        addDef("YEHAT_DEFINED", translator_alienPresence);
        addDef("VUX_DEFINED", translator_alienPresence);
        addDef("ORZ_DEFINED", translator_alienPresence);
        addDef("THRADD_DEFINED", translator_alienPresence);
        addDef("RAINBOW_DEFINED", translator_alienPresence);
        addDef("ILWRATH_DEFINED", translator_alienPresence);
        addDef("ANDROSYNTH_DEFINED", translator_alienPresence);
        addDef("MYCON_TRAP_DEFINED", translator_alienPresence);
    }

    //Slow, but it works.. ;)
    private void addDef(String define, HashMap<String, Integer> map) {
        ArrayList<Integer> tempInts = new ArrayList<Integer>(map.values());
        Collections.sort(tempInts);
        int nextInt = 0;
        if (!tempInts.isEmpty()) {
            nextInt = tempInts.get(tempInts.size() - 1) + 1;
        }
        map.put(define, nextInt);
    }

    private static void addNDef(String define, HashMap<Integer, String> map) {
        map.put(map.keySet().size(), define);
    }

    public ArrayList<Star> getStars() {
        return stars;
    }

    public boolean isSingleInConstellation(Star s) {
        boolean retval = true;
        for (Star sc : stars) {
            if (sc != s) {
                if (s.getName_constellation().equals(sc.getName_constellation())) {
                    retval = false;
                }
            }
        }
        return retval;
    }
    private static HashMap<Integer, String> starnames;

    public void loadConstellationNames(File f) throws FileNotFoundException, IOException {
        if (f.exists()) {
            starnames = new HashMap<Integer, String>();
            BufferedReader bw = new BufferedReader(new FileReader(f));
            while (bw.ready()) {
                addNDef(bw.readLine(), starnames);
            }
            bw.close();
        }
    }

    public static HashMap<Integer, String> getstarnames() {
        if (starnames == null) {
            starnames = new HashMap<Integer, String>();
            addNDef("Denebola", starnames);
            addNDef("Acamar", starnames);
            addNDef("Acoreus", starnames);
            addNDef("Adrastus", starnames);
            addNDef("Aeolus", starnames);
            addNDef("Alcyone", starnames);
            addNDef("Angstrom", starnames);
            addNDef("Apollonius", starnames);
            addNDef("Arachne", starnames);
            addNDef("Caeli", starnames);
            addNDef("Archimedes", starnames);
            addNDef("Archytas", starnames);
            addNDef("Ares", starnames);
            addNDef("Aristillus", starnames);
            addNDef("Artemis", starnames);
            addNDef("Ascella", starnames);
            addNDef("Auster", starnames);
            addNDef("Bellona", starnames);
            addNDef("Bessel", starnames);
            addNDef("Canis Majoris", starnames);
            addNDef("Canis Minoris", starnames);
            addNDef("Cassini", starnames);
            addNDef("Celaeno", starnames);
            addNDef("Conon", starnames);
            addNDef("Demeter", starnames);
            addNDef("Electra", starnames);
            addNDef("Eltanin", starnames);
            addNDef("Eurus", starnames);
            addNDef("Faunus", starnames);
            addNDef("Favonius", starnames);
            addNDef("Fortuna", starnames);
            addNDef("Fraunhofer", starnames);
            addNDef("Gacrux ", starnames);
            addNDef("Galilei", starnames);
            addNDef("Halley", starnames);
            addNDef("Hecate", starnames);
            addNDef("Heraclides", starnames);
            addNDef("Hespera", starnames);
            addNDef("Hestia", starnames);
            addNDef("Hicetas", starnames);
            addNDef("Hulse", starnames);
            addNDef("Huygens", starnames);
            addNDef("Hydri", starnames);
            addNDef("Hygea", starnames);
            addNDef("Hyginus", starnames);
            addNDef("Hymen", starnames);
            addNDef("Hypnos", starnames);
            addNDef("Janus", starnames);
            addNDef("Juno", starnames);
            addNDef("Juturna", starnames);
            addNDef("Juventus", starnames);
            addNDef("Kapteyn", starnames);
            addNDef("Kornephoros", starnames);
            addNDef("Lemaitre", starnames);
            addNDef("Eris", starnames);
            addNDef("Libitina", starnames);
            addNDef("Lupi", starnames);
            addNDef("Lycaeus", starnames);
            addNDef("Macrobius", starnames);
            addNDef("Menelaus", starnames);
            addNDef("Menkent", starnames);
            addNDef("Messier", starnames);
            addNDef("Meton", starnames);
            addNDef("Minerva", starnames);
            addNDef("Naos", starnames);
            addNDef("Notus", starnames);
            addNDef("Pales", starnames);
            addNDef("Morpheus", starnames);
            addNDef("Poena", starnames);
            addNDef("Priapus", starnames);
            addNDef("Proclus", starnames);
            addNDef("Riccioli", starnames);
            addNDef("Sylvanus", starnames);
            addNDef("Syrma", starnames);
            addNDef("Serpentis", starnames);
            addNDef("Sextantis", starnames);
            addNDef("Tauri", starnames);
            addNDef("Ursae Minoris", starnames);
            addNDef("Bootis", starnames);
            addNDef("Olber", starnames);
            addNDef("Centauri", starnames);
            addNDef("Ptolemae", starnames);
            addNDef("Gorno", starnames);
            addNDef("Velorum", starnames);
            addNDef("Virginis", starnames);
            addNDef("Volantis", starnames);
            addNDef("Vulpeculae", starnames);
            addNDef("Lalande", starnames);
            addNDef("Luyten", starnames);
            addNDef("Indi", starnames);
            addNDef("Lacaille", starnames);
            addNDef("Giclas", starnames);
            addNDef("Krueger", starnames);
            addNDef("Lyrae", starnames);
            addNDef("Wolf", starnames);
            addNDef("Saurus", starnames);
            addNDef("Raynet", starnames);
            addNDef("Zeeman", starnames);
            addNDef("Vela", starnames);
            addNDef("Mira", starnames);
            addNDef("Cerenkov", starnames);
            addNDef("Parmenides", starnames);
            addNDef("Penzias", starnames);
            addNDef("Persipina", starnames);
            addNDef("Phosphor", starnames);
            addNDef("Mensae", starnames);
            addNDef("Illuminati", starnames);
            addNDef("Vitalis", starnames);
            addNDef("Herculis", starnames);
            addNDef("Gruis", starnames);
            addNDef("Squidi", starnames);
            addNDef("Pleione", starnames);
            addNDef("Ursae Majoris", starnames);
            addNDef("Polydectes", starnames);
            addNDef("Betelgeuse", starnames);
            addNDef("Vertumnus", starnames);
            addNDef("Nereus", starnames);
            addNDef("Procyon", starnames);
            addNDef("Rigel", starnames);
            addNDef("Eidolonii", starnames);
            addNDef("Mizar", starnames);
            addNDef("Pythagoras", starnames);
            addNDef("Regulus", starnames);
            addNDef("Vindemiatrix", starnames);
            addNDef("Somnus", starnames);
            addNDef("Capella", starnames);
            addNDef("Deneb", starnames);
            addNDef("Canopus", starnames);
            addNDef("Sirius", starnames);
            addNDef("Sol", starnames);
            addNDef("Coronae", starnames);
            addNDef("UNKNOWN", starnames);
            
            //For starname detection:
            starnames.put(Integer.MAX_VALUE, "ignore_this");
        }
        return starnames;
    }

    public String getStarConstellation(Integer starIndex) {
        return getstarnames().get(starIndex);
    }

    void parseMaptoolFile(File f) throws FileNotFoundException, IOException, Exception {
        if (f.exists()) {
            File starsF = null;
            File clustersF = null;
            BufferedReader bw = new BufferedReader(new FileReader(f));
            while (bw.ready()) {
                String line = bw.readLine().trim();
                if (line.toLowerCase().startsWith("stars.definition")) {
                    starsF = new File(f.getParentFile(), line.split("\\=")[1].trim());
                }
                if (line.toLowerCase().startsWith("clusters.definition")) {
                    clustersF = new File(f.getParentFile(), line.split("\\=")[1].trim());
                }
            }
            if (starsF != null && clustersF != null && starsF.exists() && clustersF.exists()) {
                starnames = new HashMap<Integer, String>();
                int starname_index = 0;
                
                //Read stars
                BufferedReader bwS = new BufferedReader(new FileReader(starsF));
                while (bwS.ready()) {
                    String line = bwS.readLine().trim();
                    if (!line.startsWith("#")) {
                        Star genS = Star.getInstanceFromMapTool(line, this);
                        if (genS != null) {
                            stars.add(genS);
                            if (!starnames.containsValue(genS.getName_constellation())) {
                                starnames.put(starname_index, genS.getName_constellation());
                                starname_index++;
                            }
                        }
                    }
                }
                bwS.close();

                //Read constellations
                BufferedReader bwC = new BufferedReader(new FileReader(clustersF));
                while (bwC.ready()) {
                    String line = bwC.readLine().trim();
                    if (!line.startsWith("#")) {
                        Constellation c = Constellation.getInstanceFromMapTool(line, this);
                        if (c != null) {
                            constellations.add(c);
                        }
                    }
                }
                bwC.close();

            } else {
                throw new Exception("Input map file didn't contain a stars.definition and clusters.definition line, or one of the files could not be found/didn't exist. Aborting. starsF=" + starsF + "  clustersF=" + clustersF);
            }
            bw.close();
        }
    }
}
