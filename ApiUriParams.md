GeoREST allow use of URI parameters to define result.

## Parameters ##

### Standard Parameters ###

These parameters can be used with any feature-type representation, though some FDO providers may not allow the user of operations such as ordering.

| **Name** | **Value** | Description |
|:---------|:----------|:------------|
| [filter](#Parameter_Name:_filter.md) | textual notation | A filter is a construct that an application specifies to an FDO provider to identify a subset of objects of an FDO data store |
| [filter\_n\_booloper\_operation\_property](#Parameter_Name:_filter_n_booloper_operation_property.md) | filter\_n\_booloper\_operation\_propert | combination of filtering commands |
| [bbox](#Parameter_Name:_bbox.md) | <x1,y1,x2,y2> |spatial envelope to be used for spatial query |
| [select](#Parameter_Name:_select.md) | field1,field2... | fields to include in response |
| [orderfields](#Parameter_Name:_orderfields.md) | field1,field2... | comma separated list of fields names used to order query result |
| [orderdir](#Parameter_Name:_orderdir.md) | ASC/DESC  | direction of order |
| [count](#Parameter_Name:_count.md) | integer   | number of items in the query result |
| [start](#Parameter_Name:_start.md) | integer   | starting  record number in the query result |
| [\_callback](#Parameter_Name:___callback.md) | function name | Sets function name for JSONP response |
| [\_method](#Parameter_Name:___method.md) | method name  | Overrides HTTP method used in request |

### Image-specific Parameters ###

These parameters are only used with IMAGE-type representations (e.g. PNG8), currently only possible with MapGuide data sources.

| [SETVIEWSCALE](#Parameter_Name:_SETVIEWSCALE.md) | double | Set scale for map image |
|:-------------------------------------------------|:-------|:------------------------|
| [SETVIEWCENTERX](#Parameter_Name:_SETVIEWCENTERX.md) | double | Set X coordinate for center of map image |
| [SETVIEWCENTERY](#Parameter_Name:_SETVIEWCENTERY.md) | double | Set Y coordinate for center of map image |
| [FORMAT](#Parameter_Name:_FORMAT.md)             | JPG/PNG/PNG8 | Image format for rendered image |
| [SETDISPLAYDPI](#Parameter_Name:_SETDISPLAYDPI.md) | integer | Set DPI for map image   |
| [SETDISPLAYHEIGHT](#Parameter_Name:_SETDISPLAYHEIGHT.md) | integer | Set height of image in pixels |
| [SETDISPLAYWIDTH](#Parameter_Name:_SETDISPLAYWIDTH.md) | integer | Set width of image in pixels |


### Parameter Name: filter ###

Defines FDO filter.

_"FDO uses filters through its commands (including provider-specific commands) to select certain features and exclude others.
A filter is a construct that an application specifies to an FDO provider to identify a subset of objects of an FDO data store. For example, a filter may be used to identify all Road type features that have 2 lanes and that are within 200 metres of a particular location. Many FDO commands use filter parameters to specify the objects to which the command applies. For example, a select command takes a filter to identify the objects that the application wants to retrieve or a delete command takes a filter to identify the objects that the application wants to delete from the data store."_
<br>(From FDO Developer's Guide)<br>
<br>
More in FDO docs: <a href='http://fdo.osgeo.org/documentation.html'>http://fdo.osgeo.org/documentation.html</a>

<h3>Parameter Name: filter_n_booloper_operation_property</h3>

Parameter name is combination of filtering commands. Useful for creating filter parameters trough HTML forms.<br>
<br>filter<i>is fixed literal<br></i><br>n.. is number which defines level of nested in filter<br>
<br>booloper...is either AND or OR bool operator<br>
<br>operation...is EQUAL, LIKE, LIKERIGHT, LIKELEFT, LT (LESS), GT (GREATER). LTE (LESSTHEN), GTE (GREATERTHEN)<br>
<br>property...is name of property to be filter by ( property LIKE 'upd%' ) (% allows you to match any string of any length)<br>
<br>
<code>Example 1:georest.org/parcel/.xml?filter_1_AND_EQUAL_parcelnum=123</code>
<br>It will query parcels whose property parcelnum equals 123<br>
<br>
<code>Example 2:georest.org/parcel/.xml?filter_1_AND_EQUAL_parcelnum=123&amp;filter_2_AND_EQUAL_name=King</code>
<br>It will query parcels whose property parcelnum equals 123 AND property name equals "King"<br>
<br>
<code>Example 3:georest.org/parcel/.xml?filter_1_AND_EQUAL_parcelnum=123&amp;filter_2_OR_EQUAL_name=King</code>
<br>It will query parcels whose property parcelnum equals 123 OR property name equals "King"<br>
<br>
<code>Example 4:georest.org/parcel/.xml?filter_1_AND_EQUAL_parcelnum=123&amp;filter_2_OR_EQUAL_name=King&amp;filter_3_AND_GT_size=50</code>
<br>It will query parcels whose property parcelnum equals 123 OR property name equals "King" AND property size is greater than 50<br>
<br>
<code>Example 5:georest.org/parcel/.xml?filter_1_AND_GT_size=10&amp;filter_2_AND_LT_size=70</code>
<br>It will query parcels whose property size is greater than 10 AND less than 70<br>
<br>
<code>Example 6:georest.org/parcel/.xml?filter_1_AND_GTE_size=20&amp;filter_2_AND_LTE_size=60</code>
<br>It will query parcels whose property size is greater and equal to 20 AND less or equal to 60<br>
<br>
<code>Example 7:georest.org/parcel/.xml?filter_1_AND_LIKERIGHT_name=King</code>
<br>It will query parcels whose property name starts with "King"<br>
<br>
<code>Example 8:georest.org/parcel/.xml?filter_1_AND_LIKERIGHT_name=King&amp;filter_2_OR_LIKERIGHT_name=Bob&amp;filter_3_OR_LIKERIGHT_name=John</code>
<br>It will query parcels whose property name starts with "King" OR "Bob" OR "John"<br>
<br>
<br>
<br>
<h3>Parameter Name: bbox</h3>

Defines spatial envelope to be used for query.<br>
Spatial envelope can be defined as four comma separated numbers or as HTTP request to another service.<br>
<br>
<code>Example 1:georest.org/parcel/.xml?bbox=10,10,20,20</code>
<br> Example 1 will return parcels which inside bounding box with lower left x coordinate equals 10,lower left y coordinate equals 10, upper right x equals 20 and upper right y equals 20<br>
<br>
<br><code>Example 2:georest.org/parcel/.xml?bbox=http://georest.org/buildings/2.xml</code>
<br>Example 2 will return parcels which building identified with URI<br>
<br>
<h3>Parameter Name: orderfields</h3>

It is comma separated list of fields values used to order query result.<br>
<br>
<code>Example: georest.org/parcels/.xml?orderfields=parcelnum&amp;orderdir=desc</code>
<br>The items in the query result will be ordered by parcelnum field in descending order.<br>
<br>
<h3>Parameter Name: orderdir</h3>

Defines direction of order. To define ascending order parameter value is "asc" and for descending order value is "desc".<br>
<br>
<code>Example: georest.org/parcels/.xml?orderfields=parcelnum&amp;orderdir=desc</code>
<br>The items in the query result will be ordered by parcelnum field in descending order.<br>
<br>
<b>Note! Currently ordering is not supported for SDF, SHP and SQLite providers.</b>

<h3>Parameter Name: count</h3>

Defines number of items in the query result. With this parameter a limit of items returned in query result can be set.<br>
<br>
<code>Example: georest.org/parcels/.xml?count=10</code>
<br>In the example above the query will return 10 items.<br>
<br>
<h3>Parameter Name: start</h3>

Defines starting record number in the query result.<br>
<br>
<code>Example: georest.org/parcels/.xml?start=14</code>
<br>The query result will contain items from item 14 onward.<br>
<br>
<h3>Parameter Name: <code>_</code>callback</h3>

Sets function name for JSONP response<br>
<br>
<h3>Parameter Name: <code>_</code>method</h3>

Overrides HTTP method used in request.<br>
<br>
<br>
<h3>Parameter Name: SETVIEWSCALE</h3>

Defines the view scale for the map image. Value of the parameter should be greater than 0.<br>
<br>
<code>georest.org/parcels/.png?SETVIEWSCALE=1000</code>
<br>Example to get an image with view scale to 1000:<br>
<br>
<br>
<h3>Parameter Name: SETVIEWCENTERX</h3>

Defines the X coordinate for the center of the map image.<br>
<br>
<code>georest.org/parcels/.png?SETVIEWCENTERX=-87.7</code>
<br>Example to set image view center X coordinate to -87.7:<br>
<br>
<br>
<h3>Parameter Name: SETVIEWCENTERY</h3>

Defines the Y coordinate for the center of the map image.<br>
<br>
<code>georest.org/parcels/.png?SETVIEWCENTERY=43.8</code>
<br>Example to set image view center Y cooridante to 43.8:<br>
<br>
<br>
<h3>Parameter Name: FORMAT</h3>

Defines the map image format. Possible formats are PNG8 and PNG.<br>
With parameter value PNG an 32 bit PNG image is returned.<br>
<br>With parameter value PNG8 an 8 bit PNG image is returned.<br>
PNG it preserves details much better than PNG8 but PNG8 is 40% smaller in size.<br>
<br>
<code>Example: georest.org/parcels/.png?FORMAT=PNG8</code>
<br>Example URI will return 8 bit PNG image<br>
<br>
<code>Example: georest.org/parcels/.png?FORMAT=PNG</code>
<br>Example URI will return 32 bit PNG image<br>
<br>
<h3>Parameter Name: SETDISPLAYDPI</h3>

Defines the DPI(Dots per inch) of the map image.<br>
<br>
<code>georest.org/parcels/.png?DPI=96</code>
<br>Example of setting image DPI to 96.<br>
<br>
<h3>Parameter Name: SETDISPLAYHEIGHT</h3>

Defines the height of the map image. Height is in pixels<br>
<br>
<code>georest.org/parcels/.png?SETDISPLAYHEIGHT=400</code>
<br>Example of setting image height to 400 pixels.<br>
<br>
<h3>Parameter Name: SETDISPLAYWIDTH</h3>

Defines the width of the map image.<br>
<br>
Example of setting image height to 400.<br>
<br>
<code>georest.org/parcels/.png?SETDISPLAYWIDTH=400</code>
<br>Example of setting image width to 400 pixels.<br>
<br>
<b>All image parameters used together</b>

<code>georest.org/parcels/.png?FORMAT=PNG&amp;SETDISPLAYDPI=96&amp;SETDISPLAYHEIGHT=500&amp;SETDISPLAYWIDTH=500&amp;SETVIEWCENTERX=-87.73&amp;SETVIEWCENTERY=43.72&amp;SETVIEWSCALE=2860</code>
<br>In the example above it is shown how to get an image with specified view scale at specific view center coordinate.