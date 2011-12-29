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

import java.util.ArrayList;
import javax.swing.AbstractListModel;
import javax.swing.ComboBoxModel;
import javax.swing.DefaultComboBoxModel;
import javax.swing.ListModel;
import javax.swing.MutableComboBoxModel;
import javax.swing.event.ListDataListener;

/**
 *
 * @author joris
 */
public class AnimationSystem {

    ArrayList<ImagePanel> frames;
    ArrayList<Animation> animations;
    ListModel framesListModel;
    ComboBoxModel animationListModel;

    public AnimationSystem() {
        frames = new ArrayList<ImagePanel>();
        animations = new ArrayList<Animation>();
        framesListModel = new AbstractListModel() {

            @Override
            public int getSize() {
                return frames.size();
            }

            @Override
            public Object getElementAt(int index) {
                return frames.get(index);
            }
        };

        animationListModel = new DefaultComboBoxModel() {

            @Override
            public void addElement(Object anObject) {
                animations.add((Animation) anObject);
            }

            @Override
            public Object getElementAt(int index) {
                return animations.get(index);
            }

            @Override
            public int getSize() {
                return animations.size();
            }

            @Override
            public void removeElement(Object anObject) {
                animations.remove(anObject);
            }
        };

    }

    public ListModel getListModel() {
        return framesListModel;
    }

    public int size() {
        return framesListModel.getSize();
    }

    public Object get(int i) {
        return framesListModel.getElementAt(i);
    }

    public void addAnimation(Animation a) {
        animations.add(a);
    }

    public void clear() {
        frames.clear();
        animations.clear();
    }

    public void addElement(ImagePanel ip) {
        frames.add(ip);
    }

    boolean isVisible(ImagePanel i) {
        for (Animation a : animations) {
            if (a.playingState != 0) {
                if (a.aniType == AnimationType.BACKGROUND) {
                    for (AnimationFrame af : a.frames) {
                        if (af.getFrame() == i) {
                            return true;
                        }
                    }
                } else {

                    if (a.frames.get(a.currentlyPlaying).getFrame() == i) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
}
