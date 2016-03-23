# Introduction #

The PNG renderer is only available when the source of your resource is a MapGuide connection.  In these cases, you can configure a PNG representation, and use special URL parameters to affect the display of the image.

# Details #

Here is an example defining a PNG-based image within a MapGuide-based resource:

```
<Representation renderer="PNG8" pattern=".png">
  <MapDefinition>Library://Samples/Property.MapDefinition</MapDefinition>
  <SelectionLayer>RestProperty</SelectionLayer>
  <DefaultZoomFactor>1.3</DefaultZoomFactor>
</Representation>
```

Valid values for renderer are PNG and PNG8.

The MapDefinition entity is set to a MapGuide map that you want to use to represent features for the current resource.

The SelectionLayer entity is set to a layer that shares the same primary key as the Resource. This does not necessarily need to be the data source as the resource (e.g. if you want to display a UTM-based map for an LL84-based resource) but in general this is the easiest approach.

The DefaultZoomFactor is used to specify the amount of spacing around the selected object you want to provide, and is based on the dimensions of the selected object.