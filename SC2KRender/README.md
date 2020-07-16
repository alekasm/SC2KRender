# SC2KRender

This program is designed to read SimCity 2000 files (.sc2) and display them using DirectX11. Both SimCopter and Streets of SimCity are 3D Maxis games released in the late 90s which immerses players in their SimCity 2000 maps. Expect this project to take a pretty long time, especially when it comes to recreating models.

**Controls:**  
Free Cam: W,A,S,D  
Strafe Up/Down: R,F
Adjust Scale: Up-Arrow,Down-Arrow

**What's Supported:**   
-Displaying XTER data  
-Displaying ALTM data, including water  

**Map Information**   
Original Credits: [http://djm.cc/simcity-2000-info.txt](http://djm.cc/simcity-2000-info.txt)

Water Data (Credits [@petrkratina](https://twitter.com/petrkratina))
 David Moews (djm) was originally confused by ALTM water data. Bits 0 - 4 is the tile height, Bits 5 - 9 is the water height for that tile (if valid). Due to map artifacts, all tiles 0x10 to 0x45 (excluding 0x3E ETT_WATERFALL) produce a water tile directly above itself. Tiles 0x0 - 0xF may still have water height data, this can be ignored.

HIGHGROUND Tiles (0xD, 0x1D, 0x2D, 0x3D)
These tiles display as a 1-unit block above the tile height found in ALTM data
