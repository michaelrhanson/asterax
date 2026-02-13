#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

camera {
 location  <0, 7, -12>
  direction <0, -.9, .9>
   look_at   <0, .5, 0>
 
}

light_source { <-15, 50, -25> color red 1 green 1 blue 1 }

#declare engine = union {
  difference {
    cylinder {<-4.5,0,0>,<-5.5,0,0> .5}
    cylinder {<4.6,0,0>,<-5.5,0,0> .3}
  }

 pigment {color red .5 green .5 blue .7}
 finish {
   Metal
  }
}

#declare bottomRing = difference{
   torus { 5.0,  0.9 }

 pigment {
   radial 
   frequency 8
   color_map {
     [  0.0  color red 0.0 green 0.0 blue 0.0]
     [  0.3  color red 1.0 green 0.0 blue 0.0]
     [  0.7  color red 1.0 green 0.0 blue 0.0]
				 [  1.0  color red 0.0 green 0.0 blue 0.0]
    } // color_map
   scale <.6,.6,.6>
  }

  finish {
    ambient 0.2
    phong 1.0
  }
}

#declare topBody = union {
   sphere {<0,4,0> 1   pigment {color rgb <.6,0,0>} finish {ambient 0.2 phong 1.0}}
   sphere {<-1,4,0> .5   pigment {color rgb <.6,0,0>} finish {ambient 0.2 phong 1.0}}
	  difference {
	     sphere {<-1.9,4,0> .6}
      sphere {<-2.0,4,.1> .6}
      box {<-9.9,4.1,-1>,<9.9,6,1>}
      box {<-9.9,3.9,-1>,<9.9,0,1>}
      rotate y*-15
      pigment {color rgb <1,1,1>}
   }
	  difference {
	     sphere {<-1.9,4,0> .6}
      sphere {<-2.0,4,-.1> .6}
      box {<-9.9,4.1,-1>,<9.9,6,1>}
      box {<-9.9,3.9,-1>,<9.9,0,1>}
      rotate y*15
      pigment {color rgb <1,1,1>}
   }

	  scale  <1.6,1,1>
}

#declare strut = union {
  cylinder { <0,4,0>,<2.4,2.9,0>,.2 pigment {color rgb <.6,.6,.6>}}
  cylinder { <2.5,3,0>,<5,0,0>,.2 pigment {color rgb <.6,.6,.6>}}
}

union {
  object {bottomRing}
  object {topBody}
object {engine}
  object {strut rotate y * 18}
  object {strut rotate y * 48}
  object {strut rotate y * 78}
  object {strut rotate y * 108}

  object {strut rotate y * 198}
  object {strut rotate y * 228} 
  object {strut rotate y * 258} 
  object {strut rotate y * 288} 

  rotate clock * y
}