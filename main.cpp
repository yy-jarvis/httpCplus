#include <iostream>

#include "client/HttpClient.hpp"

#include <thread>
#include <chrono>

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    // TIP Press <shortcut actionId="RenameElement"/> when your caret is at the
    // <b>lang</b> variable name to see how CLion can help you rename it.
    auto lang = "C++";
    std::cout << "Hello and welcome to " << lang << "!\n";

    std::string loginUrl = "http://127.0.0.1:8082/manager/user";
    std::string configUrl = "http://127.0.0.1:8082/setting/config";
    std::string userName = "user";
    std::string password = "password";
    HttpClient httpClient(loginUrl, configUrl, userName, password);
    httpClient.login();

    std::chrono::seconds seconds_to_sleep(5);  // 暂停5秒，也可以使用其他时间单位，如std::chrono::milliseconds等
    std::this_thread::sleep_for(seconds_to_sleep);

    httpClient.setLighting(true);


    return 0;
}

// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.