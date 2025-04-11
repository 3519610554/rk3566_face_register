const socket = io.connect('http://' + document.domain + ':' + location.port);

// 监听后端发来的 'image' 事件
socket.on('image', function(data) {
    const img = document.getElementById('image');
    img.src = 'data:image/jpeg;base64,' + data.data;
});

function startStreaming() {
    console.log('开始推流按钮被点击');
    socket.emit('start_stream');
}
