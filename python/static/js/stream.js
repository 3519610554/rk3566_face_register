let socket;
let container;

function initializeSocket() {
    if (!socket) {
        socket = io.connect('http://' + document.domain + ':' + location.port);
        socket.on('connect', function() {
            console.log('Socket connected');
        });
    }
}

//显示固定页面
function showPage(pageId) {
    // 隐藏所有页面
    document.querySelectorAll('.page').forEach(p => p.classList.remove('active'));
    // 显示指定页面
    document.getElementById(pageId).classList.add('active');
}

function socket_get_image(data){

    console.log("image_show: ", data.data);
    const item = data.data;

    if (!item || !item.image) {
        console.warn('Invalid image data received');
        return;
    }

    const card = document.createElement('div');
    card.style.border = '1px solid #ccc';
    card.style.padding = '10px';
    card.style.margin = '10px';
    card.style.maxWidth = '220px';
    card.style.fontFamily = 'Arial';

    const img = document.createElement('img');
    img.src = 'data:image/jpeg;base64,' + item.image;
    img.style.maxWidth = '200px';
    img.style.display = 'block';

    const idText = document.createElement('p');
    idText.innerText = `ID: ${item.id}`;

    const timeText = document.createElement('p');
    timeText.innerText = `Time: ${item.time}`;

    card.appendChild(idText);
    card.appendChild(timeText);
    card.appendChild(img);

    container.appendChild(card);
}

function image_show(){

    if (socket) {
        socket.emit('start_stream');
        container = document.getElementById('adminPage');
        socket.on('image', socket_get_image);
    } else {
        console.error("Socket not initialized");
    }
}
