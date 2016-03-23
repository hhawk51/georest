## Output formats ##

The GeoREST API supports responses in several formats. Response format is specified by the pattern value and mimetype defined in the **Representation element** of the **Resource element**. More about enabling response format look at [Representation](Configuration#Representation.md) in the [Configuration](Configuration.md) page.

  * **HTML** response
> > The response is an HTML page rendered from template file.
> > How to configure HTML templates look at [GeoREST HTML Templates Configuration Guide](RenderersTemplateHtml.md)


> Example: HTML view of properties.
```
    georest.org/georest/rest/data/property/.html
```


  * **KML** response
> > The response is an KML page rendered from template file.
> > How to configure KML templates look at [GeoREST KML Templates Configuration Guide](RenderersTemplateKml.md)


> Example: KML view of properties.
```
    georest.org/georest/rest/data/property/.kml
```

  * **XML** response
> > The response is an XML document.
> > More about [XML Renderer Information](RenderersXml.md).


> Example: Url for XML document representation of properties (property resource).
```
    georest.org/georest/rest/data/property/.xml
```

  * **JSON** response
> > The response is an JSON object.
> > More about [JSON Renderer Information](RenderersJson.md).


> Example: Url for XML object representation of properties (property resource).
```
    georest.org/georest/rest/data/property/.json
```

  * **IMAGE** response
> > The response is an map image in PNG format.


> Example:
```
    georest.org/georest/rest/data/Image/.png
```


  * Other response examples
> > Also other response types are possible using the **Templates** renderer. HTML and KML response above are also configured as **Templates** renderer.
> > Another possible response type would be **Atom publish**.


> Example: properties publish via Atom publish.
```
    georest.org/georest/rest/data/property/.atom
```