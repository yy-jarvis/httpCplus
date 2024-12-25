//
// Created by jyany on 24-12-25.
//

#include "HttpClient.hpp"

HttpClient::HttpClient(std::string url1, std::string url2, std::string username, std::string password) {
    this->username = username;
    this->password = password;
    loginUrl = url1;
    configUrl = url2;
}

HttpClient::~HttpClient(){}

bool HttpClient::login() {
    if (username.empty() || password.empty()) {
        std::cout << "Username and password must be specified" << std::endl;
        return false;
    }

    // 设置请求体
    Poco::JSON::Object body, parameters;
    body.set("name", "manager.login");
    body.set("dir", "manager");
    parameters.set("user_name", username);
    parameters.set("user_passwd", password);
    parameters.set("utc_time", getUTC());
    parameters.set("now_utc", getUTCZone());
    body.set("parameters", parameters);

    // post 请求
    std::string responseBody;
    std::string bodyStr = JsonToString(body);
    std::cout << "login: " << bodyStr << std::endl;
    PostRequest(loginUrl, bodyStr, responseBody);
    std::cout << "Response: " << responseBody << std::endl;

    auto responsePtr = StringToJson(responseBody);
    if (responsePtr != nullptr) {
        if (responsePtr->has("parameters")) {
            Poco::JSON::Object::Ptr parametersObject = responsePtr->getObject("parameters");
            if (parametersObject->has("fingerprint")) {
                token = parametersObject->getValue<std::string>("fingerprint");
                std::cout << "fingerprint的值为: " << token << std::endl;
                return true;
            } else {
                token = nullptr;
                std::cerr << "parameters对象中不存在fingerprint属性。" << std::endl;
            }
        } else {
            token = nullptr;
            std::cerr << "顶层JSON对象中不存在parameters属性。" << std::endl;
        }
    }

    return false;
}

bool HttpClient::setLighting(bool on) {
    if (token.empty()) {
        std::cout << "token is empty" << std::endl;
        return false;
    }

    // 设置请求体
    Poco::JSON::Object body, parameters;
    body.set("name", "setting.switch_ircut");
    body.set("dir", "setting，camera");
    parameters.set("level", on?1:0);
    body.set("parameters", parameters);

    // post 请求
    std::string responseBody;
    std::string bodyStr = JsonToString(body);
    std::cout << "login: " << bodyStr << std::endl;
    PostRequest(configUrl, bodyStr, responseBody);
    std::cout << "Response: " << responseBody << std::endl;

    auto responsePtr = StringToJson(responseBody);
    if (responsePtr != nullptr) {
        if (responsePtr->has("parameters")) {
            Poco::JSON::Object::Ptr parametersObject = responsePtr->getObject("parameters");
            if (parametersObject->has("code")) {
                auto isSucessful = parametersObject->getValue<int>("code");
                std::cout << "set config successfully " << std::endl;
                return true;
            } else {
                if (parametersObject->has("descriptor")) {
                    auto msg = parametersObject->getValue<std::string>("descriptor");
                    std::cout <<"set config descriptor: " << msg << std::endl;
                }
            }
        } else {
            std::cerr << "顶层JSON对象中不存在parameters属性。" << std::endl;
        }
    }

    return false;
}



void HttpClient::PostRequest(std::string url, std::string requestBody, std::string& responseBody) {
    if (url.empty() || requestBody.empty()) {
        return;
    }
    std::map<std::string, std::string> requestHeader;

    PostRequest(url, requestBody, responseBody, requestHeader);

}

void HttpClient::PostRequest(std::string url, std::string requestBody, std::string& responseBody, std::map<std::string, std::string> requestHeader) {
    if (url.empty() || requestBody.empty()) {
        return;
    }

    try {
        // 解析url
        Poco::URI uri(url);
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        session.setKeepAlive(true);

        // request请求

        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST,  uri.getPathAndQuery(), Poco::Net::HTTPMessage::HTTP_1_1);
        request.setContentType("application/json; charset=UTF-8");

        for (auto& pair : requestHeader) {
            request.set(pair.first, pair.second);
        }

        request.setContentLength(requestBody.length());
        std::ostream &os = session.sendRequest(request); // 发送请求
        os << requestBody;

        Poco::Net::HTTPResponse response;
        std::istream &is = session.receiveResponse(response); // 接收响应
        std::ostringstream oss;
        printf("[%s:%d] %d %s\n", __FILE__, __LINE__, response.getStatus(), response.getReason().c_str());

        std::cout << "response.getStatus(): " << response.getStatus() << std::endl;
        if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
            Poco::StreamCopier::copyStream(is, oss);
            if (!oss.str().empty())
            {
                // printf("[%s:%d] %s\n", __FILE__, __LINE__, oss.str().c_str());
                responseBody = oss.str();
            }
        } else
        {
            printf("[%s:%d] -----HTTPResponse error-----\n", __FILE__, __LINE__);
            Poco::NullOutputStream null;
            Poco::StreamCopier::copyStream(is, null);
        }
    }
    catch (Poco::Exception &e) {
        std::cout << e.what() << std::endl;
    }
}


long HttpClient::getUTC() {
    std::time_t now_utc = std::time(nullptr);
    return now_utc;
}

std::string HttpClient::getUTCZone() {
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);
    std::tm* utc_time = std::gmtime(&now);
    if (local_time && utc_time) {
        int offset_hours = (local_time->tm_hour - utc_time->tm_hour);
        std::string sign = (offset_hours >= 0)? "+" : "-";
        offset_hours = (offset_hours >= 0)? offset_hours : -offset_hours;
        return "UTC" + sign + std::to_string(offset_hours);
    } else {
        return "UTC-unknown";
    }
}

std::string HttpClient::JsonToString(Poco::JSON::Object json) {
    std::ostringstream ss;
    json.stringify(ss);
    std::string body;
    body = ss.str();
    return body;
}

Poco::JSON::Object::Ptr HttpClient::StringToJson(std::string json) {
    try {
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(json);
        if (result.type() == typeid(Poco::JSON::Object::Ptr)) {
            Poco::JSON::Object::Ptr jsonObject = result.extract<Poco::JSON::Object::Ptr>();
            // 此时已经将字符串解析为JSON对象，可以进行后续操作，比如访问对象中的属性等
            return jsonObject;
        } else {
            std::cerr << "解析JSON字符串失败，未能得到JSON对象。" << std::endl;
            return nullptr;
        }
    }catch (Poco::Exception &e) {
        std::cerr << e.what() << std::endl;
        return nullptr;
    }
}

