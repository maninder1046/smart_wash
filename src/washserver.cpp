#include "washserver.h"
#include "logic.h"

WebServer server(80);
Preferences prefs;

// Persistent storage
bool autoMode = false;

String webpage = R"rawliteral(
<!DOCTYPE html>
<html>
<head><title>Smart Washer + Dryer</title></head>
<body>
<h2>ESP32 Smart Washer + Dryer</h2>

<h3>Automatic Control</h3>
<button onclick="fetch('/start_auto')">Start Auto Cycle</button>
<button onclick="fetch('/stop')">Stop All</button>

<h3>Manual Control</h3>
<button onclick="fetch('/wash')">Toggle Wash</button>
<button onclick="fetch('/spin')">Toggle Spin</button>
<button onclick="fetch('/inlet')">Toggle Inlet</button>
<button onclick="fetch('/drain')">Toggle Drain</button>
<button onclick="fetch('/dryer')">Toggle Dryer</button>

<h3>Status</h3>
<div id='status'>Loading...</div>
<script>
setInterval(()=>{
 fetch('/status').then(r=>r.text()).then(t=>document.getElementById('status').innerHTML=t);
},1000);
</script>
</body></html>
)rawliteral";

void saveState() {
  prefs.begin("washer", false);
  prefs.putBool("autoMode", autoMode);
  prefs.putInt("washerState", washerState);
  prefs.putInt("dryerState", dryerState);
  prefs.putBool("manualWash", manualWash);
  prefs.putBool("manualSpin", manualSpin);
  prefs.putBool("manualFill", manualFill);
  prefs.putBool("manualDrain", manualDrain);
  prefs.putBool("manualDryer", manualDryer);
  prefs.putULong("stateStartTime", stateStartTime);
  prefs.end();
}

void loadState() {
  prefs.begin("washer", true);
  autoMode      = prefs.getBool("autoMode", false);
  washerState   = (WasherState)prefs.getInt("washerState", W_IDLE);
  dryerState    = (DryerState)prefs.getInt("dryerState", D_IDLE);
  manualWash    = prefs.getBool("manualWash", false);
  manualSpin    = prefs.getBool("manualSpin", false);
  manualFill    = prefs.getBool("manualFill", false);
  manualDrain   = prefs.getBool("manualDrain", false);
  manualDryer   = prefs.getBool("manualDryer", false);
  stateStartTime= prefs.getULong("stateStartTime", 0);
  prefs.end();
}

void initServer() {
  server.on("/", [](){ server.send(200, "text/html", webpage); });

  server.on("/start_auto", [](){
    autoMode = true;
    washerState = W_FILLING;
    stateStartTime = millis();
    saveState();
    server.send(200, "text/plain", "Auto cycle started");
  });

  server.on("/stop", [](){
    autoMode = false;
    washerState = W_IDLE;
    dryerState = D_IDLE;
    manualWash = manualSpin = manualFill = manualDrain = manualDryer = false;
    saveState();
    server.send(200, "text/plain", "Stopped all cycles");
  });

  server.on("/wash", [](){ manualWash = !manualWash; saveState(); server.send(200, "text/plain", "Wash toggled"); });
  server.on("/spin", [](){ manualSpin = !manualSpin; saveState(); server.send(200, "text/plain", "Spin toggled"); });
  server.on("/inlet", [](){ manualFill = !manualFill; saveState(); server.send(200, "text/plain", "Inlet toggled"); });
  server.on("/drain", [](){ manualDrain = !manualDrain; saveState(); server.send(200, "text/plain", "Drain toggled"); });
  server.on("/dryer", [](){ manualDryer = !manualDryer; saveState(); server.send(200, "text/plain", "Dryer toggled"); });

  server.on("/status", [](){
    String s = "AutoMode: " + String(autoMode ? "ON" : "OFF") + "<br>";
    s += "Washer State: " + String(getWasherStateString()) + "<br>";
    s += "Dryer State: " + String(getDryerStateString()) + "<br>";
    s += "Manual: Wash=" + String(manualWash) + 
         " Spin=" + String(manualSpin) + 
         " Fill=" + String(manualFill) + 
         " Drain=" + String(manualDrain) + 
         " Dryer=" + String(manualDryer) + "<br>";
    server.send(200, "text/html", s);
  });

  server.begin();
}

void handleServer() {
  server.handleClient();
}
