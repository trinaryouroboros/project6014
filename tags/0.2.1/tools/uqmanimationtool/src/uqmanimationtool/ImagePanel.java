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

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.image.BufferedImage;
import java.awt.image.RescaleOp;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.security.NoSuchAlgorithmException;
import javax.imageio.ImageIO;
import javax.swing.JPanel;

public class ImagePanel extends JPanel {

    private String imagepath;
    public BufferedImage image;
    int xoff, yoff, zoom;
    int split1 = -2;
    int split2 = 1;
    float transparency = 1f;
    float highlight = 1f;
    boolean changed = false;

    public ImagePanel(String imagepath, BufferedImage bufferedImage, int xoff, int yoff, int zoom, int split1, int split2) {
        this.imagepath = imagepath;
        this.split1 = split1;
        this.split2 = split2;
        doImageContructorStuff(bufferedImage, xoff, yoff, zoom);
    }

    public ImagePanel(final String imagepath, int xoff, int yoff, int zoom) throws IOException, NoSuchAlgorithmException {
        this.imagepath = imagepath;
        File f = new File(imagepath);
        if (f.exists()) {
            doImageContructorStuff(ImageIO.read(f), xoff, yoff, zoom);
        } else {
            throw new FileNotFoundException("(J) File not found: " + imagepath);
        }
    }

    private void doImageContructorStuff(BufferedImage bufferedImage, int xoff, int yoff, int zoom) {
        //Convert to ARGB:
        int w = bufferedImage.getWidth(null);
        int h = bufferedImage.getHeight(null);
        BufferedImage bi = new BufferedImage(w, h, BufferedImage.TYPE_INT_ARGB);
        Graphics g = bi.getGraphics();
        g.drawImage(bufferedImage, 0, 0, null);

        this.image = bi;
        setOpaque(true);
        setSize(this.image.getWidth() * zoom, this.image.getHeight() * zoom);
        this.setVisible(true);
        this.setLayout(null);
        this.zoom = zoom;
        this.xoff = xoff;
        this.yoff = yoff;
        this.setLocation((this.xoff * -1) * zoom, (this.yoff * -1) * zoom);
    }
    
    public void setZoom(int zoom) {
        setSize(this.image.getWidth() * zoom, this.image.getHeight() * zoom);
        this.zoom = zoom;
        this.setLocation((this.xoff * -1) * zoom, (this.yoff * -1) * zoom);
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        dopaint(g);
    }

    private void dopaint(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;
        float[] scales = {highlight, highlight, highlight, transparency};
        float[] offsets = {0f, 0f, 0f, 0f};
        RescaleOp rop = new RescaleOp(scales, offsets, null);
        g2d.scale(zoom, zoom);
        g2d.drawImage(image, rop, 0, 0);

        //g.drawImage(image, 0, 0, this.getWidth(), this.getHeight(), this);
        //System.err.println("Refresh at " + System.currentTimeMillis());
    }

    public void setTransparency(float transparency) {
        this.transparency = transparency;
    }

    public float getTransparency() {
        return transparency;
    }

    public float getHighlight() {
        return highlight;
    }

    public void setHighlight(float highlight) {
        this.highlight = highlight;
    }

    public String getImagepath() {
        return imagepath;
    }

    public int getXoff() {
        return xoff;
    }

    public int getYoff() {
        return yoff;
    }

    public String toOldString() {
        return "[" + imagepath + " -- " + xoff + " -- " + yoff + "]";
    }

    @Override
    /**
     * Return the string that has to be displayed in (amongst others) the list of frames. Almost equal to "toSource()".
     */
    public String toString() {
        String sString = "NULL";
        if (imagepath != null) {
            try {
                sString = imagepath.substring(imagepath.lastIndexOf(System.getProperty("file.separator")) + 1);
            } catch (Exception e) {
                e.printStackTrace();
                sString = "ERROR";
            }
        }
        return (changed ? "*" : " ") + sString + " " + split1 + " " + split2 + " " + xoff + " " + yoff;
    }
    
    public String getFilename() {
        return imagepath.substring(imagepath.lastIndexOf(System.getProperty("file.separator")) + 1);
    }

    /**
     * Return the 'source' line for this imagepanel, as written in the relevant .ani file
     * @return the 'source' line for this imagepanel, as written in the relevant .ani file
     */
    public String toSource() {
        String sString = "NULL";
        if (imagepath != null) {
            try {
                sString = imagepath.substring(imagepath.lastIndexOf(System.getProperty("file.separator")) + 1);
            } catch (Exception e) {
                e.printStackTrace();
                sString = "ERROR";
            }
        }
        return sString + " " + split1 + " " + split2 + " " + xoff + " " + yoff;
    }

    public BufferedImage getImage() {
        return image;
    }

    public void setDXoff(int dx) {
        xoff += dx;
        Point p = getLocation();
        p.setLocation(p.getX() + ((dx * -1) * zoom), p.getY());
        setLocation(p);
        changed = true;
    }

    public void setXoff(int xoff) {
        setDXoff(xoff - this.xoff);
    }

    public void setYoff(int yoff) {
        setDYoff(yoff - this.yoff);
    }

    public void setDYoff(int dy) {
        yoff += dy;
        Point p = getLocation();
        p.setLocation(p.getX(), p.getY() + ((dy * -1) * zoom));
        setLocation(p);
        changed = true;
    }

    public void setSplit1(int split1) {
        this.split1 = split1;
    }

    public void setChanged(boolean changed) {
        this.changed = changed;
    }

    public void setSplit2(int split2) {
        this.split2 = split2;
    }

    public int getSplit1() {
        return split1;
    }

    public int getSplit2() {
        return split2;
    }
}
