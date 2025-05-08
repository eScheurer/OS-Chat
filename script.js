function getTime() {
  fetch('http://localhost:8080')
    .then(response => response.text())
    .then(data => {
      document.getElementById('time').innerText = data;
    })
    .catch(error => {
      document.getElementById('time').innerText = "Error: " + error;
    });
}

function getThreadStatus(){
    fetch('/threadstatus')
        .then(response => response.json())
        .then(data => {
            const statusList = document.getElementById('status-list');
            statusList.innerHTML = '';
            data.forEach(thread => {
                const li = document.createElement('li');
                li.textContent = `Thread ${thread.thread_id}: ${thread.tasks_handled} tasks, ${thread.active_time.toFixed(2)}s active, ${thread.is_idle ? 'idle' : 'busy'}`;
                statusList.appendChild(li);
                });
            })
        .catch(error => {console.error('Error fetching thread status:', error);
        });
}

// Fetch stats every 5 seconds
setInterval(getThreadStatus, 5000);

// Fetch once on page load
window.onload = getThreadStatus;


// Update frequently. This is usefull for our project to fetch new chat messages later on.
// setInterval(getTime, 1000);

// getTime();
