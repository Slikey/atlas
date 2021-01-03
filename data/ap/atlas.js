const Atlas = (function () {
    const interval = 5000;
    console.log('Starting atlas...');

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
                const elements = [];
                networks = networks
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
                    .map(network => {
                        let form;
                        if (network.encrypted) {
                            form = `<button class="btn btn-sm btn-a" onclick="Atlas.setConnectionPrompt('${network.ssid}'); return false;">Enter Password</button>`
                        } else {
                            form = `<button class="btn btn-sm btn-a" onclick="Atlas.connect('${network.ssid}'', ''); return false;">Connect</button>`
                        }
                        const element = document.createElement('div');
                        element.className = 'col c4';
                        element.innerHTML = `
                        <h3>${network.ssid} ${network.encrypted ? '&#128274;' : ''} ${ssids[network.ssid] > 1 ? '&#9850;' : ''}</h3>
                        <p>Quality: ${network.quality}</p>
                        ${form}`;
                        return element;
                    });

                const networkList = document.getElementById('network-list');
                networkList.innerHTML = '';
                networks.forEach(network => {
                    networkList.appendChild(network);
                });
            }).catch(reason => {
                document.getElementById('disconnect').style.display = 'block';
            });
    }, interval);

    const setConnectionPrompt = (ssid) => {
        const networkConnection = document.getElementById('network-connection');
        networkConnection.style.display = 'block';
        networkConnection.innerHTML = `
            <h2>Connect</h2>
            <form>
                <label for="ssidInput">SSID:<input id="ssidInput" type="text" value="${ssid}" readonly /></label><br />
                <label for="ssidInput">Password:<input id="passInput" type="text" placeholder="Enter Password" /></label>
                <button class="btn btn-sm btn-a" onclick="Atlas.sendNetworkConnectionForm(); return false;">Connect</button>
            </form>
            `;
    };
    const connect = (ssid, pass) => {
        return fetch('/wifi/connect', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded'
            },
            cache: 'no-cache',
            body: `ssid=${ssid}&pass=${pass}`
        });
    };
    const sendNetworkConnectionForm = () => {
        const networkConnection = document.getElementById('network-connection');
        networkConnection.style.display = 'none';
        const ssidInput = document.getElementById('ssidInput');
        const passInput = document.getElementById('passInput');
        connect(ssidInput.value, passInput.value);
    };

    return {
        setConnectionPrompt,
        connect,
        sendNetworkConnectionForm
    };
})();