convert  -size 256x8 -channel R xc: -fx 1.0-0.5*(tanh(15.0*((i/w)-0.5))+1.0) -channel G xc: -fx 1.0-0.5*(tanh(15.0*((i/w)-0.4))+1.0) gradient_text.tga
