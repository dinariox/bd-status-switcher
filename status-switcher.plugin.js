/**
 * @name StatusSwitcher
 * @version 0.0.1
 * @author Timo Nowak
 * @authorId dinario#1001
 * @description Lets you switch your Discord status by pressing key combinations or physical buttons
 * 
 */

module.exports = class StatusSwitcher {
    start() {
        console.log('StatusSwitcher started');

        window.addEventListener('keyup', (e) => {
            if (e.ctrlKey && e.altKey) {
                switch (e.key) {
                    case 'o':
                        this.switchStatus('online');
                        break;
                    case 'i':
                        this.switchStatus('idle');
                        break;
                    case 'd':
                        this.switchStatus('dnd');
                        break;
                    case 'v':
                        this.switchStatus('invisible');
                        break;
                }
            }
        })
    }

    switchStatus(status) {
        console.log('StatusSwitcher switched status');
        this.openStatusPanelEl = document.getElementsByClassName('avatarWrapper-1B9FTW')[0];
        this.openStatusPanelEl.click();

        switch (status) {
            case 'online':
                document.getElementById('status-picker-online').click();
                break;
            case 'idle':
                document.getElementById('status-picker-idle').click();
                break;
            case 'dnd':
                document.getElementById('status-picker-dnd').click();
                break;
            case 'invisible':
                document.getElementById('status-picker-invisible').click();
                break;
            default:
                console.log('StatusSwitcher: Invalid status');
                this.openStatusPanelEl.click(); // Close panel again
                break;
        }
                
    }

    stop() {
        console.log('StatusSwitcher stopped');
    }
}