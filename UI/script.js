function getTime() {
  fetch('http://localhost:8080/time')
    .then(response => response.text())
    .then(data => {
      document.getElementById('time').innerText = data;
    })
    .catch(error => {
      document.getElementById('time').innerText = "Error: " + error;
    });
}

// Update frequently. This is usefull for our project to fetch new chat messages later on.
// setInterval(getTime, 1000);

// getTime();
function getChatRooms() {
    document.getElementById('chatList').innerText = "loading...";
    fetch('http://localhost:8080/chatlist')
        .then(response => response.text())
        .then(data => {
            document.getElementById('chatList').innerText = data;
        })
        .catch(error => {
            document.getElementById('chatList').innerText = "Error: " + error;
        });
}

function getMessages() {
    document.getElementById('chatMessages').innerText = "loading...";
    fetch('http://localhost:8080/chatmessages')
        .then(response => response.text())
        .then(data => {
            document.getElementById('chatMessages').innerText = data;
        })
        .catch(error => {
            document.getElementById('chatMessages').innerText = "Error: " + error;
        });
}