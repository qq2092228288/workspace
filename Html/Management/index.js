/**
 * @description Html connection to server 
 */
function connect_server() {
    // set web server address
    if (location.search != "")
        var baseUrl = (/[?&]webChannelBaseUrl=([A-Za-z0-9\-:/\.]+)/.exec(location.search)[1]);
    else
        var baseUrl = "ws://localhost:18088";
    var socket = new WebSocket(baseUrl);
    // connect failed
    socket.onclose = function() {
        console.error("web channel closed");    // server close
    };
    socket.onerror = function(error) {
        console.error("web channel error: " + error);
    };
    // connected
    socket.onopen = function() {
        new QWebChannel(socket, function(channel) {
            // 'core' is the channel between server and web
            window.core = channel.objects.core;
            // received server newHtml
            core.sendText.connect(function(newHtml) {
                // set html from server
                document.body.innerHTML = newHtml;
            });
            // get login ui
            core.loginUi();
        });
    }
}
/**
 * @description Create a temporary file and return the file address 
 * @param {string} fdata file data
 * @param {string} ftype file type, example: text/javascript text/css
 * @returns {string}
 */
function createTempFile(fdata, ftype) {
    var blob = new Blob([fdata], {type: ftype});
    return URL.createObjectURL(blob);
}
