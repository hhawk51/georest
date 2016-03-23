# Introduction #

GeoREST enables you to publish your data using [OData Protocol](http://www.odata.org), which is in turn based on the [Atom Publishing Protocol](http://www.ietf.org/rfc/rfc4287.txt).

# Details #

This page is divided into two sections.

[OData Configuration](#OData_Configuration.md) describes how to set up the restcfg.xml configuration file to enable OData publishing.

[OData Implementation](#OData_Implementation.md) describes how OData is implemented inside GeoREST.

# OData Configuration #

To enable OData, add a representation of type "OData" to the resource you want to publish as OData.

> `<Representation renderer="OData" />`

Example:

```
<Resource uripart="Parcelfdo" >
 <Source type="FDO">
  <Provider>OSGeo.SDF</Provider>
  <ConnectionString>File=../TestData/SheboyganSDF/Parcels.sdf;ReadOnly=FALSE;
  </ConnectionString>
  <FeatureClass>SHP_Schema:Parcels</FeatureClass>
 </Source>
 <Representation renderer="OData" />
</Resource>
```


OData Representation has additional elements, `<FeedOverride>` and `<EntryOverride>`, which can be used to populate atom elements in resulting document. `<FeedOverride>` is used to set feed-specific atom elements, while `<EntryOverride>` can be used to set properties which recur in each atom entry.  You can use properties from your data source when setting entry values.  For instance, you can set `<title>` to include the value of Parcel\_Number in your data.

Example:

```
<Representation renderer="OData">
 <FeedOverride>
  <Title>This is my fixed title for Parcels collection</Title>
  <Author>
   <Name>This is 'atom:name' value in Parcels feed</Name>
   <Uri>This is 'atom:uri' value in Parcels feed</Uri>
   <Email>This is 'atom:email' value in Parcels feed</Email>
  </Author>
 </FeedOverride>
 <EntryOverride>
  <Title type="Property">Parcel_Number</Title>
  <Author>
   <Name type="Property">Parcel_Owner</Name>
   <Uri>http://georest.org/parcel_owners/</Uri>
   <Email>info@georest.org</Email>
  </Author>
 </EntryOverride>
</Representation>
```

# OData Implementation #

**Service Document**

http://example.com:99/rest/OData.svc/

**Service Metadata Document**

http://example.com:99/rest/OData.svc/$metadata

**Parcels Collection**

http://example.com:99/rest/OData.svc/Parcels

**Parcel Entry**

http://example.com:99/rest/OData.svc/Parcels(1)