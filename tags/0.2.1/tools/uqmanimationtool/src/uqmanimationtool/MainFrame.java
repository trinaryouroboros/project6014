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

/*
 * MainFrame.java
 *
 * Created on 9-okt-2010, 23:35:12
 */
package uqmanimationtool;

import com.sun.corba.se.impl.orbutil.concurrent.Mutex;
import java.awt.Color;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.image.BufferedImage;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.imageio.IIOException;
import javax.swing.AbstractAction;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JColorChooser;
import javax.swing.JComponent;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.JTable;
import javax.swing.KeyStroke;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.DefaultTableModel;
import uqmanimationtool.miscgui.ButtonColumn;

/**
 *
 * @author joris
 */
public class MainFrame extends javax.swing.JFrame {

    Mutex mu = new Mutex();
    AnimationSystem animationSystem = new AnimationSystem();
    Color hotspotColor = Color.red;
    ImagePanel hotspot;
    TimerTask t;
    Timer tim;
    boolean timon = false;
    File editingFile;
    Settings settings;
    Integer animationTo = null;
    int animPass = 0;
    private static Pattern extframePattern = Pattern.compile("\\[.*\\]");
    private AnimationStructureTableModel aniTableModel;

    private void parseExtline(String extline, StringBuilder sb) throws NumberFormatException {
        //Parse all extension lines
        extline = extline.substring(2).trim();
        String[] extsplitted = extline.toLowerCase().split(" ");


        if (extsplitted.length >= 1) {
            //#@ anidef 0 TALK [NAME]
            if (extsplitted[0].equals("anidef")) {
                if (extsplitted.length >= 3) {
                    int aniID = Integer.valueOf(extsplitted[1]);
                    while (animationSystem.animations.size() <= aniID) {
                        Animation ani = new Animation();
                        animationSystem.addAnimation(ani);
                    }

                    Animation ani = animationSystem.animations.get(aniID);
                    if (extsplitted[2].equals("talk")) {
                        ani.aniType = AnimationType.TALK;
                    }
                    if (extsplitted[2].equals("circular")) {
                        ani.aniType = AnimationType.CIRCULAR;
                    }
                    if (extsplitted[2].equals("random")) {
                        ani.aniType = AnimationType.RANDOM;
                    }
                    if (extsplitted[2].equals("yo_yo")) {
                        ani.aniType = AnimationType.YO_YO;
                    }
                    if (extsplitted[2].equals("background")) {
                        ani.aniType = AnimationType.BACKGROUND;
                    }

                    Matcher matcher = extframePattern.matcher(extline);
                    if (matcher.find()) {
                        String fSource = matcher.group();
                        fSource = fSource.substring(1, fSource.length() - 1); //trim off the [ and ]
                        ani.setName(fSource);
                    }
                } else {
                    sb.append("Too few arguments to anidef at " + extline + "\n");
                }
            }


            //#@ aniframe 0 0.25 [<<FRAME SOURCE>>]
            if (extsplitted[0].equals("aniframe")) {
                if (extsplitted.length >= 4) {
                    int aniID = Integer.valueOf(extsplitted[1]);
                    if (animationSystem.animations.size() > aniID) {
                        Animation ani = animationSystem.animations.get(aniID);
                        double duration = -1; //Default for BACKGROUND animationType
                        if (ani.aniType != AnimationType.BACKGROUND) {
                            duration = Double.valueOf(extsplitted[2]);
                        }

                        Matcher matcher = extframePattern.matcher(extline);
                        if (matcher.find()) {
                            String fSource = matcher.group();
                            fSource = fSource.substring(1, fSource.length() - 1); //trim off the [ and ]
                            boolean found = false;
                            for (ImagePanel ip : animationSystem.frames) {
                                if (ip.toSource().equals(fSource)) {
                                    ani.addFrame(ip, duration);
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) {
                                sb.append("Nothing found for " + fSource + "\n");
                                //something is wrong, couldn't find aniframe declaration in list of frames (it's assigned to an animation, but not declared as a frame to UQM)
                                //TODO: determine behaviour (add it in anyways, OR ignore it?)
                            }

                        } else {
                            sb.append("Invalid aniframe extension line\n");
                            //something is wrong, couldn't find aniframe declaration
                        }
                    } else {
                        sb.append("Animation with index " + aniID + " not found. Has it been declared previously?\n");
                    }
                } else {
                    sb.append("Too few arguments to aniframe at " + extline + "\n");
                }
            }
        }
    }

    private boolean parseLine(String line, File f, boolean negativeFound) throws NoSuchAlgorithmException, IOException, NumberFormatException {
        String[] splitted = line.split(" ");
        String fname = f.getParent() + System.getProperty("file.separator") + splitted[0];
        int split1 = Integer.valueOf(splitted[1]);
        int split2 = Integer.valueOf(splitted[2]);
        int xoff = Integer.valueOf(splitted[3]);
        int yoff = Integer.valueOf(splitted[4]);
        if (xoff < 0 || yoff < 0) {
            negativeFound = true;
        }
        ImagePanel io = new ImagePanel(fname, xoff, yoff, jSlider_Zoom.getValue());
        io.setSplit1(split1);
        io.setSplit2(split2);
        jPanel_ImageWorkspace.add(io, 0);
        animationSystem.addElement(io);
        io.setOpaque(false);
        io.setVisible(true);
        return negativeFound;
    }

    /**
     * Translate a single image panel (the one currently selected)
     * @param dx amount to translate X position by
     * @param dy amount to translate Y position by
     */
    private void translateImagePanel(int dx, int dy) {
        ImagePanel ip = getSelectedImagePanel();
        translateImagePanel(ip, dx, dy);
    }

    /**
     * Translate a single image panel
     * @param imagePanel The imagepanel to translate
     * @param dx amount to translate X position by
     * @param dy amount to translate Y position by
     */
    private void translateImagePanel(ImagePanel imagePanel, int dx, int dy) {
        if (imagePanel != null) {
            imagePanel.setDXoff(dx);
            imagePanel.setDYoff(dy);
            jPanel_ImageWorkspace.setComponentZOrder(imagePanel, 0);
            jPanel_ImageWorkspace.updateUI();
            jList_Frames.updateUI();
            repaintHotspot();
            System.out.println("New imgpanel: " + imagePanel);
        }
    }

    /**
     * Translate all image panels
     * @param dx amount to translate X position by
     * @param dy amount to translate Y position by
     */
    private void tranlateImagePanels(int dx, int dy) {
        for (int k = 0; k < animationSystem.size(); k++) {
            ImagePanel ip = (ImagePanel) animationSystem.get(k);
            if (ip != null) {
                ip.setDXoff(dx);
                ip.setDYoff(dy);
                System.out.println("New imgpanel: " + ip);
            }
        }
        jPanel_ImageWorkspace.updateUI();
        jList_Frames.updateUI();
        repaintHotspot();
    }

    /**
     * Initializes the shortcut keys (those that aren't handled by the main menu)
     * TODO: Load these from a configuration file, so that they can be altered
     * (due to AZERTY support)
     */
    private void initKeys() {
        //W
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke('w'),
                "doW");
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke('z'),
                "doW"); //AZERTY
        getRootPane().getActionMap().put("doW",
                new AbstractAction() {

                    @Override
                    public void actionPerformed(ActionEvent e) {
                        translateImagePanel(0, 1);
                    }
                });
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke('W'),
                "doW2");
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke('Z'),
                "doW2"); //AZERTY
        getRootPane().getActionMap().put("doW2",
                new AbstractAction() {

                    @Override
                    public void actionPerformed(ActionEvent e) {
                        tranlateImagePanels(0, 1);
                    }
                });

        //A
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke('a'),
                "doA");
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke('q'),
                "doA"); //AZERTY
        getRootPane().getActionMap().put("doA",
                new AbstractAction() {

                    @Override
                    public void actionPerformed(ActionEvent e) {
                        translateImagePanel(1, 0);
                    }
                });
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke('A'),
                "doA2");
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke('Q'),
                "doA2"); //AZERTY
        getRootPane().getActionMap().put("doA2",
                new AbstractAction() {

                    @Override
                    public void actionPerformed(ActionEvent e) {
                        tranlateImagePanels(1, 0);
                    }
                });

        //S
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke('s'),
                "doS");
        getRootPane().getActionMap().put("doS",
                new AbstractAction() {

                    @Override
                    public void actionPerformed(ActionEvent e) {
                        translateImagePanel(0, -1);
                    }
                });
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke('S'),
                "doS2");
        getRootPane().getActionMap().put("doS2",
                new AbstractAction() {

                    @Override
                    public void actionPerformed(ActionEvent e) {
                        tranlateImagePanels(0, -1);
                    }
                });

        //D
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke('d'),
                "doD");
        getRootPane().getActionMap().put("doD",
                new AbstractAction() {

                    @Override
                    public void actionPerformed(ActionEvent e) {
                        translateImagePanel(-1, 0);
                    }
                });
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke('D'),
                "doD2");
        getRootPane().getActionMap().put("doD2",
                new AbstractAction() {

                    @Override
                    public void actionPerformed(ActionEvent e) {
                        tranlateImagePanels(-1, 0);
                    }
                });

        //up arrow
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke(KeyEvent.VK_UP, 0),
                "doUp");
        getRootPane().getActionMap().put("doUp",
                new AbstractAction() {

                    @Override
                    public void actionPerformed(ActionEvent e) {
                        selectPrevious();
                    }
                });
        //down arrow
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke(KeyEvent.VK_DOWN, 0),
                "doDown");
        getRootPane().getActionMap().put("doDown",
                new AbstractAction() {

                    @Override
                    public void actionPerformed(ActionEvent e) {
                        selectNext();
                    }
                });
    }

    /**
     * Get the currently selected Imagepanel
     * @return the currently selected Imagepanel
     */
    private ImagePanel getSelectedImagePanel() {
        if (jList_Frames.getSelectedIndex() == -1) {
            return null;
        }
        if (animationSystem.size() >= jList_Frames.getSelectedIndex()) {
            return (ImagePanel) animationSystem.get(jList_Frames.getSelectedIndex());
        } else {
            return null;
        }
    }

    /**
     * Save (write) the current state to a file
     * @param writefile The file to write to. If null, open a dialog to ask for a file, then save it.
     */
    private void saveFile(File writefile) {
        FileWriter fw = null;
        if (writefile == null) {
            JFileChooser fc = new JFileChooser(settings.lastDirPath);
            fc.addChoosableFileFilter(new FileNameExtensionFilter("ani file", "ani", "med", "big", "sml"));
            int result = fc.showSaveDialog(this);
            if (result == JFileChooser.APPROVE_OPTION) {
                writefile = fc.getSelectedFile();
                setCurrentlyEditingFile(writefile); //Change the currently editing file to the new file, so ctrl-s works properly
            } else {
                JOptionPane.showMessageDialog(this, "No file specified. Not saving.");
            }
        }
        try {
            fw = new FileWriter(writefile);
            for (int i = 0; i < animationSystem.getListModel().getSize(); i++) {
                ImagePanel ip = (ImagePanel) animationSystem.get(i);
                if (ip != null) {
                    fw.write(ip.toSource() + "\n");
                }
                ip.setChanged(false);
            }
            for (int i = 0; i < animationSystem.animations.size(); i++) {
                Animation ani = animationSystem.animations.get(i);
                if (ani.aniType == AnimationType.BACKGROUND) {
                    for (AnimationFrame af : ani.frames) {
                        af.duration = -1d;
                    }
                }
                fw.write("#@ anidef " + i + " " + ani.aniType + " [" + ani.getName() + "]\n");
            }
            for (int i = 0; i < animationSystem.animations.size(); i++) {
                for (AnimationFrame frame : animationSystem.animations.get(i).frames) {
                    fw.write("#@ aniframe " + i + " " + frame.duration + " [" + frame.frame.toSource() + "]\n");
                }
            }
            fw.close();
            jList_Frames.updateUI();
        } catch (IOException ex) {
            Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
            JOptionPane.showMessageDialog(this, "Couldn't save file: " + ex);
        } finally {
            try {
                if (fw != null) {
                    fw.close();
                }
            } catch (IOException ex) {
                Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
                JOptionPane.showMessageDialog(this, "Couldn't close file: " + ex);
            }
        }
    }

    private void saveFile() {
        saveFile(editingFile);
    }

    private void repaintHotspot() {
        if (jCheckBoxMenuItem_ShowHotspot.isSelected()) {
            hotspot.setVisible(true);
            jPanel_ImageWorkspace.setComponentZOrder(hotspot, 0);
        } else {
            hotspot.setVisible(false);
        }
    }

    /** Creates new form MainFrame */
    public MainFrame() {
        initComponents();

        jPanel_ImageWorkspace.requestFocus();
        try {
            initSettings();
        } catch (FileNotFoundException ex) {
            Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
        } catch (IOException ex) {
            Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
        }

        initKeys();


        jList_Frames.addListSelectionListener(new ListSelectionListenerImpl());
        tim = new Timer();

        jPanel_ImageWorkspace.addMouseListener(new MyMouseListener());

        addWindowListener(new WindowAdapter() {

            public void windowClosing(WindowEvent e) {
                try {
                    settings.hideDupe = false;
                    settings.showOnlySelected = jCheckBoxMenuItem_ShowSelectedFrameOnly.isSelected();
                    settings.writeSettings();
                    System.exit(0);
                } catch (IOException ex) {
                    Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
        });

        jComboBox_Tranparency.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent e) {
                if ("comboBoxChanged".equals(e.getActionCommand())) {
                    repaintFrames();
                }
            }
        });

        jComboBox_animations.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent e) {
                if ("comboBoxChanged".equals(e.getActionCommand())) {
                    repaintFrames();
                    Animation ani = (Animation) animationSystem.animationListModel.getSelectedItem();
                    jComboBox_AnimationType.setModel(new DefaultComboBoxModel(new Object[]{AnimationType.BACKGROUND, AnimationType.CIRCULAR, AnimationType.RANDOM, AnimationType.TALK, AnimationType.YO_YO}));
                    jComboBox_AnimationType.setSelectedItem(ani.aniType);
                    aniTableModel.fireTableStructureChange();
                    jTable1.updateUI();
                }
            }
        });

        jComboBox_AnimationType.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent e) {
                if ("comboBoxChanged".equals(e.getActionCommand())) {
                    ((Animation) animationSystem.animationListModel.getSelectedItem()).aniType = (AnimationType) jComboBox_AnimationType.getSelectedItem();
//                    if (((AnimationType)jComboBox1.getSelectedItem()).equals(AnimationType.RANDOM)) {
//                        JOptionPane.showMessageDialog(rootPane, "AnimationType 'RANDOM' is not fully supported by the SMIL exporter!");
//                    }
                }
                jComboBox_animations.updateUI();
                jTable2.updateUI();
                aniTableModel.fireTableStructureChange();
                jTable1.updateUI();
            }
        });
        aniTableModel = new AnimationStructureTableModel();
        jTable1.setModel(aniTableModel);

        AnimationPlayingTableModel aniPTableModel = new AnimationPlayingTableModel();
        jTable2.setModel(aniPTableModel);
        ButtonColumn bc = new ButtonColumn(jTable2, new AbstractAction() {

            @Override
            public void actionPerformed(ActionEvent e) {
                JTable table = (JTable) e.getSource();
                int modelRow = Integer.valueOf(e.getActionCommand());
                if (modelRow >= 0) {
                    Animation a = animationSystem.animations.get(modelRow);
                    a.setIsPlaying((a.playingState == 0 ? true : false));
                }
            }
        }, 0);
        jTable2.getColumnModel().getColumn(0).setPreferredWidth(25);
        jTable2.getColumnModel().getColumn(0).setMaxWidth(25);
    }

    private ImagePanel generateHotspot(int width, int height, int zoom) {
        BufferedImage bi = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
        bi.setRGB(0, 0, hotspotColor.getRGB());
        ImagePanel ip = new ImagePanel(null, bi, 0, 0, zoom, -2, 1);
        ip.setVisible(true);
        ip.setOpaque(false);
        return ip;
    }

    private void initSettings() throws FileNotFoundException, IOException {
        settings = Settings.getInstance();
        jCheckBoxMenuItem_ShowSelectedFrameOnly.setSelected(settings.showOnlySelected);
        hotspotColor = (settings.hotspotColor == null ? Color.red : settings.hotspotColor);
    }

    /**
     * Load a file
     * @param f The file to load. If null, a file dialog will be opened.
     */
    private void loadFile(File f) {
        if (timon) {
            tim.cancel();
            timon = false;
        }
        if (f == null) {
            JFileChooser fc = new JFileChooser(settings.lastDirPath);
            fc.addChoosableFileFilter(new FileNameExtensionFilter("ani file", "ani", "med", "big", "sml"));
            int returnval = fc.showOpenDialog(this);

            if (returnval == JFileChooser.APPROVE_OPTION) {
                f = fc.getSelectedFile();
            } else {
                JOptionPane.showMessageDialog(this, "No file selected. Quitting.");
                this.dispose();
                System.exit(0);
            }
        }
        jList_Frames.setModel(animationSystem.getListModel());
        jComboBox_animations.setModel(animationSystem.animationListModel);
        setCurrentlyEditingFile(f);
        try {
            parseFile(editingFile);
        } catch (FileNotFoundException ex) {
            Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
        } catch (IOException ex) {
            Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
        } catch (NoSuchAlgorithmException ex) {
            Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    /**
     * Set the currently editing file, and update the UI accordingly.
     * @param f The file we're editing
     */
    private void setCurrentlyEditingFile(File f) {
        if (f != null) {
            jLabel_LoadedFile.setText(f.getName());
            jLabel_LoadedFile.setToolTipText(f.getAbsolutePath());
        } else {
            jLabel_LoadedFile.setText("No file loaded");
            jLabel_LoadedFile.setToolTipText("No file loaded");
        }
        editingFile = f;
    }

    /**
     * Parse a file
     * @param f the file to parse
     * @throws FileNotFoundException
     * @throws IOException
     * @throws NoSuchAlgorithmException 
     */
    private void parseFile(File f) throws FileNotFoundException, IOException, NoSuchAlgorithmException {
        settings.lastDirPath = f.getParent();
        jPanel_ImageWorkspace.removeAll();
        animationSystem.clear();
        BufferedReader br = new BufferedReader(new FileReader(f));
        int lnr = 0;
        boolean negativeFound = false;
        StringBuilder sb = new StringBuilder();
        ArrayList<String> anitool_extension_lines = new ArrayList<String>();
        while (br.ready()) {
            lnr++;
            String line = br.readLine();
            line = line.trim(); //trim the line of any trailing/leading whitespace
            if (!line.startsWith("#")) {
                try {
                    negativeFound = parseLine(line, f, negativeFound); //parse the line
                } catch (NumberFormatException ne) {
                    sb.append("NumberFormatException occured. Ignoring this line (Line Number: ").append(lnr).append(")! Input string: [").append(line).append("]. Exception message: ").append(ne.getMessage()).append("\n");
                } catch (ArrayIndexOutOfBoundsException ae) {
                    sb.append("ArrayIndexOutOfBoundException occured. This probably means your .ani file is bugged. Ignoring this line (Line Number: ").append(lnr).append("). Line: [").append(line).append("]. Exception message: ").append(ae.getMessage()).append("\n");
                } catch (IIOException ie) {
                    sb.append("ImageIOException occured. Ignoring this line (Line Number: ").append(lnr).append("): [").append(line).append("] Message: ").append(ie.getMessage()).append("\n");
                } catch (Exception e) {
                    sb.append("Ignoring this line (Line Number: ").append(lnr).append("): [").append(line).append("] Message: ").append(e.getMessage()).append("\n");
                }
            } else if (line.startsWith("#@")) { //UQMAnimatonTool extension line
                anitool_extension_lines.add(line); //Add it for parsing later
            }
        }
        for (String extline : anitool_extension_lines) {
            try {
                parseExtline(extline, sb); //parse extension lines
            } catch (NumberFormatException ne) {
                sb.append("NumberFormatException occured. Input string: [").append(extline).append("]. Exception message: ").append(ne.getMessage()).append("\n");
            } catch (ArrayIndexOutOfBoundsException ae) {
                sb.append("ArrayIndexOutOfBoundException occured. This probably means your .ani file is bugged. Ignoring this line: [").append(extline).append("]. Exception message: ").append(ae.getMessage()).append("\n");
            } catch (Exception e) {
                sb.append("Ignoring this line: ").append(" [").append(extline).append("] Message: [").append(e.getMessage()).append("] Exception: [").append(e).append("] ").append("\n");
            }
        }
        if (sb.length() > 0) {
            TextDialog td = new TextDialog(sb.toString(), "Errors occured while parsing the input file");
            td.setVisible(true);
        }
        this.hotspot = generateHotspot(100, 100, jSlider_Zoom.getValue());
        jPanel_ImageWorkspace.add(hotspot, 0);
        if (!negativeFound) {
            int xoff = (int) (jPanel_ImageWorkspace.getWidth() * 0.5);
            int yoff = (int) (jPanel_ImageWorkspace.getHeight() * 0.5);
            for (int j = 0; j < animationSystem.size(); j++) {
                ImagePanel i = (ImagePanel) animationSystem.get(j);
                Point p = i.getLocation();
                i.setLocation((int) (p.getX() + xoff), (int) (p.getY() + yoff));
            }
            Point p = hotspot.getLocation();
            hotspot.setLocation((int) (p.getX() + xoff), (int) (p.getY() + yoff));
        }
        jTable1.updateUI();
        jList_Frames.updateUI();
        jComboBox_animations.updateUI();
        jTable2.updateUI();
    }

    private class MyMouseMotionListener implements MouseMotionListener {

        private int prevx = 0, prevy = 0;

        public MyMouseMotionListener(int mousestart_x, int mousestart_y) {
            this.prevx = mousestart_x;
            this.prevy = mousestart_y;
        }

        public void mouseDragged(MouseEvent e) {
            if (editingFile != null) {
                int xoff = e.getXOnScreen() - this.prevx;
                int yoff = e.getYOnScreen() - this.prevy;
                for (int j = 0; j < animationSystem.size(); j++) {
                    ImagePanel i = (ImagePanel) animationSystem.get(j);
                    Point p = i.getLocation();
                    i.setLocation((int) (p.getX() + xoff), (int) (p.getY() + yoff));
                }
                Point p = hotspot.getLocation();
                hotspot.setLocation((int) (p.getX() + xoff), (int) (p.getY() + yoff));
                jPanel_ImageWorkspace.updateUI();
                this.prevx = e.getXOnScreen();
                this.prevy = e.getYOnScreen();
                //setLocation((e.getXOnScreen() - mousestart_x) + mousestart_loc_x, (e.getYOnScreen() - mousestart_y) + mousestart_loc_y);
            }
        }

        public void mouseMoved(MouseEvent e) {
        }
    }

    private class MyMouseListener implements MouseListener {

        private MyMouseMotionListener myml;

        public MyMouseListener() {
        }

        public void mouseClicked(MouseEvent e) {
            jPanel_ImageWorkspace.requestFocus();
        }

        public void mousePressed(MouseEvent e) {
            if (e.getButton() == e.BUTTON1) { //left mouse button
                myml = new MyMouseMotionListener(e.getXOnScreen(), e.getYOnScreen());
                jPanel_ImageWorkspace.addMouseMotionListener(myml);
            }
        }

        public void mouseReleased(MouseEvent e) {
            jPanel_ImageWorkspace.removeMouseMotionListener(myml);
        }

        public void mouseEntered(MouseEvent e) {
        }

        public void mouseExited(MouseEvent e) {
        }
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jToolBar_Quickmenu = new javax.swing.JToolBar();
        jButton_Openfile = new javax.swing.JButton();
        jButton_Savefile = new javax.swing.JButton();
        jSeparator3 = new javax.swing.JToolBar.Separator();
        jButton_HotspotColor = new javax.swing.JButton();
        jSeparator1 = new javax.swing.JToolBar.Separator();
        jButton_SetXYFrames = new javax.swing.JButton();
        jSplitPane_horiz = new javax.swing.JSplitPane();
        jPanel2 = new javax.swing.JPanel();
        jSplitPane_vert = new javax.swing.JSplitPane();
        jTabbedPane1 = new javax.swing.JTabbedPane();
        jPanel_AniTab = new javax.swing.JPanel();
        jLabel_Zoom = new javax.swing.JLabel();
        jSlider_Zoom = new javax.swing.JSlider();
        jPanel_TranspTab = new javax.swing.JPanel();
        jComboBox_Tranparency = new javax.swing.JComboBox();
        jPanel3 = new javax.swing.JPanel();
        jScrollPane1 = new javax.swing.JScrollPane();
        jList_Frames = new javax.swing.JList();
        jSplitPane1 = new javax.swing.JSplitPane();
        jPanel_ImageWorkspace = new javax.swing.JPanel();
        jTabbedPane2 = new javax.swing.JTabbedPane();
        jPanel1 = new javax.swing.JPanel();
        jComboBox_animations = new javax.swing.JComboBox();
        jButton1 = new javax.swing.JButton();
        jButton2 = new javax.swing.JButton();
        jButton3 = new javax.swing.JButton();
        jButton4 = new javax.swing.JButton();
        jComboBox_AnimationType = new javax.swing.JComboBox();
        jButton5 = new javax.swing.JButton();
        jButton6 = new javax.swing.JButton();
        jScrollPane3 = new javax.swing.JScrollPane();
        jTable1 = new javax.swing.JTable();
        jButton7 = new javax.swing.JButton();
        jButton8 = new javax.swing.JButton();
        jButton10 = new javax.swing.JButton();
        jPanel4 = new javax.swing.JPanel();
        jScrollPane2 = new javax.swing.JScrollPane();
        jTable2 = new javax.swing.JTable();
        jButton_Autoloop = new javax.swing.JButton();
        jButton11 = new javax.swing.JButton();
        jToolBar_statusbar = new javax.swing.JToolBar();
        jLabel_LoadedFile = new javax.swing.JLabel();
        jSeparator2 = new javax.swing.JToolBar.Separator();
        jLabel_MouseXY = new javax.swing.JLabel();
        jSeparator4 = new javax.swing.JToolBar.Separator();
        jLabel1 = new javax.swing.JLabel();
        jMenuBar1 = new javax.swing.JMenuBar();
        jMenu_File = new javax.swing.JMenu();
        jMenuItem_Open = new javax.swing.JMenuItem();
        jMenuItem_Reload = new javax.swing.JMenuItem();
        jMenuItem_Save = new javax.swing.JMenuItem();
        jMenuItem_SaveAs = new javax.swing.JMenuItem();
        jMenu_Display = new javax.swing.JMenu();
        jCheckBoxMenuItem_ShowSelectedFrameOnly = new javax.swing.JCheckBoxMenuItem();
        jCheckBoxMenuItem_ShowHotspot = new javax.swing.JCheckBoxMenuItem();
        jCheckBoxMenuItem_HighlightFrame = new javax.swing.JCheckBoxMenuItem();
        jMenu_Help = new javax.swing.JMenu();
        jMenuItem_Help = new javax.swing.JMenuItem();
        jMenuItem2 = new javax.swing.JMenuItem();
        jMenu1 = new javax.swing.JMenu();
        jMenuItem1 = new javax.swing.JMenuItem();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setTitle("UQMAnimationTool v0.5+aniBETA");
        setCursor(new java.awt.Cursor(java.awt.Cursor.DEFAULT_CURSOR));

        jToolBar_Quickmenu.setRollover(true);

        jButton_Openfile.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/document-open.png"))); // NOI18N
        jButton_Openfile.setToolTipText("Open file");
        jButton_Openfile.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton_OpenfileActionPerformed(evt);
            }
        });
        jToolBar_Quickmenu.add(jButton_Openfile);

        jButton_Savefile.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/document-save.png"))); // NOI18N
        jButton_Savefile.setToolTipText("Save file");
        jButton_Savefile.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton_SavefileActionPerformed(evt);
            }
        });
        jToolBar_Quickmenu.add(jButton_Savefile);
        jToolBar_Quickmenu.add(jSeparator3);

        jButton_HotspotColor.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/applications-graphics.png"))); // NOI18N
        jButton_HotspotColor.setText("Set Hotspot color");
        jButton_HotspotColor.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton_HotspotColorActionPerformed(evt);
            }
        });
        jToolBar_Quickmenu.add(jButton_HotspotColor);
        jToolBar_Quickmenu.add(jSeparator1);

        jButton_SetXYFrames.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/applications-accessories.png"))); // NOI18N
        jButton_SetXYFrames.setText("Set x/y for selected frames");
        jButton_SetXYFrames.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton_SetXYFramesActionPerformed(evt);
            }
        });
        jToolBar_Quickmenu.add(jButton_SetXYFrames);

        jSplitPane_horiz.setPreferredSize(new java.awt.Dimension(364, 597));

        jSplitPane_vert.setDividerLocation(500);
        jSplitPane_vert.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        jSplitPane_vert.setResizeWeight(1.0);

        jTabbedPane1.setTabLayoutPolicy(javax.swing.JTabbedPane.SCROLL_TAB_LAYOUT);
        jTabbedPane1.setMinimumSize(new java.awt.Dimension(100, 100));
        jTabbedPane1.setPreferredSize(new java.awt.Dimension(100, 90));

        jPanel_AniTab.setBorder(javax.swing.BorderFactory.createEtchedBorder());

        jLabel_Zoom.setText("Zoom: 3");

        jSlider_Zoom.setMajorTickSpacing(5);
        jSlider_Zoom.setMaximum(10);
        jSlider_Zoom.setMinimum(1);
        jSlider_Zoom.setMinorTickSpacing(1);
        jSlider_Zoom.setPaintTicks(true);
        jSlider_Zoom.setSnapToTicks(true);
        jSlider_Zoom.setValue(3);
        jSlider_Zoom.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                jSlider_ZoomStateChanged(evt);
            }
        });

        javax.swing.GroupLayout jPanel_AniTabLayout = new javax.swing.GroupLayout(jPanel_AniTab);
        jPanel_AniTab.setLayout(jPanel_AniTabLayout);
        jPanel_AniTabLayout.setHorizontalGroup(
            jPanel_AniTabLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel_AniTabLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel_AniTabLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(jPanel_AniTabLayout.createSequentialGroup()
                        .addComponent(jLabel_Zoom, javax.swing.GroupLayout.DEFAULT_SIZE, 202, Short.MAX_VALUE)
                        .addContainerGap())
                    .addGroup(jPanel_AniTabLayout.createSequentialGroup()
                        .addComponent(jSlider_Zoom, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addGap(14, 14, 14))))
        );
        jPanel_AniTabLayout.setVerticalGroup(
            jPanel_AniTabLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel_AniTabLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jLabel_Zoom)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jSlider_Zoom, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        jTabbedPane1.addTab("", new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/preferences-desktop.png")), jPanel_AniTab, "Animation settings"); // NOI18N

        jPanel_TranspTab.setPreferredSize(new java.awt.Dimension(100, 100));

        jComboBox_Tranparency.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Don't use any transparency helpers", "Show selected frame transparently", "Show non-selected frames transparently", "Mixed transparencies for selected/non-selected" }));

        javax.swing.GroupLayout jPanel_TranspTabLayout = new javax.swing.GroupLayout(jPanel_TranspTab);
        jPanel_TranspTab.setLayout(jPanel_TranspTabLayout);
        jPanel_TranspTabLayout.setHorizontalGroup(
            jPanel_TranspTabLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel_TranspTabLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jComboBox_Tranparency, 0, 206, Short.MAX_VALUE)
                .addContainerGap())
        );
        jPanel_TranspTabLayout.setVerticalGroup(
            jPanel_TranspTabLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel_TranspTabLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jComboBox_Tranparency, javax.swing.GroupLayout.PREFERRED_SIZE, 23, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(43, Short.MAX_VALUE))
        );

        jTabbedPane1.addTab("", new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/applications-other.png")), jPanel_TranspTab, "Transparency helper settings"); // NOI18N

        jSplitPane_vert.setRightComponent(jTabbedPane1);

        jList_Frames.setFont(new java.awt.Font("Monospaced", 0, 13));
        jList_Frames.setDragEnabled(true);
        jList_Frames.setPreferredSize(null);
        jScrollPane1.setViewportView(jList_Frames);

        javax.swing.GroupLayout jPanel3Layout = new javax.swing.GroupLayout(jPanel3);
        jPanel3.setLayout(jPanel3Layout);
        jPanel3Layout.setHorizontalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 235, Short.MAX_VALUE)
            .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                .addComponent(jScrollPane1, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 235, Short.MAX_VALUE))
        );
        jPanel3Layout.setVerticalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 499, Short.MAX_VALUE)
            .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                .addComponent(jScrollPane1, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 499, Short.MAX_VALUE))
        );

        jSplitPane_vert.setLeftComponent(jPanel3);

        javax.swing.GroupLayout jPanel2Layout = new javax.swing.GroupLayout(jPanel2);
        jPanel2.setLayout(jPanel2Layout);
        jPanel2Layout.setHorizontalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jSplitPane_vert, javax.swing.GroupLayout.DEFAULT_SIZE, 237, Short.MAX_VALUE)
        );
        jPanel2Layout.setVerticalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jSplitPane_vert, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 615, Short.MAX_VALUE)
        );

        jSplitPane_horiz.setLeftComponent(jPanel2);

        jSplitPane1.setDividerLocation(650);
        jSplitPane1.setResizeWeight(1.0);

        jPanel_ImageWorkspace.setBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED));
        jPanel_ImageWorkspace.setPreferredSize(new java.awt.Dimension(420, 595));
        jPanel_ImageWorkspace.addMouseMotionListener(new java.awt.event.MouseMotionAdapter() {
            public void mouseMoved(java.awt.event.MouseEvent evt) {
                jPanel_ImageWorkspaceMouseMoved(evt);
            }
        });

        javax.swing.GroupLayout jPanel_ImageWorkspaceLayout = new javax.swing.GroupLayout(jPanel_ImageWorkspace);
        jPanel_ImageWorkspace.setLayout(jPanel_ImageWorkspaceLayout);
        jPanel_ImageWorkspaceLayout.setHorizontalGroup(
            jPanel_ImageWorkspaceLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 645, Short.MAX_VALUE)
        );
        jPanel_ImageWorkspaceLayout.setVerticalGroup(
            jPanel_ImageWorkspaceLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 609, Short.MAX_VALUE)
        );

        jSplitPane1.setLeftComponent(jPanel_ImageWorkspace);

        jButton1.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/list-add.png"))); // NOI18N
        jButton1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton1ActionPerformed(evt);
            }
        });

        jButton2.setFont(new java.awt.Font("Tahoma", 0, 10));
        jButton2.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/go-up.png"))); // NOI18N
        jButton2.setText("Move up");
        jButton2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton2ActionPerformed(evt);
            }
        });

        jButton3.setFont(new java.awt.Font("Tahoma", 0, 10));
        jButton3.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/go-down.png"))); // NOI18N
        jButton3.setText("Move down");
        jButton3.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton3ActionPerformed(evt);
            }
        });

        jButton4.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/list-add.png"))); // NOI18N
        jButton4.setText("Import selected frame(s) (..)");
        jButton4.setToolTipText("Adds the selected item(s) from the list on the left to the currently selected animation.");
        jButton4.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton4ActionPerformed(evt);
            }
        });

        jComboBox_AnimationType.setFont(new java.awt.Font("Tahoma", 0, 11));

        jButton5.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/help-browser.png"))); // NOI18N
        jButton5.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton5ActionPerformed(evt);
            }
        });

        jButton6.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/list-remove.png"))); // NOI18N
        jButton6.setText("Remove item(s)");
        jButton6.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton6ActionPerformed(evt);
            }
        });

        jTable1.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {
                {null, null, null, null},
                {null, null, null, null},
                {null, null, null, null},
                {null, null, null, null}
            },
            new String [] {
                "Title 1", "Title 2", "Title 3", "Title 4"
            }
        ));
        jScrollPane3.setViewportView(jTable1);

        jButton7.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/list-remove.png"))); // NOI18N
        jButton7.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton7ActionPerformed(evt);
            }
        });

        jButton8.setText("rename");
        jButton8.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton8ActionPerformed(evt);
            }
        });

        jButton10.setText("Set duration for all selected ");
        jButton10.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton10ActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jButton10, javax.swing.GroupLayout.DEFAULT_SIZE, 252, Short.MAX_VALUE)
                    .addComponent(jComboBox_animations, 0, 252, Short.MAX_VALUE)
                    .addComponent(jScrollPane3, javax.swing.GroupLayout.DEFAULT_SIZE, 252, Short.MAX_VALUE)
                    .addGroup(jPanel1Layout.createSequentialGroup()
                        .addComponent(jButton1, javax.swing.GroupLayout.PREFERRED_SIZE, 33, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jButton7, javax.swing.GroupLayout.PREFERRED_SIZE, 34, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 101, Short.MAX_VALUE)
                        .addComponent(jButton8))
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel1Layout.createSequentialGroup()
                        .addComponent(jComboBox_AnimationType, 0, 217, Short.MAX_VALUE)
                        .addGap(2, 2, 2)
                        .addComponent(jButton5, javax.swing.GroupLayout.PREFERRED_SIZE, 33, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(jPanel1Layout.createSequentialGroup()
                        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                            .addComponent(jButton4, javax.swing.GroupLayout.PREFERRED_SIZE, 116, Short.MAX_VALUE)
                            .addComponent(jButton2, javax.swing.GroupLayout.DEFAULT_SIZE, 116, Short.MAX_VALUE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jButton6, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.PREFERRED_SIZE, 129, Short.MAX_VALUE)
                            .addComponent(jButton3, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 129, Short.MAX_VALUE)))))
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jComboBox_animations, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(10, 10, 10)
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jButton8)
                    .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                        .addComponent(jButton1, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(jButton7, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addComponent(jButton5, 0, 0, Short.MAX_VALUE)
                    .addComponent(jComboBox_AnimationType))
                .addGap(10, 10, 10)
                .addComponent(jScrollPane3, javax.swing.GroupLayout.DEFAULT_SIZE, 355, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButton2)
                    .addComponent(jButton3))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addComponent(jButton4, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(jButton6, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addGap(19, 19, 19)
                .addComponent(jButton10)
                .addContainerGap())
        );

        jTabbedPane2.addTab("Animation definition", new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/applications-system.png")), jPanel1); // NOI18N

        jTable2.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {

            },
            new String [] {
                "Title 1", "Title 2"
            }
        ));
        jScrollPane2.setViewportView(jTable2);

        jButton_Autoloop.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/media-playback-start.png"))); // NOI18N
        jButton_Autoloop.setText("Start/stop ani system");
        jButton_Autoloop.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton_AutoloopActionPerformed(evt);
            }
        });

        jButton11.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/help-browser.png"))); // NOI18N
        jButton11.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton11ActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout jPanel4Layout = new javax.swing.GroupLayout(jPanel4);
        jPanel4.setLayout(jPanel4Layout);
        jPanel4Layout.setHorizontalGroup(
            jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel4Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jScrollPane2, javax.swing.GroupLayout.PREFERRED_SIZE, 239, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addGroup(jPanel4Layout.createSequentialGroup()
                        .addComponent(jButton_Autoloop, javax.swing.GroupLayout.PREFERRED_SIZE, 202, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jButton11, javax.swing.GroupLayout.PREFERRED_SIZE, 31, Short.MAX_VALUE)))
                .addContainerGap())
        );
        jPanel4Layout.setVerticalGroup(
            jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel4Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButton_Autoloop)
                    .addComponent(jButton11, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jScrollPane2, javax.swing.GroupLayout.DEFAULT_SIZE, 519, Short.MAX_VALUE)
                .addContainerGap())
        );

        jTabbedPane2.addTab("Playback", new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/applications-multimedia.png")), jPanel4); // NOI18N

        jSplitPane1.setRightComponent(jTabbedPane2);

        jSplitPane_horiz.setRightComponent(jSplitPane1);

        jToolBar_statusbar.setFloatable(false);
        jToolBar_statusbar.setRollover(true);

        jLabel_LoadedFile.setText("No file loaded");
        jToolBar_statusbar.add(jLabel_LoadedFile);
        jToolBar_statusbar.add(jSeparator2);

        jLabel_MouseXY.setText("Mouse: [x,y]");
        jToolBar_statusbar.add(jLabel_MouseXY);
        jToolBar_statusbar.add(jSeparator4);

        jLabel1.setText("Layout mode");
        jToolBar_statusbar.add(jLabel1);

        jMenu_File.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/system-file-manager.png"))); // NOI18N
        jMenu_File.setText("File");

        jMenuItem_Open.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_O, java.awt.event.InputEvent.CTRL_MASK));
        jMenuItem_Open.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/document-open.png"))); // NOI18N
        jMenuItem_Open.setText("Open");
        jMenuItem_Open.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem_OpenActionPerformed(evt);
            }
        });
        jMenu_File.add(jMenuItem_Open);

        jMenuItem_Reload.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_O, java.awt.event.InputEvent.SHIFT_MASK | java.awt.event.InputEvent.CTRL_MASK));
        jMenuItem_Reload.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/anitool-document-reload.png"))); // NOI18N
        jMenuItem_Reload.setText("Reload from disk");
        jMenuItem_Reload.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem_ReloadActionPerformed(evt);
            }
        });
        jMenu_File.add(jMenuItem_Reload);

        jMenuItem_Save.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_S, java.awt.event.InputEvent.CTRL_MASK));
        jMenuItem_Save.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/document-save.png"))); // NOI18N
        jMenuItem_Save.setText("Save");
        jMenuItem_Save.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem_SaveActionPerformed(evt);
            }
        });
        jMenu_File.add(jMenuItem_Save);

        jMenuItem_SaveAs.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F6, 0));
        jMenuItem_SaveAs.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/document-save-as.png"))); // NOI18N
        jMenuItem_SaveAs.setText("Save as");
        jMenuItem_SaveAs.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem_SaveAsActionPerformed(evt);
            }
        });
        jMenu_File.add(jMenuItem_SaveAs);

        jMenuBar1.add(jMenu_File);

        jMenu_Display.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/preferences-desktop-theme.png"))); // NOI18N
        jMenu_Display.setText("Display options");

        jCheckBoxMenuItem_ShowSelectedFrameOnly.setText("Show only selected frame");
        jMenu_Display.add(jCheckBoxMenuItem_ShowSelectedFrameOnly);

        jCheckBoxMenuItem_ShowHotspot.setSelected(true);
        jCheckBoxMenuItem_ShowHotspot.setText("Show hotspot");
        jMenu_Display.add(jCheckBoxMenuItem_ShowHotspot);

        jCheckBoxMenuItem_HighlightFrame.setText("Highlight selected frame");
        jMenu_Display.add(jCheckBoxMenuItem_HighlightFrame);

        jMenuBar1.add(jMenu_Display);

        jMenu_Help.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/help-browser.png"))); // NOI18N
        jMenu_Help.setText("Help");

        jMenuItem_Help.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/help-browser.png"))); // NOI18N
        jMenuItem_Help.setText("Help & About");
        jMenuItem_Help.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem_HelpActionPerformed(evt);
            }
        });
        jMenu_Help.add(jMenuItem_Help);

        jMenuItem2.setText("XStream License Information");
        jMenuItem2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem2ActionPerformed(evt);
            }
        });
        jMenu_Help.add(jMenuItem2);

        jMenuBar1.add(jMenu_Help);

        jMenu1.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/preferences-system.png"))); // NOI18N
        jMenu1.setText("Tools (BETA)");

        jMenuItem1.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/face-smile.png"))); // NOI18N
        jMenuItem1.setText("Export SMIL File");
        jMenuItem1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem1ActionPerformed(evt);
            }
        });
        jMenu1.add(jMenuItem1);

        jMenuBar1.add(jMenu1);

        setJMenuBar(jMenuBar1);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jToolBar_Quickmenu, javax.swing.GroupLayout.DEFAULT_SIZE, 1188, Short.MAX_VALUE)
            .addComponent(jToolBar_statusbar, javax.swing.GroupLayout.DEFAULT_SIZE, 1188, Short.MAX_VALUE)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jSplitPane_horiz, javax.swing.GroupLayout.DEFAULT_SIZE, 1164, Short.MAX_VALUE)
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jToolBar_Quickmenu, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jSplitPane_horiz, javax.swing.GroupLayout.DEFAULT_SIZE, 617, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jToolBar_statusbar, javax.swing.GroupLayout.PREFERRED_SIZE, 38, javax.swing.GroupLayout.PREFERRED_SIZE))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void jButton_AutoloopActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton_AutoloopActionPerformed
        toggleTimer();
    }//GEN-LAST:event_jButton_AutoloopActionPerformed

    private void toggleTimer() {
        if (timon) {
            tim.cancel();
            timon = false;
            jButton_Autoloop.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/media-playback-start.png")));
            jLabel1.setText("Layout mode");
        } else {
            jLabel1.setText("Animation mode");
            for (Animation a : animationSystem.animations) { //Reset all animation timers
                a.resetTimer();
            }
            for (int i = 0; i < animationSystem.frames.size(); i++) { //Fix the z-indexes
                jPanel_ImageWorkspace.setComponentZOrder(animationSystem.frames.get(i), animationSystem.frames.size() - i);
            }
            jButton_Autoloop.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/media-playback-stop.png")));
            tim = new Timer();
            t = new TimerTask() {

                @Override
                public void run() {
                    try {
                        for (Animation a : animationSystem.animations) {
                            a.processAnimationDelta();
                        }
                        repaintFrames();
                    } catch (IllegalArgumentException ex) {
                        Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
                    }
                }
            };
            tim.scheduleAtFixedRate(t, 0, 15); //a bit more than 60fps
            timon = true;
        }
    }

    /**
     * Select the next frame from the list (wraps around)
     */
    private void selectNext() {
        if (jList_Frames.getSelectedIndex() < animationSystem.getListModel().getSize() - 1) {
            jList_Frames.setSelectedIndex(jList_Frames.getSelectedIndex() + 1);
        } else {
            jList_Frames.setSelectedIndex(0);
        }
    }

    /**
     * Select the previous frame from the list (wraps around)
     */
    private void selectPrevious() {
        if (jList_Frames.getSelectedIndex() > 0) {
            jList_Frames.setSelectedIndex(jList_Frames.getSelectedIndex() - 1);
        } else {
            jList_Frames.setSelectedIndex(animationSystem.size() - 1);
        }
    }

    private void jButton_SavefileActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton_SavefileActionPerformed
        saveFile();
    }//GEN-LAST:event_jButton_SavefileActionPerformed

    private void jSlider_ZoomStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_jSlider_ZoomStateChanged
        try {
            jLabel_Zoom.setText("Zoom: " + jSlider_Zoom.getValue());
            mu.acquire();

            for (int k = 0; k < animationSystem.size() + 1; k++) {
                ImagePanel i = hotspot;
                if (k < animationSystem.size()) {
                    i = (ImagePanel) animationSystem.get(k);
                }
                i.setZoom(jSlider_Zoom.getValue());
            }
            jPanel_ImageWorkspace.updateUI();
            mu.release();
        } catch (InterruptedException ex) {
            Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
        }
    }//GEN-LAST:event_jSlider_ZoomStateChanged

    private void jButton_OpenfileActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton_OpenfileActionPerformed
        loadFile(null);
    }//GEN-LAST:event_jButton_OpenfileActionPerformed

    private void jButton_HotspotColorActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton_HotspotColorActionPerformed
        Color c = JColorChooser.showDialog(this, "Choose a color for the hotspot pixel", hotspotColor);
        if (c != null) {
            hotspotColor = c;
            settings.hotspotColor = hotspotColor;
            hotspot.getImage().setRGB(0, 0, hotspotColor.getRGB());
        }
    }//GEN-LAST:event_jButton_HotspotColorActionPerformed

    private void jPanel_ImageWorkspaceMouseMoved(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jPanel_ImageWorkspaceMouseMoved
        if (editingFile != null && jSlider_Zoom != null) {
            int mx = (evt.getX() - hotspot.getX()) / jSlider_Zoom.getValue();
            int my = (evt.getY() - hotspot.getY()) / jSlider_Zoom.getValue();

            jLabel_MouseXY.setText("Mouse: [" + mx + "," + my + "]");
        }
    }//GEN-LAST:event_jPanel_ImageWorkspaceMouseMoved

    private void jButton_SetXYFramesActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton_SetXYFramesActionPerformed
        String newx = JOptionPane.showInputDialog("What should the new X position be for ALL frames (cancel to not change)?");
        String newy = JOptionPane.showInputDialog("What should the new Y position be for ALL frames (cancel to not change)?");
        if (newx != null && !newx.equals("")) {
            try {
                int n = Integer.valueOf(newx);
                for (int i : jList_Frames.getSelectedIndices()) {
                    ImagePanel p = (ImagePanel) animationSystem.get(i);
                    p.setXoff(n);
                }
            } catch (NumberFormatException e) {
                JOptionPane.showMessageDialog(this, "Please input a valid integer");
            }
        }
        if (newy != null && !newy.equals("")) {
            try {
                int n = Integer.valueOf(newy);
                for (int i : jList_Frames.getSelectedIndices()) {
                    ImagePanel p = (ImagePanel) animationSystem.get(i);
                    p.setYoff(n);
                }
            } catch (NumberFormatException e) {
                JOptionPane.showMessageDialog(this, "Please input a valid integer");
            }
        }
        jList_Frames.updateUI();
    }//GEN-LAST:event_jButton_SetXYFramesActionPerformed

    private void jMenuItem_SaveAsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem_SaveAsActionPerformed
        saveFile(null);
    }//GEN-LAST:event_jMenuItem_SaveAsActionPerformed

    private void jMenuItem_OpenActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem_OpenActionPerformed
        loadFile(null);
    }//GEN-LAST:event_jMenuItem_OpenActionPerformed

    private void jMenuItem_HelpActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem_HelpActionPerformed
        TextDialog td = new TextDialog("UQMAnimationTool\n"
                + "\n"
                + "Keyboard commands - Playback:\n"
                + "UP/DOWN arrows: select the next/previous frame in the animation\n"
                + "SPACE: loop through the animation once\n"
                + "P: play the animation repeatedly\n"
                + "\n"
                + "Keyboard commands - Editing:\n"
                + "W: Move the selected frame one pixel to the top\n"
                + "A: Move the selected frame one pixel to the left\n"
                + "S: Move the selected frame one pixel to the bottom\n"
                + "D: Move the selected frame one pixel to the right\n"
                + "SHIFT+W/A/S/D: Same as W/A/S/D, except it moves *all* frames one pixel.\n"
                + "\n"
                + "HELP:\n"
                + "-Everything should be pretty self-explanatory. Load a file, click on an item in the list to bring it to the front of all images displayed. Tune things with various options.\n"
                + "-NOTE THAT everything is ALWAYS displayed and I am only fucking with the Z order, UNLESS the \"only show selected image\" box is checked. If an animation works in UQMAnimationTool, this does not guarantee that it also works in UQM itself (code, etc). Alignment should mostly work allright though. \n"
                + "\n"
                + "Notes:\n"
                + "-Zooming no longer reloads image files. Reloading a file, however, will.\n"
                + "-You might see screwups with complex animations (like the one used in the Lurg from Project6014). Unfortunately, I cannot help this as the code makes sure that screwups like that should not happen, and I'm not parsing any code! :P\n"
                + "-May still have some threading issues regarding zooming. If things bug out; that's why... I guess I should have some kind of 'dev console' that is mapped to System.err and System.out.. hmm..\n"
                + "-The Hotspot color does not have an Alpha value, despite what the .conf file might say... :)", "Help and Notes");
        td.setWrapping(true, true);
        td.setVisible(true);
    }//GEN-LAST:event_jMenuItem_HelpActionPerformed

    private void jMenuItem_SaveActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem_SaveActionPerformed
        saveFile();
    }//GEN-LAST:event_jMenuItem_SaveActionPerformed

    private void jMenuItem_ReloadActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem_ReloadActionPerformed
        loadFile(editingFile);
    }//GEN-LAST:event_jMenuItem_ReloadActionPerformed

    private void jButton1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed
        Animation a = new Animation();
        a.aniType = AnimationType.CIRCULAR;
        animationSystem.addAnimation(a);
        jComboBox_animations.setSelectedIndex(animationSystem.animations.size() - 1); //Select the item we just added
        jComboBox_animations.updateUI();
        jTable2.updateUI();
    }//GEN-LAST:event_jButton1ActionPerformed

    private void jButton2ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton2ActionPerformed
        moveAnimationFrame(-1);
    }//GEN-LAST:event_jButton2ActionPerformed

    private void jButton3ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton3ActionPerformed
        moveAnimationFrame(1);
    }//GEN-LAST:event_jButton3ActionPerformed

    private void jButton4ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton4ActionPerformed
        Animation a = (Animation) animationSystem.animationListModel.getSelectedItem();
        for (int i : jList_Frames.getSelectedIndices()) {
            a.addFrame(new AnimationFrame((ImagePanel) animationSystem.frames.get(i), (a.aniType == AnimationType.BACKGROUND ? -1d : 1d)));
        }
        jTable1.updateUI();
        jComboBox_animations.updateUI();
        jTable2.updateUI();
    }//GEN-LAST:event_jButton4ActionPerformed

    private void jMenuItem1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem1ActionPerformed
        JFileChooser fc = new JFileChooser(settings.lastDirPath);
        fc.addChoosableFileFilter(new FileNameExtensionFilter("SMIL file", "smil"));
        int result = fc.showSaveDialog(this);
        if (result == JFileChooser.APPROVE_OPTION) {
            int resultO = JOptionPane.showOptionDialog(rootPane, "Do you want to negate the X and Y offsets for all frames in the Layout? Answer 'yes' for communication screens, and 'no' for anything else.", "Question", JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, null, JOptionPane.YES_OPTION);

            if (resultO == 0) {
                resultO = -1;
            }
            System.out.println("result is " + resultO);
            SMILExporter smile = new SMILExporter(animationSystem, fc.getSelectedFile(), resultO);
            try {
                smile.doExport();
            } catch (IOException ex) {
                Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
            }
        } else {
            JOptionPane.showMessageDialog(this, "No file specified. Not saving.");
        }
    }//GEN-LAST:event_jMenuItem1ActionPerformed

    private void jButton5ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton5ActionPerformed
        TextDialog td = new TextDialog("Animation types:\n"
                + "----------------\n"
                + "BACKGROUND: Background frames. These should *always* be visible in the background.\n"
                + "CIRCULAR: Circular animation. Shows all frames in order, then restarts from the top again.\n"
                + "RANDOM: Random animation. Frames are shown in a random order. Because SMIL does not appear to support randomness, the SMIL generator generates a random order.\n"
                + "TALK: Talk animation. These should be shown whenever the character is talking. Not sure if this should be RANDOM or CIRCULAR. Currently the exact same as CIRCULAR. Also not sure how we can integrate these with UQM.\n"
                + "YO_YO: Yo-yo animation. Shows all frames in order, then shows them again in backwards order. You can create awesome yo-yo's with this.", "Animation types HELP");
        td.setWrapping(true, true);
        td.setVisible(true);
    }//GEN-LAST:event_jButton5ActionPerformed

    private void jMenuItem2ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem2ActionPerformed
        TextDialog td = new TextDialog("XStream License -- http://xstream.codehaus.org/license.html\n"
                + "\n"
                + "----------------------\n"
                + "Copyright (c) 2003-2006, Joe Walnes\n"
                + "Copyright (c) 2006-2007, XStream Committers\n"
                + "All rights reserved.\n"
                + "\n"
                + "Redistribution and use in source and binary forms, with or without\n"
                + "modification, are permitted provided that the following conditions are met:\n"
                + "\n"
                + "Redistributions of source code must retain the above copyright notice, this list of\n"
                + "conditions and the following disclaimer. Redistributions in binary form must reproduce\n"
                + "the above copyright notice, this list of conditions and the following disclaimer in\n"
                + "the documentation and/or other materials provided with the distribution.\n"
                + "\n"
                + "Neither the name of XStream nor the names of its contributors may be used to endorse\n"
                + "or promote products derived from this software without specific prior written\n"
                + "permission.\n"
                + "\n"
                + "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY\n"
                + "EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES\n"
                + "OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT\n"
                + "SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,\n"
                + "INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED\n"
                + "TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR\n"
                + "BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN\n"
                + "CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY\n"
                + "WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH\n"
                + "DAMAGE.\n"
                + "", "XStream License Information");
        td.setWrapping(true, true);
        td.setVisible(true);
    }//GEN-LAST:event_jMenuItem2ActionPerformed

    private void jButton6ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton6ActionPerformed
        Animation a = (Animation) animationSystem.animationListModel.getSelectedItem();
        int[] ints = jTable1.getSelectedRows();
        for (int i = ints.length - 1; i >= 0; i--) {
            if (ints[i] >= 0 && ints[i] < a.frames.size()) {
                a.frames.remove(ints[i]);
            }
        }
        jTable1.clearSelection();
        jTable1.updateUI();
    }//GEN-LAST:event_jButton6ActionPerformed

    private void jButton7ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton7ActionPerformed
        Animation a = (Animation) animationSystem.animationListModel.getSelectedItem();
        animationSystem.animations.remove(a);
        if (animationSystem.animations.size() >= 0) {
            animationSystem.animationListModel.setSelectedItem(animationSystem.animations.get(0));
        } else {
            animationSystem.animationListModel.setSelectedItem(null);
        }
        jTable1.clearSelection();
        jTable1.updateUI();
    }//GEN-LAST:event_jButton7ActionPerformed

    private void jButton8ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton8ActionPerformed
        String result = JOptionPane.showInputDialog("What should the new name of the animation be?");
        if (result.contains("]") || result.contains("[")) {
            JOptionPane.showMessageDialog(rootPane, "Characters '[' and ']' are not allowed. Removing them.");
        }
        Animation a = (Animation) animationSystem.animationListModel.getSelectedItem();
        a.setName(result);
        jComboBox_animations.updateUI();
        jTable2.updateUI();
    }//GEN-LAST:event_jButton8ActionPerformed

    private void jButton10ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton10ActionPerformed
        Animation a = (Animation) animationSystem.animationListModel.getSelectedItem();
        String newd = JOptionPane.showInputDialog("What should the new duration be for ALL selected frames (cancel to not change)?");
        if (newd != null && !newd.equals("")) {
            for (int i : jTable1.getSelectedRows()) {
                a.frames.get(i).setDuration(Double.valueOf(newd));
            }
        }
        jTable1.updateUI();
    }//GEN-LAST:event_jButton10ActionPerformed

    private void jButton11ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton11ActionPerformed
        TextDialog td = new TextDialog(
                "To use the animation system's playback feature, it first needs to be enabled by hitting the \"start/stop ani system\" button.\n"
                + "Once the playback system is running (UQMAnimationTool should display \"Animation mode\" in the bottom status bar of the main screen), you can start or stop individual animations by clicking the playback buttons in the table.\n"
                + "Note that even background 'animations' have to be started, or else they won't be visible.", "Animation system playback notes");
        td.setWrapping(true, true);
        td.setVisible(true);
    }//GEN-LAST:event_jButton11ActionPerformed

    private void moveAnimationFrame(int delta) {
        Animation a = (Animation) animationSystem.animationListModel.getSelectedItem();
        int indexold = jTable1.getSelectedRow();
        int indexnew = Math.min(Math.max(0, indexold + delta), jTable1.getModel().getRowCount() - 1);
        System.out.println("indexold=" + indexold + "  indexnew=" + indexnew);
        AnimationFrame p = a.frames.get(indexold);
        a.frames.remove(p);
        a.frames.add(indexnew, p);
        jTable1.setRowSelectionInterval(indexnew, indexnew);
        jTable1.updateUI();
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        final String[] argsf = args;
        java.awt.EventQueue.invokeLater(new Runnable() {

            @Override
            public void run() {
                
                MainFrame m = new MainFrame();
                m.setVisible(true);
                for (String s : argsf) {
                    File f = new File(s);
                    if (f.exists()) {
                        m.loadFile(f);
                        break;
                    }
                }
                m.setIconImage(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/uqmani-icon.png")).getImage());
            }
        });
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton jButton1;
    private javax.swing.JButton jButton10;
    private javax.swing.JButton jButton11;
    private javax.swing.JButton jButton2;
    private javax.swing.JButton jButton3;
    private javax.swing.JButton jButton4;
    private javax.swing.JButton jButton5;
    private javax.swing.JButton jButton6;
    private javax.swing.JButton jButton7;
    private javax.swing.JButton jButton8;
    private javax.swing.JButton jButton_Autoloop;
    private javax.swing.JButton jButton_HotspotColor;
    private javax.swing.JButton jButton_Openfile;
    private javax.swing.JButton jButton_Savefile;
    private javax.swing.JButton jButton_SetXYFrames;
    private javax.swing.JCheckBoxMenuItem jCheckBoxMenuItem_HighlightFrame;
    private javax.swing.JCheckBoxMenuItem jCheckBoxMenuItem_ShowHotspot;
    private javax.swing.JCheckBoxMenuItem jCheckBoxMenuItem_ShowSelectedFrameOnly;
    private javax.swing.JComboBox jComboBox_AnimationType;
    private javax.swing.JComboBox jComboBox_Tranparency;
    private javax.swing.JComboBox jComboBox_animations;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel_LoadedFile;
    private javax.swing.JLabel jLabel_MouseXY;
    private javax.swing.JLabel jLabel_Zoom;
    private javax.swing.JList jList_Frames;
    private javax.swing.JMenu jMenu1;
    private javax.swing.JMenuBar jMenuBar1;
    private javax.swing.JMenuItem jMenuItem1;
    private javax.swing.JMenuItem jMenuItem2;
    private javax.swing.JMenuItem jMenuItem_Help;
    private javax.swing.JMenuItem jMenuItem_Open;
    private javax.swing.JMenuItem jMenuItem_Reload;
    private javax.swing.JMenuItem jMenuItem_Save;
    private javax.swing.JMenuItem jMenuItem_SaveAs;
    private javax.swing.JMenu jMenu_Display;
    private javax.swing.JMenu jMenu_File;
    private javax.swing.JMenu jMenu_Help;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JPanel jPanel4;
    private javax.swing.JPanel jPanel_AniTab;
    private javax.swing.JPanel jPanel_ImageWorkspace;
    private javax.swing.JPanel jPanel_TranspTab;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JScrollPane jScrollPane3;
    private javax.swing.JToolBar.Separator jSeparator1;
    private javax.swing.JToolBar.Separator jSeparator2;
    private javax.swing.JToolBar.Separator jSeparator3;
    private javax.swing.JToolBar.Separator jSeparator4;
    private javax.swing.JSlider jSlider_Zoom;
    private javax.swing.JSplitPane jSplitPane1;
    private javax.swing.JSplitPane jSplitPane_horiz;
    private javax.swing.JSplitPane jSplitPane_vert;
    private javax.swing.JTabbedPane jTabbedPane1;
    private javax.swing.JTabbedPane jTabbedPane2;
    private javax.swing.JTable jTable1;
    private javax.swing.JTable jTable2;
    private javax.swing.JToolBar jToolBar_Quickmenu;
    private javax.swing.JToolBar jToolBar_statusbar;
    // End of variables declaration//GEN-END:variables

    private class ListSelectionListenerImpl implements ListSelectionListener {

        public ListSelectionListenerImpl() {
        }

        public void valueChanged(ListSelectionEvent e) {
            repaintFrames();
        }
    }

    /**
     * Repaint all the frames. Set the Z index properly. Don't show certain frames if the UI wants us to ("show only selected frame").
     */
    private void repaintFrames() {
        if (timon) {
            try {
                mu.acquire();
                for (int j = 0; j < animationSystem.size(); j++) {
                    ImagePanel i = (ImagePanel) animationSystem.get(j);
                    i.setVisible(animationSystem.isVisible(i));
                }
                jPanel_ImageWorkspace.updateUI();
                mu.release();
            } catch (InterruptedException ex) {
                Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
            }
        } else {
            try {
                for (int j = 0; j < animationSystem.size(); j++) {
                    ImagePanel i = (ImagePanel) animationSystem.get(j);
                    if (jComboBox_Tranparency.getSelectedIndex() == 0) {
                        i.setTransparency(1f);
                        i.setHighlight(1f);
                    } else if (jComboBox_Tranparency.getSelectedIndex() == 1) {
                        i.setTransparency(1f);
                        i.setHighlight(1f);
                    } else if (jComboBox_Tranparency.getSelectedIndex() == 2) {
                        i.setTransparency(0.5f);
                        i.setHighlight(0.5f);
                    } else if (jComboBox_Tranparency.getSelectedIndex() == 3) {
                        i.setTransparency(0.25f);
                        i.setHighlight(0.25f);
                    }
                }
                if (jList_Frames.getSelectedIndex() != -1) {
                    mu.acquire();
                    if (jCheckBoxMenuItem_ShowSelectedFrameOnly.isSelected()) {
                        for (int j = 0; j < animationSystem.size(); j++) {
                            ImagePanel i = (ImagePanel) animationSystem.get(j);
                            i.setVisible(false);
                        }
                    }
                    try {
                        if (getSelectedImagePanel() != null) {
                            jPanel_ImageWorkspace.setComponentZOrder(getSelectedImagePanel(), 0);
                            getSelectedImagePanel().setVisible(true);

                            if (jComboBox_Tranparency.getSelectedIndex() == 0) {
                                getSelectedImagePanel().setTransparency(1f);
                            } else if (jComboBox_Tranparency.getSelectedIndex() == 1) {
                                getSelectedImagePanel().setTransparency(0.5f);
                            } else if (jComboBox_Tranparency.getSelectedIndex() == 2) {
                                getSelectedImagePanel().setTransparency(1f);
                            } else if (jComboBox_Tranparency.getSelectedIndex() == 3) {
                                getSelectedImagePanel().setTransparency(0.5f);
                            }
                            if (jCheckBoxMenuItem_HighlightFrame.isSelected()) {
                                getSelectedImagePanel().setHighlight(2f);
                            } else {
                                getSelectedImagePanel().setHighlight(getSelectedImagePanel().getTransparency());
                            }
                        }
                        repaintHotspot();
                        jPanel_ImageWorkspace.updateUI();


                    } catch (IllegalArgumentException ex) {
                        Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
                    }
                    mu.release();

                }
            } catch (InterruptedException ex) {
                Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }

    private class AnimationPlayingTableModel extends AbstractTableModel {

        @Override
        public int getRowCount() {
            return animationSystem.animations.size();
        }

        @Override
        public int getColumnCount() {
            return 2;
        }

        @Override
        public Object getValueAt(int rowIndex, int columnIndex) {
            Animation a = (Animation) animationSystem.animations.get(rowIndex);
            if (columnIndex == 0) {
                if (a.playingState == 0) {
                    return new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/media-playback-start.png"));
                } else {
                    return new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/media-playback-stop.png"));
                }
            } else if (columnIndex == 1) {
                return a.toString();
            }
            return "??WTF??";
        }

        @Override
        public String getColumnName(int column) {
            if (column == 0) {
                return "Play";
            } else if (column == 1) {
                return "Animation";
            }
            return "?UNDEFINED_COLUMN?"; //This should never happen as those columns don't exist.
        }

        @Override
        public boolean isCellEditable(int rowIndex, int columnIndex) {
            if (columnIndex == 0) {
                return true;
            } else {
                return false;
            }
        }

        @Override
        public void setValueAt(Object aValue, int rowIndex, int columnIndex) {
            //Don't do anything
        }
    }

    private class AnimationStructureTableModel extends AbstractTableModel {

        public void fireTableStructureChange() {
            fireTableStructureChanged();
        }

        @Override
        public int getRowCount() {
            Animation a = (Animation) animationSystem.animationListModel.getSelectedItem();
            if (a != null) {
                return a.frames.size();
            } else {
                return 0;
            }
        }

        @Override
        public int getColumnCount() {
            Animation a = (Animation) animationSystem.animationListModel.getSelectedItem();
            if (a == null) {
                return 2;
            }
            if (a.aniType == AnimationType.BACKGROUND) {
                return 1;
            } else {
                return 2;
            }
        }

        @Override
        public Object getValueAt(int rowIndex, int columnIndex) {
            Animation a = (Animation) animationSystem.animationListModel.getSelectedItem();
            if (a.aniType == AnimationType.BACKGROUND) {
                if (columnIndex == 0) {
                    return a.frames.get(rowIndex).frame;
                } else {
                    return "?UNDEFINED_COLUMN?"; //This should never happen as those columns don't exist.
                }
            } else {
                if (columnIndex == 0) {
                    return a.frames.get(rowIndex).duration;
                } else if (columnIndex == 1) {
                    return a.frames.get(rowIndex).frame;
                } else {
                    return "?UNDEFINED_COLUMN?"; //This should never happen as those columns don't exist.
                }
            }
        }

        @Override
        public String getColumnName(int column) {
            Animation a = (Animation) animationSystem.animationListModel.getSelectedItem();
            if (a != null && a.aniType == AnimationType.BACKGROUND) {
                if (column == 0) {
                    return "Frame";
                }
            } else {
                if (column == 0) {
                    return "Duration";
                } else if (column == 1) {
                    return "Frame";
                }
            }
            return "?UNDEFINED_COLUMN?"; //This should never happen as those columns don't exist.
        }

        @Override
        public boolean isCellEditable(int rowIndex, int columnIndex) {
            Animation a = (Animation) animationSystem.animationListModel.getSelectedItem();
            if (a.aniType == AnimationType.BACKGROUND) {
                return false;
            } else {
                if (columnIndex == 0) {
                    return true;
                } else {
                    return false;
                }
            }
        }

        @Override
        public void setValueAt(Object aValue, int rowIndex, int columnIndex) {
            Animation a = (Animation) animationSystem.animationListModel.getSelectedItem();
            if (columnIndex == 0) {
                try {
                    a.frames.get(rowIndex).setDuration(Double.valueOf((String) aValue));
                } catch (Exception e) {
                    JOptionPane.showMessageDialog(rootPane, "Invalid value specified. Exception: " + e);
                }
            }
            fireTableCellUpdated(rowIndex, columnIndex);
        }
    }
}
