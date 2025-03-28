#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include "ipp.h"  


size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* userData) {
    userData->append((char*)contents, size * nmemb);
    return size * nmemb;
}


void getPrinterAttributes(const std::string& printer_ip) {
    std::string printer_uri = "http://" + printer_ip + ":631/ipp/printer";

    
    ipp::Request request(ipp::Operation::GetPrinterAttributes);
    request.setPrinterURI("ipp://" + printer_ip + "/ipp/printer");

    
    std::vector<uint8_t> ipp_request = request.write();

   
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Error: Could not initialize CURL" << std::endl;
        return;
    }

    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_URL, printer_uri.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ipp_request.data());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, ipp_request.size());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

   
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Error: Failed to connect to printer (" << curl_easy_strerror(res) << ")" << std::endl;
    } else {
        std::cout << "IPP Response Received:\n" << response_data << std::endl;

      
        std::vector<uint8_t> ipp_response_data(response_data.begin(), response_data.end());
        ipp::Response response;
        if (response.read(ipp_response_data)) {
            std::cout << "Printer Attributes:" << std::endl;
            for (const auto& attr : response.getAttributes()) {
                std::cout << attr.first << ": ";
                for (const auto& value : attr.second) {
                    std::cout << value.toString() << " ";
                }
                std::cout << std::endl;
            }
        } else {
            std::cerr << "Failed to parse IPP response." << std::endl;
        }
    }

    curl_easy_cleanup(curl);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <printer_ip>" << std::endl;
        return 1;
    }

    std::string printer_ip = argv[1];
    getPrinterAttributes(printer_ip);
    return 0;
}
