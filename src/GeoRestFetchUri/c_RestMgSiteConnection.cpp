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

#include "stdafx.h"
#include "c_RestMgSiteConnection.h"
#include "Poco\Path.h"
#include "Poco\UnicodeConverter.h"

#ifdef _WIN32
#undef CreateService
#endif

static ACE_thread_key_t g_threadLocalSiteConnection = 0;

//////////////////////////////////////////////////////////////////
/// <summary>
/// Creates a site connection.  The local site defined
/// for the web tier or server is used.
/// EXCEPTION:
/// MgConfigurationException, if config file is missing
/// </summary>

c_RestMgSiteConnection::c_RestMgSiteConnection()
{
    Initialize();
}


c_RestMgSiteConnection::c_RestMgSiteConnection(CREFSTRING url)
{
    m_url = url;
    Initialize();
}

/// <summary>
/// Initializes and determine the machine type
/// </summary>

void c_RestMgSiteConnection::Initialize()
{
    MG_TRY()

    // get instanced MgConfiguration for local use
    //m_config = MgConfiguration::GetInstance();

    m_isServer     = false;
    m_isSiteServer = false;
    m_isWebTier    = false;
    m_http         = IsHttpConnection();

    if (!m_http)
    {
        m_isServer     = IsServer();
        m_isSiteServer = IsSiteServer();
        m_isWebTier    = IsWebTier();
    }

    MG_CATCH_AND_THROW(L"c_RestMgSiteConnection::Initialize")
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>

c_RestMgSiteConnection::~c_RestMgSiteConnection()
{
}

extern wchar_t g_AppFileName[];
bool m_IsConfigLoaded=false;

c_RestMgSiteConnection* c_RestMgSiteConnection::Open(const wstring& UserName,const wstring& Password,const wstring& ServerIP,int ServerPort)
{
  Ptr<MgUserInformation> userinfo = new MgUserInformation(UserName,Password);
  c_RestMgSiteConnection* siteconn = new c_RestMgSiteConnection();
  
  
  siteconn->m_connProp = new MgConnectionProperties(userinfo, ServerIP, ServerPort);
  
  MgConfiguration* config = MgConfiguration::GetInstance();
  
  if( !m_IsConfigLoaded )
  {
    std::string buff_utf8;
    Poco::UnicodeConverter::toUTF8(g_AppFileName,buff_utf8);

    Poco::Path appfolder;    
    appfolder = buff_utf8;
    appfolder.setFileName("");

    appfolder.setFileName("webconfig.ini");


    wstring buff_utf16;
    Poco::UnicodeConverter::toUTF16(appfolder.toString(),buff_utf16);
    config->LoadConfiguration(buff_utf16);

    m_IsConfigLoaded = true;
  }
  //siteconn->Open(userinfo);
  Ptr<MgSite> site = siteconn->GetSite(); // GetSite() performs authentication
  
  return siteconn;
}
//////////////////////////////////////////////////////////////////
/// <summary>
/// Establishes a connection to a site.
/// </summary>
/*
void c_RestMgSiteConnection::Open(MgUserInformation* userInformation)
{
  MgConfiguration* config = MgConfiguration::GetInstance();
  if( !m_IsConfigLoaded )
  {
    std::string buff_utf8;
    Poco::UnicodeConverter::toUTF8(g_AppFileName,buff_utf8);

    Poco::Path appfolder;    
    appfolder = buff_utf8;
    appfolder.setFileName("");
    
    appfolder.setFileName("webconfig.ini");
    
    
    wstring buff_utf16;
    Poco::UnicodeConverter::toUTF16(appfolder.toString(),buff_utf16);
    config->LoadConfiguration(buff_utf16);
    
    m_IsConfigLoaded = true;
  }
  
  
    // We can not have null userInformation
    if (NULL == userInformation)
    {
        throw new MgAuthenticationFailedException(L"c_RestMgSiteConnection.Open", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_connProp = NULL;

    // If none of them is valid, means we have a problem.
    if (!m_http && !m_isWebTier && !m_isServer && !m_isSiteServer)
    {
        MgStringCollection arguments;
       
        throw new MgConfigurationException(L"c_RestMgSiteConnection.Open",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // Get the service IP address from the site server
    if (m_http)
    {
        // Connection property for HTTP request
         m_connProp = new MgConnectionProperties(userInformation, m_url);
    }
    // Site connection properties for local site
    // We need to first determine whether it is site server.
    else if (m_isSiteServer)
    {
        m_connProp = new MgConnectionProperties(userInformation, L"", 0);
    }
    // Site connection properties for web-tier or server-2-server interaction
    else if (m_isWebTier)
    {
        MgSiteManager* siteManager = MgSiteManager::GetInstance();
        m_connProp = siteManager->GetConnectionProperties(userInformation, MgSiteInfo::Client, true);
    }
    else if(m_isServer)
    {
        MgSiteManager* siteManager = MgSiteManager::GetInstance();
        m_connProp = siteManager->GetConnectionProperties(userInformation, MgSiteInfo::Client, false);
    }

    if (m_connProp == NULL)
    {
        throw new MgLogicException(L"c_RestMgSiteConnection.Open",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!m_connProp->IsLocal())
    {
        Authenticate(userInformation);
    }
}
*/

//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the specified service from the site.  A connection
/// to the machine hosting the service will be created if required.
/// Configuration information determines whether the service is hosted locally
/// or not.
/// </summary>
/// <param name="serviceType">
/// Type of service to create, a value from MgServiceType
/// <returns>
/// Returns the created service
/// </returns>
/// EXCEPTIONS:
/// MgServiceNotSupported
/// MgConnectionNotOpen

MgService* c_RestMgSiteConnection::CreateService(INT32 serviceType)
{
    Ptr<MgConnectionProperties> connProp;


    if (IsServiceLocal(serviceType))
    {
        // Service is hosted by this site itself
        // Therefore, no connection needed to any other site.
        // In otherwords, empty connection properties.
        // This is based on assumption that a machine hosting
        // a particular service can be directly used without going
        // to site server.
        Ptr<MgUserInformation> userInformation = m_connProp->GetUserInfo();

        connProp = new MgConnectionProperties(userInformation.p, L"", 0);
    }
    else
    {
        // Get the service IP address from the site server
        connProp = GetConnectionProperties(serviceType);
    }

    return CreateService(serviceType, connProp);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Creates service on specific machine.  Works for local and proxy
/// services based on passed in connection properties
/// </summary>
MgService* c_RestMgSiteConnection::CreateService(INT32 serviceType,
    MgConnectionProperties* connProp)
{
    assert(NULL != connProp);
    ServerConnectionType connType;

    if (connProp->IsLocal())
    {
        connType = sctLocalInProc;
    }
    else if (connProp->GetUrl().empty())
    {
        connType = sctRemoteServerToServer;
    }
    else
    {
        connType = sctServerToHttp;
    }

    Ptr<MgService> service = CreateService(connType, serviceType, connProp);

    if (service == NULL)
    {
        throw new MgServiceNotSupportedException(L"CreateService",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return service.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Create the appropriate service based on the connection type
/// and desired service
/// </summary>
/// <returns>
/// MgService instance or NULL
/// </returns>
MgService* c_RestMgSiteConnection::CreateService(ServerConnectionType connType,
    INT32 serviceType, MgConnectionProperties* connProp)
{
    MgServiceRegistry* registry = MgServiceRegistry::GetInstance();

    MgService* service = registry->CreateService(serviceType, connType);

    if (NULL != service)
    {
        service->SetConnectionProperties(connProp);
    }

    return service;
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Get the class Id
/// </summary>
/// <returns>
/// The integer value
/// </returns>
INT32 c_RestMgSiteConnection::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Self Destructor
/// </summary>
/// <returns>
/// Nothing
/// </returns>
void c_RestMgSiteConnection::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Looks in config.ini to find out whether it is a site server.
/// This is currently determined based on availablity of resource
/// service.
/// </summary>
/// <returns>
/// true -  resource service is hosted
/// false - resource service is not hosted
/// </returns>
bool c_RestMgSiteConnection::IsSiteServer()
{
    // NOTE: We are assuming that no more than one machine can host site service.
    return IsServiceLocal(MgServiceType::SiteService);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Determines whether it is a HTTP connection or TCP/IP connection
/// </summary>
/// <returns>
/// true - HTTP connection
/// false - TCP/IP connection
/// </returns>
bool c_RestMgSiteConnection::IsHttpConnection()
{
    return (m_url.empty() ? false : true);
}

/// <summary>
/// Determines whether it hosts any service. If so, it is the server.
/// </summary>

bool c_RestMgSiteConnection::IsServer()
{
    bool isServer = false;

    // If any one of the service is available means it is a server
    if ( IsServiceLocal(MgServiceType::DrawingService)   ||
         IsServiceLocal(MgServiceType::FeatureService)   ||
         IsServiceLocal(MgServiceType::MappingService)   ||
         IsServiceLocal(MgServiceType::RenderingService) ||
         IsServiceLocal(MgServiceType::ResourceService)  ||
         IsServiceLocal(MgServiceType::TileService)      ||
         IsServiceLocal(MgServiceType::KmlService) )
    {
        isServer = true;
    }

    return isServer;
}


/// <summary>
/// Determines whether it is instantiated on webtier
/// </summary>

bool c_RestMgSiteConnection::IsWebTier()
{
    bool isWebTier = false;

    //if ( m_config->IsFileLoaded() && !IsServer() )
    {
        isWebTier = true;
    }

    return isWebTier;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Determines whether server is hosting a particular service.
/// This is based on the config.ini properties.
/// </summary>
/// <returns>
/// true - hosts the service
/// false - does not host the service
/// </returns>
bool c_RestMgSiteConnection::IsServiceLocal(INT32 serviceType)
{
    bool isHosting = false;
/*
    try
    {
        switch(serviceType)
        {
            case MgServiceType::DrawingService:
                m_config->GetBoolValue(MgConfigProperties::HostPropertiesSection,
                                    MgConfigProperties::HostPropertyDrawingService,
                                    isHosting,
                                    MgConfigProperties::DefaultHostPropertyDrawingService);
                break;
            case MgServiceType::FeatureService:
                m_config->GetBoolValue(MgConfigProperties::HostPropertiesSection,
                                    MgConfigProperties::HostPropertyFeatureService,
                                    isHosting,
                                    MgConfigProperties::DefaultHostPropertyFeatureService);
                break;
            case MgServiceType::MappingService:
                m_config->GetBoolValue(MgConfigProperties::HostPropertiesSection,
                                    MgConfigProperties::HostPropertyMappingService,
                                    isHosting,
                                    MgConfigProperties::DefaultHostPropertyMappingService);
                break;
            case MgServiceType::RenderingService:
                m_config->GetBoolValue(MgConfigProperties::HostPropertiesSection,
                                    MgConfigProperties::HostPropertyRenderingService,
                                    isHosting,
                                    MgConfigProperties::DefaultHostPropertyRenderingService);
                break;
            case MgServiceType::ResourceService:
                // TODO: We should find a better way to determine the location of site server.
                // If an entry in serverconfig.ini is missing, it would end up in infinite
                // TCP/IP calls or until it times out
                m_config->GetBoolValue(MgConfigProperties::HostPropertiesSection,
                                    MgConfigProperties::HostPropertyResourceService,
                                    isHosting,
                                    MgConfigProperties::DefaultHostPropertyResourceService);
                break;
            case MgServiceType::SiteService:
                // TODO: We should find a better way to determine the location of site server.
                // If an entry in serverconfig.ini is missing, it would end up in infinite
                // TCP/IP calls or until it times out
                m_config->GetBoolValue(MgConfigProperties::HostPropertiesSection,
                                    MgConfigProperties::HostPropertySiteService,
                                    isHosting,
                                    MgConfigProperties::DefaultHostPropertySiteService);
                break;
            case MgServiceType::TileService:
                m_config->GetBoolValue(MgConfigProperties::HostPropertiesSection,
                                    MgConfigProperties::HostPropertyTileService,
                                    isHosting,
                                    MgConfigProperties::DefaultHostPropertyTileService);
                break;
            case MgServiceType::KmlService:
                 m_config->GetBoolValue(MgConfigProperties::HostPropertiesSection,
                                    MgConfigProperties::HostPropertyKmlService,
                                    isHosting,
                                    MgConfigProperties::DefaultHostPropertyKmlService);
                 break;
            case MgServiceType::ServerAdminService:
                if (IsServer())
                {
                    isHosting = true;
                }
                break;
            default:
                break;
        }
    }
    catch (MgException* me)
    {
        // Object not found or invalid property value
        // means service does not exist
        SAFE_RELEASE(me);
    }
*/
    return isHosting;
}

void c_RestMgSiteConnection::AuthenticateWithHttpServer(MgUserInformation* userInformation)
{
    assert(NULL != userInformation);

    // Send HTTP request to the URL specified for authentication.

    throw new MgNotImplementedException(L"c_RestMgSiteConnection.AuthenticateWithHttpServer",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

void c_RestMgSiteConnection::AuthenticateWithSiteServer(MgUserInformation* userInformation)
{
    assert(NULL != userInformation);
    Ptr<MgSite> site = GetSite(); // GetSite() performs authentication
}

/// <summary>
/// Determines connection properties for http/site server/webtier,server-2-server interaction
/// </summary>

MgConnectionProperties* c_RestMgSiteConnection::GetConnectionProperties(INT32 serviceType)
{
    Ptr<MgConnectionProperties> connProp;

    // Send HTTP request to the URL specified for authentication
    if (m_http)
    {
        Ptr<MgUserInformation> userInformation = m_connProp->GetUserInfo();

        connProp = new MgConnectionProperties(userInformation.p, m_url);
    }
    else if (m_isWebTier)
    {
        connProp = GetConnectionPropertiesFromSiteServer(serviceType);
    }
    else
    {
        // THIS SHOULD NEVER BE CALLED!!
        // Servers should be going through the service manager and not c_RestMgSiteConnection
        // to get their services.
        throw new MgNotImplementedException(L"c_RestMgSiteConnection.GetConnectionProperties", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return connProp.Detach();
}

/// <summary>
/// Determines connection properties from site server for the services
/// </summary>

MgConnectionProperties* c_RestMgSiteConnection::GetConnectionPropertiesFromSiteServer(INT32 serviceType)
{
    CHECKARGUMENTNULL((MgConnectionProperties*)m_connProp, L"c_RestMgSiteConnection.GetConnectionPropertiesFromSiteServer");

    Ptr<MgConnectionProperties> connProp;

    if (m_connProp->IsLocal())
    {
        throw new MgInvalidOperationException(
            L"c_RestMgSiteConnection.GetConnectionPropertiesFromSiteServer",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else
    {
        // We have deprecated support servers.  All load balanced servers
        // should support all services.  With this change, we do not need to
        // contact the site server to determine where to vector service requests.
        //
        // A site connection has already been established if we are looking for services.
        // Just use the target and port from the existing connection.  This will
        // bind all service requests to the machine initially connected to in
        // the c_RestMgSiteConnection.Open call.

        connProp = m_connProp;
    }

    return connProp.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Authenticates a user.
/// </summary>
///
/// <param name="userInformation">
/// User information to authenticate against.
/// </param>
///
/// <returns>
/// Nothing.
/// </returns>
///
/// EXCEPTIONS:
/// MgNullArgumentException
/// MgAuthenticationFailedException
/// MgUnauthorizedAccessException

void c_RestMgSiteConnection::Authenticate(MgUserInformation* userInformation)
{
    if (IsHttpConnection())
    {
        AuthenticateWithHttpServer(userInformation);
    }
    else if (!IsSiteServer())
    {
        AuthenticateWithSiteServer(userInformation);
    }
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns an instance of the MgSite object.  This object
/// is responsible for user/group management and session management.
/// MgSite exposes functionality to enumerate users and groups and to
/// manage user sessions.  The connection must already be open to
/// obtain access to the site.
/// </summary>
MgSite* c_RestMgSiteConnection::GetSite()
{
    Ptr<MgSite> site;

    //MgSiteManager* siteManager = MgSiteManager::GetInstance();
    //Ptr<MgSiteInfo> siteInfo = siteManager->GetSiteInfo(m_connProp->GetTarget(), m_connProp->GetPort());
    
    Ptr<MgSiteInfo> siteInfo = new MgSiteInfo(m_connProp->GetTarget(),m_connProp->GetPort(), m_connProp->GetPort(), m_connProp->GetPort());
    Ptr<MgUserInformation> userInfo = m_connProp->GetUserInfo();

    if (NULL != userInfo.p)
    {
        site = new MgSite();
        if (NULL != siteInfo.p)
        {
            site->Open(userInfo, siteInfo);
        }
        else
        {
            site->Open(userInfo);
        }
    }
    else
    {
        throw new MgConnectionNotOpenException(L"c_RestMgSiteConnection.GetSite", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return site.Detach();
}

///////////////////////////////
///<summary>
///Sets the site connection for the current thread.  This
///function uses thread local storage.
///</summary>
void c_RestMgSiteConnection::SetCurrentConnection(c_RestMgSiteConnection* siteConnection)
{
    if (0 == g_threadLocalSiteConnection)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance ()));
        if (0 == g_threadLocalSiteConnection)
        {
            if (ACE_OS::thr_keycreate(&g_threadLocalSiteConnection, NULL) < 0)
            {
                g_threadLocalSiteConnection = 0;
            }
        }
    }

    if (0 != g_threadLocalSiteConnection)
    {
        ACE_OS::thr_setspecific(g_threadLocalSiteConnection, siteConnection);
    }
}

///////////////////////////////
///<summary>
///Gets the site connection for the current thread which was
///set previously using SetCurrentConnection.  This
///function uses thread local storage.
///</summary>
c_RestMgSiteConnection* c_RestMgSiteConnection::GetCurrentConnection()
{
    c_RestMgSiteConnection* connection = NULL;
    if (0 != g_threadLocalSiteConnection)
    {
        ACE_OS::thr_getspecific(g_threadLocalSiteConnection, (void**) &connection);
    }

    if (NULL == connection)
    {
        // No site connection means that we not have opened the connection to the site server yet.
        throw new MgConnectionNotOpenException(L"c_RestMgSiteConnection.GetCurrentConnection", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return connection;
}

MgUserInformation* c_RestMgSiteConnection::GetUserInfo()
{
    if (NULL == m_connProp.p)
    {
        throw new MgConnectionNotOpenException(L"c_RestMgSiteConnection.GetUserInfo", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    return  m_connProp->GetUserInfo();
}
