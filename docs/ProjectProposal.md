# Project Proposal

Project name:
Chat Room

## Description
Multi-Threaded Web Server as Chatroom: We want to create a multithreaded webserver 
hosting a chatroom for multiple clients. The chat should be accessible on a website. 
There the user can connect to different chats, some only accessible with password.
A client can only be in one chat at a time (though they can open multiple tabs at a time).


## Functions Backend
* The client-server-communication works via internet. The server and clients do not have to be in the same network
* Login from user on website should request Client thread from Server interface
* Server interface should create new Client thread (or assign client to existing thread)
* Client thread is used for communication between user/website and chatroom on server
* Server should group messages in different chatrooms via prefix
* The number of Clients hosted by the server should be restrictable


## Sketch Backend
![Sketch_ProjectProposal](Sketch_ProjectProposal.png)


## Functions Frontend
* All currently existing chats are displayed on website
* Join public chatroom 
* Join private chatroom with password
* Create new chatroom (public or private)
* Exit chat

  
## Requirements
* Domain for Website


### Ideas for extensions in case our project isn't complex enough
* Thread pool: All treads are created before used, clients get assigned to existing tread when they connect
* Host server on a Raspberry Pi or similar
* Emojis, Gifs, Sticker, Voice messages
