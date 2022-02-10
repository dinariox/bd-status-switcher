# bd-status-switcher

Idea: Let the user switch their Discord status (online, afk, dnd, invisible) by pressing physical buttons.

### General TODOs

-   Add microphone/headphone mute status

---

## bd-plugin/StatusSwitcher.plugin.js

### Purpose

A plugin for BetterDiscord to switch the user's status (online, afk, dnd, invisible) via keyboard shortcuts and provide an api.

### Keyboard Shortcuts

-   `Ctrl + Alt + o` to set your status to online
-   `Ctrl + Alt + a` to set your status to afk
-   `Ctrl + Alt + d` to set your status to dnd
-   `Ctrl + Alt + i` to set your status to invisible
-   `Ctrl + Alt + g` to show a popup with your current status (for development/debugging purposes)

### API

`localhost:5142`

-   `GET /` get the current status in plain text (online, afk, dnd, invisible)
-   `POST /` switch the users status. Body must be in format `{"status": "online"}`. Returns status 200 if it successfully switched the status and 400 if the JSON or the status is invalid.

---

## arduino/bd-status-switcher/bd-status-switcher.ino

### Purpose

An Arduino program to light up leds to show the discord status it receives via a USB Serial connection and to change the discord status via button presses.

### Serial Communication

Baudrate: 38400

Commands to the Arduino initiated by the user/the python script:

-   `name` prints the name set in the code
-   `version` prints the version set in code
-   `setstatus <status>` where status is one of `online, afk, dnd, invisible, unknown`
-   `getstatus` prints the current status

Commands from the Arduino sent to the user/the python script:

-   `status <status>` where status is one of `online, afk, dnd, invisible, unknown`

---

## python/app.py

### Purpose

A python script to connect the BetterDiscord plugin to the Arduino. It communicates with the API provided by the plugin and the Arduino via USB Serial.
It scans available COM ports (or tty on Linux) and sends the `name` command to it. If it responds its probably the Arduino. It then prints a list with all the available ports (and names if it got a response to the command).

### TODOs

-   Check if the discord plugin is reachable. If not send status `unknown` to arduino
