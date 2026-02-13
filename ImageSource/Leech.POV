#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

camera {
   location  <0, 7, -12>
   direction <0, -.9, .9>
   look_at   <0, -1, 0>
}

light_source { <-15, 50, -25> color red 1 green 1 blue 1 }

#declare dish = difference {
	  sphere {<0,1.5,0>,2.5 scale <1,.8,1>}
   sphere {<0,1.5,0>,2.45 scale <1,.8,1>}
   box    {<-5,1.5*.78,-5>,<5,5,5>}
   pigment {color rgb <.3,.3,.3>}
   finish {
     ambient 0.5
     phong .5
   }
}

#declare antenna = union {
   cylinder {<0,-.8,0>,<0,3,0> .4}
   pigment {color rgb <1,0,1>}
   finish {
     phong 1.0
  }
}

#declare body = difference {
   sphere { <0,0,0>,5 scale <1,.2,1>}
   sphere { <0,1.5,0>,2.5 scale <1,.8,1>}   

  pigment {color rgb <1,.5,.1>}
  finish {
    ambient 0.2
    phong 5.0
  }
}

union {
   object {body}
   object {dish}
   object {antenna}
   object {antenna translate <4,0,0> scale <1,.5,1>}
   object {antenna translate <-4,0,0> scale <1,.5,1>}
   object {antenna translate <0,0,4> scale <1,.5,1>}
   object {antenna translate <0,0,-4> scale <1,.5,1>}
   rotate y*clock
}