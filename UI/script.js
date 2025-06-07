console.log('Script loaded'); //Test if js is loaded

let url = "http://localhost:8080"
//This can be changed, standart is NoUsername for testing
let usrname = "NoUsername"
//This can be changed on buttton press, standart is general for testing
//let chatname = "general"

// For reading URL-parameters (in case needed somewhere else too)
function getURLParam(param) {
    const urlSearchParams = new URLSearchParams(window.location.search);
    return urlSearchParams.get(param);
}
// For updating Chat Title on chatTemplate.html
function initChatTemplate(){
    document.getElementById('chat-title').innerText = getURLParam('chatname');
}

window.addEventListener('DOMContentLoaded', () => {
    // Handles the creation of a new chatroom.
// Opening popup
    document.getElementById("openPopup").addEventListener("click", function (){
        document.getElementById("popup").style.display = "flex";
    });
// Closing popup
    document.getElementById("closePopup").addEventListener("click", function (){
        document.getElementById("popup").style.display = "none";
    });
// The "Create" button
    document.getElementById("confirmButton").addEventListener("click", function (){
        const input = document.getElementById("userInput").value;
        createNewChatroom(input);
        document.getElementById("userInput").value = ""; // Reset input
    });
});
function createNewChatroom(name){
    window.location.href = "chatTemplate.html?chatname=" + encodeURIComponent(name);
    let message = name;
    message = name + ":" + usrname + " created a new chat room!";
    fetch(url + '/newChatroom/', {
        method: 'POST',
        body: message
    }).catch(error => {
        document.getElementById('chatMessages').placeholder = "Error: " + error;
    });
    console.log(message);
}


 function getTime() {
  fetch(url)
    .then(response => response.text())
    .then(data => {
      document.getElementById('time').innerText = data;
    })
    .catch(error => {
      document.getElementById('time').innerText = "Error: " + error;
    });
}

// For displaying currently available chatrooms
function getChatRooms() {
    const ul = document.getElementById('chatRoomsList');
    ul.innerHTML = ''; // List needs to be emptied so that no duplicate chatrooms are shown
    //document.getElementById('chatList').innerText = "Loading...";
    //ul.innerHTML = "<li>Loading...</li>";
    fetch(url + '/chatList/')
        .then(response => response.text())
        .then(data => {
            //const chatRoomsList = data.split('$');
            //document.getElementById('chatList').innerText = data.split('$').join('\n');
            const chatRoomsList = data.split('$').filter(name => name !== '' && name !== 'Empty chatList');

            if (chatRoomsList.length === 0) { // No chatrooms yet
                ul.innerHTML = '<li>No existing chat rooms yet</li>';
                return;
            }
            for (let i = 0; i < chatRoomsList.length; i++) {
                const chatName = chatRoomsList[i].trim();
                const li = document.createElement('li');
                li.textContent = chatName;
                const button = document.createElement('button') ;
                button.textContent = 'Join'; button.onclick = function () {
                    joinChatroom(chatName);
                };
                li.appendChild(button);
                ul.appendChild(li);
            }

        })
        .catch(error => {
            document.getElementById('chatList').innerText = "Error: " + error;
        });
}

// For displaying currently available chatrooms
function getChatRoomsfromChat() {
    //extract ChatName from HTML
    const chatNameElement = document.getElementById('chat-title');
    const chatName = chatNameElement.innerText.trim(); //.trim() erases unwanted added elements from formating

    const ul = document.getElementById('chatRoomsList');
    ul.innerHTML = ''; // List needs to be emptied so that no duplicate chatrooms are shown
    fetch(url + '/chatList/')
        .then(response => response.text())
        .then(data => {
            const chatRoomsList = data.split('$').filter(name => name !== '' && name !== 'Empty chatList' && name !== chatName);
            if (chatRoomsList.length === 0) { // No chatrooms yet
                ul.innerHTML = '<li>No other chat rooms yet</li>';
                return;
            }
            for (let i = 0; i < chatRoomsList.length; i++) {
                const chatName = chatRoomsList[i].trim();
                const li = document.createElement('li');
                li.textContent = chatName;
                const button = document.createElement('button') ;
                button.textContent = 'Join'; button.onclick = function () {
                    joinChatroom(chatName);
                };
                li.appendChild(button);
                ul.appendChild(li);
            }

        })
        .catch(error => {
            document.getElementById('chatList').innerText = "Error: " + error;
        });
}

// for joining a chatroom on button click
function joinChatroom(chatName){
    window.location.href = "chatTemplate.html?chatname=" + encodeURIComponent(chatName);
    chatName = chatName.trim();
    let message = chatName + ":" + usrname + " joined this chat room!";
    fetch(url + '/sendmessage', {
        method: 'POST',
        body: message
    }).catch(error => {
        document.getElementById('chatMessages').placeholder = "Error: " + error;
    });
    console.log(message);
}



function sendMessage() {
    const textarea = document.getElementById("message-text")
    let message = textarea.value;
    //extract ChatName from HTML
    const chatNameElement = document.getElementById('chat-title');
    const chatName = chatNameElement.innerText.trim(); //.trim() erases unwanted added elements from formating
    message = chatName + ":" + usrname + ": " + message
    textarea.value = "";
    fetch(url + '/sendmessage', {
        method: 'POST',
        headers: {
            'Content-Type': 'text/plain'
        },
        body: message
    }).catch(error => {
        document.getElementById('message-text').placeholder = "Error: " + error;
    });
    console.log(message);
}

//Does not seem to work yet, but not too important
document.getElementById("message-text").addEventListener("keypress", function(event) {
    // If the user presses the "Enter" key on the keyboard
    if (event.key === "Enter") {
        // Cancel the default action, if needed
        event.preventDefault();
        sendMessage()
    }
});

function getThreadStatus() {
    console.log('Fetching thread status...'); // Test if thread_stats is fetched
    fetch(url + '/threadstatus')
        .then(response => response.json())
        .then(data => {
            const statusList = document.getElementById('status-list');
            statusList.innerHTML = '';  // clear previous

            data.forEach((thread, index) => {
                const li = document.createElement('li');
                li.textContent = `#${index} - thread_id: ${thread.thread_id}, tasks_handled: ${thread.tasks_handled}, active_time: ${thread.active_time.toFixed(2)}s, is_idle: ${thread.is_idle}`;
                statusList.appendChild(li);
            });
        })
        .catch(error => {
            document.getElementById('thread-status').innerText = "Error: " + error;
        });
}

function getChatUpdate() {
    //extract ChatName from HTML
    const chatNameElement = document.getElementById('chat-title');
    const chatName = chatNameElement.innerText.trim(); //.trim() erases unwanted added elements from formating

    //send request for update
    fetch(url + '/chatUpdate/', {
        method: 'POST',
        body: chatName
    })
        .then(response => response.text())
        .then(data => { //display response in field chatMessages
            document.getElementById('chatMessages').innerText = data.split('$').join('\n'); // Format data
        })
        .catch(error => {
            document.getElementById('chatMessages').innerText = "Error: " + error;
        })
}
// Fetch in defined interval
setInterval(getChatUpdate, 1000);

// Differentiate that chat stuff only happens on the chat page
console.log("Current path:", window.location.pathname);
if (window.location.pathname.endsWith('chatTemplate.html')) {
    window.addEventListener('load', () => {
        console.log("initChatTemplate is running");
        initChatTemplate();
    });
}



// Fetch in defined interval
//setInterval(getThreadStatus, 10000);

// Update frequently. This is usefull for our project to fetch new chat messages later on.
// setInterval(getTime, 1000);

// getTime();