console.log('Script loaded'); //Test if js is loaded
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

function getThreadStatus() {
    console.log('Fetching thread status...'); // Test if thread_stats is fetched
    fetch('http://localhost:8080/threadstatus')
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

/** Run once on load
window.onload = function() {
    getThreadStatus();
    // Optional: refresh every 5 seconds
    setInterval(getThreadStatus, 5000);
};
    */

// Fetch in defined interval
setInterval(getThreadStatus, 5000);

// Fetch once on page load
window.onload = getThreadStatus;


// Update frequently. This is usefull for our project to fetch new chat messages later on.
// setInterval(getTime, 1000);

// getTime();
