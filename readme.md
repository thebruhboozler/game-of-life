
John Conway's Game Of Life( https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life ) by sababugs112 .

***John Conway's Game Of Life***

**general info**


The grid is infinite , but sections of it are managed by a chunk system.  Each chunk has a x and y coordinate on the grid and their size is 256 by 256.
In order to conserve memory the chunks also have an array of numbers which are the indexes of the cell which are alive within their bounds , instead of storing a 2d array . In addition , chunks which have no currently alive cells within their bounds are deleted.
The indexes can be mapped mathematically to x and y coordinates.

New chunks can be manually created by either importing structures or left clicking on the grid to create a cell. 
New chunks will be automatically created whenever a sturcture goes into a section of the grid which doesn't have a chunk already allocated to it.

**user interface** 

Right clicking will reveal a pop up menu which will have various options 


*import*: select a file in which a sturcture is stored then click on the grid to place the structure.

*export*: left click twice on different points and save a .txt file containing various numbers.
    (the first two numbers in the file are width and length of the selected area in terms of sqaures. the rest are indexes of all alive cells within the region.)
 
*clear all* : deletes every chunk and cell , which effectively clears the grid of all structures.

*teleport* : will teleport you to waypoints which you create. teleport option cycles through the list of waypoints based on chronological order.

*add waypoint* : adds a waypoint at the coordinates which the user is at when it's pressed. The coordinates can be seen on the top right of the programme window.

*clear waypoints* : deletes all existing waypoints.

*help* : brings up the help screen.

**compiling**

Simply open the file directory in a terminal and type make .  keep in mind this project requires for you to have glfw , glut and gtk installed.
