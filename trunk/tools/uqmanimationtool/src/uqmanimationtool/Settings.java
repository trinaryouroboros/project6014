/*
Copyright (C) 2010 Joris van de Donk

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

import java.awt.Color;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.Logger;

public class Settings {

    private static Settings settingsInstance;
    public String lastDirPath = "";
    public boolean hideDupe = false; //not really used anymore
    public boolean showOnlySelected = false;
    public Color hotspotColor = Color.red;

    public static Settings getInstance() {
        if (settingsInstance == null) {
            settingsInstance = new Settings();
        }
        return settingsInstance;
    }

    public Settings() {
        File aniprops = getPropertiesFile();
        if (aniprops.exists()) {
            Properties settingsProps = new Properties();
            try {
                settingsProps.load(new FileInputStream(aniprops));
                lastDirPath = settingsProps.getProperty("lastDirPath");
                showOnlySelected = settingsProps.getProperty("showOnlySelected").equals("True");
                String hotspotCS = settingsProps.getProperty("hotspotColor");
                if (hotspotCS != null) {
                    hotspotColor = new Color(Integer.valueOf(hotspotCS));
                }
            } catch (IOException ex) {
                //Can't read the properties file. Cleverly ignore. :)
                Logger.getLogger(Settings.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }

    public void writeSettings() throws IOException {
        File writeFile = getPropertiesFile();
        Properties settingsProps = new Properties();
        settingsProps.setProperty("lastDirPath", lastDirPath);
        settingsProps.setProperty("showOnlySelected", (showOnlySelected == true ? "True" : "False"));
        settingsProps.setProperty("hotspotColor", "" + hotspotColor.getRGB());
        settingsProps.store(new FileWriter(writeFile), "Settings file for the UQM Animation tool");
    }

    private File getPropertiesFile() {
        File homedir = new File(System.getProperty("user.home"));
        File uqmtoolsdir = new File(homedir, ".uqmtools");
        if (!uqmtoolsdir.exists()) {
            uqmtoolsdir.mkdir();
        }
        File aniprops = new File(uqmtoolsdir, "uqanimationtool.properties");
        return aniprops;
    }
}
