# Model Sources 


### Loading Custom Models
Models are expected to be in .cmo file format. This can be achieved by exporting your model to a popular model format which is Wavefront (.obj) alongside the material file (.mtl). In Blender this is File -> Export -> Wavefront (.obj). To convert from .obj to .cmo there is an included program called meshconvert.exe, the DirectXMesh project can be found [here](https://github.com/microsoft/DirectXMesh).

On Windows command line, use the following command:
**meshconvert.exe source.obj -cmo -nodds -flipz -y**

Where source.obj is the name of your exported Wavefront.obj file.

Now we are ready to custom load the asset into SC2KRender. To do this you will need to modify assets/assetmap.cfg. On a new line use the decimal value of the XBLD object, comma separated by the filename of the object without the file extension. For instance if you want to add a new toy store, your assetmap.cfg may have the line:
**131,toystore**

Of course, you will need to add the .cmo file into your models directory. The .cmo prefix must match your assetmap.cfg, so in this example your model directory would be expected to have toystore.cmo. Do not add non-cmo files to the models directory, the program will crash.

 For a full list of building types, please refer here: http://djm.cc/simcity-2000-info.txt
  
   ### Creating Custom Models
    By default, a 1x1 tile in Blender is a 1x1 tile in SC2KRender - which is 0.5m x 0.5m. Hence a 2x2 model would be 1m x 1m. Currently there is no standard on height, so use your best judgement. Always try and round using precise measurements. This is especially true when it comes to objects that must connect such as power lines, roads, etc. For such aforementioned objects, they will typically have transformations. You may either create separate objects, however SC2KRender does handle object transformations. In a later update I will allow the assetmap.cfg to take arguments that specifies transformations.
