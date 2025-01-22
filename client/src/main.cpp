#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QFont>
#include "QRCodeWindow.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

int main(int argc, char *argv[]) {
    // Fetch data from the server
    httplib::Client cli("http://127.0.0.1:5000");
    auto res = cli.Get("/hello_world?name=Alexander");

    if (res) {
        // Parse JSON response from the server
        auto resp = json::parse(res->body);
        std::string receivedMessage = resp["msg"].get<std::string>();

        // Initialize the Qt application
        QApplication app(argc, argv);

        // Create and show the QR code window with the server message as the QR code data
        QRCodeWindow window(QString::fromStdString(receivedMessage));
        window.show();

        // Run the Qt application event loop
        return app.exec();
    } else {
        std::cerr << "Error fetching data from server" << std::endl;
        return 1;
    }

    return 0;
}