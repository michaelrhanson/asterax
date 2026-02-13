#include "Shapes.inc"
#include "Shapes2.inc"
#include "colors.inc"

// set viewer's position in the scene
camera
{
  location  <0.0, 0, -8> // <X Y Z>
 look_at   <0.0,  0.0,  0.0> // point center of view at this point <X Y Z>
}

light_source {
  0*x
  color red .7  green .7  blue 1.0 
  translate <-3, 5, -3>
}


light_source {
  0*x
  color red .6 green .7 blue 1.0 
  translate <3, 2, -3>
}


light_source {
  0*x
  color red .6 green .7 blue 1.0 
  translate <0.5, 0 , 0>
}

// Set the color of the background (sky)
// background { color red 0.1 green 0.3 blue 0.8 }
 background {color rgb <0,0,0>}

// Shiny creates a small, tight highlight on the object's surface
#declare Shiny = finish {specular 1 roughness 0.001}


union {
	object {Dodecahedron}
	pigment {color rgbf <.6,.7,1,.6>}
 scale <2.5,2.5,2.5>
 finish {
   Shiny
   phong 5
   ambient .2
   diffuse .9
 }
 rotate y * clock
 rotate z * 30
}