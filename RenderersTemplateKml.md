In this document configuration of the restcfg.xml and template files for KML representation is shown.

To enable KML representation of the resource the **Representation** element with syntax bellow must be added in the restcfg.xml file.

Representation element Syntax:
```
<Representation renderer="Template" pattern=".kml" mimetype="application/vnd.google-earth.kml+xml" >
   <Templates section="<section name>" prefix="<prefix name>">
     <Many file="<file name>" />
     <Single file="<file name>" />
     <None file="<file name>" />
     <Error file="<file name>" />
   </Templates>
   <Method name="GET"/>
</Representation>
```

Description of the elements from configuration syntax above
| **Element** |
|:------------|
| [Representation](#Representation_element.md) |
| [Method](#Method_element.md) |
| [Templates](#Templates_element.md) |
| [Many](#Many_element.md) |
| [Single](#Single_element.md) |
| [None](#None_element.md) |
| [Error](#Error_element.md) |


### Representation element ###

To configure GeoREST to render template files as KML add **Representation** element to the restcfg.xml file. Set the renderer attribute of this element to value "Template", pattern attribute to value ".kml" and mimetype attribute to value "application/vnd.google-earth.kml+xml". Details about configuring [Configuration](Configuration.md).

In the example bellow it is shown a common configuration of an georest.xml file for the Resource representation as KML. **Renderer** attribute value is "Template", pattern attribute is ".kml" and mimetype attribute is "application/vnd.google-earth.kml+xml".

Example:
```
 <Representation renderer="Template" pattern=".kml" mimetype="application/vnd.google-earth.kml+xml" >
 ...
 </Representation>
```


> #### Method element ####
> A Method element defines the allowed HTTP method for Representation. For KML templates we need only **GET** method.

> Example: Allow GET method
```
     <Representation renderer="Template" pattern=".kml" mimetype="application/vnd.google-earth.kml+xml" >
      ...
      <Method name="GET" />
     </Representation>
```



> #### Templates element ####
> To configure the **Templates element** two attribute values must be set. The **section attribute** value and the **prefix attribute** value. Look at [Section and Prefix](RenderersTemplate#Section_and_Prefix.md).
The **section** and **prefix** attributes define the section name and property prefix names to be used in the templates file.
To mark the **Section** enclose section name as `{{#section name}}` and to mark the section end enclose the section name as `{{/section name}}`.

Example:
section and prefix of **Templates element** definition from restfg.xml
```
     <Representation renderer="Template" pattern=".kml" mimetype="application/vnd.google-earth.kml+xml" >
       <Templates section="PROPERTY" prefix="PROPERTY_">
       </Templates>
       <Method name="GET" />
     </Representation>
```

Section and prefix names used in KML template. Section starts with enclosed section name name `{{#PROPERTY}}` and section ends with `{{/PROPERTY}}`.
To render property name RPROPAD as KML Placemark name use prefix PROPERTY before property name, enclosed with double curly brackets : `{{PROPERTY_RPROPAD}}`.
```
  <kml xmlns="http://www.opengis.net/kml/2.2" xmlns:atom="http://www.w3.org/2005/Atom">
  <Document>
    <name></name>
    <open>1</open>
    <Folder>
      <name>Properties in View</name>
      <open>1</open>
      {{#PROPERTY}}
      <Placemark>
        <name>{{PROPERTY_RPROPAD}}</name>
      </Placemark>
      {{/PROPERTY}}
    </Folder> 
  </Document> 
</kml>
```
**NOTE! The above example will not work because additional Templates element child elements must be defined and also KML geometry it is not rendered in template**

  * Child elements of the Templates element
> > #### Many element ####
> > In the **Many element** the name of the file is defined to be used for "query" with "many" results. The filename is defined with the **file** attribute of the the **Many element**.
Examples:
      * Many element configuration in restcfg.xml. File to be used for queries with "many" result is "property\_kml\_many.tpl".
```
<Representation renderer="Template" pattern=".kml" mimetype="application/vnd.google-earth.kml+xml" >
   <Templates section="PROPERTY" prefix="PROPERTY_">
     <Many file="property_kml_many.tpl" />
   </Templates>
   <Method name="GET"/>
</Representation>
```
      * KML code from property\_kml\_many.tpl file.
In example bellow section starts and ends with section name PROPERTY. **RPROPAD** property is used to be rendered as Placemark name. With **KML\_MULTI** suffix we render KML
geometry for Placemark. About KML suffix read bellow at chapter [KML Geometry suffix](#KML_Geometry_suffix.md).

How to define template file for query with "Single" result look bellow at **Single element** configuration.
```
<kml xmlns="http://www.opengis.net/kml/2.2" xmlns:atom="http://www.w3.org/2005/Atom">
  <Document>
    <name></name>
    <open>1</open>
    <Folder>
      <name>Properties in View</name>
      <open>1</open>
      {{#PROPERTY}}
      <Placemark>
        <name>{{PROPERTY_RPROPAD}}</name>
        <description></description>
        <snippet/>
        <styleUrl></styleUrl>
        {{PROPERTY_SHPGEOM_KML_MULTI}}
      </Placemark>
      {{/PROPERTY}}
    </Folder> 
  </Document> 
</kml>

```


> #### Single element ####
**Single  element** defines the name of the file used for "queries" with single results. The filename is defined with the **file attribute** of the the **Single element**.
Examples:
    * Single element configuration in restcfg.xml.
```
<Representation renderer="Template" pattern=".kml" mimetype="application/vnd.google-earth.kml+xml" >
   <Templates section="PROPERTY" prefix="PROPERTY_">
     <Many file="property_kml_many.tpl" />
     <Single file="property_kml_single.tpl" />
   </Templates>
   <Method name="GET"/>
</Representation>
```
    * HTML code from property\_kml\_single.tpl file.

In example bellow section starts and ends with section name PROPERTY. For the Document name and the Placemark name property value RPROPAD is rendered. In the KML description
an HTML <a> element, with href to HTML template of item, is rendered. Look at <a href='RenderersTemplateHtml.md'>GeoREST HTML Templates Configuration Guide</a>.<br>
In the KML ExtendedData part, Autogenerated_SDF_ID and RPROPAD property values are rendered.<br>
<br>
<pre><code>{{#PROPERTY}}<br>
&lt;kml xmlns="http://www.opengis.net/kml/2.2" xmlns:atom="http://www.w3.org/2005/Atom"&gt;<br>
  &lt;Document&gt;<br>
    &lt;name&gt;GeoREST Demo - {{PROPERTY_RPROPAD}}&lt;/name&gt;<br>
    &lt;Placemark&gt;<br>
      &lt;name&gt;{{PROPERTY_RPROPAD}}&lt;/name&gt;<br>
      &lt;description&gt;&lt;![CDATA[View property report for &lt;a href="http://localhost:99/rest/data/property/{{PROPERTY_Autogenerated_SDF_ID}}.html"&gt;{{PROPERTY_RPROPAD}}&lt;/a&gt;]]&gt;&lt;/description&gt;<br>
      &lt;ExtendedData&gt;<br>
          &lt;SchemaData schemaUrl="http://localhost:99/kml_static/schema_property_v1.kml#georest-property-schema"&gt;<br>
            &lt;SimpleData name="Autogenerated_SDF_ID"&gt;{{PROPERTY_Autogenerated_SDF_ID}}&lt;/SimpleData&gt;<br>
            &lt;SimpleData name="RPROPAD"&gt;{{PROPERTY_RPROPAD}}&lt;/SimpleData&gt;<br>
          &lt;/SchemaData&gt;<br>
      &lt;/ExtendedData&gt;<br>
      {{PROPERTY_SHPGEOM_KML}}<br>
    &lt;/Placemark&gt;<br>
    &lt;atom:author&gt;      <br>
      &lt;atom:name&gt;GeoREST Sample Data&lt;/atom:name&gt;    <br>
      &lt;atom:uri&gt;http://www.georest.org/&lt;/atom:uri&gt;    <br>
    &lt;/atom:author&gt;    <br>
  &lt;/Document&gt; <br>
&lt;/kml&gt;{{/PROPERTY}}<br>
</code></pre>

<blockquote><h4>None element</h4>
In the <b>None element</b> the file is defined to be used for "query" which do not return any result. The filename is defined with the file attribute of the the <b>None element</b>.</blockquote>

Examples:<br>
<pre><code>&lt;Representation renderer="Template" pattern=".kml" mimetype="application/vnd.google-earth.kml+xml" &gt;<br>
   &lt;Templates section="PROPERTY" prefix="PROPERTY_"&gt;<br>
     &lt;Many file="property_kml_many.tpl" /&gt;<br>
     &lt;Single file="property_kml_single.tpl" /&gt;<br>
     &lt;None file="property_kml_none.tpl" /&gt;<br>
   &lt;/Templates&gt;<br>
   &lt;Method name="GET"/&gt;<br>
&lt;/Representation&gt;<br>
</code></pre>

<ul><li>KML code from property_kml_none.tpl file.<br>
<pre><code>&lt;kml xmlns="http://www.opengis.net/kml/2.2"&gt;<br>
  &lt;Document&gt;<br>
    &lt;Folder&gt;<br>
      &lt;name&gt;No Results Found&lt;/name&gt;<br>
    &lt;/Folder&gt;<br>
  &lt;/Document&gt; <br>
&lt;/kml&gt;<br>
</code></pre></li></ul>


<blockquote><h4>Error element</h4>
<b>Error element</b> defines the file to be used for "query" which return an error or exception as result. The filename is defined with the file attribute of the the <b>Error element</b>.</blockquote>

Examples:<br>
<pre><code>&lt;Representation renderer="Template" pattern=".kml" mimetype="application/vnd.google-earth.kml+xml" &gt;<br>
   &lt;Templates section="PROPERTY" prefix="PROPERTY_"&gt;<br>
     &lt;Many file="property_kml_many.tpl" /&gt;<br>
     &lt;Single file="property_kml_single.tpl" /&gt;<br>
     &lt;None file="property_kml_none.tpl" /&gt;<br>
     &lt;Error file="property_kml_error.tpl" /&gt;<br>
   &lt;/Templates&gt;<br>
   &lt;Method name="GET"/&gt;<br>
&lt;/Representation&gt;<br>
</code></pre>

<ul><li>KML code from property_kml_error.tpl file.</li></ul>

<pre><code>&lt;kml xmlns="http://www.opengis.net/kml/2.2"&gt;<br>
  &lt;Document&gt;<br>
    &lt;name&gt;Sorry, an error has occured during your query.&lt;/name&gt;<br>
    &lt;open&gt;1&lt;/open&gt;<br>
    &lt;Folder&gt;<br>
      &lt;open&gt;1&lt;/open&gt;<br>
    &lt;/Folder&gt;<br>
  &lt;/Document&gt;<br>
&lt;/kml&gt;<br>
</code></pre>




<h4>KML Geometry suffix</h4>
KML Geometry Property suffix perform geometry rendering in KML geometry format. KML Geometry suffix it's used in KML template to render feature geometry to be recognized as KML geometry. For details and sample look at <a href='RenderersTemplateKml.md'>Configuring KML Templates</a>.<br>
<br>
<ul><li><b>KML</b> suffix<br>
<blockquote>For the <b>KML</b> suffix, GeoREST renderer renders item geometry in KML geometry format.</blockquote></li></ul>

<blockquote>Description for the example bellow: PROPERTY<i>is prefix, SHPGEOM is geometry property name and KML is geometry suffix.<br>
Example:<br>
<pre><code>    &lt;Placemark&gt;<br>
        {{PROPERTY_SHPGEOM_KML}}<br>
    &lt;/Placemark&gt;<br>
</code></pre></blockquote></i>

<ul><li><b>KML_CENTROID</b> suffix</li></ul>

<blockquote>For the <b>KML_CENTROID</b> suffix, GeoREST renderer renders KML geometry of the item geometry center point.<br>
Description for the example bellow: PROPERTY<i>is prefix, SHPGEOM is geometry property name and KML_CENTROID is geometry suffix.<br>
Example:<br>
<pre><code>    &lt;Placemark&gt;<br>
        {{PROPERTY_SHPGEOM_KML_CENTROID}}<br>
    &lt;/Placemark&gt;<br>
</code></pre></blockquote></i>


<ul><li><b>KML_MULTI</b>
<blockquote>For the <b>KML_MULTI</b> suffix, GeoREST renderer renders KML multi geometry of the item geometry center point.<br>
Description for the example bellow: PROPERTY<i>is prefix, SHPGEOM is geometry property name and KML_MULTI is geometry suffix.<br>
Example:<br>
<pre><code>    &lt;Placemark&gt;<br>
        {{PROPERTY_SHPGEOM_KML_MULTI}}<br>
    &lt;/Placemark&gt;<br>
</code></pre></blockquote></li></ul></i>


<pre><code>{{#PROPERTY}}<br>
&lt;kml xmlns="http://www.opengis.net/kml/2.2" xmlns:atom="http://www.w3.org/2005/Atom"&gt;<br>
  &lt;Document&gt;<br>
    &lt;name&gt;{{PROPERTY_RPROPAD}}&lt;/name&gt;<br>
    &lt;Placemark&gt;<br>
      &lt;name&gt;{{PROPERTY_RPROPAD}}&lt;/name&gt;<br>
      {{PROPERTY_SHPGEOM_KML}}<br>
    &lt;/Placemark&gt;<br>
  &lt;/Document&gt; <br>
&lt;/kml&gt;{{/PROPERTY}}<br>
</code></pre>


To configure GeoREST to render template file as another response, change <b>pattern</b> and <b>mimetype</b> attribute to appropriate values.<br>
Look at <a href='RenderersTemplate.md'>Template Renderer Information</a> and at <a href='RenderersTemplateHtml.md'>GeoREST HTML Templates</a>.