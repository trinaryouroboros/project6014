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

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;

public class DiaConverter {

    public static void ConvertToDia(File f, ArrayList<Star> stars, ArrayList<Constellation> constellations) throws IOException {
        if (!f.exists()) {
            f.createNewFile();
        }
        BufferedWriter bfw = new BufferedWriter(new FileWriter(f));
        StringBuilder sb = new StringBuilder();
        sb.append(diaHeader());
        sb.append(diaGrid());
        for (Star s : stars) {
            sb.append(diaStar(s));
        }
        for (Constellation c : constellations) {
            for (ConstellationLine cl : c.cElements) {
                sb.append(diaConstellationLine(cl));
            }
        }
        sb.append(diaFooter());
        bfw.write(sb.toString());
        bfw.close();
    }

    private static String diaGrid() {
        return "    <dia:object type=\"Misc - Grid\" version=\"0\" id=\"O0\">\n"
                + "      <dia:attribute name=\"obj_pos\">\n"
                + "        <dia:point val=\"0.4,0.4\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"obj_bb\">\n"
                + "        <dia:rectangle val=\"0.4,0.4;14.6,14.6\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"meta\">\n"
                + "        <dia:composite type=\"dict\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"elem_corner\">\n"
                + "        <dia:point val=\"0.4,0.4\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"elem_width\">\n"
                + "        <dia:real val=\"14.6\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"elem_height\">\n"
                + "        <dia:real val=\"14.6\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"line_width\">\n"
                + "        <dia:real val=\"0.05000000074505806\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"line_colour\">\n"
                + "        <dia:color val=\"#000000\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"fill_colour\">\n"
                + "        <dia:color val=\"#ffffff\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"show_background\">\n"
                + "        <dia:boolean val=\"true\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"grid_rows\">\n"
                + "        <dia:int val=\"10\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"grid_cols\">\n"
                + "        <dia:int val=\"10\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"gridline_colour\">\n"
                + "        <dia:color val=\"#7f7f7f\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"gridline_width\">\n"
                + "        <dia:real val=\"0.01\"/>\n"
                + "      </dia:attribute>\n"
                + "    </dia:object>\n";
    }
    private static int constellationLineID = 0;

    private static String diaConstellationLine(ConstellationLine l) {
        constellationLineID += 1;
        String s1id = l.star1.getName_constellation() + "-" + l.star1.getName_prefix();
        String s2id = l.star2.getName_constellation() + "-" + l.star2.getName_prefix();
        return "    <dia:object type=\"Standard - Line\" version=\"0\" id=\"cline-" + constellationLineID + "\">\n"
                + "      <dia:attribute name=\"obj_pos\">\n"
                + "        <dia:point val=\"8.56556,4.34061\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"obj_bb\">\n"
                + "        <dia:rectangle val=\"8.36612,4.3483;8.57326,4.7692\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"conn_endpoints\">\n"
                + "        <dia:point val=\"8.56556,4.34061\"/>\n"
                + "        <dia:point val=\"8.37381,4.77689\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"numcp\">\n"
                + "        <dia:int val=\"1\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"line_width\">\n"
                + "        <dia:real val=\"0.029999999329447746\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"absolute_start_gap\">\n"
                + "        <dia:real val=\"0.029999999999999999\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"absolute_end_gap\">\n"
                + "        <dia:real val=\"0.029999999999999999\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:connections>\n"
                + "        <dia:connection handle=\"0\" to=\"" + s1id + "\" connection=\"8\"/>\n"
                + "        <dia:connection handle=\"1\" to=\"" + s2id + "\" connection=\"8\"/>\n"
                + "      </dia:connections>\n"
                + "    </dia:object>";
    }

    private static String diaStar(Star s) {
        String pos = ((s.getX_coord() * 0.00146) + 0.4) + "," + (((10000 - s.getY_coord()) * 0.00146) + 0.4);
        String color = "#" + Integer.toHexString(StarmapManager.getColor(s.getStarcolor()).getRGB()).substring(2);
        String id = s.getName_constellation() + "-" + s.getName_prefix();
        String size = "0.1";
        return "    <dia:object type=\"Geometric - Perfect Circle\" version=\"1\" id=\"" + id + "\">\n"
                + "      <dia:attribute name=\"obj_pos\">\n"
                + "        <dia:point val=\"" + pos + "\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"obj_bb\">\n"
                + "        <dia:rectangle val=\"0.685,0.61;0.861875,0.786875\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"meta\">\n"
                + "        <dia:composite type=\"dict\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"elem_corner\">\n"
                + "        <dia:point val=\"" + pos + "\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"elem_width\">\n"
                + "        <dia:real val=\"" + size + "\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"elem_height\">\n"
                + "        <dia:real val=\"" + size + "\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"line_width\">\n"
                + "        <dia:real val=\"0.029999999329447746\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"line_colour\">\n"
                + "        <dia:color val=\"#000000\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"fill_colour\">\n"
                + "        <dia:color val=\"" + color + "\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"show_background\">\n"
                + "        <dia:boolean val=\"true\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"line_style\">\n"
                + "        <dia:enum val=\"0\"/>\n"
                + "        <dia:real val=\"1\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"flip_horizontal\">\n"
                + "        <dia:boolean val=\"false\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"flip_vertical\">\n"
                + "        <dia:boolean val=\"false\"/>\n"
                + "      </dia:attribute>\n"
                + "      <dia:attribute name=\"subscale\">\n"
                + "        <dia:real val=\"1\"/>\n"
                + "      </dia:attribute>\n"
                + "    </dia:object>\n";
    }

    private static String diaHeader() {
        return "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                + "<dia:diagram xmlns:dia=\"http://www.lysator.liu.se/~alla/dia/\">\n"
                + "  <dia:diagramdata>\n"
                + "    <dia:attribute name=\"background\">\n"
                + "      <dia:color val=\"#ffffff\"/>\n"
                + "    </dia:attribute>\n"
                + "    <dia:attribute name=\"pagebreak\">\n"
                + "      <dia:color val=\"#000099\"/>\n"
                + "    </dia:attribute>\n"
                + "    <dia:attribute name=\"paper\">\n"
                + "      <dia:composite type=\"paper\">\n"
                + "        <dia:attribute name=\"name\">\n"
                + "          <dia:string>#A4#</dia:string>\n"
                + "        </dia:attribute>\n"
                + "        <dia:attribute name=\"tmargin\">\n"
                + "          <dia:real val=\"2.8222000598907471\"/>\n"
                + "        </dia:attribute>\n"
                + "        <dia:attribute name=\"bmargin\">\n"
                + "          <dia:real val=\"2.8222000598907471\"/>\n"
                + "        </dia:attribute>\n"
                + "        <dia:attribute name=\"lmargin\">\n"
                + "          <dia:real val=\"2.8222000598907471\"/>\n"
                + "        </dia:attribute>\n"
                + "        <dia:attribute name=\"rmargin\">\n"
                + "          <dia:real val=\"2.8222000598907471\"/>\n"
                + "        </dia:attribute>\n"
                + "        <dia:attribute name=\"is_portrait\">\n"
                + "          <dia:boolean val=\"true\"/>\n"
                + "        </dia:attribute>\n"
                + "        <dia:attribute name=\"scaling\">\n"
                + "          <dia:real val=\"1\"/>\n"
                + "        </dia:attribute>\n"
                + "        <dia:attribute name=\"fitto\">\n"
                + "          <dia:boolean val=\"false\"/>\n"
                + "        </dia:attribute>\n"
                + "      </dia:composite>\n"
                + "    </dia:attribute>\n"
                + "    <dia:attribute name=\"grid\">\n"
                + "      <dia:composite type=\"grid\">\n"
                + "        <dia:attribute name=\"width_x\">\n"
                + "          <dia:real val=\"1\"/>\n"
                + "        </dia:attribute>\n"
                + "        <dia:attribute name=\"width_y\">\n"
                + "          <dia:real val=\"1\"/>\n"
                + "        </dia:attribute>\n"
                + "        <dia:attribute name=\"visible_x\">\n"
                + "          <dia:int val=\"1\"/>\n"
                + "        </dia:attribute>\n"
                + "        <dia:attribute name=\"visible_y\">\n"
                + "          <dia:int val=\"1\"/>\n"
                + "        </dia:attribute>\n"
                + "        <dia:composite type=\"color\"/>\n"
                + "      </dia:composite>\n"
                + "    </dia:attribute>\n"
                + "    <dia:attribute name=\"color\">\n"
                + "      <dia:color val=\"#d8e5e5\"/>\n"
                + "    </dia:attribute>\n"
                + "    <dia:attribute name=\"guides\">\n"
                + "      <dia:composite type=\"guides\">\n"
                + "        <dia:attribute name=\"hguides\"/>\n"
                + "        <dia:attribute name=\"vguides\"/>\n"
                + "      </dia:composite>\n"
                + "    </dia:attribute>\n"
                + "  </dia:diagramdata>\n"
                + "  <dia:layer name=\"Background\" visible=\"true\" active=\"true\">\n";
    }

    private static String diaFooter() {
        return "  </dia:layer>\n"
                + "</dia:diagram>\n";
    }
}
