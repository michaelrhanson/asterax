/* 
  Render at 32x32
  Expects an 18-step animation from 0 to 340
*/


#include "colors.inc"
#include "textures.inc"

camera {
   location  <0, 0, -3.0>
   direction <0, 0, 1.071>
   look_at   <0, 0, 0>
}

light_source { <-5, 0, -8> color red 1 green 1 blue 1 }
light_source { < 15, 15, -25> color red 1 green 1 blue 1 }

#declare centerSphere = .5
#declare orbitSphere = .15

sphere {
  	<0,0,0>, centerSphere
	pigment {
    color rgb <.8,0,.8>
/*    gradient z
     color_map {
		     [  0.0  color red 0.2 green 0.0 blue 0.5]
		     [  0.5  color red 1.0 green 0.0 blue 1.0]
		     [  1.0  color red 0.2 green 0.0 blue 0.5]
  		}
    phase clock/360
*/
	}
}

sphere {
    <0,0,1>, orbitSphere
 
    texture {
       finish {
	        ambient 0.2
      }
      pigment {color rgb <0,1,1>}

    }
    rotate x * clock
}

sphere {
    <1,0,0>, orbitSphere
    texture {
       finish {
	        ambient 0.2
      }
    pigment {color rgb <0,1,1>}
  
    }
    rotate y * (clock + 60)
}

sphere {
    <1,0,0>, orbitSphere
    texture {
       finish {
	        ambient 0.2
      }
   pigment {color rgb <0,1,1>}
 
    }
   rotate z * (clock - 60)
   rotate y * 45
}

