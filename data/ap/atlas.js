(function () {
    const interval = 5000;

    const containerSettings = document.getElementById('container-settings');
    const containerList = document.getElementById('container-list');
    const containerDisconnected = document.getElementById('container-disconnected');
    const tableNetworkList = document.getElementById('table-network-list');

    const btnSettingsConnect = document.getElementById('btn-settings-connect');
    const btnSettingsCancel = document.getElementById('btn-settings-cancel');
    const btnListCustom = document.getElementById('btn-list-custom');

    const inputSettingsSSD = document.getElementById('configInputSSID');
    const inputSettingsPassword = document.getElementById('configInputPassword');

    function openConnectionSettings(ssid = '') {
        containerList.style.display = 'none';
        containerSettings.style.display = 'block';

        inputSettingsSSD.value = ssid;
        inputSettingsSSD.readOnly = ssid !== ''; // empty ssid means custom
        inputSettingsPassword.value = '';
    }

    function submitConnectionSettings() {
        containerSettings.style.display = 'none';

        sendConnectionSettings(inputSettingsSSD.value, inputSettingsPassword.value);
    }

    function cancelConnectionSettings() {
        containerList.style.display = 'block';
        containerSettings.style.display = 'none';
    }

    function getSignalIcon(protected, signal) {
        if (signal < 20) {
            if (protected) {
                return '<svg style="width:24px;height:24px" viewBox="0 0 24 24"><path fill="currentColor" d="M12 13C12.8 13 13.61 13.13 14.38 13.36C14.28 13.73 14.2 14.11 14.2 14.5V14.74C13.5 15.34 13 16.24 13 17.2V20.24L12 21.5C7.88 16.37 4.39 12.06 .365 7C3.69 4.41 7.78 3 12 3C16.2 3 20.31 4.41 23.64 7L20.91 10.39C20.32 10.14 19.68 10 19 10C18.87 10 18.75 10.03 18.62 10.04L20.7 7.45C18.08 5.86 15.06 5 12 5S5.9 5.85 3.26 7.44L8.38 13.8C9.5 13.28 10.74 13 12 13M23 17.3V20.8C23 21.4 22.4 22 21.7 22H16.2C15.6 22 15 21.4 15 20.7V17.2C15 16.6 15.6 16 16.2 16V14.5C16.2 13.1 17.6 12 19 12S21.8 13.1 21.8 14.5V16C22.4 16 23 16.6 23 17.3M20.5 14.5C20.5 13.7 19.8 13.2 19 13.2S17.5 13.7 17.5 14.5V16H20.5V14.5Z" /></svg>';
            } else {
                return '<svg style="width:24px;height:24px" viewBox="0 0 24 24"><path fill="currentColor" d="M12,3C7.79,3 3.7,4.41 0.38,7C4.41,12.06 7.89,16.37 12,21.5C16.08,16.42 20.24,11.24 23.65,7C20.32,4.41 16.22,3 12,3M12,5C15.07,5 18.09,5.86 20.71,7.45L15.61,13.81C14.5,13.28 13.25,13 12,13C10.75,13 9.5,13.28 8.39,13.8L3.27,7.44C5.91,5.85 8.93,5 12,5Z" /></svg>';
            }
        } else if (signal < 50) {
            if (protected) {
                return '<svg style="width:24px;height:24px" viewBox="0 0 24 24"><path fill="currentColor" d="M14.2 14.5V14.74C13.5 15.34 13 16.24 13 17.2V20.24L12 21.5C7.88 16.37 4.39 12.06 .365 7C3.69 4.41 7.78 3 12 3C16.2 3 20.31 4.41 23.64 7L20.91 10.39C20.32 10.14 19.68 10 19 10C18.87 10 18.74 10.03 18.61 10.04L20.7 7.45C18.08 5.86 15.06 5 12 5S5.9 5.85 3.26 7.44L6.5 11.43C7.73 10.75 9.61 10 12 10C13.68 10 15.12 10.38 16.26 10.84C15.03 11.67 14.2 13 14.2 14.5M23 17.3V20.8C23 21.4 22.4 22 21.7 22H16.2C15.6 22 15 21.4 15 20.7V17.2C15 16.6 15.6 16 16.2 16V14.5C16.2 13.1 17.6 12 19 12S21.8 13.1 21.8 14.5V16C22.4 16 23 16.6 23 17.3M20.5 14.5C20.5 13.7 19.8 13.2 19 13.2S17.5 13.7 17.5 14.5V16H20.5V14.5Z" /></svg>';
            } else {
                return '<svg style="width:24px;height:24px" viewBox="0 0 24 24"><path fill="currentColor" d="M12,3C7.79,3 3.7,4.41 0.38,7C4.41,12.06 7.89,16.37 12,21.5C16.08,16.42 20.24,11.24 23.65,7C20.32,4.41 16.22,3 12,3M12,5C15.07,5 18.09,5.86 20.71,7.45L17.5,11.43C16.26,10.74 14.37,10 12,10C9.62,10 7.74,10.75 6.5,11.43L3.27,7.44C5.91,5.85 8.93,5 12,5Z" /></svg>';
            }
        } else if (signal < 90) {
            if (protected) {
                return '<svg style="width:24px;height:24px" viewBox="0 0 24 24"><path fill="currentColor" d="M19 10C19.68 10 20.32 10.14 20.91 10.39L23.64 7C20.31 4.41 16.2 3 12 3C7.78 3 3.69 4.41 .365 7C4.39 12.06 7.88 16.37 12 21.5L13 20.24V17.2C13 16.24 13.5 15.34 14.2 14.74V14.5C14.2 12.06 16.4 10 19 10M12 8C9 8 6.67 9 5.2 9.84L3.26 7.44C5.9 5.85 8.91 5 12 5S18.08 5.86 20.7 7.45L18.76 9.88C17.25 9 14.87 8 12 8M21.8 16V14.5C21.8 13.1 20.4 12 19 12S16.2 13.1 16.2 14.5V16C15.6 16 15 16.6 15 17.2V20.7C15 21.4 15.6 22 16.2 22H21.7C22.4 22 23 21.4 23 20.8V17.3C23 16.6 22.4 16 21.8 16M20.5 16H17.5V14.5C17.5 13.7 18.2 13.2 19 13.2S20.5 13.7 20.5 14.5V16Z" /></svg>';
            } else {
                return '<svg style="width:24px;height:24px" viewBox="0 0 24 24"><path fill="currentColor" d="M12,3C7.79,3 3.7,4.41 0.38,7C4.41,12.06 7.89,16.37 12,21.5C16.08,16.42 20.24,11.24 23.65,7C20.32,4.41 16.22,3 12,3M12,5C15.07,5 18.09,5.86 20.71,7.45L18.77,9.88C17.26,9 14.88,8 12,8C9,8 6.68,9 5.21,9.84L3.27,7.44C5.91,5.85 8.93,5 12,5Z" /></svg>';
            }
        } else {
            if (protected) {
                return '<svg style="width:24px;height:24px" viewBox="0 0 24 24"><path fill="currentColor" d="M14.2 14.5V14.74C13.5 15.34 13 16.24 13 17.2V20.24L12 21.5C7.88 16.37 4.39 12.06 .365 7C3.69 4.41 7.78 3 12 3C16.2 3 20.31 4.41 23.64 7L20.91 10.39C20.32 10.14 19.68 10 19 10C16.4 10 14.2 12.06 14.2 14.5M23 17.3V20.8C23 21.4 22.4 22 21.7 22H16.2C15.6 22 15 21.4 15 20.7V17.2C15 16.6 15.6 16 16.2 16V14.5C16.2 13.1 17.6 12 19 12S21.8 13.1 21.8 14.5V16C22.4 16 23 16.6 23 17.3M20.5 14.5C20.5 13.7 19.8 13.2 19 13.2S17.5 13.7 17.5 14.5V16H20.5V14.5Z" /></svg>';
            } else {
                return '<svg style="width:24px;height:24px" viewBox="0 0 24 24"><path fill="currentColor" d="M12,3C7.79,3 3.7,4.41 0.38,7C4.41,12.06 7.89,16.37 12,21.5C16.08,16.42 20.24,11.24 23.65,7C20.32,4.41 16.22,3 12,3Z" /></svg>';
            }
        }
    }

    function getMeshIcon(mesh) {
        if (mesh) {
            return '<svg style="width:24px;height:24px" viewBox="0 0 24 24"><path fill="currentColor" d="M10,2C8.89,2 8,2.89 8,4V7C8,8.11 8.89,9 10,9H11V11H2V13H6V15H5C3.89,15 3,15.89 3,17V20C3,21.11 3.89,22 5,22H9C10.11,22 11,21.11 11,20V17C11,15.89 10.11,15 9,15H8V13H16V15H15C13.89,15 13,15.89 13,17V20C13,21.11 13.89,22 15,22H19C20.11,22 21,21.11 21,20V17C21,15.89 20.11,15 19,15H18V13H22V11H13V9H14C15.11,9 16,8.11 16,7V4C16,2.89 15.11,2 14,2H10M10,4H14V7H10V4M5,17H9V20H5V17M15,17H19V20H15V17Z" /></svg>';
        } else {
            return '';
        }
    }

    function createListRow(ssid, protected, mesh, signal) {
        return `<tr>
            <td>
                ${getSignalIcon(protected, signal)}
                ${getMeshIcon(mesh)}
            </td>
            <td>${ssid}</td>
            <td class="d-grid">
                <button type="button" class="btn btn-sm btn-success" onclick="openConnectionSettings('${ssid}'); return false;">
                    ${protected ? 'Enter Password' : 'Connect'}
                </button>
            </td>
        </tr>`;
    }

    function sendConnectionSettings(ssid, pass) {
        return fetch('/wifi/connect', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded'
            },
            cache: 'no-cache',
            body: `ssid=${ssid}&pass=${pass}`
        });
    };

    btnSettingsConnect.onclick = function () {
        submitConnectionSettings();
        return false;
    };

    btnSettingsCancel.onclick = function () {
        cancelConnectionSettings();
        return false;
    };

    btnListCustom.onclick = function () {
        openConnectionSettings();
        return false;
    };

    setInterval(function () {
        fetch('/wifi/networks')
            .then(response => {
                if (response.status !== 200) {
                    return Promise.reject(response);
                } else {
                    return response.json();
                }
            })
            .then(data => data.networks)
            .then(networks => {
                const ssids = {};
                const text = networks
                    .filter(network => {
                        if (network.ssid in ssids) {
                            ssids[network.ssid] += 1;
                            return false;
                        } else {
                            ssids[network.ssid] = 1;
                            return true;
                        }
                    })
                    .sort((a, b) => b.quality - a.quality) // quality in descending order
                    .map(network => createListRow(network.ssid, network.encrypted, ssids[network.ssid] > 1, network.quality))
                    .join('');
            }).catch(reason => {
                document.getElementById('disconnect').style.display = 'block';
            });
    }, interval);
})();