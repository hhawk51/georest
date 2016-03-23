# Introduction #

GeoREST enables you to add extra renderer's for data. Beside built-in renderer's like: XML, JSON you can add new ones.


# Details #

Custom Renderer is added to config file as another "Representation".
Example:


&lt;Representation renderer="newformat name" library="newformat.dll" pattern=".newformat" mimetype="application/octet-stream" /&gt;



When new representation is accessed using URI like "http://../rest/data/property.newformat" library "newformat.dll" is loaded and new renderer is executed.

Renderer library has to export function 'Content2Stream(void**DataReader,void** OutStream);'
First parameter DataReader is pointer to c\_RestdataReader class and second parameter OutStream is pointer to' std::ostream'.