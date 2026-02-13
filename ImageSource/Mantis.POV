#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

camera {
   location  <0, 7, -12>
   direction <0, -.67, .67>
   look_at   <0, -5, 0>
}

light_source { <-15, 50, -25> color red 1 green 1 blue 1 }

#declare wing = difference {
       box {<0,-.05,0>,<1,.05,1>}
       sphere {<0,0,1>,.5 scale <2,1,1>}
       sphere {<.86,0,.1>,.15 scale <1,1,1>}
       box {<.86,-.06,-.01>,<1.01,.06,.1>}
       box {<-.01,-.06,-.01>,<.86,.06,.25>}

		     translate <-.5,0,-.5>
       scale <8,4,12>
 
//      pigment {color rgb <.2,.8,0>}

       pigment {
           gradient x
           color_map
           {
 															 [  0.1  color red 0.0 green 0.8 blue 0.2]
															  [  0.3  color red 0.2 green 0.8 blue 0.2]
															  [  0.5  color red 0.2 green 1.0 blue 0.2]
 															 [  0.6  color red 0.0 green 0.8 blue 0.2]
 															 [  0.8  color red 0.0 green 0.0 blue 0.0]
 															 [  1.0  color red 0.0 green 0.8 blue 0.2]
											} 
  									scale <3,1,1>
      }

}

#declare engine = union {
  cylinder {<0,0,0>,<3,0,0> .4 open}
  sphere {<3,0,0> .4}

  rotate y*180
  translate <5.5,-.5,0>

 pigment {color red .7 green .7 blue .9}
 finish {
   Metal
  }
}


#declare bodycap = difference {
       quadric { Paraboloid_X }
       plane {<-1,0,0>,-1}
       scale <2,1,1>
}

#declare cockpit = intersection {
       quadric {Paraboloid_X}
       box {<.7,0,-.5>,<.9,1,.5>}
       pigment {color rgb <1,1,1>}
       translate <0,-.1,0>
}


#declare body = union {
		     object {bodycap translate <-5,0,0>}
       object {cockpit translate <-5,0,0>}
       cylinder {<-3,0,0>,<3,0,0>,1}
       object {bodycap rotate y*180 translate <5,0,0>}

//       pigment {color rgb <.05,.9,.16>}


       pigment {
           gradient y
           color_map
           {
 															 [  0.1  color red 0.0 green 0.8 blue 0.2]
															  [  0.3  color red 0.2 green 0.8 blue 0.2]
															  [  0.5  color red 1.0 green 1.0 blue 0.2]
 															 [  0.8  color red 0.0 green 0.8 blue 0.2]
 															 [  1.0  color red 0.0 green 0.8 blue 0.2]
											} 									
      }


}

union {
   object {body}
   object {wing rotate x*180 rotate y*90 rotate x * -30 translate <1,-2,-3.1>}
   object {wing rotate y*-90 rotate x * 30 translate <1,-2,3.1>}
   object {engine}


   pigment {color rgb <0,1,0>}
   finish {
      ambient 0.2
      diffuse 0.8
      phong 0.8
   }

   rotate y * clock
   rotate x * 10
}