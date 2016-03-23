The GeoREST configuration is stored in the restcfg.xml file in the **conf** folder or in the **conf** sub folders. There can be more restcfg.xml files in different **conf** subfolders.
For example if we have GeoREST application for sewer management,water management and property management we can create folder structure as bellow:

Example of **conf** folder structure
```
conf->
     water->
          restcfg.xml
          ...
     sewer->
          restcfg.xml
          ...
     property->
          restcfg.xml
          ...

```


# RestConfig #
The GeoREST restcfg.xml file contains root **RestConfig** element and **Data** element. The **Data** element encapsulates all additional configuration elements.

```
<RestConfig>
  <Data>
  </Data>
</RestConfig>
```

# Configuration elements #

| **Element** | Description|Attributes|
|:------------|:-----------|:---------|
| [Resource](#Resource.md) | defines a new resource with the **unique URI part** to identify the resource and access data|[uripart](#uripart.md)|
| [Source](#Source.md) | defines the data source of the resource|[type](#type.md)|
| [Representation](#Representation.md) | enables the Resource representation type | [renderer](#renderer.md)  [pattern](#pattern.md)|
| [MapDefinition](#MapDefinition.md) | defines MapGuide server Map resource for rendering PNG images |          |
| [DefaultZoomFactor](#MapDefinition.md) | defines the default zoom factor of the PNG image.|          |
| [Templates](#Templates.md) | defines the Representation template files for different HTTP response results | [section](#section.md)  [preffix](#preffix.md)|
| [Method](#Method.md) | defines the allowed HTTP method for Representation  | [name](#name.md)  [maxcount](#maxcount.md)  [maxbboxwidth](#maxbboxwidth.md)  [maxbboxheight](#maxbboxheight.md) |
| [Order](#Order.md) | used to set the data ordering of query result | [fields](#fields.md)  [direction](#direction.md)|
| [AdditionalData](#AdditionalData.md)  | defines an additional "data source" to be used in the Templates element|[resource](#resource.md)  [section](#section.md)  [rowsection](#rowsection.md)   [preffix](#preffix.md)|



## Resource ##
A **Resource** element defines a new resource with the **unique URI part** to identify the resource and access data. URI part is defined with the **uripart attribute**  value. If there are more **Resource** elements with the same **uripart** attribute value in the restcfg.xml file, the first found is valid and all the other **Resource** elements are ignored.

Data source for the resource is specified in the child element **Source**.
<br>
<br>
<b>Attributes:</b>
<h4>uripart</h4>
identifies the URI part to access the resource.<br>
<br>
<blockquote>Example of Resource element and the <b>uripart</b> attribute definition:<br>
<pre><code>   &lt;RestConfig&gt;<br>
    &lt;Data&gt;<br>
     &lt;Resource uripart="property"&gt;<br>
     &lt;/Resource&gt;<br>
    &lt;/Data&gt;<br>
  &lt;/RestConfig&gt;<br>
</code></pre></blockquote>


<h2>Source</h2>
The information about the resource data source is set a <b>Source</b> element. The <b>Source</b> element is child element of the a <b>Resource</b> element.<br>
GeoREST can use an MapGuide server or and FDO connection as the data source. This is specified with the <b>Source</b> element attribute named <b>type</b>.<br>
To use MapGuide server set <b>type</b> attribute value to "MapGuide". To use FDO connection set <b>type</b> attribute value to "FDO".<br>
<br>
<br>
<b>Attributes:</b>
<blockquote><h4>type</h4>
With the <b>type</b> attribute the data source type for this Resource is defined.</blockquote>

<blockquote><h4>The <b>Source</b> element with MapGuide type</h4>
The Source element with <b>type</b> attribute with value set to "MapGuide" defines that MapGuide server Map Source is the data source for this Resource.</blockquote>

<blockquote>Child elements:<br>
</blockquote><ul><li>FeatureSource element: defines the name of the MapGuide Server feature source library<br>
</li><li>FeatureClass element: defines the name of the FDO class<br>
</li><li>ServerIp element: defines IP address of computer running MapGuide server. If this element is not defined GeoREST uses localhost IP address.<br>
</li><li>ServerPort element: defines port of the mapguide server.</li></ul>

<b>NOTE! If ServerIp element is not defined GeoREST uses IP address of localhost computer.</b>

<b>NOTE! If ServerPort element is not defined GeoREST used default port 2812 of MapGuide Open Source server</b>.<br>
<br>
<blockquote>MapGuide <b>Source</b> element syntax:<br>
<pre><code>     &lt;Source type="MapGuide"&gt;<br>
        &lt;FeatureSource&gt;!MapGuide Server feature source library&lt;/FeatureSource&gt;<br>
        &lt;FeatureClass&gt;FDO class name&lt;/FeatureClass&gt;<br>
        &lt;ServerIp&gt;IP address of computer running MapGuide server&lt;/ServerIp&gt;<br>
        &lt;ServerPort&gt;port on which is running the mapguide server&lt;/ServerPort&gt;<br>
     &lt;/Source&gt;<br>
</code></pre></blockquote>

<blockquote>In the example bellow the Resource is accessed with URI part "property". example:  <code>/rest/data/property</code>. The Source type it is set to MapGuide server. For feature source library "Library://Samples/Sheboygan/Data/Parcels.FeatureSource" it is used. FeatureClass value it is set to "SHP_Schema:Parcels". GeoREST connect to the local MapGuide server.</blockquote>

<blockquote>Example for MapGuide source:<br>
<pre><code>   &lt;Resource uripart="property"&gt;<br>
     &lt;Source type="MapGuide"&gt;<br>
      &lt;FeatureSource&gt;Library://Samples/Sheboygan/Data/Parcels.FeatureSource&lt;/FeatureSource&gt;<br>
      &lt;FeatureClass&gt;SHP_Schema:Parcels&lt;/FeatureClass&gt;<br>
     &lt;/Source&gt;<br>
   &lt;/Resource&gt;<br>
</code></pre></blockquote>


<blockquote>In the example bellow GeoREST connect to the MapGuide Server on computer with ip adress 192.168.0.33 and port 2812. This is defined with optional ServerIp and ServerPort elements. The Resource is accessed with URI part "property". example:  <code>/rest/data/property</code>. The Source type it is set to MapGuide server. For feature source library "Library://Samples/Sheboygan/Data/Parcels.FeatureSource" it is used. FeatureClass value it is set to "SHP_Schema:Parcels".</blockquote>

<blockquote>Example for MapGuide source and connection to specified MapGuide server:<br>
<pre><code>   &lt;Resource uripart="property"&gt;<br>
     &lt;Source type="MapGuide"&gt;<br>
      &lt;FeatureSource&gt;Library://Samples/Sheboygan/Data/Parcels.FeatureSource&lt;/FeatureSource&gt;<br>
      &lt;FeatureClass&gt;SHP_Schema:Parcels&lt;/FeatureClass&gt;<br>
      &lt;ServerIp&gt;192.168.0.33&lt;/ServerIp&gt;<br>
      &lt;ServerPort&gt;2812&lt;/ServerPort&gt;<br>
     &lt;/Source&gt;<br>
   &lt;/Resource&gt;<br>
</code></pre></blockquote>



<blockquote><h4>The <b>Source</b> element with the FDO type</h4>
With the <b>Source</b> element <b>type</b> attribute set to "FDO" and FDO connection is the data source for this Resource. <b>Source</b> element has 3 child elements, <b>Provider</b> ,<b>ConnectionString</b> and FeatureClass.</blockquote>

<blockquote>Child elements:<br>
</blockquote><ul><li>Provider element: defines the name of the FDO provider to be used<br>
</li><li>ConnectionString element: defines the connection string for the FDO provider<br>
</li><li>FeatureClass element: defines the name of the FDO class</li></ul>

<blockquote>FDO <b>Source</b> element syntax:<br>
<pre><code>     &lt;Source type="FDO"&gt;<br>
        &lt;Provider&gt;FDO provider name&lt;/Provider&gt;<br>
        &lt;ConnectionString&gt;FDO connection string&lt;/ConnectionString&gt;<br>
        &lt;FeatureClass&gt;FDO class name&lt;/FeatureClass&gt;<br>
     &lt;/Source&gt;<br>
</code></pre></blockquote>

<blockquote>In the example bellow resource is accessed with URI part "property". example: <code>/rest/data/property</code>. Provider element value is set to "SGeo.SDF" meaning that an SDF file will be used. ConnectionString element is set to "File=C:\GEOREST\data\Sheboygan\Parcels.sdf;ReadOnly=FALSE;" and the FeatureClass value it is set to "SHP_Schema:Parcels" FDO class.</blockquote>

<blockquote>Example for FDO source:<br>
<pre><code>    &lt;Resource uripart="property"&gt;<br>
     &lt;Source type="FDO"&gt;<br>
      &lt;Provider&gt;OSGeo.SDF&lt;/Provider&gt;<br>
      &lt;ConnectionString&gt;File=C:\GEOREST\data\Sheboygan\Parcels.sdf;ReadOnly=FALSE;&lt;/ConnectionString&gt;<br>
      &lt;FeatureClass&gt;SHP_Schema:Parcels&lt;/FeatureClass&gt;<br>
     &lt;/Source&gt;<br>
    &lt;/Resource&gt;<br>
</code></pre></blockquote>



<h2>Representation</h2>
A <b>Representation</b> element enables the <b>Resource</b> representation type. To add a new representation for the resource, add<br>
a new <b>Representation</b> element to the <b>Resource</b> element.<br>
<br>
<b>Attributes:</b>
<blockquote><h4>renderer</h4>
The <b>renderer</b> attribute defines the type of data "representation". Examples of <b>renderer</b> attribute values include (consult documentation for other values):<br>
<ul><li>XML<br>
</li><li>JSON<br>
</li><li>PNG/PNG8<br>
</li><li>Template<br>
</li></ul><h4>pattern</h4>
The <b>pattern</b> attribute defines an URI part to access "representation". For example to get JSON response set <code>pattern=".json"</code></blockquote>

<blockquote>Representation element syntax:<br>
<pre><code>    &lt;Representation renderer="&lt;name of the renderer&gt;" pattern="&lt;.pattern string&gt;"/&gt;<br>
</code></pre></blockquote>

<blockquote>In the example bellow an common configuration of <b>Representation</b> elements is shown.<br>
<pre><code>      &lt;Source type="FDO"&gt;<br>
        &lt;Provider&gt;OSGeo.SDF&lt;/Provider&gt;<br>
        &lt;ConnectionString&gt;File=C:\GEOREST\data\Sheboygan\Parcels.sdf;ReadOnly=FALSE;&lt;/ConnectionString&gt;<br>
        &lt;FeatureClass&gt;SHP_Schema:Parcels&lt;/FeatureClass&gt;<br>
       &lt;/Source&gt;<br>
       &lt;Representation renderer="JSON" pattern=".json"/&gt;<br>
       &lt;Representation renderer="XML" pattern=".xml"/&gt;<br>
       &lt;Representation renderer="PNG8" pattern=".png"&gt;<br>
	 &lt;MapDefinition&gt;Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition&lt;/MapDefinition&gt;				<br>
	 &lt;DefaultZoomFactor&gt;1.3&lt;/DefaultZoomFactor&gt;<br>
       &lt;/Representation&gt;<br>
       &lt;Representation renderer="Template" pattern=".kml" mimetype="application/vnd.google-earth.kml+xml" &gt;<br>
	 &lt;Templates section="PROPERTY" prefix="PROPERTY_"&gt;<br>
	  &lt;Many file="property_kml_many.tpl" /&gt;<br>
	  &lt;Single file="property_kml_single.tpl" /&gt;<br>
	  &lt;None file="property_kml_none.tpl" /&gt;<br>
	  &lt;Error file="property_kml_error.tpl" /&gt;<br>
	 &lt;/Templates&gt;<br>
         &lt;Method name="GET"  /&gt;<br>
       &lt;/Representation&gt;<br>
       &lt;Representation renderer="Template" pattern=".html" mimetype="text/html" &gt;<br>
	 &lt;Templates section="PROPERTY" prefix="PROPERTY_"&gt;<br>
	  &lt;Many file="property_html_many.tpl" /&gt;<br>
	  &lt;Single file="property_html_single.tpl" /&gt;<br>
	  &lt;None file="property_html_none.tpl" /&gt;<br>
	  &lt;Error file="property_html_error.tpl" /&gt;<br>
	 &lt;/Templates&gt;<br>
	 &lt;Method name="GET"  /&gt;<br>
       &lt;/Representation&gt;<br>
 &lt;/Resource&gt;<br>
</code></pre></blockquote>

<h3>MapDefinition & DefaultZoomFactor</h3>

<blockquote><h3>MapDefinition</h3>
</blockquote><ul><li>A <b>MapDefinition</b> element is a child element of the <b>Representation</b> element with the renderer attribute value <b>PNG</b>. With <b>MapDefinition</b> element MapGuide server Map resource for rendering PNG images is defined. The value the <b>MapDefinition</b> element is MapGuide Server <b>Map resource</b>.<br>
<b>Note</b>, to use PNG Representation, the <b>Source</b> element must be set to MapGuide.</li></ul>

<blockquote>MapDefinition element syntax:<br>
<pre><code>    &lt;MapDefinition&gt;MapGuide Server Map resource&lt;/MapDefinition&gt;				<br>
</code></pre></blockquote>


<blockquote><h3>DefaultZoomFactor</h3>
</blockquote><ul><li>A <b>DefaultZoomFactor</b> element is a child element of the <b>Representation</b> element with the <b>PNG</b> renderer value. The value of this element is default zoom factor for MapGuide image.</li></ul>

<blockquote>DefaultZoomFactor element syntax:<br>
<pre><code>    &lt;DefaultZoomFactor&gt;default zoom factor value&lt;/DefaultZoomFactor&gt;<br>
</code></pre></blockquote>

<blockquote>Example:<br>
In the example bellow <b>Representation</b> element with renderer value "PNG" has <b>MapDefinition</b> element set to value "Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition" and<br>
the <b>DefaultZoomFactor</b> element value to 1.3.<br>
<pre><code>    &lt;Representation renderer="PNG8" pattern=".png"&gt;<br>
      &lt;MapDefinition&gt;Library://Samples/Sheboygan/Maps/Sheboygan.!MapDefinition&lt;/MapDefinition&gt;				<br>
      &lt;DefaultZoomFactor&gt;1.3&lt;/DefaultZoomFactor&gt;<br>
    &lt;/Representation&gt;<br>
</code></pre></blockquote>

<h3>Templates</h3>
<ul><li>A <b>Templates</b> element is a child element of the <b>Representation</b> element that has the <b>renderer</b> attribute set to the value "Templates". This element defines <b>Representation</b> template files for different HTTP response results.</li></ul>

<blockquote>Templates element syntax:<br>
<pre><code>     &lt;Templates section="section name" prefix="property prefix"&gt;<br>
        &lt;Many file="template file name" /&gt;<br>
        &lt;Single file="template file name" /&gt;<br>
        &lt;None file="template file name" /&gt;<br>
        &lt;Error file="template file name" /&gt;<br>
     &lt;/Templates&gt;<br>
</code></pre></blockquote>

<b>Attributes:</b>
<blockquote><h4>section</h4>
section attribute defines a section name to use in the template file<br>
<h4>prefix</h4>
prefix attribute defines an attribute-feature property prefix to use in the template file</blockquote>

<blockquote>child elements:<br>
<ul><li><b>Many</b>: <b>file</b> attribute sets the template file name used for "many" results<br>
</li><li><b>Single</b>: <b>file</b> attribute sets the template file name used for "Single" results<br>
</li><li><b>None</b>: <b>file</b> attribute sets the template file name used for "None" results<br>
</li><li><b>Error</b>: <b>file</b> attribute sets the template file name used for "Error" results</li></ul></blockquote>

More about configuring Templates read at <a href='RenderersTemplateHtml.md'>GeoREST HTML Templates Configuration Guide</a> and <a href='RenderersTemplateKml.md'>GeoREST KML Templates Configuration Guide</a>.<br>
<br>
<h3>Method</h3>
<ul><li>A <b>Method</b> element defines the allowed HTTP method for <b>Representation</b>. To allow the method, add a new <b>Method</b> element with the method name as the value of the <b>name</b> attribute. Possible methods are GET, PUT, POST, DELETE, OPTION AND HEADER. In the example, bellow only the GET method is allowed.<br>
<pre><code>&lt;Resource uripart="property"&gt;<br>
   &lt;Source type="MapGuide"&gt;<br>
    &lt;FeatureSource&gt;Library://Samples/Sheboygan/Data/Parcels.FeatureSource&lt;/FeatureSource&gt;<br>
    &lt;FeatureClass&gt;SHP_Schema:Parcels&lt;/FeatureClass&gt;<br>
   &lt;/Source&gt;<br>
   &lt;Representation renderer="JSON" pattern=".json"/&gt;<br>
   &lt;Representation renderer="XML" pattern=".xml"/&gt;<br>
   &lt;Representation renderer="PNG8" pattern=".png"&gt;<br>
     &lt;MapDefinition&gt;Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition&lt;/MapDefinition&gt;<br>
     &lt;DefaultZoomFactor&gt;1.3&lt;/DefaultZoomFactor&gt;		<br>
   &lt;/Representation&gt;<br>
   &lt;Representation renderer="Template" pattern=".html" mimetype="text/html" &gt;<br>
    &lt;Templates section="PROPERTY" prefix="PROPERTY_"&gt;<br>
        &lt;Many file="property_html_many.tpl" /&gt;<br>
        &lt;Single file="property_html_single.tpl" /&gt;<br>
        &lt;None file="property_html_none.tpl" /&gt;<br>
        &lt;Error file="property_html_error.tpl" /&gt;<br>
    &lt;/Templates&gt;<br>
    &lt;Method name="GET"  /&gt;<br>
   &lt;/Representation&gt;<br>
&lt;/Resource&gt;<br>
</code></pre></li></ul>

<b>Attributes:</b>
<blockquote><h4>name</h4>
name defines the method name.<br>
Method element with method name "GET" can have additional attributes:<br>
<h4>maxcount</h4>
maxcount defines the max number of the data rows (features) returned by the query.<br>
<h4>maxbboxwidth</h4>
maxbboxwidth defines the maximum bounding box height. If query parameter bounding box height is higher an exception is returned.<br>
<h4>maxbboxheight</h4>
maxbboxheight defines the maximum bounding box width. If query parameter bounding box width is wider an exception is returned.</blockquote>

<blockquote>More about setting bounding box parameter</blockquote>

<blockquote>In the example bellow .kml representation query return maximum number of 500 features. Maximum bounding box width and height are set to 0.009.<br>
If query with bigger bounding box width and height in the parameter (BBOX) is used, query does not return any result.<br>
Example:<br>
<pre><code>    &lt;Representation renderer="Template" pattern=".kml" mimetype="application/vnd.google-earth.kml+xml" &gt;<br>
     &lt;Templates section="PROPERTY" prefix="PROPERTY_"&gt;<br>
      &lt;Many file="property_kml_many.tpl" /&gt;<br>
      &lt;Single file="property_kml_single.tpl" /&gt;<br>
      &lt;None file="property_kml_none.tpl" /&gt;<br>
      &lt;Error file="property_kml_error.tpl" /&gt;<br>
     &lt;/Templates&gt;<br>
     &lt;Method name="GET" maxcount="500" maxbboxwidth="0.009" maxbboxheight="0.009" /&gt;<br>
     &lt;/Representation&gt;		<br>
</code></pre></blockquote>

<blockquote>More about <b>Method</b> read at <a href='ApiMethods.md'>API Method</a>.</blockquote>


<h2>Other configuration elements</h2>

<h3>Order</h3>
<blockquote>An <b>Order</b> element is a child element of the <b>Representation element</b>. The <b>Order</b> element is used to set the data ordering of query result. To define the ordering, ordering fields and ordering direction must be defined.<br>
The ordering fields are defined with <b>fields attribute</b>.<br>
The order direction is defined with <b>direction attribute</b>. This value can be "asc" for ascending order or "desc" for the descending order.<br>
With the <b>Order</b> element an "ORDER BY" option for the query is added.</blockquote>

<b>Attributes</b>
<blockquote><h4>fields</h4>
fields defines comma separated list of field names used to order query result.<br>
<h4>direction</h4>
direction defines direction of order. To define ascending order parameter value is "asc" and for descending order value is "desc".</blockquote>

<ul><li>Order element syntax:<br>
<pre><code>&lt;Representation renderer="JSON" pattern=".json"&gt;<br>
    ...<br>
    &lt;Order fields="field name1,field name2,..." direction="order direction,ASC or DESC" /&gt;<br>
&lt;/Representation&gt;<br>
</code></pre></li></ul>

<ul><li>Example: in example bellow query result are ordered in ascending order by filed "Autogenerated_SDF_ID".<br>
<pre><code>&lt;Representation renderer="Template" pattern=".html" mimetype="text/html" &gt;<br>
    &lt;Templates section="PROPERTY" prefix="PROPERTY_"&gt;<br>
        &lt;Many file="property_html_many.tpl" /&gt;<br>
        &lt;Single file="property_html_single.tpl" /&gt;<br>
        &lt;None file="property_html_none.tpl" /&gt;<br>
        &lt;Error file="property_html_error.tpl" /&gt;<br>
    &lt;/Templates&gt;<br>
    &lt;Method name="GET"  defaultcount="20" maxcount="500" /&gt;<br>
    &lt;Order fields="Autogenerated_SDF_ID" direction="asc" /&gt;<br>
&lt;/Representation&gt;<br>
</code></pre></li></ul>

<h3>AdditionalData</h3>
An <b>AdditionalData</b> element defines an additional "data source"/resource to be used. The <b>AdditionalData</b> element it is a child element of the <b>Templates element</b>.  This additional data are fetched in the template files.<br>
<br>
In the sample bellow the <b>AdditionalData</b> element definition is shown.<br>
As <b>Resource attribute</b> the building resource with <a href='ApiUriParams#Parameter_Name:_bbox.md'>BBOX</a> parameter ( bounding box parameter defines envelope to be used for spatial query ) is defined. Bounding box parameter value is bounding box of the property geometry. This defines that for this property the additional data are buildings, whose geometries are in intersection with bounding box of property geometry.<br>
<br>
<blockquote>AdditionalData element:<br>
<pre><code>&lt;Representation renderer="Template" pattern=".html" mimetype="text/html" &gt;<br>
    &lt;Templates section="PROPERTY" prefix="PROPERTY_"&gt;<br>
        &lt;Many file="property_html_many.tpl" /&gt;<br>
        &lt;Single file="property_html_single.tpl" /&gt;<br>
        &lt;None file="property_html_none.tpl" /&gt;<br>
        &lt;Error file="property_html_error.tpl" /&gt;<br>
	&lt;AdditionalData resource="http://localhost/georest/rest/data/building/.xml?BBOX={{#PROPERTY}}{{PROPERTY_SHPGEOM_BBOX}}{{/PROPERTY}}" section="BUILDING" rowsection="BUILDING_ROW" prefix="BUILDING_" &gt;<br>
        &lt;/AdditionalData&gt;<br>
    &lt;/Templates&gt;<br>
    &lt;Method name="GET"  defaultcount="20" maxcount="500" /&gt;<br>
    &lt;Order fields="Autogenerated_SDF_ID" direction="asc" /&gt;<br>
&lt;/Representation&gt;<br>
</code></pre></blockquote>

<b>Attributes</b>
<blockquote><h4>resource</h4>
resource attribute defines URI to additional data <b>Resource</b>.<br>
<h4>section</h4>
section attribute defines the section name in template file to fetch the additional data<br>
<h4>rowsection</h4>
rowsection attribute defines the name of row section in template file<br>
<h4>prefix</h4>
prefix attribute defines the <b>attribute prefix</b> defines the property prefix for the additional data in the template file