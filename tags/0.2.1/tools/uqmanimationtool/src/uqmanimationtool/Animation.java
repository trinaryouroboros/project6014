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
import java.util.Random;

/**
 *
 * @author joris
 */
public class Animation {

    AnimationType aniType;
    ArrayList<AnimationFrame> frames;
    String name;
    int currentlyPlaying;
    long lastPlayingEndTime;
    long lastProcessing;
    int playingState; //0=stopped, 1=forwards, -1=backwards (for yoyo)
    private Random random;

    public Animation() {
        frames = new ArrayList<AnimationFrame>();
        aniType = AnimationType.UNDEFINED;
        name = "Unnamed animation";
        playingState = 0;
        random = new Random();
    }

    public void setIsPlaying(boolean isPlaying) {
        if (isPlaying == true && frames.size() > 0) {
            playingState = 1;
            if (aniType != AnimationType.BACKGROUND) {
                resetTimer();
            }
        } else {
            playingState = 0;
        }
    }

    public void resetTimer() {
        lastProcessing = System.currentTimeMillis();
        currentlyPlaying = 0;
        if (frames.size() > 0) {
            lastPlayingEndTime = (System.currentTimeMillis() + (long) (frames.get(currentlyPlaying).duration * 1000));
        }
    }

    public void processAnimationDelta() {
        if (aniType != AnimationType.BACKGROUND && frames.size() > 0 && playingState != 0) {
            lastProcessing = System.currentTimeMillis();
            if (lastPlayingEndTime < lastProcessing) {
                nextFrame();
            }
        }
    }

    private void nextFrame() {
        if (aniType == AnimationType.CIRCULAR || aniType == AnimationType.TALK) {
            currentlyPlaying += 1;
            if (currentlyPlaying >= frames.size()) {
                currentlyPlaying = 0;
            }
        } else if (aniType == AnimationType.RANDOM) {
            currentlyPlaying = random.nextInt(frames.size());
        } else if (aniType == AnimationType.YO_YO) {
            if (currentlyPlaying >= frames.size() - 1) { //Last element?
                playingState = -1;
            } else if (currentlyPlaying == 0) {
                playingState = 1;
            }
            currentlyPlaying = currentlyPlaying + playingState;
        }
        lastPlayingEndTime = lastProcessing + (long) (frames.get(currentlyPlaying).duration * 1000) - (lastProcessing - lastPlayingEndTime);
    }

    @Override
    public String toString() {
        return name + " (" + frames.size() + ")";
    }

    public void addFrame(ImagePanel frame, double duration) {
        frames.add(new AnimationFrame(frame, duration));
    }

    public void addFrame(AnimationFrame frame) {
        frames.add(frame);
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name.replaceAll("\\[", "").replaceAll("\\]", "");
    }
}
