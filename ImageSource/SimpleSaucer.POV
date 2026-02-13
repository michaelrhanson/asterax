#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

camera {
   location  <0, 7, -12>
   direction <0, -.9, .9>
   look_at   <0, -1, 0>
}

/*camera {
   location  <0, 0, -12>
   direction <0, -.67, .67>
   look_at   <0, -5, 0>
}*/


light_source { <-15, 50, -25> color red 1 green 1 blue 1 }

#declare topRing = difference{
   torus { 5.0,  0.9 }
   cone {<0,0,0>,0,<8,0,0>,2.5 }
   cone {<0,0,0>,0,<8,0,0>,2.5 rotate y * 72 }
   cone {<0,0,0>,0,<8,0,0>,2.5 rotate y * 144}
   cone {<0,0,0>,0,<8,0,0>,2.5 rotate y * 216}
   cone {<0,0,0>,0,<8,0,0>,2.5 rotate y * 288}

  pigment {color rgb <.3,1,.3>}
  finish {
    ambient 0.2
    phong 1.0
  }
}

#declare bottom = difference {
   sphere {<0,-6,0> 1.5}
   box {<-2,0,-2>,<2,-6,2>}

	  scale <1.4,.6,1.4>
   pigment {color rgb <1,.4,.1>}
   finish {
     ambient 0.2
     phong 1.0
  }
}

#declare strut = cylinder {
  <1.7,-3.6,0>,<5,0,0>,.4 pigment {color rgb <1,1,1>}
}

union {
  object {topRing}
  object {bottom}
  object {strut rotate y * 36}
  object {strut rotate y * 108} 
  object {strut rotate y * 180} 
  object {strut rotate y * 252} 
  object {strut rotate y * 328} 

  rotate clock * y
}