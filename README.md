# AI-ArmiesBattleGame

2 teams, red and blue, fighting each other.
Each group has 2 soldiers (one aggressive and the other is careful), and a combat supporter whose goal is to deliver ammunition / heal the soldiers in his group.

The decisions of the players are made by a decision tree attached to the project, according to their data at any given moment.  
I would like to elaborate on the state of the fighting:  
The combat mode is basically beyond a match-making decision tree (i.e., the situation where the player is with an opposing player in the same room).
During this time there are 5 considerations for making the decision (attack using a grenade or a shot, approaching, escaping from the opponent).    
  
The considerations are:  
  
•	HP level  
• Ammunition level  
• How far is the opponent?  
• Is the opponent on the vision map?  
• How many grenades left?  

All parameters are normalized to percentages, and on them are applied exp, ln functions, which helps in defining the urgency of each of the operations in intelligently way.  
• ln - For parameters whose urgency is greater when their value is low.  
• exp - for parameters whose urgency is greater when their value is high.  

Astar - The G function is a combination of distance from the starting point + safe / unsafe cell cost.
In this case -> a cell with a security value of 0 is safer, and 1 is unsafe.
So I used exp function to punish unsafe cells as well as actually prevent the player from going through them.

https://streamable.com/sx8urg
