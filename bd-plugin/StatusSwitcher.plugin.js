/**
 * @name StatusSwitcher
 * @version 0.1.2
 * @author Timo Nowak
 * @authorId dinario#1111
 * @description Lets you switch your Discord status by pressing key combinations or physical buttons
 *
 */

const http = require('http');

module.exports = class StatusSwitcher {
	start() {
		console.log('StatusSwitcher started');

		this.controller = new AbortController(); // To remove the event listener when the plugin is stopped

		window.addEventListener(
			'keyup',
			(e) => {
				if (e.ctrlKey && e.altKey) {
					switch (e.key) {
						case 'o':
							this.switchStatus('online');
							break;
						case 'a':
							this.switchStatus('afk');
							break;
						case 'd':
							this.switchStatus('dnd');
							break;
						case 'i':
							this.switchStatus('invisible');
							break;
						case 'g':
							BdApi.alert('Current status', this.getStatus());
							break;
					}
				}
			},
			{ signal: this.controller.signal }
		);

		this.setupHttpServer();
	}

	setupHttpServer() {
		this.server = http.createServer((req, res) => {
			if (req.method === 'GET') {
				res.writeHead(200);
				res.end(this.getStatus());
			} else if (req.method === 'POST') {
				let body = '';
				req.on('data', (chunk) => {
					body += chunk.toString(); // convert Buffer to string
				});
				req.on('end', () => {
					res.writeHead(this.handlePost(body));
					res.end();
				});
			}
		});
		this.server.listen(5142, 'localhost', () => {
			console.log('StatusSwitcher: Listening on localhost:5142');
		});
	}

	handlePost(body) {
		try {
			let data = JSON.parse(body);
			if (data.status) {
				return this.switchStatus(data.status) ? 200 : 400;
			}
		} catch (e) {
			console.log('StatusSwitcher: Invalid JSON');
			return 400;
		}
	}

	switchStatus(status) {
		console.log('StatusSwitcher switched status (' + status + ')');
		this.openStatusPanelEl = document.getElementsByClassName('avatarWrapper-1B9FTW')[0];
		this.openStatusPanelEl.click();

		switch (status) {
			case 'online':
				document.getElementById('status-picker-online').click();
				return true;
			case 'afk':
				document.getElementById('status-picker-idle').click();
				return true;
			case 'dnd':
				document.getElementById('status-picker-dnd').click();
				return true;
			case 'invisible':
				document.getElementById('status-picker-invisible').click();
				return true;
			default:
				console.log('StatusSwitcher: Invalid status');
				this.openStatusPanelEl.click(); // Close panel again
				return false;
		}
	}

	getStatus() {
		console.log('StatusSwitcher: getStatus');
		this.avatarEl = document.getElementsByClassName('avatar-1EWyVD')[0];
		this.avatarAriaLabel = this.avatarEl.getAttribute('aria-label');
		this.userStatus = this.avatarAriaLabel.split(/[, ]+/).pop().trim().toLowerCase();

		// Overrides the status text with unifyed status
		switch (this.userStatus) {
			case 'abwesend':
			case 'idle':
				this.userStatus = 'afk';
				break;
			case 'stören':
			case 'disturb':
				this.userStatus = 'dnd';
				break;
			case 'unsichtbar':
				this.userStatus = 'invisible';
		}

		return this.userStatus;
	}

	stop() {
		console.log('StatusSwitcher stopped');
		this.controller.abort(); // Event listener will be removed
		this.server = null;
	}
};
