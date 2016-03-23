The GeoREST Template Renderer renders data as textual file. We can render data as HTML, KML , Atom Syndication, or any other textual document type. To enable Resource rendering as "Templates" an Representation element with renderer attribute value "Templates" must be added in restcfg.xml file. More about [GeoREST Configuration Guide](Configuration.md).

## Section and Prefix ##

GeoREST renders data inside the **Section** inside template file.
To mark the **Section** **start** enclose section name as `{{#<section_name>}}` and to mark the section **end** enclose the section name as `{{/section name}}`.

To render item property **Prefix** must be put before item property name  `{{<prefix
name+item property name>}}`. Both ( prefix and property name ) must be enclosed with {{}}.

Section and Prefix element template file syntax:
```
     {{#<section_name>}}
       {{<prefix name+item property name1>}}
       {{<prefix name+item property name2>}}
     {{/<section_name>}}
```

**Section** and **Prefix** attribute values are defined in restfg.xml file in **Templates element**.
In the example bellow, the value of **section attribute** in the **Templates element** defines that data for the **Resource** will be rendered inside section named **PROPERTY**.
The **prefix attribute** defines the prefix name to be used before item property name. In the example bellow all the item properties to be rendered in template must start with _PROPERTY.
How to configure Template Renderer read at [Configuration Templates](Configuration#Templates.md)._

  * Example:
    * **section** and **prefix** definition from restfg.xml
```
     <Templates section="PROPERTY" prefix="PROPERTY_">
```

  * **section** and **prefix** names used in template.
> > To render property name RPROPAD use **prefix** PROPERTY_before property name.
```
    <html>
     <head>
     </head>
      <body>    
       {{#PROPERTY}}
        {{PROPERTY_RPROPAD}}
       {{/PROPERTY}}
     </body>
    </html>
```_

  * **Section** name is defined with the **section attribute** value in the **Templates element** for the **Representation element**.

  * **Prefix** name is defined with the **prefix  attribute** value in the **Templates element** for the **Representation element**.


## Property name suffix ##
By using property name suffix it is possible to perform additional operations on the property value.
GeoREST renderer reads the property suffix and executes operation defined with the property suffix. Suffix is always separated from property name with `_` character. GeoREST renderer reads the suffix and performs the requested operation on the property and renders the result in the template.


> ### Geometry Property suffix ###

| **Suffix** | **Description**|
|:-----------|:|
| [CENTROID\_X](#CENTROID_X.md) | renders the X coordinate of the geometry property centroid|
| [CENTROID\_Y](#CENTROID_Y.md) | renders the Y coordinate of the geometry property centroid|
| [REST\_ENVELOPE](#REST_ENVELOPE.md) | renders the geometry envelope coordinates |
| [BBOX](#BBOX.md) | renders the geometry bounding box coordinates |
| [BBOX\_MIN\_X](#BBOX_MIN_X.md) | renders lower left coordinate X of the geometry bounding box|
| [BBOX\_MIN\_Y](#BBOX_MIN_Y.md) | renders lower left coordinate Y of the geometry bounding box |
| [BBOX\_MID\_X](#BBOX_MID_X.md) | renders the bounding box middle coordinate X   |
| [BBOX\_MID\_Y](#BBOX_MID_Y.md) | renders the bounding box middle coordinate Y   |
| [BBOX\_MAX\_X](#BBOX_MAX_X.md) | renders upper right coordinate X of the geometry bounding box |
| [BBOX\_MAX\_Y](#BBOX_MAX_Y.md)  | renders upper right coordinate Y of the geometry bounding box|
| [BBOX\_MAX\_Y](#BBOX_MAX_Y.md)  | renders upper right coordinate Y of the geometry bounding box|
| [KML](#KML.md)  | renders item geometry in KML geometry format|
| [KML\_CENTROID](#KML_CENTROID.md)  | renders the item geometry center point as KML point|
| [KML\_MULTI](#KML_MULTI.md)  || renders the item geometry as KML geometry and geometry|
center point as KML point
| [GeoRSS](#GeoRSS.md)  | renders item geometry in Simple GeoRSS format|
| [GML](#GML.md)  | renders item geometry in GML format|
| [ATOM\_DATE](#ATOM_DATE.md)  | generates Atom date format for date filed|

Geometry Property suffix enables additional "functions" to be peformed on the item geometry property. Bellow is list of all the possible geometry suffixes. `{{prefix name+item geometry property name+_+geometry suffix}}_

> #### CENTROID\_X ####
> GeoREST renderer reads the CENTROID\_X suffix and renders the X coordinate of the geometry property centroid to the template file.
> In the example bellow it is show how the CENTROID\_X suffix it is used.
> PROPERTY_is prefix, SHPGEOM is geometry property name and CENTROID\_X is geometry suffix._

> Example:
```
     {{#PROPERTY}}
     <label>Geometry Centroid X</label>
     <div class="geo">
        <span class="geocentX">{{PROPERTY_SHPGEOM_CENTROID_X}}</span>
     </div>
     {{/PROPERTY}}
```


> #### CENTROID\_Y ####
GeoREST renderer reads the CENTROID\_Y suffix and renders the Y coordinate of the geometry property centroid to the template file.
> In the example bellow it is show how the CENTROID\_Y suffix it is used.
> Description for the example bellow PROPERTY_is prefix, SHPGEOM is geometry property name and CENTROID\_Y is geometry suffix._

> Example:
```
     {{#PROPERTY}}
     <label>Geometry Centroid Y</label>
     <div class="geo">
        <span class="geocentX">{{PROPERTY_SHPGEOM_CENTROID_Y}}</span>
     </div>
     {{/PROPERTY}}
```


> Example:
> In this example it is demonstrated how to render item geometry location
```
    <tr>
    <td>Location</td>
    <td>
      <div class="geo">
        <span class="latitude">{{PROPERTY_SHPGEOM_CENTROID_Y}}</span>,
        <span class="longitude">{{PROPERTY_SHPGEOM_CENTROID_X}}</span>
      </div>
    </td>
    </tr>
```

> #### REST\_ENVELOPE ####
**REST\_ENVELOPE** suffix 'tells' the GeoREST renderer to calculate the minimum bounding rectangle of a geometry and render this envelope data to the template file.
The rendering result are comma separated values of lower left and upper right envelope coordinates: `lower left X,lower left Y,uper right X,upper right Y`

> Description for the example bellow: PROPERTY_is prefix, SHPGEOM is geometry property name and REST\_ENVELOPE is geometry suffix._

> Example:
```
    <tr>
    <td>Geometry Envelope</td>
    <td>
      <div >
        <span >{{PROPERTY_SHPGEOM_REST_ENVELOPE}}</span>,
      </div>
    </td>
    </tr>
```

> #### BBOX ####
**BBOX** suffix ( bounding box ) 'tells' the GeoREST renderer to calculate the minimum bounding rectangle of a geometry and render this bounding box data to the template file.
The rendering result are comma separated values of lower left and upper right envelope coordinates: `lower left X,lower left Y,uper right X,upper right Y`. The rendering result  of the BBOX suffix is the same as using REST\_ENVELOP suffix.

> Description for the example bellow: PROPERTY_is prefix, SHPGEOM is geometry property name and BBOX is geometry suffix._

> Example:
```
    <tr>
    <td>Geometry Envelope</td>
    <td>
      <div >
        <span >{{PROPERTY_SHPGEOM_BBOX}}</span>,
      </div>
    </td>
    </tr>
```

> #### BBOX\_MIN\_X ####
**BBOX\_MIN\_X** suffix ( X coordinate of the lower left bounding box coordinate) 'tells' the GeoREST renderer to calculate the X coordinate of the lower left bounding box coordinate of a geometry and render this value to the template file.

> Description for the example bellow: PROPERTY_is prefix, SHPGEOM is geometry property name and BBOX\_MIN\_X is geometry suffix._

> Example:
```
    <tr>
    <td>Lower left X of the Geometry Bounding Box</td>
    <td>
      <div >
        <span >{{PROPERTY_SHPGEOM_BBOX_MIN_X}}</span>,
      </div>
    </td>
    </tr>
```

> #### BBOX\_MIN\_Y ####
**BBOX\_MIN\_Y** suffix ( Y coordinate of the lower left bounding box corner ) 'tells' the GeoREST renderer to calculate the Y coordinate of the lower left bounding box corner of a geometry and render this value to the template file.

> Description for the example bellow: PROPERTY_is prefix, SHPGEOM is geometry property name and BBOX\_MIN\_Y is geometry suffix._

> Example:
```
    <tr>
    <td>Lower left Y of the Geometry Bounding Box</td>
    <td>
      <div >
        <span >{{PROPERTY_SHPGEOM_BBOX}}</span>,
      </div>
    </td>
    </tr>
```

> #### BBOX\_MID\_X ####
**BBOX\_MID\_X** suffix ( X coordinate of the bounding box middle coordinate) 'tells' the GeoREST renderer to calculate the  X coordinate of the geometry bounding box middle coordinate and render this value to the template file.

> Description for the example bellow: PROPERTY_is prefix, SHPGEOM is geometry property name and BBOX\_MID\_X is geometry suffix._

> Example:
```
    <tr>
    <td>Middle X of the Geometry Bounding Box</td>
    <td>
      <div >
        <span >{{PROPERTY_SHPGEOM_BBOX}}</span>,
      </div>
    </td>
    </tr>
```

> #### BBOX\_MID\_Y ####
**BBOX\_MID\_Y** suffix ( Y coordinate of the bounding box middle coordinate) 'tells' the GeoREST renderer to calculate the  Y coordinate of the geometry  bounding box middle coordinate and render this value to the template file.

> Description for the example bellow: PROPERTY_is prefix, SHPGEOM is geometry property name and BBOX\_MID\_Y is geometry suffix._

> Example:
```
    <tr>
    <td>Middle Y of the Geometry Bounding Box</td>
    <td>
      <div >
        <span >{{PROPERTY_SHPGEOM_BBOX}}</span>,
      </div>
    </td>
    </tr>
```

> #### BBOX\_MAX\_X ####
**BBOX\_MAX\_X** suffix ( X coordinate of the upper right bounding box coordinate) 'tells' the GeoREST renderer to calculate the X coordinate of the upper right bounding box coordinate of a geometry and render this value to the template file.

> Description for the example bellow: PROPERTY_is prefix, SHPGEOM is geometry property name and BBOX\_MAX\_X is geometry suffix._

> Example:
```
    <tr>
    <td>Upper right X of the Geometry Bounding Box</td>
    <td>
      <div >
        <span >{{PROPERTY_SHPGEOM_BBOX_MAX_X}}</span>,
      </div>
    </td>
    </tr>
```

> #### BBOX\_MAX\_Y ####
**BBOX\_MAX\_Y** suffix ( Y coordinate of the upper right bounding box coordinate) 'tells' the GeoREST renderer to calculate the Y coordinate of the upper right bounding box coordinate of a geometry and render this value to the template file.

> Description for the example bellow: PROPERTY_is prefix, SHPGEOM is geometry property name and BBOX\_MAX\_Y is geometry suffix._

> Example:
```
    <tr>
    <td>Upper right Y of the Geometry Bounding Box</td>
    <td>
      <div >
        <span >{{PROPERTY_SHPGEOM_BBOX_MAX_Y}}</span>,
      </div>
    </td>
    </tr>
```



#### KML Geometry suffix ####
KML Geometry Property suffix perform geometry rendering in KML geometry format. KML Geometry suffix it's used in KML template to render feature geometry to be recognized as KML geometry. For details and sample look at [Configuring KML Templates](RenderersTemplateKml.md).

> #### KML ####
> For the **KML** suffix, GeoREST renderer renders the item geometry in KML geometry format.

> #### KML\_CENTROID ####
> For the **KML\_CENTROID** suffix, GeoREST renderer renders the item geometry center point as KML point.

> #### KML\_MULTI ####
> For the **KML\_MULTI** suffix, GeoREST renderer renders the item geometry as KML geometry and geometry center point as KML point.

More about [KML Geometry suffix](RenderersTemplateKml#KML_Geometry_suffix.md)

#### GeoRSS ####
> For the **GeoRSS** suffix, GeoREST renderer renders the item geometry in simple GeoRSS geometry format http://www.georss.org/simple.

Example : PROPERTY_is prefix, SHPGEOM is geometry property name and GeoRSS is suffix._

> Example of template to create RSS entry with geometry:
```
    <entry>
    <title>M 3.2, Mona Passage</title>
    <link href="http://example.org/2005/09/09/atom01"/>
    <id>urn:uuid:1225c695-cfb8-4ebb-aaaa-80da344efa6a</id>
    <updated>2005-08-17T07:02:32Z</updated>
    <summary>We just had a big one.</summary>
    {{PROPERTY_SHPGEOM_GeoRSS}}    
    </entry>
    
    In template {{PROPERTY_SHPGEOM_GeoRSS}} would be transformed into something like:
    <georss:point>45.256 -71.92</georss:point>
    or
    <georss:line>45.256 -110.45 46.46 -109.48 43.84 -109.86</georss:line> or ...
    depending on type of geometry.
```

#### GML ####
> For the **GML** suffix, GeoREST renderer renders the item geometry as GML.

#### ATOM\_DATE ####
> For date field, GeoREST will create string as Atom date like "2003-12-13T18:30:02Z".
> Example of Atom template with date written in data filed named "DATEFIELD".
```
    <entry>
    ...
    <updated>{{PROPERTY_DATEFIELD_ATOM_DATE}}</updated>
    ....
```