

# Overview #

Basic installation of GeoREST is simple and consists of four steps:

  1. Download GeoREST and extract to a likely location (we assume c:\GeoRest)
  1. Install the [Microsoft Visual C++ 2008 SP1 Runtime](http://www.microsoft.com/downloads/details.aspx?familyid=A5C84275-3B97-4AB7-A40D-3802B2AF5FC2&displaylang=en)
  1. Copy appropriate binaries for the MapGuide platform you are using GeoREST with into the bin directory (if necessary)
  1. Configure GeoREST to serve pages through a web server

The first two steps should be self-explanatory, the third and fourth are



# Copying binaries #

The standard distribution contains a "bin" folder which is compatible with MapGuide Open Source 2.1.  If you are using MapGuide Enterprise 2011 or !Infrastucture Map Server 2012 and wish to access their feature data from GeoREST, you will need to copy all of the files from the appropriate version folder ("bin.MgEnt2010" or "bin.MgEnt2011") into the main "bin" folder.  Please note that the underlying FDO version for direct file access by GeoREST is not affected by this, these are just the files required to access the MapGuide Web APIs.  You may have to copy the correct FDO dlls as well.  For example, if you use Autodesk IMS 2012, you may have to copy the FDO 3.6 dlls into the GeoRest BIN folder.

![http://i1046.photobucket.com/albums/b461/mapguide_guy/GeoRest%20Documentation/georest_installdir.png](http://i1046.photobucket.com/albums/b461/mapguide_guy/GeoRest%20Documentation/georest_installdir.png)


# Web Server setup #

Setting up GeoREST to work with a web server requires some manual steps.  There are several possible configurations, documented below.  We would suggest starting with the standalone GeoRestHttpServer step first to make sure that everything is working before you move on to integrating with a full webserver like IIS or Apache.

## GeoRestHttpServer ##

This is the easiest way of ensuring that you have all of the dependencies in place.

**This does not require a web server.**  It uses its own PORT 99 and you can use this method without installing IIS or Apache.

  1. Open a command prompt (on Windows Vista and above, this must be an administrative command prompt)
  1. Change to the c:\GeoRest\bin directory and run the GeoRestHttpServer.exe executable.  You should see "HTTP Server running on port: 99" displayed on the console.  Depending on your operating system, you may be prompted to allow the GeoRest HTTP Server to access the network.
  1. Open your web browser and browse to `http://localhost:99/rest/hello`

You should see a web page saying something like:

> `Hello back from GeoREST - GeoSpatial RESTful Web Services `

Now, open the following page in your browser to get an idea of what GeoREST can do:

> http://localhost:99/index.html


## IIS7 (Vista and Windows Server 2008) ##

  1. Set permissions on the C:\GeoRest folder as follows:
    * Network Service: Read
    * IUSR: Read
  1. Set permissions on the C:\GeoRest\bin folder as follows:
    * Network Service: Full Control
  1. Create new application pool to be used by this service.  It does not require any managed components. NOTE: Only perform the following if using the 32bit GeoRest dlls: Unless you are using the 64bit version of GeoRest, you must enable a 32-bit Application Pool. Right click on application pool and select "Advanced Settings" and change "Enable 32-bit Application" property to True.
  1. Create a new virtual directory at the root of your website with an alias of "rest" pointing to C:\GeoRest\bin
  1. Convert this virtual directory into an application, using the application pool you set up earlier.
  1. Under Handler Mappings, create a new Wildcard Module Mapping, that sets the wildcard to `*` and pointing to C:\GeoRest\bin\GeoRest\_ISAPI.dll.  Make sure to allow this extension to run, or set up a new web extension rule for it manually.
  1. browse to http://localhost/rest/hello

You should see a web page saying something like:

> `Hello back from GeoREST - GeoSpatial RESTful Web Services v1.3`

## IIS5.1 and IIS6 (Windows XP, Windows Server 2003) ##

  1. Set permissions on the C:\GeoRest folder as follows:
    * Network Service: Read
    * IUSR: Read
  1. Set permissions on the C:\GeoRest\bin folder as follows:
    * Network Service: Full Control

![http://i1046.photobucket.com/albums/b461/mapguide_guy/GeoRest%20Documentation/georest_iis6_bin.png](http://i1046.photobucket.com/albums/b461/mapguide_guy/GeoRest%20Documentation/georest_iis6_bin.png)
> 3.  Create a new virtual directory at the root of your website with an alias of "rest" pointing to C:\GeoRest\bin. In creation dialog check Execute (such as ISAPI application or CGI) check box.

![http://i1046.photobucket.com/albums/b461/mapguide_guy/GeoRest%20Documentation/georest_iis6.png](http://i1046.photobucket.com/albums/b461/mapguide_guy/GeoRest%20Documentation/georest_iis6.png)
> 4.  Right-click the rest virtual directory and choose Properties.

> 5.  Under Virtual Directory, click Configuration button.

> 6.  Click Add.

> 7.  Browse to C:\GeoRest\bin\GeoRest\_ISAPI.dll.

> 8.  For Extension enter `'.*'`

> 9.  Make sure to uncheck Check that file exists check box.

![http://i1046.photobucket.com/albums/b461/mapguide_guy/GeoRest%20Documentation/georest_iis6_extension.png](http://i1046.photobucket.com/albums/b461/mapguide_guy/GeoRest%20Documentation/georest_iis6_extension.png)

> `10.`  Click OK 3 times.

> `11.`  In your Browser open http://localhost/rest/hello

You should see a web page saying something like:

> `Hello back from GeoREST - GeoSpatial RESTful Web Services v1.3`

## CGI (Apache, or as FastCGI) ##

GeoRestCGI.exe module can be used as CGI or FastCGI extension.

NOTE: There is no Apache CGI built for 64bit versions.

Below are instructions how to edit httpd.conf file to run GeoREST as CGI or FastCGI.

If you are running MapGuide Open Source, the httpd.conf file is found at: C:\Program Files\OSGeo\MapGuide\Web\Apache2\conf\



**CGI conf**


1. In Alias module add
```
<IfModule alias_module>
.
.
ScriptAliasMatch ^/rest/(.*) "c:/georest/bin/GeoRestCGI.exe"
</IfModule>

NOTE: If you are running MapGuide Enterprise 2011, you will not find the <IfModule alias_module> 
so you will have to add this to your httpd.conf file (around line 205)

```

2. Add GeoREST folder to the bottom of the httpd.conf
```
<Directory "c:/georest/bin/">
    AllowOverride None
    Options None
    Order allow,deny
    Allow from all
</Directory>
```
3. Pass Environment variable for CS Mentor (only required for MapGuide Open Source - not Enterprise)
```
PassEnv MENTOR_DICTIONARY_PATH

Example: C:\Program Files\OSGeo\MapGuide\CS-Map\Dictionaries\

Note: MENTOR_DICTIONARY_PATH needs to set in order for CS library to initialize properly.
```

**FastCGI conf**

1. Load Module for Fast CGI (see http://www.fastcgi.com/)
```
LoadModule fastcgi_module modules/mod_fastcgi.so 
```
2. In Alias module add
```
<IfModule alias_module>
.
.
ScriptAliasMatch ^/rest/(.*) "c:/georest/bin/GeoRestCGI.exe"
</IfModule>
```

3. Add GeoREST folder to the bottom of the httpd.conf
```
<Directory "c:/georest/bin/">
    AllowOverride None
    Options None
    Order allow,deny
    Allow from all
</Directory>
```
4. Configure FastCgi Server
```
FastCgiServer "c:/georest/bin/GeoRestCGI.exe" -port 9001 -initial-env MENTOR_DICTIONARY_PATH

Example: C:\Program Files\OSGeo\MapGuide\CS-Map\Dictionaries\

Note: MENTOR_DICTIONARY_PATH needs to set in order for CS library to initialize properly. 

(only required for MapGuide Open Source - not Enterprise)
```