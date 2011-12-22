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
import javax.imageio.IIOException;
import javax.swing.AbstractAction;
import javax.swing.DefaultListModel;
import javax.swing.JColorChooser;
import javax.swing.JComponent;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.KeyStroke;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.filechooser.FileNameExtensionFilter;

/**
 *
 * @author joris
 */
public class MainFrame extends javax.swing.JFrame {

    Mutex mu = new Mutex();
    DefaultListModel listModel = new DefaultListModel();
    Color hotspotColor = Color.red;
    ImagePanel hotspot;
    TimerTask t;
    Timer tim;
    boolean timon = false;
    File editingFile;
    Settings settings;
    Integer animationTo = null;
    int animPass = 0;

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
        for (int k = 0; k < listModel.size(); k++) {
            ImagePanel ip = (ImagePanel) listModel.get(k);
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
        //Space
        jList_Frames.getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).remove(KeyStroke.getKeyStroke(' '));
        for (KeyListener k : jList_Frames.getKeyListeners()) {
            jList_Frames.removeKeyListener(k);
        }
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke(' '),
                "doSpace");
        getRootPane().getActionMap().put("doSpace",
                new AbstractAction() {

                    @Override
                    public void actionPerformed(ActionEvent e) {
                        if (jList_Frames.getSelectedIndex() != -1) {
                            animationTo = jList_Frames.getSelectedIndex();
                        } else {
                            animationTo = 0;
                        }
                        animPass = 0;
                        toggleTimer();
                    }
                });

        //P
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke('p'),
                "doP");
        getRootPane().getActionMap().put("doP",
                new AbstractAction() {

                    @Override
                    public void actionPerformed(ActionEvent e) {
                        toggleTimer();
                    }
                });

        //W
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke('w'),
                "doW");
        getRootPane().getActionMap().put("doW",
                new AbstractAction() {

                    @Override
                    public void actionPerformed(ActionEvent e) {
                        translateImagePanel(0, 1);
                    }
                });
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke('W'),
                "doW2");
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
        getRootPane().getActionMap().put("doA",
                new AbstractAction() {

                    @Override
                    public void actionPerformed(ActionEvent e) {
                        translateImagePanel(1, 0);
                    }
                });
        getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(KeyStroke.getKeyStroke('A'),
                "doA2");
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
        if (listModel.size() >= jList_Frames.getSelectedIndex()) {
            return (ImagePanel) listModel.get(jList_Frames.getSelectedIndex());
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
            for (int i = 0; i < listModel.getSize(); i++) {
                ImagePanel ip = (ImagePanel) listModel.get(i);
                if (ip != null) {
                    fw.write(ip.toSource() + "\n");
                }
                ip.setChanged(false);
            }
            fw.close();
            jList_Frames.updateUI();
        } catch (IOException ex) {
            Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
            JOptionPane.showMessageDialog(this, "Couldn't save file: " + ex);
        } finally {
            try {
                fw.close();
            } catch (IOException ex) {
                Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
                JOptionPane.showMessageDialog(this, "Couldn't close file: " + ex);
            }
        }
    }

    private void showNotes() {
        new Notes(this, true).setVisible(true);
    }

    private void saveFile() {
        saveFile(editingFile);
    }

    private class KeyAction extends AbstractAction {

        private char key;

        public KeyAction(char key) {
            this.key = key;
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            switch (key) {
                case 'w':
                    System.out.println("LOL W PRESSED");
                    break;
            }
        }
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
        jList_Frames.setModel(listModel);
        listModel.clear();
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
        listModel.clear();
        BufferedReader br = new BufferedReader(new FileReader(f));
        int lnr = 0;
        boolean negativeFound = false;
        while (br.ready()) {
            lnr++;
            String line = br.readLine();
            if (!line.startsWith("#")) {
                String[] splitted = null;
                try {
                    splitted = line.split(" ");
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
                    listModel.addElement(io);
                    io.setOpaque(false);
                    io.setVisible(true);
                } catch (NumberFormatException ne) {
                    JOptionPane.showMessageDialog(this, "NumberFormatException occured. Bah. Ignoring this line (Line Number: " + lnr + ")! Input string: [" + line + "]. Exception message: " + ne.getMessage());
                } catch (ArrayIndexOutOfBoundsException ae) {
                    JOptionPane.showMessageDialog(this, "ArrayIndexOutOfBoundException occured. This probably means your .ani file is bugged. Ignoring this line (Line Number: " + lnr + "). Line: [" + line + "]. Exception message: " + ae.getMessage());
                } catch (IIOException ie) {
                    JOptionPane.showMessageDialog(this, "Ignoring this line (Line Number: " + lnr + "): [" + line + "] Message: " + ie.getMessage(), "IIOException occured (" + ie + ")", JOptionPane.ERROR_MESSAGE);
                } catch (Exception e) {
                    JOptionPane.showMessageDialog(this, "Ignoring this line (Line Number: " + lnr + "): [" + line + "] Message: " + e.getMessage(), "Unhandled exception occured (" + e + ")", JOptionPane.ERROR_MESSAGE);
                }
            }
        }
        this.hotspot = generateHotspot(100, 100, jSlider_Zoom.getValue());
        jPanel_ImageWorkspace.add(hotspot, 0);
        if (!negativeFound) {
            int xoff = (int) (jPanel_ImageWorkspace.getWidth() * 0.5);
            int yoff = (int) (jPanel_ImageWorkspace.getHeight() * 0.5);
            for (int j = 0; j < listModel.size(); j++) {
                ImagePanel i = (ImagePanel) listModel.get(j);
                Point p = i.getLocation();
                i.setLocation((int) (p.getX() + xoff), (int) (p.getY() + yoff));
            }
            Point p = hotspot.getLocation();
            hotspot.setLocation((int) (p.getX() + xoff), (int) (p.getY() + yoff));


        }
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
                for (int j = 0; j < listModel.size(); j++) {
                    ImagePanel i = (ImagePanel) listModel.get(j);
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
        jButton_Autoloop = new javax.swing.JButton();
        jSplitPane_horiz = new javax.swing.JSplitPane();
        jPanel2 = new javax.swing.JPanel();
        jSplitPane_vert = new javax.swing.JSplitPane();
        jScrollPane1 = new javax.swing.JScrollPane();
        jList_Frames = new javax.swing.JList();
        jTabbedPane1 = new javax.swing.JTabbedPane();
        jPanel_AniTab = new javax.swing.JPanel();
        jLabel_Rate = new javax.swing.JLabel();
        jSlider_Rate = new javax.swing.JSlider();
        jLabel_Zoom = new javax.swing.JLabel();
        jSlider_Zoom = new javax.swing.JSlider();
        jPanel_TranspTab = new javax.swing.JPanel();
        jComboBox_Tranparency = new javax.swing.JComboBox();
        jPanel_ImageWorkspace = new javax.swing.JPanel();
        jToolBar_statusbar = new javax.swing.JToolBar();
        jLabel_LoadedFile = new javax.swing.JLabel();
        jSeparator2 = new javax.swing.JToolBar.Separator();
        jLabel_MouseXY = new javax.swing.JLabel();
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

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setTitle("UQMAnimationTool v0.5");
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

        jButton_Autoloop.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/media-playback-start.png"))); // NOI18N
        jButton_Autoloop.setText("Start/Stop autoloop");
        jButton_Autoloop.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton_AutoloopActionPerformed(evt);
            }
        });
        jToolBar_Quickmenu.add(jButton_Autoloop);

        jSplitPane_horiz.setPreferredSize(new java.awt.Dimension(364, 597));

        jSplitPane_vert.setDividerLocation(400);
        jSplitPane_vert.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        jSplitPane_vert.setResizeWeight(1.0);

        jList_Frames.setFont(new java.awt.Font("Monospaced", 0, 13)); // NOI18N
        jList_Frames.setPreferredSize(new java.awt.Dimension(100, 300));
        jScrollPane1.setViewportView(jList_Frames);

        jSplitPane_vert.setTopComponent(jScrollPane1);

        jTabbedPane1.setTabLayoutPolicy(javax.swing.JTabbedPane.SCROLL_TAB_LAYOUT);
        jTabbedPane1.setMinimumSize(new java.awt.Dimension(100, 100));
        jTabbedPane1.setPreferredSize(new java.awt.Dimension(100, 90));

        jPanel_AniTab.setBorder(javax.swing.BorderFactory.createEtchedBorder());

        jLabel_Rate.setText("Rate: 50");

        jSlider_Rate.setMajorTickSpacing(100);
        jSlider_Rate.setMaximum(1000);
        jSlider_Rate.setMinimum(1);
        jSlider_Rate.setMinorTickSpacing(25);
        jSlider_Rate.setPaintTicks(true);
        jSlider_Rate.setCursor(new java.awt.Cursor(java.awt.Cursor.DEFAULT_CURSOR));
        jSlider_Rate.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                jSlider_RateStateChanged(evt);
            }
        });

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
                    .addComponent(jSlider_Zoom, javax.swing.GroupLayout.DEFAULT_SIZE, 202, Short.MAX_VALUE)
                    .addComponent(jLabel_Rate)
                    .addComponent(jSlider_Rate, javax.swing.GroupLayout.DEFAULT_SIZE, 202, Short.MAX_VALUE)
                    .addComponent(jLabel_Zoom, javax.swing.GroupLayout.DEFAULT_SIZE, 202, Short.MAX_VALUE))
                .addGap(12, 12, 12))
        );
        jPanel_AniTabLayout.setVerticalGroup(
            jPanel_AniTabLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel_AniTabLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jLabel_Rate)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jSlider_Rate, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jLabel_Zoom)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jSlider_Zoom, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(17, Short.MAX_VALUE))
        );

        jTabbedPane1.addTab("", new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/preferences-desktop.png")), jPanel_AniTab, "Animation settings"); // NOI18N

        jPanel_TranspTab.setPreferredSize(new java.awt.Dimension(100, 100));

        jComboBox_Tranparency.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Don't use any transparency helpers", "Show selected frame transparently", "Show non-selected frames transparently", "Mixed transparencies for selected/non-selected" }));
        jComboBox_Tranparency.addItemListener(new java.awt.event.ItemListener() {
            public void itemStateChanged(java.awt.event.ItemEvent evt) {
                jComboBox_TranparencyItemStateChanged(evt);
            }
        });
        jComboBox_Tranparency.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jComboBox_TranparencyActionPerformed(evt);
            }
        });
        jComboBox_Tranparency.addPropertyChangeListener(new java.beans.PropertyChangeListener() {
            public void propertyChange(java.beans.PropertyChangeEvent evt) {
                jComboBox_TranparencyPropertyChange(evt);
            }
        });

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
                .addContainerGap(123, Short.MAX_VALUE))
        );

        jTabbedPane1.addTab("", new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/applications-other.png")), jPanel_TranspTab, "Transparency helper settings"); // NOI18N

        jSplitPane_vert.setRightComponent(jTabbedPane1);

        javax.swing.GroupLayout jPanel2Layout = new javax.swing.GroupLayout(jPanel2);
        jPanel2.setLayout(jPanel2Layout);
        jPanel2Layout.setHorizontalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jSplitPane_vert, javax.swing.GroupLayout.DEFAULT_SIZE, 237, Short.MAX_VALUE)
        );
        jPanel2Layout.setVerticalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jSplitPane_vert, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 609, Short.MAX_VALUE)
        );

        jSplitPane_horiz.setLeftComponent(jPanel2);

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
            .addGap(0, 916, Short.MAX_VALUE)
        );
        jPanel_ImageWorkspaceLayout.setVerticalGroup(
            jPanel_ImageWorkspaceLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 605, Short.MAX_VALUE)
        );

        jSplitPane_horiz.setRightComponent(jPanel_ImageWorkspace);

        jToolBar_statusbar.setFloatable(false);
        jToolBar_statusbar.setRollover(true);

        jLabel_LoadedFile.setText("No file loaded");
        jToolBar_statusbar.add(jLabel_LoadedFile);
        jToolBar_statusbar.add(jSeparator2);

        jLabel_MouseXY.setText("Mouse: [x,y]");
        jToolBar_statusbar.add(jLabel_MouseXY);

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

        jMenuBar1.add(jMenu_Help);

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
                .addComponent(jSplitPane_horiz, javax.swing.GroupLayout.DEFAULT_SIZE, 611, Short.MAX_VALUE)
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
        } else {
            jButton_Autoloop.setIcon(new javax.swing.ImageIcon(getClass().getResource("/uqmanimationtool/icons/media-playback-stop.png")));
            tim = new Timer();
            t = new TimerTask() {

                @Override
                public void run() {
                    try {
                        //mu.acquire();
                        boolean proceed = true;
                        if (animationTo != null) {
                            if (jList_Frames.getSelectedIndex() == animationTo) {
                                animPass++;
                                if (animPass > 1) {
                                    animPass = 0;
                                    animationTo = null;
                                    proceed = false;
                                    toggleTimer();
                                }
                            }
                        }
                        if (proceed) {
                            selectNext();


                        }
                        //mu.release();
                        //} catch (InterruptedException ex) {
                        //    Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
                    } catch (IllegalArgumentException ex) {
                        Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
                    }
                }
            };
            tim.scheduleAtFixedRate(t, 0, jSlider_Rate.getValue());
            timon = true;
        }
    }

    /**
     * Select the next frame from the list (wraps around)
     */
    private void selectNext() {
        if (jList_Frames.getSelectedIndex() < listModel.getSize() - 1) {
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
            jList_Frames.setSelectedIndex(listModel.size() - 1);
        }
    }

    private void jSlider_RateStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_jSlider_RateStateChanged
        jLabel_Rate.setText("Rate: " + jSlider_Rate.getValue());
        if (timon) {
            toggleTimer();
            toggleTimer();
        }
    }//GEN-LAST:event_jSlider_RateStateChanged

    private void jButton_SavefileActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton_SavefileActionPerformed
        saveFile();
    }//GEN-LAST:event_jButton_SavefileActionPerformed

    private void jSlider_ZoomStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_jSlider_ZoomStateChanged
        try {
            jLabel_Zoom.setText("Zoom: " + jSlider_Zoom.getValue());
            mu.acquire();
            jPanel_ImageWorkspace.removeAll();
            ArrayList<ImagePanel> newimgpanels = new ArrayList<ImagePanel>();

            boolean negativefound = false;
            int xoff = (int) (jPanel_ImageWorkspace.getWidth() * 0.5);
            int yoff = (int) (jPanel_ImageWorkspace.getHeight() * 0.5);
            for (int j = 0; j < listModel.size(); j++) {
                ImagePanel i = (ImagePanel) listModel.get(j);
                if (i.getXoff() * -1 < 0 || i.getYoff() * -1 < 0) {
                    negativefound = true;
                }
            }

            for (int k = 0; k < listModel.size() + 1; k++) {
                ImagePanel i = hotspot;
                if (k < listModel.size()) {
                    i = (ImagePanel) listModel.get(k);
                }
                //
                ImagePanel j = new ImagePanel(i.getImagepath(), i.getImage(), i.getXoff(), i.getYoff(), jSlider_Zoom.getValue(), i.getSplit1(), i.getSplit2());
                jPanel_ImageWorkspace.add(j, 0);
                j.setOpaque(false);
                j.setVisible(true);
                //j.setLocation(i.getLocation());
                if (i.equals(hotspot)) {
                    hotspot = j;
                } else {
                    newimgpanels.add(j);
                }
                if (negativefound) {
                    j.setLocation((int) (j.getLocation().getX() + xoff), (int) (j.getLocation().getY() + yoff));
                }
            }
            listModel.clear();
            for (ImagePanel i : newimgpanels) {
                listModel.addElement(i);
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

    private void jComboBox_TranparencyActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jComboBox_TranparencyActionPerformed
    }//GEN-LAST:event_jComboBox_TranparencyActionPerformed

    private void jComboBox_TranparencyItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_jComboBox_TranparencyItemStateChanged
    }//GEN-LAST:event_jComboBox_TranparencyItemStateChanged

    private void jComboBox_TranparencyPropertyChange(java.beans.PropertyChangeEvent evt) {//GEN-FIRST:event_jComboBox_TranparencyPropertyChange
    }//GEN-LAST:event_jComboBox_TranparencyPropertyChange

    private void jButton_SetXYFramesActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton_SetXYFramesActionPerformed
        String newx = JOptionPane.showInputDialog("What should the new X position be for ALL frames (cancel to not change)?");
        String newy = JOptionPane.showInputDialog("What should the new Y position be for ALL frames (cancel to not change)?");
        if (newx != null && !newx.equals("")) {
            try {
                int n = Integer.valueOf(newx);
                for (int i : jList_Frames.getSelectedIndices()) {
                    ImagePanel p = (ImagePanel) listModel.get(i);
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
                    ImagePanel p = (ImagePanel) listModel.get(i);
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
        showNotes();
    }//GEN-LAST:event_jMenuItem_HelpActionPerformed

    private void jMenuItem_SaveActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem_SaveActionPerformed
        saveFile();
    }//GEN-LAST:event_jMenuItem_SaveActionPerformed

    private void jMenuItem_ReloadActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem_ReloadActionPerformed
        loadFile(editingFile);
    }//GEN-LAST:event_jMenuItem_ReloadActionPerformed

    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        java.awt.EventQueue.invokeLater(new Runnable() {

            public void run() {
                new MainFrame().setVisible(true);
            }
        });
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton jButton_Autoloop;
    private javax.swing.JButton jButton_HotspotColor;
    private javax.swing.JButton jButton_Openfile;
    private javax.swing.JButton jButton_Savefile;
    private javax.swing.JButton jButton_SetXYFrames;
    private javax.swing.JCheckBoxMenuItem jCheckBoxMenuItem_HighlightFrame;
    private javax.swing.JCheckBoxMenuItem jCheckBoxMenuItem_ShowHotspot;
    private javax.swing.JCheckBoxMenuItem jCheckBoxMenuItem_ShowSelectedFrameOnly;
    private javax.swing.JComboBox jComboBox_Tranparency;
    private javax.swing.JLabel jLabel_LoadedFile;
    private javax.swing.JLabel jLabel_MouseXY;
    private javax.swing.JLabel jLabel_Rate;
    private javax.swing.JLabel jLabel_Zoom;
    private javax.swing.JList jList_Frames;
    private javax.swing.JMenuBar jMenuBar1;
    private javax.swing.JMenuItem jMenuItem_Help;
    private javax.swing.JMenuItem jMenuItem_Open;
    private javax.swing.JMenuItem jMenuItem_Reload;
    private javax.swing.JMenuItem jMenuItem_Save;
    private javax.swing.JMenuItem jMenuItem_SaveAs;
    private javax.swing.JMenu jMenu_Display;
    private javax.swing.JMenu jMenu_File;
    private javax.swing.JMenu jMenu_Help;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel_AniTab;
    private javax.swing.JPanel jPanel_ImageWorkspace;
    private javax.swing.JPanel jPanel_TranspTab;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JToolBar.Separator jSeparator1;
    private javax.swing.JToolBar.Separator jSeparator2;
    private javax.swing.JToolBar.Separator jSeparator3;
    private javax.swing.JSlider jSlider_Rate;
    private javax.swing.JSlider jSlider_Zoom;
    private javax.swing.JSplitPane jSplitPane_horiz;
    private javax.swing.JSplitPane jSplitPane_vert;
    private javax.swing.JTabbedPane jTabbedPane1;
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
        try {
            for (int j = 0; j < listModel.size(); j++) {
                ImagePanel i = (ImagePanel) listModel.get(j);
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
                    for (int j = 0; j < listModel.size(); j++) {
                        ImagePanel i = (ImagePanel) listModel.get(j);
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
