# Prototype (Maze Solving robot powered by PIC16F)

# Differential drive maze solving robot employing left-hand rule
  
# Able to memorize simplified pathway to the exit after running 1st round in maze
  
# System Overview

  Simple line following robot on maze made by non-reflective dark coloured material
                          →
               Let robot run in maze and maneuver around
                          →
                Prompt user to save the maze pattern once robot detected exit
                          →
  Solve the maze by looking for any combination of turns/direction it takes to move through junction
  (read EEPROM for an array of turn the robot takes during maze solving, compare and update arraylist.
  eg: robot turn left on junction 1, move straight and detected dead end in front, then it U-turn and turn left
  based in left-hand rule then proceed to next junction. This combination can be updated in EEPROM to inform robot to 
  move staright next time it arrive this junction)
