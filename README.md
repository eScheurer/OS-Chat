# Operating Systems Project - Dynamic Multithreaded Chat Server

### Requirements
1. Linux system
2. C compiler & cmake
3. Python3
4. (Optional) 2x Playit.gg tunnels

## How to use
1. Open a terminal on Linux
2. Clone the github repository
3. Open the directory OS-Chat
4. Compile the CMakeFiles.txt -> (First execute cmake and then make on Raspberry Pi or other Linux device)
5. Execute the OS_Chat executable (./OS_Chat) -> The Server should now display that it's running.
6. In a new terminal open the subdirectory ./UI 
7. Running the server over the internet requires a change of the 3rd line of the script.js file "let url = "http://localhost:8080" to your own domain. Simply substituting the line with your own domain of the backend is enough. If the port is to be changed then in the server.h file the 13th lines needs to be updated to the desired port.
8. Execute python3 -m http.server 9998 -> Website should be running as well
9. (Optional) Run playit.gg tunneling service to make both the backend on port 8080 as well as the website on port 9998 reachable from the internet.

## A project by
Cielle Hawrylenko\
Enya Scheurer\
Helene Spörri\
Luis Wenger\
\
Finished on 11.06.2025


