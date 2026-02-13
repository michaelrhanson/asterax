#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"

camera {
   location  <0, 3, -12>
   direction <0, -.25, 1>
   look_at   <0, 0, 0>
}

light_source { <-3,10,-8> color red 1 green 1 blue 1 }

sphere {<0,0,0>,3
  texture { Silver1 }
}

cylinder {<-8.1+clock,0,0>,<8-clock,0,0>,1
  pigment {Yellow}
  finish {phong 1 ambient .5}
}

cylinder {<0, -clock-.1,0>,<0, clock ,0>,1
  pigment {Yellow}
  finish {phong 1 ambient .5}
}