View GeoREST MapGuide layers and OpenLayers:

1. Create HTML

> Create HTML in which your viewer will be seen. Inside HTML include a script from  http://svn.openlayers.org/sandbox/simonpel/openlayers/openlayers/lib/OpenLayers.js
> link  which includes GeoREST support for OpenLayers library.
```
<html>
<head>
  <title>GeoREST OpenLayers Example</title>
    <script src="http://svn.openlayers.org/sandbox/simonpel/openlayers/openlayers/lib/OpenLayers.js"></script>
    </head>
    <body>
      <div style="width:100%; height:100%" id="map"></div>
    </body>
</html>
```
> Example 1: Creating HTML Page

2. Create Viewer
> Create the viewer, so that you specify 2 arguments constructor. First argument is ID of   the element in which the map will be placed. Second arguments is options list in which you specify map options. For example extent,displayProjection,units...
```
var mapOptions = {
 maxExtent: new OpenLayers.Bounds(551808,139792,590267,143119),
 units: 'm',
 displayProjection: new OpenLayers.Projection("EPSG:31275"),
 maxResolution: 'auto'
 };
 map = new OpenLayers.Map( 'map', mapOptions);
```
Example 2: GeoREST Map Constructor

3. Creating the GeoREST MapGuide layer

The next step to creating a viewer is to add a GeoREST MapGuide layer to the Map.
Add 4 arguments to GeoREST MapGuide layer. First argument is layer name,second argument is URL of GeoREST PNG renderer, third parameter is optional and in fourth layers are layer options.
```
 var options = {
  isBaseLayer: true,
  buffer: 1,
  useOverlay: false,
  useAsyncOverlay: false,
  singleTile: true
  };
  
 var params = {
  FORMAT: 'PNG8'
  };

  var url = '/georest/rest/data/odsek/.png';

  var layerMapGuide = new OpenLayers.Layer.GeoREST( "MapGuideLayers", url, params, options );
  map.addLayer(layerMapGuide);
```
Example 3: GeoREST MapGuide Layer Constructor

Finally, in order to display the map, you must set a zoom extent center and zoom level.Use the zoomToMaxExtent function.

4. Putting it All Together¶
```
<html> 
<head>
<script src="/OpenLayers REST/openlayers/lib/OpenLayers.js"></script>
<script type="text/javascript">
 var map;
 function init()
 {
   var mapOptions = {
    maxExtent: new OpenLayers.Bounds(551808,139792,590267,143119),
    units: 'm',
    displayProjection: new OpenLayers.Projection("EPSG:31275"),
    maxResolution: 'auto'
  };
  
  map = new OpenLayers.Map( 'map', mapOptions);
  
  var options = {
   isBaseLayer: true,
   buffer: 1,
   useOverlay: false,
   useAsyncOverlay: false,
   singleTile: true
  };
  
  var params = {
   FORMAT: 'PNG8'
  };

  var url = '/georest/rest/data/odsek/.png';

  var layerMapGuide = new OpenLayers.Layer.GeoREST( "MapGuideLayers", url, params, options    );

  map.addLayer(layerMapGuide);
  map.zoomToExtent(new OpenLayers.Bounds(551808,139792,590267,143119),false); 
  
}
</script>
<style >
.smallmap {
    width: 100%;
    height:100%;
	margin: 0px; padding: 0px; 
	border: 0px solid #ccc;
}
</style>
<body  onLoad="init();">
<div id="map" class="smallmap">
</body>
</html>
```