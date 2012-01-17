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

/**
 *
 * @author joris
 */
public class AnimationFrame {

    ImagePanel frame;
    Double duration;

    public AnimationFrame(ImagePanel frame, Double duration) {
        this.frame = frame;
        this.duration = duration;
    }

    @Override
    public String toString() {
        if (duration < 0) {
            return "b [" + frame.toSource() + "]";
        } else {
        return duration + " [" + frame.toSource() + "]";
        }
    }

    public Double getDuration() {
        return duration;
    }

    public ImagePanel getFrame() {
        return frame;
    }

    public void setDuration(Double duration) {
        this.duration = duration;
    }
    
       
    
}
