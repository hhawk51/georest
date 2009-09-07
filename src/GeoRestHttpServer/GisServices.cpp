//
//  Copyright (C) 2009 by  Haris Kurtagic  www.sl-king.com
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
// GisServices.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>

#include "c_GisHttpServer.h"

using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Timestamp;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;



class HTTPGeoRestServer: public Poco::Util::ServerApplication
{
public:
    HTTPGeoRestServer(): _helpRequested(false)
    {
    }

    ~HTTPGeoRestServer()
    {
    }

protected:
    void initialize(Application& self)
    {
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    void uninitialize()
    {
        ServerApplication::uninitialize();
    }

    void defineOptions(OptionSet& options)
    {
        ServerApplication::defineOptions(options);

        options.addOption(
        Option("help", "h", "display argument help information")
            .required(false)
            .repeatable(false)
            .callback(OptionCallback<HTTPGeoRestServer>(
                this, &HTTPGeoRestServer::handleHelp)));
                
                
        options.addOption(
          Option("port", "p", "Port to be used by HTTP Server. Deafult is 99.")
          .required(false) 
          .repeatable(false)
          .argument("port number")
          .binding("port")
          //.callback(OptionCallback<CcApp>(this, &CcApp::handleConfig))
          );                
    }

    void handleHelp(const std::string& name, 
                    const std::string& value)
    {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader(
            "GeoREST Geospatial RESTfull Web Services.");
        helpFormatter.format(std::cout);
        stopOptionsProcessing();
        _helpRequested = true;
        
    }

    int main(const std::vector<std::string>& args)
    { 
      
        if (!_helpRequested)
        {
          int serverport = config().getInt("port",99);
          printf("HTTP Server running on port: %d",serverport);
            c_GisHttpServer * srv = c_GisHttpServer::CreateServer(serverport);
            srv->StartServer();
            waitForTerminationRequest();
            srv->StopServer();
            delete srv;
        }
        return Application::EXIT_OK;
    }

private:
    bool _helpRequested;
};

int main(int argc, char** argv)
{
    HTTPGeoRestServer app;
    return app.run(argc, argv);
}

int _tmain(int argc, _TCHAR* argv[])
{
  HTTPGeoRestServer app;
  return app.run(argc, argv);
	
}

