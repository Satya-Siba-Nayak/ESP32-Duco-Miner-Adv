# 🚀 ESP32-Duco-Miner-Adv
An advanced, feature-rich Duino-Coin miner for ESP32 and ESP8266 with a stunning Web Dashboard and real-time telemetry.

---

## 👋 Hey there!
Welcome to the **ESP32 Duino-Coin Miner Adv**! This is a super-charged version of the classic miner, designed to look cool and run fast. Whether you're 12 or 100, this guide will help you get your own little mining rig up and running in no time!

---

## ✨ Features In-Depth

### 🧠 Smart Mining
*   **Dual-Core Power:** Unlike standard miners, this one utilizes both cores of the ESP32. One core handles the network and system tasks, while both work together to solve hashes.
*   **Boot Boost:** When you first turn it on, the miner runs at 240MHz (Max Power) for 2 minutes to give you a "head start" before settling into a stable 160MHz to stay cool.
*   **Thermal Failsafe:** The miner constantly watches its own temperature. If it gets too hot (over 72°C), it automatically throttles down to "Eco Mode" to prevent damage.

### 🌐 Connectivity & Fail-safes
*   **VIP WiFi List:** You can save multiple WiFi networks. If you take your miner to a friend's house, it will automatically connect if their WiFi is in your list.
*   **Captive Portal (WiFi Setup):** If the miner can't find any known WiFi, it won't just give up! It will start its own WiFi network named `Duino-Miner-[YourRigName]`. Connect to it with your phone, and a setup page will pop up where you can enter new WiFi details and your Duco username.

### 📊 Advanced Web Dashboard
The dashboard isn't just for show—it's a high-tech telemetry engine:
*   **Hashrate (kH/s):** Shows how many kilo-hashes your miner is doing per second.
*   **Core Symmetry:** Measures if both cores are working equally. 100% means perfect balance.
*   **Efficiency:** The percentage of "Good" shares vs. total shares.
*   **Ping/Latency:** How fast your internet is talking to the Duino-Coin servers.
*   **Matrix Log Feed:** A real-time terminal showing exactly what your miner is doing at any moment.

---

## 🛠️ What You Need (Hardware)
1.  **ESP32 or ESP8266 Board:** (ESP32 is highly recommended for Dual-Core support).
2.  **Micro-USB Cable:** A high-quality cable for power and data.
3.  **A Computer:** To flash the code.

---

## 📚 What You Need (Software)
Install these libraries in your **Arduino IDE** (Library Manager):
1.  `ArduinoJson` (by Benoit Blanchon)
2.  `WiFiManager` (by tzapu)
3.  `Ticker` (Built-in for ESP boards)
4.  `U8g2` (Optional, for OLED displays)

---

## 🚀 Step-by-Step Setup

### 1. Download & Open
Download this repository. Open the **`ESP32-Miner-Code`** folder and click on the **`ESP32-Miner-Code.ino`** file. 

> [!IMPORTANT]
> To keep your ESP32 memory clean and avoid confusion, **only use the files inside the `ESP32-Miner-Code` folder** for uploading to your device. The other files in the root directory are for documentation and GitHub organization only.

### 2. Identity Setup (`credentials.h`)
1.  Open the `ESP32-Miner-Code` folder.
2.  Rename `credentials.h.example` to **`credentials.h`**.
3.  Fill in your **Duino-Coin username** and **WiFi details**.
4.  *Don't worry:* `credentials.h` is ignored by Git, so your passwords stay private!

### 3. ⚠️ IMPORTANT: Partition Scheme (Don't skip!)
Because this code includes a beautiful dashboard and OTA features, it is **larger than a normal sketch**. You **MUST** change the partition scheme in the Arduino IDE, or it won't fit!
1.  Go to **Tools** > **Partition Scheme**.
2.  Select **"Minimal SPIFFS (1.9MB APP with OTA)"** or **"Huge APP (3MB No OTA)"**.
3.  If you use an ESP8266, ensure you select a configuration with at least 2MB of Flash.

### 4. Upload Methods
*   **Wired (First time):** Connect via USB, select your port, and click **Upload**.
*   **Wireless (OTA):** Once it's running, you can update the code over your WiFi! In the Arduino IDE, go to **Tools** > **Port** and look for your miner's name under "Network Ports".

---

## 📊 Understanding the Dashboard Stats

| Stat | Explanation | How it's calculated |
| :--- | :--- | :--- |
| **Total Hashrate** | Combined speed of both cores. | `(Core0 + Core1) / 1000` |
| **Share Velocity** | How many shares you get per minute. | `Accepted Shares / Minutes Uptime` |
| **Daily Yield** | Estimated DUCO you'll earn today. | `Shares per Minute * 1440 * Average DUCO per share` |
| **Kolka Trust** | Your "reputation" with the server. | Based on your Ping and rejected share count. |
| **Silicon Health** | How "healthy" your chip is running. | Checks free RAM and thermal levels. |

---

## 🔧 Power Modes
*   **ECO:** Runs at 80MHz. Cool and quiet, uses very little power.
*   **BAL (Balanced):** Runs at 160MHz. The sweet spot for 24/7 mining.
*   **MAX:** Runs at 240MHz. Maximum performance, but will run hotter.

---

## 🆘 Support & Troubleshooting
If you face any issues, bugs, or compilation errors:
1.  Check the **[GitHub Issues](https://github.com/Satya-Siba-Nayak/ESP32-Duco-Miner-Adv/issues)** tab to see if someone else has already solved it.
2.  If not, feel free to **[Open a New Issue](https://github.com/Satya-Siba-Nayak/ESP32-Duco-Miner-Adv/issues/new)** with details about your problem!

---

## 💖 Credits & Support
Made with ♡ by **[Satya](https://satya.is-a.dev)**.
For help or more info, visit the **[Duino-Coin Official Website](https://duinocoin.com)**.

---
*Keep mining, keep shining! ⛏️💎*
