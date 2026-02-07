Chat room
=============
A course work made by C++
------------
a multi-user chat room that supports duplicate-name detection, private chat windows, and notification sound effects.

The client detects the server connection status and displays an animated indicator showing whether the server is online. During login, the server checks for duplicate usernames🔽.
<p align="left">
  <img src="https://github.com/Wang-Yuan0813/Warwick-Chat-room/raw/master/Examples/login.gif" width="400">
  <img src="https://github.com/Wang-Yuan0813/Warwick-Chat-room/raw/master/Examples/namesame.gif" width="400">
</p>
Users can open or close the private chat window by clicking on the icon🔽.
<p align="left">
  <img src="https://github.com/Wang-Yuan0813/Warwick-Chat-room/raw/master/Examples/publicchat.gif" width="400">
  <img src="https://github.com/Wang-Yuan0813/Warwick-Chat-room/raw/master/Examples/dm.gif" width="400">
</p>
When a client disconnects, the server notifies all other online clients and synchronizes the updated online‑user list. (As shown below, after Client #4 closed the application, the online user lists of the remaining clients were updated accordingly.)When a new private message arrives, a red notification dot appears on the top‑right corner of the user’s avatar. 🔽
<p align="left">
  <img src="https://github.com/Wang-Yuan0813/Warwick-Chat-room/raw/master/Examples/byebye.gif" width="400">
  <img src="https://github.com/Wang-Yuan0813/Warwick-Chat-room/raw/master/Examples/dmnotification.gif" width="400">
</p>
The chat window also supports auto‑scrolling, ensuring that the public chat view automatically scrolls to the bottom whenever new messages are added.🔽
<p align="left">
  <img src="https://github.com/Wang-Yuan0813/Warwick-Chat-room/raw/master/Examples/autoscroll.gif" width="200">
</p>
