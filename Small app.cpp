#include <iostream>
#include <windows.h>
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

void getPrinterAttributes() {
    HRESULT hres;

    // Step 1: Initialize COM Library
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres)) {
        std::cerr << "COM initialization failed. Error: 0x"
                  << std::hex << hres << std::endl;
        return;
    }

    // Step 2: Initialize COM Security
    hres = CoInitializeSecurity(
        NULL, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE, NULL
    );
    if (FAILED(hres)) {
        std::cerr << "COM security initialization failed. Error: 0x"
                  << std::hex << hres << std::endl;
        CoUninitialize();
        return;
    }

    // Step 3: Get WMI Locator
    IWbemLocator* pLoc = NULL;
    hres = CoCreateInstance(
        CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&pLoc
    );
    if (FAILED(hres)) {
        std::cerr << "Failed to create IWbemLocator object. Error: 0x"
                  << std::hex << hres << std::endl;
        CoUninitialize();
        return;
    }

    // Step 4: Connect to WMI Service
    IWbemServices* pSvc = NULL;
    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc
    );
    if (FAILED(hres)) {
        std::cerr << "Could not connect to WMI service. Error: 0x"
                  << std::hex << hres << std::endl;
        pLoc->Release();
        CoUninitialize();
        return;
    }

    // Step 5: Set security levels
    hres = CoSetProxyBlanket(
        pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
        RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE
    );
    if (FAILED(hres)) {
        std::cerr << "Could not set security levels. Error: 0x"
                  << std::hex << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return;
    }

    // Step 6: Execute WMI Query to Get Printer Information
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_Printer"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator
    );
    if (FAILED(hres)) {
        std::cerr << "WMI Query Failed. Error: 0x"
                  << std::hex << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return;
    }

    // Step 7: Process Results
    IWbemClassObject* pclsObj = NULL;
    ULONG uReturn = 0;
    while (pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn) == S_OK) {
        VARIANT vtProp;

        // Printer Name
        pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
        std::wcout << L"\nPrinter Name: " << vtProp.bstrVal << std::endl;
        VariantClear(&vtProp);

        // Printer Status
        pclsObj->Get(L"PrinterStatus", 0, &vtProp, 0, 0);
        std::wcout << L"Printer Status: " << vtProp.uintVal << std::endl;
        VariantClear(&vtProp);

        // Location
        pclsObj->Get(L"Location", 0, &vtProp, 0, 0);
        if (vtProp.vt != VT_NULL) {
            std::wcout << L"Location: " << vtProp.bstrVal << std::endl;
        }
        VariantClear(&vtProp);

        // Driver Name
        pclsObj->Get(L"DriverName", 0, &vtProp, 0, 0);
        std::wcout << L"Driver Name: " << vtProp.bstrVal << std::endl;
        VariantClear(&vtProp);

        // Default Printer
        pclsObj->Get(L"Default", 0, &vtProp, 0, 0);
        std::wcout << L"Default Printer: " << (vtProp.boolVal ? L"Yes" : L"No") << std::endl;
        VariantClear(&vtProp);

        pclsObj->Release();
    }

    // Step 8: Cleanup
    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    CoUninitialize();
}

int main() {
    std::cout << "Fetching printer attributes...\n";
    getPrinterAttributes();
    return 0;
}
