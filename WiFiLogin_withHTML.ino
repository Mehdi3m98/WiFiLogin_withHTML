#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* apSSID = "MyAP";
const char* apPassword = "password";

const char loginPage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            text-align: center;
            background-color: #000;
            color: #fff;
        }

        .container {
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            height: 100vh;
        }

        h2 {
            margin-bottom: 20px;
        }

        .form-group {
            margin-bottom: 20px;
        }

        .form-group label {
            display: block;
            margin-bottom: 5px;
        }

        .form-group input {
            width: 250px;
            padding: 10px;
            border: 1px solid #ccc;
            border-radius: 4px;
            font-size: 16px;
        }

        .form-group button {
            padding: 10px 20px;
            background-color: #4CAF50;
            color: #fff;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            font-size: 16px;
        }
    </style>
</head>

<body>
    <div class="container">
        <h2>Wi-Fi Login</h2>
        <form id="login-form" method="POST">
            <div class="form-group">
                <label for="ssid">Wi-Fi SSID:</label>
                <input type="text" id="ssid" name="ssid" required>
            </div>
            <div class="form-group">
                <label for="password">Wi-Fi Password:</label>
                <input type="password" id="password" name="password" required>
            </div>
            <div class="form-group">
                <button type="submit">Connect</button>
            </div>
        </form>
        <p id="message"></p>
    </div>

    <script>
        document.getElementById("login-form").addEventListener("submit", function (event) {
            event.preventDefault();
            var ssid = document.getElementById("ssid").value;
            var password = document.getElementById("password").value;
            var xhr = new XMLHttpRequest();
            xhr.open("POST", "/login");
            xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
            xhr.onreadystatechange = function () {
                if (xhr.readyState === 4) {
                    if (xhr.status === 200) {
                        document.getElementById("message").textContent = "Connected to Wi-Fi successfully!";
                    } else {
                        document.getElementById("message").textContent = "Failed to connect to Wi-Fi. Please try again.";
                    }
                }
            };
            xhr.send("ssid=" + encodeURIComponent(ssid) + "&password=" + encodeURIComponent(password));
        });
    </script>
</body>

</html>
)=====";

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", loginPage);
}

void handleLogin() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    // اتصال به شبکه Wi-Fi با استفاده از اطلاعات ورودی کاربر
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    
    // ارسال پاسخ موفقیت آمیز به کاربر
    server.send(200, "text/plain", "Connected to Wi-Fi successfully!");
  } else {
    // ارسال پاسخ خطا به کاربر
    server.send(400, "text/plain", "Invalid request");
  }
}

void setup() {
  Serial.begin(9600);

  // حالت Access Point را فعال می‌کنیم
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID);

  // آدرس IP AP را چاپ می‌کنیم
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // راه‌اندازی وب سرور
  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.begin();
}

void loop() {
  server.handleClient();
}
