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

/**
 *
 * @author joris
 */
public class ConstellationLine {
    public Star star1;
    public Star star2;

    public ConstellationLine(Star star1, Star star2) {
        this.star1 = star1;
        this.star2 = star2;
    }

    @Override
    public String toString() {
        return star1.getStarALPHA() + star2.getStarALPHA();
    }
    
    
}
