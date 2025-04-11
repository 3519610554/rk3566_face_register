let socket;

function startStreamInit(){
    socket = io.connect('http://' + document.domain + ':' + location.port);

    socket.on('image', function(data) {
        const img = document.getElementById('image');
        img.src = 'data:image/jpeg;base64,' + data.data;
    });
    initFormSubmit();
}

//开始推流视频
function startStreaming() {
    if (socket) {
        socket.emit('start_stream');
    } else {
        console.error("Socket not initialized");
    }
}

//显示固定页面
function showPage(pageId) {
    console.log("隐藏界面:", pageId);
    // 隐藏所有页面
    document.querySelectorAll('.page').forEach(p => p.classList.remove('active'));
    // 显示指定页面
    document.getElementById(pageId).classList.add('active');
}
