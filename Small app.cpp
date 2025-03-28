#include <iostream>
#include <string>
#include <cups/cups.h>
#include <memory> 


void getPrinterAttributes(const std::string& printer_ip) {
   
    std::string printer_uri = "ipp://" + printer_ip + "/ipp/print";
    const char* uri = printer_uri.c_str();

   
    ipp_t* request = ippNewRequest(IPP_OP_GET_PRINTER_ATTRIBUTES);
    if (!request) {
        std::cerr << "Error: Failed to create IPP request." << std::endl;
        return;
    }

   
    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri", nullptr, uri);

   
    std::unique_ptr<http_t, decltype(&httpClose)> http(
        httpConnect2(printer_ip.c_str(), 631, nullptr, AF_UNSPEC, HTTP_ENCRYPT_IF_REQUESTED, 1, 3000, nullptr),
        &httpClose);

    if (!http) {
        std::cerr << "Error: Could not connect to printer at " << printer_ip << std::endl;
        ippDelete(request);
        return;
    }

  
    ipp_t* response = cupsDoRequest(http.get(), request, "/");
    if (!response) {
        std::cerr << "Error: Failed to retrieve printer attributes. CUPS Error: "
                  << cupsLastErrorString() << std::endl;
        ippDelete(request);
        return;
    }

   
    std::cout << "Printer attributes from " << printer_ip << ":\n";
    ipp_attribute_t* attr = ippGetFirstAttribute(response);
    while (attr) {
        const char* name = ippGetName(attr);
        if (name) { 
            std::cout << name << ": ";
            if (ippGetValueTag(attr) == IPP_TAG_TEXT || ippGetValueTag(attr) == IPP_TAG_NAME) {
                std::cout << ippGetString(attr, 0, nullptr);
            } else if (ippGetValueTag(attr) == IPP_TAG_INTEGER) {
                std::cout << ippGetInteger(attr, 0);
            }
            std::cout << std::endl;
        }
        attr = ippGetNextAttribute(response);
    }

   
    ippDelete(response);
}

/
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <printer_ip>" << std::endl;
        return 1;
    }

    std::string printer_ip = argv[1];

   
    getPrinterAttributes(printer_ip);

    return 0;
}
