/* 
  Render at 32x32
  Expects an 36-step animation from 0 to 350
*/

#include "colors.inc"
#include "textures.inc"
#include "Shapes.inc"
#include "shapes2.inc"

camera {
   location  <0, 0, -3.0>
   direction <0, 0, 3.5>
   look_at   <0, 0, 0>
}

light_source { <-15, 30, -25> color red 1 green 1 blue 1 }
light_source { < 15, 30, -25> color red 1 green 1 blue 1 }

#declare colLen = 0.4

#declare moneyObject = union { 
  	cylinder {<-colLen,0,0>,<colLen,0,0>,.07}
   cylinder {<0,-colLen,0>,<0,colLen,0>,.07}
   cylinder {<0,0,-colLen>,<0,0,colLen>,.07}

  pigment { color red .2 green 1 blue 0.2 }

   finish {
     SilverFinish
   }

   scale <2.8,2.8,2.8>
   rotate x*40
  rotate y*clock // clock
  rotate z*45

}


object {
	moneyObject
   pigment {color rgbf <.2,1,.2, 0>}
   finish {SilverFinish phong 1.0}
   scale <.35,.35,.35>
   rotate <10,0,10>
   rotate clock*y
}

light_source { < 0, 0, 0> color red 1 green 1 blue 1 }