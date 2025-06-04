console.log('Script loaded'); //Test if js is loaded

let url = "http://localhost:8080"
//This can be changed, standart is NoUsername for testing
let usrname = "NoUsername"
//This can be changed on buttton press, standart is general for testing
let chatname = "general"

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

// Update frequently. This is usefull for our project to fetch new chat messages later on.
// setInterval(getTime, 1000);

// getTime();
function getChatRooms() {
    document.getElementById('chatList').innerText = "loading...";
    fetch(url + '/chatlist')
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
    fetch(url + '/chatmessages')
        .then(response => response.text())
        .then(data => {
            document.getElementById('chatMessages').innerText = data;
        })
        .catch(error => {
            document.getElementById('chatMessages').innerText = "Error: " + error;
        });
}
function sendMessage() {
    const textarea = document.getElementById("message-text")
    let message = textarea.value;
    message = chatname + ":" + usrname + ": " + message
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
    console.log("ich bin im getChat Update");
    //extract ChatName from HTML
    const chatNameElement = document.getElementById('chat-title');
    const chatName = chatNameElement.innerText.trim(); //.trim() erases unwanted added elements from formating

    //send request for update
    fetch(url + "/chatUpdate/" + encodeURIComponent(chatName) + "$") //endocdeURIComponent: encodes it to be URL safe, could contain special characters that might be unsafe
        .then(respons => respons.text())
        .then(data => { //display resoonse in field chatMessages
            document.getElementById('chatMessages').innerText = data;
        })
        .catch(error => {
            document.getElementById('chatMessages').innerText = "Error: " + error;
        })
}
// Fetch in defined interval
setInterval(getChatUpdate, 1000);



// Fetch in defined interval
//setInterval(getThreadStatus, 10000);

// Update frequently. This is usefull for our project to fetch new chat messages later on.
// setInterval(getTime, 1000);

// getTime();