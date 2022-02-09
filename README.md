# bd-status-switcher

## Purpose

A plugin for BetterDiscord to switch the user's status (online, afk, dnd, invisible) via keyboard shortcuts and provide an api.

## Keyboard Shortcuts

-   `Ctrl + Alt + o` to set your status to online
-   `Ctrl + Alt + a` to set your status to afk
-   `Ctrl + Alt + d` to set your status to dnd
-   `Ctrl + Alt + i` to set your status to invisible
-   `Ctrl + Alt + g` to show a popup with your current status (for development/debugging purposes)

## API

`localhost:5142`

-   `GET /` get the current status in plain text (online, afk, dnd, invisible)
-   `POST /` switch the users status. Body must be in format `{"status": "online"}`. Returns status 200 if it successfully switched the status and 400 if the JSON or the status is invalid.
