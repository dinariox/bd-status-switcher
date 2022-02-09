# bd-status-switcher

A plugin for BetterDiscord to switch the user's status (online, afk, dnd, invisible) via keyboard shortcuts and provide an api.

## API

`localhost:5142`

-   `GET /` get the current status in plain text (online, afk, dnd, invisible)
-   `POST /` switch the users status. Body must be in format `{"status": "online"}`. Returns status 200 if it successfully switched the status and 400 if the JSON or the status is invalid.
