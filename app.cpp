//name: Aahan Nayak
//roll no: 2501AI21
/*
 * Declaration of Authorship:
 * I declare that this assignment is my own work in accordance
 * with IIT Patna Academic Honesty Policy.
 */

#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

int main() {
    cout << "======================================================" << endl;
    cout << "  IITPSx Processor - Visual IDE & Emulator Launcher   " << endl;
    cout << "  Author: Aahan Nayak                                " << endl;
    cout << "======================================================" << endl;

#if defined(_WIN32) || defined(_WIN64)
    cout << "[Windows] Launching standalone app window..." << endl;
    // Uses Edge or Chrome in app-mode (dedicated window without browser UI)
    system("start msedge --app=\"%CD%\\index.html\" 2>nul || start chrome --app=\"%CD%\\index.html\" 2>nul || start index.html");

#elif defined(__APPLE__)
    cout << "[macOS] Launching standalone app window..." << endl;
    // Opens native IITPSx.app if built, or Chrome/Brave app-mode, or default browser
    system("open IITPSx.app 2>/dev/null || "
           "open -na 'Google Chrome' --args --app=\"file://$(pwd)/index.html\" 2>/dev/null || "
           "open -na 'Brave Browser' --args --app=\"file://$(pwd)/index.html\" 2>/dev/null || "
           "open \"file://$(pwd)/index.html\"");

#elif defined(__linux__)
    cout << "[Linux] Launching standalone app window..." << endl;
    // Linux standalone app-mode
    system("google-chrome --app=\"file://$(pwd)/index.html\" 2>/dev/null || "
           "chromium-browser --app=\"file://$(pwd)/index.html\" 2>/dev/null || "
           "xdg-open index.html");

#else
    cout << "[Warning] Unknown operating system. Opening index.html manually..." << endl;
#endif

    cout << "Simulator launched successfully!" << endl;
    return 0;
}