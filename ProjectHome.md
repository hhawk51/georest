GeoREST is a web-centric framework for distributing geospatial data.  It allows RESTful feature-based access to spatial data sources, including full editing capabilities, through a MapGuide server or directly via FDO.

Examples of data sources:

  * SHP
  * SDF
  * SQLite (FDO)
  * SpatiaLite
  * PostGIS
  * Oracle Spatial
  * Microsoft SQL Server Spatial
  * MySQL
  * Any other FDO vector data source

Example output formats:

  * GeoJSON
  * XML
  * OData
  * PNG (MapGuide layers only)
  * HTML (Template)
  * KML (Template)
  * GeoRSS (Template)
  * CSV (Template)
  * Any other templatable text-based format

Getting Started:


  * [Download](http://code.google.com/p/georest/downloads/list)
  * [Installation notes](Installation.md)
  * [Technical documentation](Documentation.md)

External resources discussing GeoREST:

  * Dzenana Muracevic Dissertation "Metoda integracije geoprostornih podataka sa poslovnim inteligentnim sistemima bazirana na servis orijentiranoj arhitekturi“
  * Jason's initial announcement of the City of Nanaimo's [public GeoREST implementation](http://www.jasonbirch.com/nodes/2009/01/31/269/mapguide-rest-extension-feedback-wanted/)
  * Jason's [blog post](http://www.jasonbirch.com/nodes/2009/08/03/312/do-you-see-spiders-making-government-data-truly-open/) and [slide deck](http://www.slideshare.net/JasonBirch/moving-beyond-the-desk) discussing the benefits and genuine need for resource-based geodata on the web.
  * Geoff Zeiss' presentation from FOSS4G2009 [An Open Architecture for RESTful Geospatial Web Services](http://2009.foss4g.org/presentations/#presentation_5)

GeoREST would not be possible without many underlying open source libraries, including:

  * [MapGuide](http://mapguide.osgeo.org/) : an OSGeo web mapping project
  * [FDO](http://fdo.osgeo.org/) : an OSGeo spatial data access library
  * [POCO](http://pocoproject.org/) : next generation c++ class libraries for network-centric applications
  * [CTemplate](http://code.google.com/p/google-ctemplate/) : a simple but powerful template language for C++
  * [libkml](http://code.google.com/p/libkml/) : an implementation of the OGC KML 2.2 standard

Other external resources on RESTful GIS:
  * [Geo-Web-REST mailing list](http://groups.google.com/group/geo-web-rest), lots of smart folks on here...
  * [Sean Gillies](http://sgillies.net/blog) is a strong advocate and watchdog for RESTful geo
  * [FeatureServer](http://featureserver.org/), a Python-based RESTful geodata engine
  * GeoServer's [REST configuration extension](http://docs.geoserver.org/2.0.x/en/user/extensions/rest/index.html), a method for configuring GeoServer RESTfully
  * ESRI's [ArcGIS Server REST API](http://help.arcgis.com/EN/arcgisserver/10.0/apis/rest/index.html) docs
  * [PostGIS RESTful Web Service Framework](http://code.google.com/p/postgis-restful-web-service-framework/)