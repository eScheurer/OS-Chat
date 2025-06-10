console.log('Script loaded'); //Test if js is loaded

const url = "http://localhost:8080"


/**
 * Retrieves value of param from current URL (after '?') (example: if "...com/?chatname=general", it returns "gereral")
 * @param param
 * @returns {string}
 */
function getURLParam(param) {
    const urlSearchParams = new URLSearchParams(window.location.search); // ChatGPT was used to get the idea for this
    return urlSearchParams.get(param);
}

/**
 * Initializes chat UI by setting chat title form URL
 */
function initChatTemplate(){
    document.getElementById('chat-title').innerText = getURLParam('chatname');
}

//--------------USERNAME-----------------------
/**
 * Takes the input of the user and tries to load it into the session storage after checking if it is unique
 */
function setUserName(){
    let usernameInput = document.getElementById("username")
    //Check for null-pointers
    if(!usernameInput){
        console.log("Failed to set Username, couldn't fetch element")
        return;
    }
    let username = usernameInput.value.trim();
    //Check for no special characters
    if(isValidName(username)){
        //Send to server to check if name is already taken
        fetch(url + '/checkUsername/', {
            method: 'POST',
            body: username
        })
            .then(response => response.text())
            .then(text => {
                if (text.includes("FREE")) {
                    //If it's free we put it into session storage and clear the input
                    console.log("username = " + username);
                    usernameInput.placeholder = username;
                    sessionStorage.setItem("username",username);
                    usernameInput.value = "";
                    console.log("stored username: " + sessionStorage.getItem("username"));
                } else if (text.includes("TAKEN")) {
                    //If it's already taken we inform the user about it
                    document.getElementById('usernameHint').textContent = "Username already taken, please choose another one.";
                }
            })
            .catch(error => {
                document.getElementById('chatnameHint').textContent = "Error: Something went wrong with checking the name." + error;
            });
    }
}

/**
 * Fetches username out of Session storage. Implemented in case we switch to local storage or similar.
 * @returns username
 */
function getUserName(){
    console.log("stored username: " + sessionStorage.getItem("username"));
    return sessionStorage.getItem("username");
}

//--------------CHAT-CREATION-----------------------
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
        //Check that we even have a name before allowing the creation
        if(sessionStorage.getItem("username")==null){
            document.getElementById("usernameHint").innerText = "Username not yet set!";
            return;
        }
        const input = document.getElementById("userInput").value;
        checkChatName(input); // check if name is valid
        document.getElementById("userInput").value = ""; // Reset input
    });
});

/**
 * Sends request to server for checking the name, creates chatroom if free, informs user if taken
 * @param name entered by user
 */
function checkChatName(name) {
    fetch(url + '/checkChatName/', {
        method: 'POST',
        body: name
    })
        .then(response => response.text())
        .then(text => {
            if (text.includes("FREE")) {
                createNewChatroom(name); // if name free, create chatroom
            } else if (text.includes("TAKEN")) {
                // inform user that name taken
                document.getElementById('chatnameHint').textContent = "Name already taken, please choose another one.";
            } else {
                console.log("Something went wrong with checking the name. Response was:", text);
            }
        })
        .catch(error => {
            document.getElementById('chatnameHint').textContent = "Error: " + error;
        });
}

/**
 * Creates new chatroom by displaying new UI and sending initial message to server.
 * @param name for chatroom
 */
function createNewChatroom(name){
    window.location.href = "chatTemplate.html?chatname=" + encodeURIComponent(name);
    let message = name;
    message = name + ":" + getUserName() + " created a new chat room!";
    fetch(url + '/newChatroom/', {
        method: 'POST',
        body: message
    }).catch(error => {
        document.getElementById('chatRoomsList').placeholder = "Error: " + error;
    });
    console.log(message);
}

//--------------FETCH/JOIN-CHATS-----------------------

/**
 * Requests all currently existing chatrooms from server, displays in dynamic list with join buttons. On main page.
 */
function getChatRooms() {
    const ul = document.getElementById('chatRoomsList');
    // ChatGPT was used to understand how a list with buttons can be created dynamically in javascript.
    // ChatGPT provided very basic general code. I then implemented it myself, following that general idea and adapting it for our use.
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
                    //Check that we even have a name before allowing the creation
                    if(sessionStorage.getItem("username")==null){
                        if(document.getElementById("usernameHint")) {
                            document.getElementById("usernameHint").innerText = "Username not yet set!";
                        }
                        console.log("tried joining chat but username is null");
                        return;
                    }
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

 /**
 * Requests all currently existing chatrooms from server, displays in dynamic list with join buttons. On chat page.
 */
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


/**
 * For joining another chat. Switches to new UI and sends initial message
 * @param chatName
 */
function joinChatroom(chatName){
    window.location.href = "chatTemplate.html?chatname=" + encodeURIComponent(chatName);
    chatName = chatName.trim();
    let message = chatName + ":" + getUserName() + " joined this chat room!";
    fetch(url + '/sendmessage', {
        method: 'POST',
        body: message
    }).catch(error => {
        document.getElementById('chatMessages').placeholder = "Error: " + error;
    });
    console.log(message);
}

//--------------SEND-MESSAGES-----------------------

let previousSend = 0;
/**
 * Takes input of the message text field and sends it to the server.
 * Function blocks if it is used too frequently
 */
function sendMessage() {
    const now = Date.now();
    if (now - previousSend < 100) {
        alert("Please don't spam.");
        console.log("Please wait before sending new messages");
        return;
    }
    previousSend = now;
    const textarea = document.getElementById("message-text")
    if(textarea == null){
        console.log("Could not fetch textarea to send message!");
        return
    }
    let message = textarea.value.trim();

    if (message === ""){
        console.log("Empy request");
        return;
    }
    //extract ChatName from HTML
    const chatNameElement = document.getElementById('chat-title');
    if(chatNameElement == null){
        console.log("Could not fetch chatname to send message!");
        return;
    }
    const chatName = chatNameElement.innerText.trim(); //.trim() erases unwanted added elements from formating
    message = chatName + ":" + getUserName() + ": " + message
    textarea.value = "";
    fetch(url + '/sendmessage', {
        method: 'POST',
        headers: {
            'Content-Type': 'text/plain'
        },
        body: message
    }).catch(error => {
        console.error("Could not send message: " + error)
    });
    console.log(message);
}

//Section to check for enter inputs to send messages!
//We store this variable to check if enter and shift are pressed together
let shiftIsPressed = false;
document.getElementById("message-text").addEventListener("keydown", function(event) {
    if (event.key === "Shift") {
        shiftIsPressed = true;
    }
    if (event.key === "Enter") {
        if(shiftIsPressed){
            return;
        }
        // Cancel the default action, if needed
        event.preventDefault();
        sendMessage()
    }
});
document.getElementById("message-text").addEventListener("keyup", function(event) {
    //If we let go of shift we set it back to false
    if (event.key === "Shift") {
        shiftIsPressed = false;
    }
});


//--------------FETCH-UPDATES-----------------------


setInterval(getThreadStatus, 10000);
function getThreadStatus() {
    console.log('Fetching thread status...');
    // Test if thread_stats is fetched
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

/**
 * requests content of current chat, displays reply in chat format
 */
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
// the idea for the following lines is from ChatGPT when I did research on how to use a single javascript file for multiple html files.
console.log("Current path:", window.location.pathname);
if (window.location.pathname.endsWith('chatTemplate.html')) {
    window.addEventListener('load', () => {
        console.log("initChatTemplate is running");
        initChatTemplate();
    });
}


//--------------REGEX-CHECKS-----------------------

function isValidName(name) {
    const pattern = /^[a-zA-Z0-9]+$/;
    return pattern.test(name);
}

function validateUsername() {
    const input = document.getElementById("username").value;
    const hint = document.getElementById("usernameHint");

    if (!isValidName(input)) {
        hint.innerText = "Username not valid. No special characters allowed.";
        return false; // Submission not possible
    } else {
        hint.innerText = ""; // No hint
        return true; // Submission allowed
    }
}

function validateChatname() {
    const input = document.getElementById("userInput").value;
    const hint = document.getElementById("chatnameHint");

    if (!isValidName(input)) {
        hint.innerText = "Chatname not valid. No special characters allowed.";
        return false; // Submission not possible
    } else {
        hint.innerText = ""; // No hint
        return true; // Submission allowed
    }
}

function isValidMessage(message) {
    const pattern = /^[^$]+$/;
    return pattern.test(message);
}

function validateMessage() {
    const input = document.getElementById("message-text").value;
    const hint = document.getElementById("messageHint");

    if (!isValidMessage(input)) {
        hint.innerText = "Message not valid. No Dollarsign allowed.";
        return false; // Submission not possible
    } else {
        hint.innerText = ""; // No hint
        return true; // Submission allowed
    }
}