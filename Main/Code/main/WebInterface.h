#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

const char *otaUpdatePage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 OTA Update</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f9;
            color: #333;
            text-align: center;
            padding: 50px;
        }
        h1 {
            color: #4CAF50;
        }
        form {
            background: #fff;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
            display: inline-block;
        }
        input[type="file"] {
            margin: 10px 0;
        }
        input[type="submit"] {
            background-color: #4CAF50;
            color: white;
            border: none;
            padding: 10px 20px;
            text-transform: uppercase;
            cursor: pointer;
            border-radius: 5px;
        }
        input[type="submit"]:hover {
            background-color: #45a049;
        }
    </style>
    <script>
        function updateProgress() {
            fetch('/progress')
                .then(response => response.text())
                .then(data => {
                    document.getElementById('progress').innerText = data;
                });
        }
        setInterval(updateProgress, 200); // Update every 200ms
    </script>
</head>
<body>
    <h1>ESP32 OTA Update</h1>
    <form method="POST" action="/update" enctype="multipart/form-data">
        <input type="file" name="update">
        <br>
        <input type="submit" value="Update">
    </form>
    <p id="progress">Waiting for update...</p>
</body>
</html>
)rawliteral";

#endif // WEB_INTERFACE_H