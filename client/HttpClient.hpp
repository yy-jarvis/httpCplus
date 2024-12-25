//
// Created by jyany on 24-12-25.
//

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "iostream"
#include "vector"

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/NetException.h"
#include <Poco/StreamCopier.h>
#include "Poco/NullStream.h"

#include "Poco/JSON/JSON.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"

#include "Poco/URI.h"

class HttpClient {
public:
    HttpClient(std::string url1, std::string url2, std::string username, std::string password);
    ~HttpClient();

    bool login();

    bool setLighting(bool on);

protected:
    /**
     * post请求
     * @param url 请求地址
     * @param request 请求体
     * @return 响应体
     */
    void PostRequest(std::string url, std::string request, std::string& responseBody);
    void PostRequest(std::string url, std::string request, std::string& responseBody, std::map<std::string, std::string> requestHeader);

private:
    /**
     * 获取时区
     * @return
     */
    long getUTC();
    std::string getUTCZone();

    std::string JsonToString(Poco::JSON::Object json);
    Poco::JSON::Object::Ptr StringToJson(std::string json);

private:
    std::string token;
    std::string username;
    std::string password;

    std::string loginUrl;
    std::string configUrl;

};



#endif //HTTPREQUEST_HPP
