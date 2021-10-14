# Flocking-AI
An AI using behaviour trees which simulated herd flocking behaviours using multiple determinator values

This project simulates a herd or 'flock' of AI's which exhibit their behaviours through multiple determinator
values. These values are 'Cohesion', 'Seperation', 'Velocity match', 'Seek' and 'Wander'

in the /bin folder you will be able to find executables running the program.

## Partitioning
This setting enables or disables partitioning. The partitioning is a perfomance
improvement so that the simulation can handle larger areas and more AI's running concurrently.

The partitioning is done by seperating the simulation area into smaller zones. where the interaction
calculations for each AI's is only handled for it's own zone and adjacent ones. This reduces to total
calculation workload for the CPU by a lot. Which is also seen as a visual improvement in the FPS counter
located in the app itself.

## Cohesion
This value determines how much the herd will try to stay together in groups.

## Seperation
This value will determine if the herd members will try to keep a distance from eachother.

## Velocity match
This influences how the movement velocity of the herd members changes according to their surroundings. 
When this value is maxed out the herd will move as a single unit and the members stay in a relatively fixed position
in relation to eachother. However when the value lowers the members will move more on their own and sometimes
seperate from the herd because they drifted too far.

## Seek
When this value rises the herd members will give priority to finding a marker point indicated by the uses. 
Both the herd and the 'herder' will seek this point.

## Wander
The opposite of the seek behaviour. When this value rises the herd members will randomly wander around
by searching for a point in a cone in front of them, moving to that point and then repeat the process.
Doing so creates a fairly random wandering pattern.

# DISCLAIMER<br>
Parts of this project code was provided by HoWest Digital Arts and Entertainment (DAE) as they provided the framework
in which the AI was built. If you specifically wish to find my code. Look for the classes implementing the AI's behaviours.
All rights and any form of ownership of this code goes to DAE.