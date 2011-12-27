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
package uqmanimationtool;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Random;

/**
 *
 * @author joris
 */
public class SMILExporter {

    AnimationSystem aniSystem;
    File outFile;
    int multiplier = 1;

    public SMILExporter(AnimationSystem aniSystem, File outFile, int multiplier) {
        this.aniSystem = aniSystem;
        this.outFile = outFile;
        this.multiplier = multiplier;
    }

    public void doExport() throws IOException {
        FileWriter fw = new FileWriter(outFile);
        BufferedWriter bw = new BufferedWriter(fw);
        bw.write(genSmilXMLRoot());
        bw.write(genHead());
        bw.write(genBody());
        bw.write(genEndRoot());
        bw.flush();
        fw.close();
        bw.close();
    }

    private String genSmilXMLRoot() {
        return "<?xml version=\"1.0\"?>\n"
                + "<!DOCTYPE smil PUBLIC \"-//W3C//DTD SMIL 3.0//EN\"\n"
                + "                      \"http://www.w3.org/2008/SMIL30/SMIL30Language.dtd\">\n"
                + "<smil xmlns=\"http://www.w3.org/ns/SMIL\">";
    }

    private String genHead() {
        StringBuilder sb = new StringBuilder();
        sb.append("  <head>\n"
                + "    <meta name=\"title\" content=\"UQMAnimationtool Generated File\"/>\n"
                + "    <meta name=\"author\" content=\"UQMAnimationtool\"/>\n"
                + "    <layout>\n");

        for (ImagePanel ip : aniSystem.frames) {
            sb.append("<region xml:id=\"" + ip.getFilename() + "\" width=\"" + ip.getWidth() + "\" height=\"" + ip.getHeight() + "\" left=\"" + (ip.getXoff() * multiplier) + "\" top=\"" + (ip.getYoff() * multiplier) + "\" z-index=\"-1\"/>\n");
        }

        sb.append("    </layout>\n"
                + "  </head>\n");
        return sb.toString();
    }

    private String genBody() {
        StringBuilder sb = new StringBuilder();
        sb.append("  <body>\n"
                + "  <par>\n");

        //Write out all animations:
        for (Animation a : aniSystem.animations) {
            if (a.aniType == AnimationType.BACKGROUND) { //Background 'animation': no sequence, no duration (ALWAYS VISIBLE)
                for (AnimationFrame af : a.frames) {
                    af.duration = -1d;
                    sb.append(_genParForAFrame(af) + "\n");
                }
            } else if (a.aniType == AnimationType.CIRCULAR) { //Circular animation: all frames after each other
                sb.append(_genSeqForAni(a));
                for (AnimationFrame af : a.frames) {
                    sb.append(_genParForAFrame(af) + "\n");
                }
                sb.append("</seq>\n");
            } else if (a.aniType == AnimationType.RANDOM) { //Random animation: all frames randomly. Pre-determined generation because SMIL does not (?) support randomness
                sb.append(_genSeqForAni(a));
                //Generate 'random' list of frame elements sufficiently large enough to appear random to the player
                Random random = new Random();
                for (int i = 0; i < 256; i++) {
                    sb.append(_genParForAFrame(a.frames.get(random.nextInt(a.frames.size()))) + "\n");
                }
                sb.append("</seq>\n");
            } else if (a.aniType == AnimationType.TALK) { //Same as circular for now. Should be same as random??
                sb.append(_genSeqForAni(a));                
                for (AnimationFrame af : a.frames) {
                    sb.append(_genParForAFrame(af) + "\n");
                }
                sb.append("</seq>\n");
            } else if (a.aniType == AnimationType.YO_YO) { //Yo-yo animation: loops through all frames once, then loops through them again in reverse.
                sb.append(_genSeqForAni(a));
                for (AnimationFrame af : a.frames) {
                    sb.append(_genParForAFrame(af) + "\n");
                }
                ArrayList<ImagePanel> tempA = new ArrayList(a.frames);
                Collections.reverse(tempA);
                for (AnimationFrame af : a.frames) {
                    sb.append(_genParForAFrame(af) + "\n");
                }
                sb.append("</seq>\n");
            }
        }

        sb.append("  </par>\n"
                + "  </body>\n");
        return sb.toString();
    }

    private String _genSeqForAni(Animation a) {
        return "<seq begin=\"0s\" repeatCount=\"indefinite\" > <!-- Animation: " + a.toString() + " -->\n";
    }

    private String _genParForAFrame(AnimationFrame frame) {
        return "<par" + (frame.duration != null && frame.duration >= 0 ? " dur=\"" + frame.duration + "s\"" : "") + "><img src=\"" + frame.frame.getFilename() + "\" region=\"" + frame.frame.getFilename() + "\"/></par>";
    }

    private String genEndRoot() {
        return "</smil>\n";
    }
}
