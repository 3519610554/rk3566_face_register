let socket;
let container;

//页面初始化
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

//显示图片
function image_show(){

    if (!socket){
        console.error("Socket not initialized");
        return;
    }
    socket.emit('start_stream');
    container = document.getElementById('adminPage');
    socket.on('image', function(data){
        const item = data.data;

        if (!item || !item.image) {
            console.warn('Invalid image data received');
            return;
        }

        const card = document.createElement('div');
        card.className = 'card';

        const img = document.createElement('img');
        img.src = 'data:image/jpeg;base64,' + item.image;

        const idText = document.createElement('p');  
        idText.className = 'id_text';
        idText.innerText = `照片ID: ${item.id}`;

        const timeText = document.createElement('p');
        timeText.className = 'time_text';
        timeText.innerText = `上传时间: ${item.time}`;

        // 创建右侧的容器，将 ID 和时间放入其中
        const rightContainer = document.createElement('div');
        rightContainer.className = 'right-container';
        rightContainer.appendChild(idText);
        rightContainer.appendChild(timeText);

        // 创建删除按钮
        const deleteButton = document.createElement('button');
        deleteButton.className = 'delete-button';
        deleteButton.innerText = '删除记录';
        
        // 添加点击事件，向后端发送请求并删除该卡片
        deleteButton.addEventListener('click', function(){
            fetch('/delete_image', {
                method: 'POST',
                body: JSON.stringify({id: item.id}),
                headers: {
                    'Content-Type': 'application/json'
                }
            })
            .then(response => response.json())
            .then(data => {
                console.log('删除成功: ', data);
                container.removeChild(card);
            })
            .catch(error => {
                console.log('删除失败: ', error);
            })
        });
        // 将删除按钮添加到右侧容器
        rightContainer.appendChild(deleteButton);

        // 将图片和右侧容器添加到卡片中
        card.appendChild(img);
        card.appendChild(rightContainer);

        // 将卡片添加到容器中
        container.appendChild(card);
    });
}

function submitForm(event) {

    event.preventDefault();

    const name = document.getElementById('name').value;
    console.log('提交的姓名:', name);

    fetch('/submit_form', {
        method: 'POST',
        body: JSON.stringify({ name: name }),
        headers: {
            'Content-Type': 'application/json'
        }
    })
    .then(response => response.json())
    .then(data => {
        console.log('服务器响应:', data);
    })
    .catch(error => {
        console.error('提交失败:', error);
    });
}
